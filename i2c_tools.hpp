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

#define VERSION_I2C_TOOLS "v0.0.6d"

///list of I2C buses
int i2c_bus_list(std::string &out,bool print=false)
{
  //system calls
  std::ostringstream cmd;
  std::string ftb="/dev/shm/i2c_bus_list.txt";
  cmd<<"i2cdetect -l > "<<ftb;
  //error
  if(system(std::string(cmd.str()).c_str())) {printf("error: while accessing I2C buses, see \"%s\"\n",std::string(cmd.str()).c_str());return 1;}
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
  if(!fb.is_open()){printf("error: while accessing file: \"%s\"\n",ftb.c_str());return 1;}
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

//!list of I2C devices
/**
 *  \param [in]  bus_index: I2C bus index, e.g. 1
 *  \param [out] device_addresses: device addresses of selected I2C bus, e.g. 0x18,0x19,0x1F
**/
template<typename T>
int i2c_device_list(const int bus_index, std::vector<T> &device_addresses, const int verbose=0)
{
///table of I2C devices
  //system calls
  std::ostringstream cmd;
  std::string ftd="/dev/shm/i2c_device_list.txt";
  cmd<<"i2cdetect -y "<<bus_index<<" > "<<ftd;
  if(!system(std::string(cmd.str()).c_str()))
  {
    printf("\nI2C device table:\n");fflush(stdout);
    cmd=std::ostringstream();cmd<<"cat "<<ftd;
    system(std::string(cmd.str()).c_str());
  }
  else {printf("error: while accessing I2C bus, see \"%s\"\n",std::string(cmd.str()).c_str());return 1;}

///list of I2C devices
  cmd=std::ostringstream();cmd<<"sed -i \"s/--//g;s/..://\" "<<ftd;
  if(!system(std::string(cmd.str()).c_str()))
  {
    if(verbose) {printf("\nI2C devices:\n");fflush(stdout);}
    std::filebuf fb;
    if(fb.open(ftd,std::ios::in))
    {//get hexa. addresses
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
    }else {printf("error: while accessing file: \"%s\"\n",ftd.c_str());return 1;}
  }
  else {printf("error: while accessing I2C bus, see \"%s\"\n",std::string(cmd.str()).c_str());return 1;}
  return 0;
}//i2c_bus_list

#endif //_I2C_TOOLS_LIB_HPP_
