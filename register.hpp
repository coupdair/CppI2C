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

#define REGISTER_VERSION "v0.1.4f"

//version
//! register library version
/**
 * \note template has no code design interest, it is used only for compilation (e.g. no double). 
 **/
template <typename T>
int register_lib_version(std::string &out, T dummy=0)
{
  out=REGISTER_VERSION;
  return 0;
}//register_lib_version
//! register library pretty version
/**
 * \note template has no code design interest, it is used only for compilation (e.g. no double). 
 **/
template <typename T>
int register_pretty_version(std::string &out, T dummy=0)
{
  out
  = "register."
    REGISTER_VERSION;
  return 0;
}//register_pretty_version

class Register
{
 protected:
  std::string name;
  bool debug;
  virtual void set_name(std::string register_name) {name=register_name; if(debug) std::cout<<name<<"::"<<__func__<<"(\""<<name<<"\")"<<std::endl;}
 public:
  virtual std::string get_name() {return name;}
  //! constructor
  Register() {name="none";debug=false;}
  virtual int read() = 0;
  virtual int get() = 0;
  virtual int write() = 0;
  virtual int write(const int &value) = 0;
  virtual int set() = 0;
  //! destructor (need at least empty one)
  virtual ~Register() {}
};//Register
template <typename T>
class RegisterT: public Register
{
 protected:
  T value;
 public:
  RegisterT() {set_name("RegisterT");value=(T)1.234;}
  virtual int get() {return value;};
  virtual int set() {std::cout<<this->name<<"::"<<__func__<<"() empty"<<std::endl;return -1;};
  virtual int set(T value_) {value=value_;return 0;};
  virtual int write() = 0;
  virtual int write(const int &val)
  {if(this->debug) std::cout<<this->name<<"::"<<__func__<<"("<<val<<")"<<std::endl;
    value=(T)val;
    return write();
  }
  //! destructor (need at least empty one)
  virtual ~RegisterT() {}
};//RegisterT
class FakeRegister: public RegisterT<char>
{
 public:
  FakeRegister() {set_name("FakeRegister");}
  virtual int read()  {return this->get();}
  virtual int write(const int &val){return RegisterT<char>::write(val);};
  virtual int write() {return 0;}
  //! destructor (need at least empty one)
  virtual ~FakeRegister() {}
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
  virtual int read()
  {if(this->debug) std::cout<<this->name<<"::"<<__func__<<"()"<<std::endl;
  //libI2C read
    char i2c_dev_desc[128];
    this->value=99;//dummy
///show device desc.
    /* Print i2c device description */
    if(this->debug)
    {
      fprintf(stdout, "%s\n", i2c_get_device_desc(this->pDevice, i2c_dev_desc, sizeof(i2c_dev_desc)));
      fprintf(stdout, "internal register address=0x%02x\n", this->id);
      fprintf(stdout, "reading %d bytes\n", this->size);
    }//debug
    ssize_t ret = 0;
    unsigned char buf[16];
    size_t buf_size = sizeof(buf);

    /* Read */
    memset(buf, 0, buf_size);

///read data
    ret = i2c_read(pDevice, id, buf, this->size);
    if (ret == -1 || ret != this->size)
    {
        fprintf(stderr, "Read i2c error!\n");
        return -5;
    }
    /* Print read result */
    if(this->debug)
    {
	  fprintf(stdout, "Read data:\n");
      print_i2c_data(buf, this->size);
    }//debug
    if(size==1) {this->value=(int)buf[0];}
    if(size==2) {short val=(short)buf[0]*256;val+=buf[1];this->value=(int)val;}
    return this->value;
   }//read
  virtual int write()
  {if(this->debug) std::cout<<this->name<<"::"<<__func__<<"() value="<<this->value<<std::endl;
    //libI2C write
    char i2c_dev_desc[128];
///show device desc.
    /* Print i2c device description */
    if(this->debug)
    {
      fprintf(stdout, "%s\n", i2c_get_device_desc(this->pDevice, i2c_dev_desc, sizeof(i2c_dev_desc)));
      fprintf(stdout, "internal register address=0x%02x\n", this->id);
      fprintf(stdout, "writing %d bytes\n", this->size);
    }//debug
    ssize_t ret = 0;
    unsigned char buf[256];
    size_t buf_size = sizeof(buf);
    memset(buf, 0, buf_size);
///fill data
    buf[0]=this->value;
    /* Print before write */
    if(this->debug)
    {
      fprintf(stdout, "Write data:\n");
      print_i2c_data(buf, this->size);
    }//debug
///write data
    ret = i2c_write(this->pDevice, this->id, buf, this->size);
    if(ret != this->size)
    {
        fprintf(stderr, "Write i2c error!\n");
        return -4;
    }
    return 0;
  }//write
  //! destructor (need at least empty one)
  virtual ~I2CRegister() {}
};//I2CRegister
class I2CRegisterByte: public I2CRegister<char>
{
 public:
  I2CRegisterByte() {set_name("I2CRegisterByte");}
  //! destructor (need at least empty one)
  virtual ~I2CRegisterByte() {}
};//I2CRegisterByte
class I2CRegisterWord: public I2CRegister<short>
{
 public:
  I2CRegisterWord() {set_name("I2CRegisterWord");}
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
    list+=", I2CRegisterWord";
    list+=".";
    return list;
  }
};//RegisterFactory

//! implementation of registers, i.e. tests
/**
 * \note template has no code design interest, it is used only for compilation (e.g. no double). 
 **/
template <typename T>
void register_implementation(T dummy=0)
{
  {//basis
  FakeRegister fake;    fake.read();
  I2CRegisterByte regB; regB.read();
  I2CRegisterWord regW; regW.read();
  }//basis
  {//factory
  Register *fake=RegisterFactory::NewRegister("FakeRegister");   if(fake!=NULL) fake->read();
  Register *regB=RegisterFactory::NewRegister("I2CRegisterByte");if(regB!=NULL) regB->read();
  Register *regW=RegisterFactory::NewRegister("I2CRegisterWord");if(regW!=NULL) regW->read();
  }//factory
}//register_implementation

#endif //__REGISTER_FACTORY__
