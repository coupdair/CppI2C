/**
 * \file i2c-tools.hpp
 * \brief i2c info via i2c-tools
 * \author Sebastien COUDERT
**/

#ifndef _I2C_TOOLS_LIB_HPP_
#define _I2C_TOOLS_LIB_HPP_

#include <stdlib.h>
#include <string.h>

#include <string>
#include <sstream>  // std::ostringstream
#include <iostream> // std::ios, std::istream, std::cout
#include <fstream>  // std::filebuf
#include <vector>   // std::vector

#define VERSION_I2C_TOOLS "v0.0.8f"

//! \todo [medium] . setup i2c_bus_list( with ../i2c-tools/tools/i2cbusses.c[gather_i2c_busses(] as ../i2c-tools/tools/i2cdetect.c[print_i2c_busses(] does
//! \todo [medium] setup i2c_device_list( with ../i2c-tools/tools/i2cdetect.c[scan_i2c_bus(]

#ifdef USE_I2C_TOOLS_CODE
//i2c-tools
extern "C" {
#include "../i2c-tools/tools/i2cbusses.c"
}//C

//! i2c-tools//print_i2c_busses
int i2c_bus_list(std::string &out,const bool print=false)
{
  std::ostringstream os;
  struct i2c_adap *adapters;
  int count;
  adapters = gather_i2c_busses();
  if (adapters == NULL)
  {
    fprintf(stderr, "Error: Out of memory!\n");
    return 1;
  }//error
  for (count = 0; adapters[count].name; count++)
  {
    printf("i2c-%d\t%-10s\t%-32s\t%s\n",
    adapters[count].nr, adapters[count].funcs,
    adapters[count].name, adapters[count].algo);
    
    os<<"i2c-"<<adapters[count].nr;
    os<<"\t";os.fill(' ');os.width(10);os<<adapters[count].funcs;
    os<<"\t";os.fill(' ');os.width(32);os<<adapters[count].name;
    os<<"\t"<<adapters[count].algo
      <<"\n";
    
  }//adapter loop
  free_adapters(adapters);
  out=os.str();
  return 0;
}//i2c_bus_list

#else //USE_I2C_TOOLS_CODE

///list of I2C buses
int i2c_bus_list(std::string &out,const bool print=false)
{
  //system calls
  std::ostringstream cmd;
  std::string ftb="/dev/shm/i2c_bus_list.txt";
  cmd<<"i2cdetect -l > "<<ftb;
  //error
  if(system(std::string(cmd.str()).c_str()))
  {
    printf("error: while accessing I2C buses, see \"%s\"\n",std::string(cmd.str()).c_str());
    out="error: i2cdetect list";
    return 1;
  }//error
  //print
  if(print)
  {
    printf("I2C bus(es):\n");fflush(stdout);
    cmd=std::ostringstream();cmd<<"cat "<<ftb;
    system(std::string(cmd.str()).c_str());
  }
  //store
  std::ifstream fb;
  fb.open(ftb,std::ios::in);
  if(!fb.is_open())
  {
    printf("error: while accessing file: \"%s\"\n",ftb.c_str());
    out="error: opening bus list file";
    return 1;
  }//error
  out.assign((std::istreambuf_iterator<char>(fb))
            ,(std::istreambuf_iterator<char>(  ))
            );
  fb.close();
/*/debug
std::cout<<"I2C bus(es):"<<std::endl;
std::cout<<"|"<<out<<"|"<<std::endl;
*/
  return 0;
}//i2c_bus_list

#endif //!USE_I2C_TOOLS_CODE

//!list of I2C devices
/**
 *  \param [in]  bus_index: I2C bus index, e.g. 1
 *  \param [out] device_addresses: device addresses of selected I2C bus, e.g. 0x18,0x19,0x1F
**/
template<typename T>
int i2c_device_list(const int bus_index, std::vector<T> &device_addresses, std::string &out, const int verbose=0, const bool print=false)
{
///table of I2C devices
  //system calls
  std::ostringstream cmd;
  std::string ftd="/dev/shm/i2c_device_list.txt";
  cmd<<"i2cdetect -y "<<bus_index<<" > "<<ftd;
  if(system(std::string(cmd.str()).c_str()))
  {
    printf("error: while accessing I2C bus, see \"%s\"\n",std::string(cmd.str()).c_str());
    out="error: i2cdetect devices";
    return 1;
  }//error
 //print
  if(print)
  {
    printf("\nI2C device table:\n");fflush(stdout);
    cmd=std::ostringstream();cmd<<"cat "<<ftd;
    system(std::string(cmd.str()).c_str());
  }
///get table
  //store
  std::ifstream fi;
  fi.open(ftd,std::ios::in);
  if(!fi.is_open()){printf("error: while accessing file: \"%s\"\n",ftd.c_str());return 1;}
  out.assign((std::istreambuf_iterator<char>(fi))
            ,(std::istreambuf_iterator<char>(  ))
            );
  fi.close();
///list of I2C devices
  cmd=std::ostringstream();cmd<<"sed -i \"s/--//g;s/..://\" "<<ftd;
  if(system(std::string(cmd.str()).c_str()))
  {
    printf("error: while accessing I2C bus, see \"%s\"\n",std::string(cmd.str()).c_str());
    out="error: sed filter devices";
    return 1;
  }//error
  if(verbose) {printf("\nI2C devices:\n");fflush(stdout);}
  std::filebuf fb;
  fb.open(ftd,std::ios::in);
  if(!fb.is_open())
  {
    printf("error: while accessing file: \"%s\"\n",ftd.c_str());
    out="error: opening filtered devices file";
    return 1;
  }//error
  //get hexa. addresses
  std::istream is(&fb);
  is.setf(std::ios::hex,std::ios::basefield);//set hex as the basefield
  //skip first line
  char tmp[256];is.getline(tmp,256);
  //read addresses
  int i,j;
  j=0;
  is>>i;
  std::cout.setf(std::ios::showbase);//activate showbase
  while(!is.eof())
  {
    if(verbose)
    {
      std::cout.setf(std::ios::dec,std::ios::basefield);//set dec as the basefield
      std::cout<<j++<<":";
      std::cout.setf(std::ios::hex,std::ios::basefield);//set hex as the basefield
      std::cout<<"="<<i<<std::endl;
    }//verbose
    device_addresses.push_back(i);
    is>>i;
    if(j>128) break;
  }//loop
  fb.close();
  return 0;
}//i2c_device_list

//version
//! I2C library version
int i2c_lib_version(std::string &out)
{
  out=VERSION_I2C_TOOLS;
  return 0;
}//i2c_lib_version
//! I2C library pretty version
int i2c_pretty_version(std::string &out)
{
  out
  = "i2c-tool-lib."
    VERSION_I2C_TOOLS;
  return 0;
}//i2c_pretty_version

#endif //_I2C_TOOLS_LIB_HPP_
