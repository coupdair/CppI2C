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

#ifdef USE_I2C_LIB
#warning "use I2C lib"
#include "i2c/i2c.h"
#endif //USE_I2C_LIB

#define REGISTER_VERSION "v0.1.0e"

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
// protected:
 public:
  int bus,addr,id;
  int size;
  I2CDevice *pDevice;
 public:
  I2CRegister() {this->set_name("I2CRegister");size=sizeof(T);}
virtual void print_i2c_data(const unsigned char *data, size_t len)
{
    size_t i = 0;

    for (i = 0; i < len; i++) {

        if (i % 16 == 0) {

            fprintf(stdout, "\n");
        }

        fprintf(stdout, "%02x ", data[i]);
    }

    fprintf(stdout, "\n");
}//print_i2c_data
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
class I2CRegisterWord: public I2CRegister<short>
{
 public:
  I2CRegisterWord() {set_name("I2CRegisterWord");}
  virtual void Run()
  {std::cout<<name<<" run"<<std::endl;
  //libI2C read
    char i2c_dev_desc[128];
///show device desc.
    /* Print i2c device description */
    fprintf(stdout, "%s\n", i2c_get_device_desc(this->pDevice, i2c_dev_desc, sizeof(i2c_dev_desc)));
    fprintf(stdout, "internal register address=0x%02x\n", this->id);
    fprintf(stdout, "reading %d bytes\n", this->size);

    ssize_t ret = 0;
    unsigned char buf[16];
    size_t buf_size = sizeof(buf);

    /* Read */
    memset(buf, 0, buf_size);

///read data
    ret = i2c_read(pDevice, id, buf, this->size);
    if (ret == -1 || (size_t)ret != this->size)
    {
        fprintf(stderr, "Read i2c error!\n");
        exit(-5);
    }
    /* Print read result */
    fprintf(stdout, "Read data:\n");
    print_i2c_data(buf, this->size);
   }//read
  virtual void Stop() {value=123;}
  //! destructor (need at least empty one)
  virtual ~I2CRegisterWord() {}
};//I2CRegisterWord
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
    if(name == "I2CRegisterWord")
      return new I2CRegisterWord;
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
  I2CRegisterWord regW; regW.Run();
  }//basis
  {//factory
  Register *fake=RegisterFactory::NewRegister("FakeRegister");   if(fake!=NULL) fake->Run();
  Register *regB=RegisterFactory::NewRegister("I2CRegisterByte");if(regB!=NULL) regB->Run();
  Register *regW=RegisterFactory::NewRegister("I2CRegisterWord");if(regW!=NULL) regW->Run();
  }//factory
}//register_implementation

#endif //__REGISTER_FACTORY__
