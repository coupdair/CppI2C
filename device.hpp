/**
 * \file device.hpp
 * \brief devices with template and factory
 * \author Sebastien COUDERT
 * \ref https://en.wikibooks.org/wiki/C%2B%2B_Programming/Code/Design_Patterns#Factory
**/

#ifndef __DEVICE_FACTORY__
#define __DEVICE_FACTORY__

#include <string>   // std::string
#include <iostream> // std::cout
#include <map>      // std::map
#include <unistd.h> // usleep

#include "register.hpp"

#ifdef USE_I2C_LIB
#warning "use I2C lib"
#include "i2c/i2c.h"
#else //USE_I2C_LIB
#ifdef WARNING_NO_I2C_LIB
#undef WARNING_NO_I2C_LIB
#endif //undef
#define WARNING_NO_I2C_LIB std::cerr<<"warning: "<<this->name<<"::"<<__func__<<" empty as no I2C lib. compiled, need to define USE_I2C_LIB or use Fake*."<<std::endl;
#endif // !USE_I2C_LIB

#define DEVICE_VERSION "v0.2.1m"

//version
//! device library version
/**
 * \note template has no code design interest, it is used only for compilation (e.g. no double). 
 **/
template <typename T>
int device_lib_version(std::string &out, T dummy=0)
{
  out=DEVICE_VERSION;
  return 0;
}//device_lib_version
//! device library pretty version
/**
 * \note template has no code design interest, it is used only for compilation (e.g. no double). 
 **/
template <typename T>
int device_pretty_version(std::string &out, T dummy=0)
{
  out
  = "device."
    DEVICE_VERSION;
  return 0;
}//device_pretty_version

class Device: public std::map<std::string,Register*>
{
 protected:
  std::string name;
  int update_time;
  bool debug;
  virtual void set_name(std::string device_name) {name=device_name; if(debug) std::cout<<name<<"/"<<__func__<<"(\""<<name<<"\")"<<std::endl;}
 public:
  //! constructor
  Device() {name="none";update_time=0;debug=false;}
  virtual std::string get_name() {return name;}
  virtual void create_register(std::string register_name,std::string register_type_name)
  {
    Register *reg=RegisterFactory::NewRegister(register_type_name);
    this->insert(std::pair<std::string,Register*>(register_name,reg));
  }//create_register
  virtual /*std::string*/void register_list(const std::string prefix="") {for (std::map<std::string,Register*>::iterator it=this->begin(); it!=this->end(); ++it) std::cout <<prefix<< it->first << " => " << (it->second)->get_name() << '\n';};
  virtual int get(const std::string &register_name) {if(this->debug) std::cout<<name<<"::get(...)"<<std::endl;update_time++;return ((this->find(register_name))->second)->read();};
  virtual int set(const std::string &register_name,const int &value) {if(this->debug) std::cout<<name<<"::set(...)"<<std::endl;update_time++;return ((this->find(register_name))->second)->write(value);};
  //! destructor (need at least empty one)
  virtual int get_update_time() {return update_time;}
  virtual std::string get_update_time_str() {std::ostringstream str;str<<update_time;return str.str();}
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
#ifdef USE_I2C_LIB
  I2CDevice device;
#endif //USE_I2C_LIB
 public:
  I2C_Device()
  {
    set_name("I2C_Device");
    bus=-1;open();
    addr=-1;//init();//to call in daughter class
this->debug=true;
  }//constructor
  //! open I2C bus
  /**
   * \return I2C fd or <0 on error
  **/
  virtual int open(int bus_num=1)
  {//Open i2c bus
    bus_num = 1;
#ifdef USE_I2C_LIB
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
#else //USE_I2C_LIB
    WARNING_NO_I2C_LIB
#endif //!USE_I2C_LIB
    return bus;
  }//open
  //! init. i2c device
  virtual void init(int addr_=0x18)
  {if(this->debug) std::cout<<this->name<<"::"<<__func__<<"("<<addr_<<")"<<std::endl;
	addr=addr_;
#ifdef USE_I2C_LIB
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
#else //USE_I2C_LIB
    WARNING_NO_I2C_LIB
#endif //!USE_I2C_LIB
  }//init
  virtual void create_register(std::string register_name,std::string register_type_name, int id_=0x05)
  {
#ifdef USE_I2C_LIB
    //! \todo [low] check "I2C" in register_type_name
    //Register *i2c_reg=Register::create_register(register_name,register_type_name);
    Register *reg=RegisterFactory::NewRegister(register_type_name);
    this->insert(std::pair<std::string,Register*>(register_name,reg));
    I2CRegisterWord *i2c_reg=(I2CRegisterWord*)reg;
    i2c_reg->bus=bus;
    i2c_reg->addr=addr;
    i2c_reg->id=id_;
    i2c_reg->pDevice=&device;
#else //USE_I2C_LIB
    WARNING_NO_I2C_LIB
#endif //!USE_I2C_LIB
  }//create_register
  virtual int get(const std::string &register_name) {if(this->debug) std::cout<<name<<"::get(...)"<<std::endl;return Device::get(register_name);}
  virtual int set(const std::string &register_name,const int &value) {std::cout<<name<<"::set(...) empty"<<std::endl;return 0;};
  //! destructor (need at least empty one)
  virtual ~I2C_Device()
  {
#ifdef USE_I2C_LIB
    i2c_close(bus);
#else //USE_I2C_LIB
    WARNING_NO_I2C_LIB
#endif //!USE_I2C_LIB
  }
 private:
  bool mHibernating;//Whether or not the machine is hibernating
};//I2C_Device
//! MCP9808
class TemperatureDevice:
#ifdef FAKE_TEMPERATURE
public FakeDevice
#else
public I2C_Device
#endif
{
 private:
  std::string default_register_name;
 public:
  TemperatureDevice()
  {
    default_register_name="AmbiantTemperature";
#ifdef FAKE_TEMPERATURE
    set_name("FakeTemperatureDevice");
//! \todo [low] FakeRegisterWord_RO
    create_register(default_register_name,  "FakeRegister");//RW, but should be RO and Word
    create_register("TemperatureResolution","FakeRegister");//RW
#else
    set_name("TemperatureDevice");
//! \todo [low] static I2C id for TemperatureDevice (to dyn.)
  #ifdef FAKE_MC2SA
    I2C_Device::init(0x19);
  #else
    I2C_Device::init(0x18);
  #endif //!FAKE_MC2SA
    create_register(default_register_name,  "I2CRegisterWord_RO",0x05);//RO
    create_register("TemperatureResolution","I2CRegisterByte",0x08);//RW
//    //create_register in .init()
#endif //!FAKE_TEMPERATURE
  }//constructor
/*
  virtual void init(int addr_=0x19)
  {if(this->debug) std::cout<<this->name<<"::"<<__func__<<"("<<addr_<<")"<<std::endl;
//! \bug [low] adding init in TemperatureDevice do seg. fault ?!
#ifdef FAKE_TEMPERATURE
    //create_register in constructor
#else
    I2C_Device::init(addr_);
    create_register(default_register_name,  "I2CRegisterWord_RO",0x05);//RO
    create_register("TemperatureResolution","I2CRegisterByte",0x08);//RW
#endif
  }//init
*/
  virtual void read()
  {if(this->debug) std::cout<<name<<"::"<<__func__<<"()"<<std::endl;
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
  {if(this->debug) std::cout<<name<<"::"<<__func__<<"() "<<default_register_name<<std::endl;
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
  virtual int set(const std::string &register_name,const int &value)
  {if(this->debug) std::cout<<name<<"::"<<__func__<<"("<<register_name<<", "<<value<<")"<<std::endl;
    return Device::set(register_name,value);
  }//set
  virtual int set(const int &value)
  {if(this->debug) std::cout<<name<<"::"<<__func__<<"("<<value<<") TemperatureResolution, e.g. 0x2"<<std::endl;
    int r=this->set("TemperatureResolution",value);
    //sleep a while for resolution command to perform on device
    usleep(345678);
    return r;
  }//set default
  virtual int set()
  {if(this->debug) std::cout<<name<<"::"<<__func__<<"() TemperatureResolution 0.5Celcius"<<std::endl;
    return this->set(0x0);
  }//set default2
  virtual int set_Celcius(const float &value)
  {
    const std::string register_name("TemperatureResolution");
    if(this->debug) std::cout<<name<<"::"<<__func__<<"("<<value<<") "<<register_name<<", e.g. 0.25"<<std::endl;
    int val=-1; 
    if(value==0.5) val=0x0;
    if(value==0.25) val=0x1;
    if(value==0.125) val=0x2;
    if(value==0.0625) val=0x3;
    if(val<0) {std::cerr<<name<<"::"<<__func__<<"("<<register_name<<", "<<value<<") bad value, should be either 0.5, 0.25, 0.125 or 0.0625 !"<<std::endl;return -3;}
    return this->set(val);
  }//set default Celcius
  //! destructor (need at least empty one)
  virtual ~TemperatureDevice() {}
};//TemperatureDevice

//! MC2SA
/**
 * I2C or fake mode using -DFAKE_MC2SA
**/
class MC2SADevice:
#ifdef FAKE_MC2SA
public FakeDevice
#else
public I2C_Device
#endif
{
 public:
  MC2SADevice()
  {
#ifdef FAKE_MC2SA
    set_name("fakeMC2SADevice");
    create_register("gain","FakeRegister");//RW
    create_register("resistor","FakeRegister");//RW
    create_register("discri","FakeRegister");//RW
    create_register("offset","FakeRegister");//RW
    create_register("amplitude","FakeRegister");//RW
    create_register("testNdiscri","FakeRegister");//RW
    init();
#else
    set_name("MC2SADevice");
    //create_register in .init()
#endif
  }//constructor
  virtual void read()
  {if(this->debug) std::cout<<name<<"::"<<__func__<<"()"<<std::endl;
    this->register_list("");
    for (std::map<std::string,Register*>::iterator it=this->begin(); it!=this->end(); ++it)
    {
      std::cout<<"  "<< it->first << " = 0x"<<std::hex<<(it->second)->read()<<std::dec<<std::endl;
    }//register loop
    this->update_time++;
  }//read
  virtual void read_list(std::vector<int> &reg_values)
  {if(this->debug) std::cout<<name<<"::"<<__func__<<"()"<<std::endl;
    reg_values.clear();
    for (std::map<std::string,Register*>::iterator it=this->begin(); it!=this->end(); ++it)
    {
      reg_values.push_back((it->second)->read());
    }//register loop
    this->update_time++;
  }//read
  //! init. device with its I2C address (and create its registers)
  /**
   * \param [in] addr_ : device address on I2C bus
  **/
  virtual void init(int addr_=0x14)
  {if(this->debug) std::cout<<this->name<<"::"<<__func__<<"("<<addr_<<")"<<std::endl;
//! \bug do not init. "true" register by default, see remove below
#ifdef FAKE_MC2SA
    //create_register in constructor
#else
    I2C_Device::init(addr_);
//! \todo [high] I2CRegisterByte_WO
    create_register("gain",       "I2CRegisterByte_WO",0x1);//WO
    create_register("resistor",   "I2CRegisterByte_WO",0x5);//WO
    create_register("discri",     "I2CRegisterByte_WO",0x4);//WO
    create_register("offset",     "I2CRegisterByte_WO",0x2);//WO
    create_register("amplitude",  "I2CRegisterByte_WO",0x0);//WO
    create_register("testNdiscri","I2CRegisterByte_WO",0x3);//WO
#endif
    //register initialisation
//! \todo [high] put default value to reset function
    (*this)["gain"]->write(0x1);//0x21);//16 and 1 pF
    (*this)["resistor"]->write(0x2);//0x28);//200 and 500 kOhm
    (*this)["discri"]->write(128);//1.65V as byte
    (*this)["offset"]->write(0);//0V
    (*this)["amplitude"]->write(0);//0V
    (*this)["testNdiscri"]->write(0x4);
  }//init
  //! destructor (need at least empty one)
  virtual ~MC2SADevice() {}
};//MC2SADevice

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
    if(name == "MC2SADevice")
      return new MC2SADevice;
    std::cerr<<"Device name is unknown, i.e. \""<<name<<"\"."<<std::endl;
    return NULL;
  }//NewDevice
  static std::string List(void)
  {
    std::string list;
    list="FakeDevice";
    list+=", TemperatureDevice";
    list+=", MC2SADevice";
    list+=".";
    return list;
  }

};//DeviceFactory

//! implementation of devices, i.e. tests
/**
 * \note template has no code design interest, it is used only for compilation (e.g. no double). 
 **/
template <typename T>
void device_implementation(T dummy=0)
{
  {//basis
  FakeDevice fake;        fake.register_list();
  TemperatureDevice temp; temp.register_list();
  MC2SADevice mc2s;       mc2s.register_list();
  }//basis
  {//factory
  Device *fake=DeviceFactory::NewDevice("FakeDevice");       if(fake!=NULL) fake->register_list();
  Device *temp=DeviceFactory::NewDevice("TemperatureDevice");if(temp!=NULL) temp->register_list();
  Device *mc2s=DeviceFactory::NewDevice("MC2SADevice");      if(temp!=NULL) mc2s->register_list();
  }//factory
}//device_implementation

#endif //__DEVICE_FACTORY__
