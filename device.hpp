/**
 * \file device.hpp
 * \brief devices with template and factory
 * \author Sebastien COUDERT
 * \ref https://en.wikibooks.org/wiki/C%2B%2B_Programming/Code/Design_Patterns#Factory
**/

#ifndef __DEVICE_FACTORY__
#define __DEVICE_FACTORY__

#include <string>         // std::string
#include <iostream>       // std::cout
#include <map>            // std::map

#include "register.hpp"

#ifdef USE_I2C_LIB
#warning "use I2C lib"
#include "i2c/i2c.h"
#endif //USE_I2C_LIB

#define DEVICE_VERSION "v0.1.1"

class Device: public std::map<std::string,Register*>
{
 protected:
  std::string name;
  bool debug;
  virtual void set_name(std::string device_name) {name=device_name; if(debug) std::cout<<name<<"/"<<__func__<<"(\""<<name<<"\")"<<std::endl;}
 public:
  //! constructor
  Device() {name="none";debug=false;}
  virtual std::string get_name() {return name;}
  virtual void create_register(std::string register_name,std::string register_type_name)
  {
    Register *reg=RegisterFactory::NewRegister(register_type_name);
    this->insert(std::pair<std::string,Register*>(register_name,reg));
  }//create_register
  virtual /*std::string*/void register_list(const std::string prefix="") {for (std::map<std::string,Register*>::iterator it=this->begin(); it!=this->end(); ++it) std::cout <<prefix<< it->first << " => " << (it->second)->get_name() << '\n';};
  virtual int get(const std::string &register_name) {return ((this->find(register_name))->second)->read();};
  virtual int set(const std::string &register_name,const int &value) {((this->find(register_name))->second)->write(value);};
  //! destructor (need at least empty one)
  virtual ~Device() {}
};//Device
class FakeDevice: public Device
{
 public:
  FakeDevice()
  {
    set_name("FakeDevice");
    create_register("FakeRegister0","FakeRegister");
    create_register("FakeRegister1","FakeRegister");
  }//constructor
  virtual int get(const std::string &register_name) {if(this->debug) std::cout<<name<<"::get(...)"<<std::endl;return Device::get(register_name);}
  virtual int set(const std::string &register_name,const int &value) {std::cout<<name<<"::set(...) empty"<<std::endl;return 0;};
  //! destructor (need at least empty one)
  virtual ~FakeDevice() {}
};//FakeDevice
class I2C_Device: public Device
{
 public:
  int bus,addr;
  I2CDevice device;
 public:
  I2C_Device()
  {
    set_name("I2C_Device");
    bus=-1;open();
    addr=-1;init();
  }//constructor
  //! open I2C bus
  /**
   * \return I2C fd or <0 on error
  **/
  virtual int open(int bus_num=1)
  {//Open i2c bus
    bus_num = 1;
    char bus_name[32];
    memset(bus_name, 0, sizeof(bus_name));
    if (snprintf(bus_name, sizeof(bus_name), "/dev/i2c-%u", bus_num) < 0)
    {
        fprintf(stderr, "Format i2c bus name error!\n");
        return -3;
    }//bus_name
///open bus, i.e. fd
    if ((bus = i2c_open(bus_name)) == -1)
    {
       fprintf(stderr, "Open i2c bus:%s error!\n", bus_name);
        return -3;
    }//open
    return bus;
  }//open
  virtual int init(int addr_=0x18)
  {//Init i2c device
	addr=addr_;
	int iaddr_bytes = 1, page_bytes = 16;
///setup device desc.
    memset(&device, 0, sizeof(device));
    //default init.
    i2c_init_device(&device);
    //specific init., e.g. from CLI
    device.bus = bus;
    device.addr = addr & 0x3ff;
    device.page_bytes = page_bytes;
    device.iaddr_bytes = iaddr_bytes;
    return 0;
  }//init
  virtual void create_register(std::string register_name,std::string register_type_name, int id_=0x05)
  {
    //! \todo [low] check "I2C" in register_type_name
    //Register *i2c_reg=Register::create_register(register_name,register_type_name);
    Register *reg=RegisterFactory::NewRegister(register_type_name);
    this->insert(std::pair<std::string,Register*>(register_name,reg));
    I2CRegisterWord *i2c_reg=(I2CRegisterWord*)reg;
    i2c_reg->bus=bus;
    i2c_reg->addr=addr;
    i2c_reg->id=id_;
    i2c_reg->pDevice=&device;
  }//create_register
  virtual int get(const std::string &register_name) {if(this->debug) std::cout<<name<<"::get(...)"<<std::endl;return Device::get(register_name);}
  virtual int set(const std::string &register_name,const int &value) {std::cout<<name<<"::set(...) empty"<<std::endl;return 0;};
  //! destructor (need at least empty one)
  virtual ~I2C_Device() {i2c_close(bus);}
 private:
  bool mHibernating;//Whether or not the machine is hibernating
};//I2C_Device
//! MCP9808
class TemperatureDevice: public I2C_Device
{
 private:
  std::string default_register_name;
 public:
  TemperatureDevice()
  {
    set_name("TemperatureDevice");
    default_register_name="AmbiantTemperature";
    create_register(default_register_name,  "I2CRegisterWord",0x05);//,RO);
    create_register("TemperatureResolution","I2CRegisterByte",0x08);//,RW);
  }//constructor
  virtual void read()
  {if(this->debug) std::cout<<name<<"::read()"<<std::endl;
    this->register_list("");
    for (std::map<std::string,Register*>::iterator it=this->begin(); it!=this->end(); ++it)
    {
      std::cout<<"  "<< it->first << " = ";
      (it->second)->read();
    }//register loop
  }//read
  virtual int get(const std::string &register_name) {if(this->debug) std::cout<<name<<"::get("<<register_name<<")"<<std::endl;return Device::get(register_name);}
  virtual int get() {if(this->debug) std::cout<<name<<"::get() "<<default_register_name<<std::endl;return this->get(default_register_name);}
  virtual float get_Celcius()
  {if(this->debug) std::cout<<name<<"::get_Celcius() "<<default_register_name<<std::endl;
    short data=this->get();
    char *buf=(char*)(&data);
    int ub=buf[1];
    int lb=buf[0];
    float temperature=-99.99;
    if(this->debug) fprintf(stdout, "Read data: 0x%x 0x%x\n",buf[0],buf[1]);
    ub=ub&0x1F;//Clear flag bits
    if( (ub&0x10) == 0x10)
    {//Ta < 0°C
      ub=ub&0x0F;//Clear SIGN
      temperature=256.0-(ub*16.0+lb/16.0);
    }
    else
    {//Ta ≥ 0°C
      temperature=(ub*16.0+lb/16.0);//Temperature = Ambient Temperature (°C)
    }
    //std::cout<<"temperature="<<temperature<<std::endl;
    return temperature;
  }
  virtual int set(const std::string &register_name,const int &value) {if(this->debug) std::cout<<name<<"::set(...)"<<std::endl;return Device::set(register_name,value);return 0;};
  virtual int set() {if(this->debug) std::cout<<name<<"::set() TemperatureResolution"<<std::endl;this->set("TemperatureResolution",0x0);return 0;}
  //! destructor (need at least empty one)
  virtual ~TemperatureDevice() {}
};//TemperatureDevice

//! device factory
/**
 *  \note instanciation based on name (i.e. string, as done only one time)
**/
class DeviceFactory
{
  public:
  static Device *NewDevice(const std::string &name)
  {
    if(name == "FakeDevice")
      return new FakeDevice;
    if(name == "TemperatureDevice")
      return new TemperatureDevice;
    std::cerr<<"Device name is unknown, i.e. \""<<name<<"\"."<<std::endl;
    return NULL;
  }//NewDevice
  static std::string List(void)
  {
    std::string list;
    list="FakeDevice";
    list+=", TemperatureDevice";
    list+=".";
    return list;
  }

};//DeviceFactory

void device_implementation()
{
  {//basis
  FakeDevice fake;        fake.register_list();
  TemperatureDevice temp; temp.register_list();
  }//basis
  {//factory
  Device *fake=DeviceFactory::NewDevice("FakeDevice");       if(fake!=NULL) fake->register_list();
  Device *temp=DeviceFactory::NewDevice("TemperatureDevice");if(temp!=NULL) temp->register_list();
  }//factory
}//device_implementation

#endif //__DEVICE_FACTORY__
