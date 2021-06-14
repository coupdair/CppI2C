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

#define DEVICE_VERSION "v0.1.0e"

class Device: public std::map<std::string,Register*>
{
 protected:
  std::string name;
  virtual void set_name(std::string device_name) {name=device_name; std::cout<<name<<"/"<<__func__<<"(\""<<name<<"\")"<<std::endl;}
 public:
  virtual std::string get_name() {return name;}
  virtual void create_register(std::string register_name,std::string register_type_name)
  {
    Register *reg=RegisterFactory::NewRegister(register_type_name);
    this->insert(std::pair<std::string,Register*>(register_name,reg));
  }//create_register
  virtual /*std::string*/void register_list(const std::string prefix="") {for (std::map<std::string,Register*>::iterator it=this->begin(); it!=this->end(); ++it) std::cout <<prefix<< it->first << " => " << (it->second)->get_name() << '\n';};
  //! constructor
  Device() {name="none";}
  virtual void Run() = 0;
  virtual void Stop() = 0;
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
  virtual void Run()  {std::cout<<name<<" run"<<std::endl; mHibernating = false;}
  virtual void Stop() {mHibernating = true;}
  //! destructor (need at least empty one)
  virtual ~FakeDevice() {}
 private:
  bool mHibernating;//Whether or not the machine is hibernating
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
  virtual void Run()  {std::cout<<name<<" run"<<std::endl; mHibernating = false;}
  virtual void Stop() {mHibernating = true;}
  //! destructor (need at least empty one)
  virtual ~I2C_Device() {i2c_close(bus);}
 private:
  bool mHibernating;//Whether or not the machine is hibernating
};//I2C_Device
class TemperatureDevice: public I2C_Device
{
 public:
  TemperatureDevice() {set_name("TemperatureDevice");}
  virtual void Run()  {std::cout<<name<<" run"<<std::endl; mHibernating = false;}
  virtual void Stop() {mHibernating = true;}
  //! destructor (need at least empty one)
  virtual ~TemperatureDevice() {}
 private:
  bool mHibernating;//Whether or not the machine is hibernating
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
  FakeDevice fake;        fake.Run();fake.register_list();
  TemperatureDevice temp; temp.Run();
  }//basis
  {//factory
  Device *fake=DeviceFactory::NewDevice("FakeDevice");       if(fake!=NULL) fake->Run();
  Device *temp=DeviceFactory::NewDevice("TemperatureDevice");if(temp!=NULL) temp->Run();
  }//factory
}//device_implementation

#endif //__DEVICE_FACTORY__
