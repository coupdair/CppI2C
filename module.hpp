/**
 * \file module.hpp
 * \brief modules with template and factory
 * \author Sebastien COUDERT
 * \ref https://en.wikibooks.org/wiki/C%2B%2B_Programming/Code/Design_Patterns#Factory
**/

#ifndef __MODULE_FACTORY__
#define __MODULE_FACTORY__

#include <string>         // std::string
#include <iostream>       // std::cout
#include <map>            // std::map

#include "device.hpp"

#define MODULE_VERSION "v0.1.3"

//version
//! module library version
int module_lib_version(std::string &out)
{
  out=MODULE_VERSION;
  return 0;
}//device_lib_version
//! module library pretty version
int module_pretty_version(std::string &out)
{
  out
  = "module."
    MODULE_VERSION;
  return 0;
}//module_pretty_version

class Module: public std::map<std::string,Device*>
{
 protected:
  std::string name;
  virtual void set_name(std::string module_name) {name=module_name; std::cout<<name<<"/"<<__func__<<"(\""<<name<<"\")"<<std::endl;}
 public:
  //! constructor
  Module() {name="none";}
  virtual void Run() = 0;
  virtual void Stop() = 0;
  virtual void create_device(std::string device_name,std::string device_type_name)
  {
    Device *fake=DeviceFactory::NewDevice(device_type_name);
    this->insert(std::pair<std::string,Device*>(device_name,fake));
  }//create_device
  virtual /*std::string*/void device_list(const std::string prefix="") {for (std::map<std::string,Device*>::iterator it=this->begin(); it!=this->end(); ++it) std::cout <<prefix<< it->first << " => " << (it->second)->get_name() << '\n';};
  virtual /*std::string*/void list(const std::string prefix="  ")
  {
std::cout<<name<<"::"<<__func__<<"(\""<<prefix<<"\")"<<std::endl;
    Device* dev;
    for(std::map<std::string,Device*>::iterator it=this->begin(); it!=this->end(); ++it)
    {
      dev=it->second;
      std::cout<<prefix<< it->first << " => " << dev->get_name() << '\n';
      dev->register_list(prefix+prefix);
    }//dev. loop
  };//list
  //! destructor
  virtual ~Module() {};//without this, you do not call Laptop or Desktop destructor in this example!
};//Module
class FakeModule: public Module
{
 public:
  FakeModule()
  {
    set_name("Fake");
    create_device("FakeDeviceA","FakeDevice");
    create_device("FakeDeviceB","FakeDevice");
  }//constructor
  void Run()  {std::cout<<name<<" run"<<std::endl; mHibernating = false;}
  void Stop() {mHibernating = true; }
  virtual ~FakeModule() {};//because we have virtual functions, we need virtual destructor
 private:
  bool mHibernating;//Whether or not the machine is hibernating
};//FakeModule

//! PiPoE module
class PiPoE: public Module
{
 public:
  //! constructor
  //* \note do not instanciate especially v6 */
  PiPoE()
  {
    set_name("PiPoE");
    create_device("TemperatureDeviceA","TemperatureDevice");
  }//constructor
  virtual void Run()  {std::cout<<name<<" run"<<std::endl;mOn = true; }
  void Stop() {mOn = false;}
  virtual ~PiPoE() {}
 private:
  bool mOn;//Whether or not the machine has been turned on
};//PiPoE

//! module factory
/**
 *  \note instanciation based on name (i.e. string, as done only one time)
**/
class ModuleFactory
{
  public:
  static Module *NewModule(const std::string &name)
  {
    if(name == "Fake")
      return new FakeModule;
    if(name == "PiPoE")
      return new PiPoE;
    std::cerr<<"Module name is unknown, i.e. \""<<name<<"\"."<<std::endl;
    std::cerr<<"  should be one of the following: "<<ModuleFactory::List()<<std::endl;
    return NULL;
  }//NewModule
  static std::string List(void)
  {
    std::string list;
    list="Fake, PiPoE";
    list+=".";
    return list;
  }
};//ModuleFactory

void module_implementation()
{
  {//basis
  FakeModule fake; fake.Run();fake.device_list();
  PiPoE pipoe;     pipoe.Run();
  }//basis
  {//factory
  Module* fake=ModuleFactory::NewModule("Fake");  if(fake!=NULL)  fake->list ("  ");
  Module* pipoe=ModuleFactory::NewModule("PiPoE");if(pipoe!=NULL) pipoe->list("  ");
  }//factory
}//module_implementation

#endif //__MODULE_FACTORY__
