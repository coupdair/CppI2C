/**
 * \file register.hpp
 * \brief registers with template and factory
 * \author Sebastien COUDERT
 * \ref https://en.wikibooks.org/wiki/C%2B%2B_Programming/Code/Design_Patterns#Factory
**/

#ifndef __REGISTER_FACTORY__
#define __REGISTER_FACTORY__

#include <string>         // std::string
#include <iostream>       // std::cout
#include <map>            // std::map

#define REGISTER_VERSION "v0.0.9"

class Register
{
 protected:
  std::string name;
  virtual void set_name(std::string device_name) {name=device_name; std::cout<<name<<"/"<<__func__<<"(\""<<name<<"\")"<<std::endl;}
 public:
  virtual std::string get_name() {return name;}
  //! constructor
  Register() {name="none";}
  virtual void Run() = 0;
  virtual void Stop() = 0;
  //! destructor (need at least empty one)
  virtual ~Register() {}
};//Register
template <typename T>
class RegisterT: public Register
{
 protected:
  T value;
 public:
  RegisterT() {set_name("RegisterT");value=(T)12.34;}
  virtual void set_value(T value_) {value=value_;};
  virtual void Run()  {std::cout<<name<<" run"<<std::endl;}
  //! destructor (need at least empty one)
  virtual ~RegisterT() {}
};//RegisterT
class FakeRegister: public RegisterT<char>
{
 public:
  FakeRegister() {set_name("FakeRegister");}
  virtual void Run()  {std::cout<<name<<" run"<<std::endl;set_value(1);}
  virtual void Stop() {mHibernating = true;}
  //! destructor (need at least empty one)
  virtual ~FakeRegister() {}
 private:
  bool mHibernating;//Whether or not the machine is hibernating
};//FakeRegister
template <typename T>
class I2CRegister: public RegisterT<T>
{
  int bus,addr,id;
  int size;
 public:
  I2CRegister() {this->set_name("I2CRegister");size=sizeof(T);}
  virtual void Run()  {std::cout<<this->name<<" run"<<std::endl; this->set_value((T)32.1); mHibernating = false;}
  virtual void Stop() {mHibernating = true;}
  //! destructor (need at least empty one)
  virtual ~I2CRegister() {}
 private:
  bool mHibernating;//Whether or not the machine is hibernating
};//I2CRegister
class I2CRegisterByte: public I2CRegister<char>
{
 public:
  I2CRegisterByte() {set_name("I2CRegisterByte");}
  virtual void Run()  {std::cout<<name<<" run"<<std::endl;}
  virtual void Stop() {value=123;}
  //! destructor (need at least empty one)
  virtual ~I2CRegisterByte() {}
};//I2CRegisterByte
//! device factory
/**
 *  \note instanciation based on name (i.e. string, as done only one time)
**/
class RegisterFactory
{
  public:
  static Register *NewRegister(const std::string &name)
  {
    if(name == "FakeRegister")
      return new FakeRegister;
    if(name == "I2CRegisterByte")
      return new I2CRegisterByte;
    std::cerr<<"Register name is unknown, i.e. \""<<name<<"\"."<<std::endl;
    return NULL;
  }//NewRegister
  static std::string List(void)
  {
    std::string list;
    list="FakeRegister";
    list+=", I2CRegisterByte";
    list+=".";
    return list;
  }
};//RegisterFactory

void register_implementation()
{
  {//basis
  FakeRegister fake;    fake.Run();
  I2CRegisterByte regB; regB.Run();
  }//basis
  {//factory
  Register *fake=RegisterFactory::NewRegister("FakeRegister");   if(fake!=NULL) fake->Run();
  Register *regB=RegisterFactory::NewRegister("I2CRegisterByte");if(regB!=NULL) regB->Run();
  }//factory
}//register_implementation

#endif //__REGISTER_FACTORY__
