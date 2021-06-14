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

#define DEVICE_VERSION "v0.0.9"

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
class TemperatureDevice: public Device
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
