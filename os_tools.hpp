/**
 * \file os-tools.hpp
 * \brief linux OS info
 * \author Sebastien COUDERT
**/

#ifndef _OS_TOOLS_LIB_HPP_
#define _OS_TOOLS_LIB_HPP_

#include <stdlib.h>
#include <string.h>

#include <string>
#include <sstream>  // std::ostringstream
#include <iostream> // std::ios, std::istream, std::cout
#include <fstream>  // std::filebuf
#include <vector>   // std::vector

#define VERSION_OS_TOOLS "v0.0.2d"

///list of I2C buses
int os_pretty_name(std::string &out)
{
  std::string f="/etc/os-release";
  //store
  std::ifstream fb;
  fb.open(f,std::ios::in);
  if(!fb.is_open())
  {
    printf("error: while accessing file: \"%s\"\n",f.c_str());
    out="error: opening os-release file";
    return 1;
  }//error
  out.assign((std::istreambuf_iterator<char>(fb))
            ,(std::istreambuf_iterator<char>(  ))
            );
  fb.close();
/*/debug
std::cout<<"I2C bus(es):"<<std::endl;
std::cout<<"|"<<out<<"|"<<std::endl;
/**/
  return 0;
}//os_pretty_name

#endif //_OS_TOOLS_LIB_HPP_
