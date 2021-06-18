/**
 * \file i2c.cpp
 * \brief i2c info
 * \author Sebastien COUDERT
**/

//ArgP
#include <error.h>
#include <argp.h>
#include <stdlib.h>

#include <string.h>
#include <string>
#include <sstream>  // std::ostringstream
#include <iostream> // std::ios, std::istream, std::cout
#include <fstream>  // std::filebuf
#include <vector>   // std::vector
#include <map>      // std::map

#include <unistd.h> //usleep
#include "module.hpp"
#include "i2c_tools.hpp"

#define VERSION "v0.1.2f"

//Program option/documentation
//{argp
//! [argp] version of program
const char *argp_program_version=VERSION;
//! [argp] author of program
const char *argp_program_bug_address="sebastien.coudert@ganil.fr";
//! [argp] documentation of program
static char doc[]=
   "i2c: I2C bus and its devices\
\n  i2c."VERSION"\
\n\
examples:\n\
  i2c --help\n\
  i2c -v\n\
  i2c -v -i 1 -s XYZ\n\
  i2c -V\n\
  i2c --usage";

//! [argp] A description of the arguments
static char args_doc[] = "";

//! [argp] The options and its description
static struct argp_option options[]=
{
  {"version",  'V', 0, 0,           "Produce version output and exit" },
  {"verbose",  'v', 0, 0,           "Produce verbose output" },
  {"list",     'l', 0, 0,           "Produce factory lists" },
  {"integer",  'i', "VALUE", 0,     "bus index, e.g. 1" },
  {"resolution", 'r', "VALUE", 0,   "temperature resolution, e.g. 0x0=0.5°, 0x1=0.25°, 0x2=0.125°, 0x3=0.0625°" },
  {"resolutionCelcius", 'c', "VALUE", 0,   "temperature resolution, e.g. 0x0=0.5°, 0x1=0.25°, 0x2=0.125°, 0x3=0.0625°" },
  {"string",   's', "STRING", 0,    "get string (unsed)" },
//default options
  { 0 }
};//options (CLI)

//! [argp] Used by main to communicate with parse_option
struct arguments
{
  //! display version (boolean)
  int version;
  //! verbose mode (boolean)
  int verbose;
  //! factory list
  int list;
  //! integer value
  int integer;
  //! resolution of temperature mesurment
  int resolution;
  //! resolution of temperature mesurment in Celcius
  float resolutionCelcius;
  //! string value
  char* string;
};//arguments (CLI)

//! [argp] Parse a single option
static error_t
parse_option(int key, char *arg, struct argp_state *state)
{
  //Get the input argument from argp_parse
  struct arguments *arguments=(struct arguments *)(state->input);
  switch (key)
  {
    case 'V':
      arguments->version=1;
      break;
    case 'v':
      arguments->verbose=1;
      break;
    case 'l':
      arguments->list=1;
      break;
    case 'i':
      arguments->integer=atoi(arg);
      break;
    case 'r':
      arguments->resolution=atoi(arg);
      break;
    case 'c':
      arguments->resolutionCelcius=atof(arg);
      break;
    case 's':
      arguments->string=arg;
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }//switch
  return 0;
}//parse_option


//! [argp] print argument values
void print_args(struct arguments *p_arguments)
{
  printf (".version=%s\n.verbose=%s\n.list=%s\n.bus_index=%d\n.temperature_resolution=%d\n.temperature_resolutionCelcius=%f\n.string=%s\n"
  , p_arguments->version?"yes":"no"
  , p_arguments->verbose?"yes":"no"
  , p_arguments->list?"yes":"no"
  , p_arguments->integer
  , p_arguments->resolution
  , p_arguments->resolutionCelcius
  , p_arguments->string
  );
}//print_args

//! [argp] setup argp parser
static struct argp argp = { options, parse_option, args_doc, doc };

//}argp

//! CLI option parse and ...
int main(int argc, char **argv)
{
  //CLI arguments
  struct arguments arguments;
  arguments.version=0;
  arguments.verbose=0;
  arguments.list=0;
  arguments.integer=1;
  arguments.resolution=0x1;//0.25°
  arguments.resolutionCelcius=0.125;//0x2
  arguments.string="ABC";

//! - print default option values (static)
  if(0)//0 or 1
  {
    printf("default values:\n");
    print_args(&arguments);
  }//print default option values

//! - Parse arguments (see parse_option) and eventually show usage, help or version and exit for these lasts
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  if(arguments.version)
  {
    std::cout<<VERSION<<std::endl;
    std::cout<<"i2c-tools."<<VERSION_I2C_TOOLS<<std::endl;
    std::cout<<"register."<<REGISTER_VERSION<<std::endl;
    std::cout<<"device."<<DEVICE_VERSION<<std::endl;
    std::cout<<"module."<<MODULE_VERSION<<std::endl;
    return 0;
  }//version

  ///print option values
  if(arguments.verbose)
  {
    printf("command line option values:\n");
    print_args(&arguments);
  }//print default option values
  
  if(false)//implementation
  {
    std::cout<<"## implementation ##"<<std::endl;
    register_implementation();
    device_implementation();
//    module_implementation();
  }//implementation

  ///show factory lists
  if(arguments.list)
  {
//    std::cout<<"Module   list: "<<ModuleFactory::List()  <<std::endl;
    std::cout<<"Device   list: "<<DeviceFactory::List()  <<std::endl;
    std::cout<<"Register list: "<<RegisterFactory::List()<<std::endl;
    return 0;
  }

///list of I2C buses
  std::string s;
  i2c_bus_list(s);
  std::cout<<"I2C bus(es):"<<std::endl;
  std::cout<<s;
///list of I2C devices
  std::vector<int> device_addresses;
  i2c_device_list(arguments.integer,device_addresses,s,arguments.verbose);
  std::cout<<"\nI2C devices address table:"<<std::endl;
  std::cout<<s;

///vector of I2C devices
  std::cout<<"\nI2C devices address(es):";
  std::cout.setf(std::ios::showbase);//activate showbase
  std::cout.setf(std::ios::hex,std::ios::basefield);//set hex as the basefield
  std::vector<int>::iterator it=device_addresses.begin();std::cout<<"  "<<*it;
  for(++it;it!=device_addresses.end();++it)              std::cout<<", "<<*it;
  std::cout<<'.'<<std::endl;

///read temperature
  const std::string type_name="TemperatureDevice";
  std::cout<<std::endl<<"DeviceFactory:"<<type_name<<std::endl;
  TemperatureDevice *temp=(TemperatureDevice *)DeviceFactory::NewDevice(type_name); if(temp==NULL) return -9;
  temp->register_list("  ");
//Write resolution
  std::string name="TemperatureResolution";
  {//Device
//  std::cout<<std::endl<<"TemperatureDevice::set("<<arguments.resolution<<")"<<std::endl;
//  temp->set(arguments.resolution);
  std::cout<<std::endl<<"TemperatureDevice::set_Celcius("<<arguments.resolutionCelcius<<")"<<std::endl;
  temp->set_Celcius(arguments.resolutionCelcius);
  Register *reg=(temp->find(name))->second;
  int r=reg->read();
  std::cout<<"resolution mode="<<r<<std::endl;
  }//Device
/** /
  {//Register
  std::cout<<std::endl<<"Device["<<name<<"]"<<std::endl;
  Register *reg=(temp->find(name))->second;
  int r=reg->write(arguments.resolution);
  std::cout<<"write return "<<r<<std::endl;
  r=reg->read();
  std::cout<<"resolution mode="<<r<<std::endl;
  }//Register
/**/

//Read temperature
  name="AmbiantTemperature";
  {//Device
  std::cout<<std::endl<<"Device::get("<<name<<")"<<std::endl;
  short t=temp->get(name);
  std::cout<<"t="<<t<<std::endl;
  }//Device
  {//Celcius
  std::cout<<std::endl<<"TemperatureDevice::get_Celcius("<<name<<")"<<std::endl;
  float t=temp->get_Celcius();
  std::cout<<"t="<<t<<" Celcius"<<std::endl;
  }//Celcius
/** /
  {//Register
  std::cout<<std::endl<<"Device["<<name<<"]"<<std::endl;
  Register *reg=(temp->find(name))->second;
  short t=reg->read();
  std::cout<<"t="<<t<<std::endl;
  }//Register
  {//reg.
  std::cout<<std::endl<<"Device["<<name<<"]"<<std::endl;
  short t=((temp->find(name))->second)->read();
  std::cout<<"t="<<t<<std::endl;
  }//reg.
/**/

  return 0;
}//main
