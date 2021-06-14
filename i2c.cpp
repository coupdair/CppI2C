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

#include "module.hpp"
#include "i2c_tools.hpp"

#define VERSION "v0.0.9g"

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
  printf (".version=%s\n.verbose=%s\n.list=%s\n.integer=%d\n.string=%s\n"
  , p_arguments->version?"yes":"no"
  , p_arguments->verbose?"yes":"no"
  , p_arguments->list?"yes":"no"
  , p_arguments->integer
  , p_arguments->string
  );
}//print_args

//! [argp] setup argp parser
static struct argp argp = { options, parse_option, args_doc, doc };

//}argp

void print_i2c_data(const unsigned char *data, size_t len)
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

//! CLI option parse and ...
int main(int argc, char **argv)
{
  //CLI arguments
  struct arguments arguments;
  arguments.version=0;
  arguments.verbose=0;
  arguments.list=0;
  arguments.integer=1;
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
  
  //
    char i2c_dev_desc[128];
    I2C_READ_HANDLE i2c_read_handle = i2c_read;
    unsigned int addr=0, iaddr=0, num_bytes=0, iaddr_bytes = 1, page_bytes = 16, bus_num = -1;

    /* Open i2c bus */
bus_num = 1;
    int bus;
    char bus_name[32];
    memset(bus_name, 0, sizeof(bus_name));

    if (snprintf(bus_name, sizeof(bus_name), "/dev/i2c-%u", bus_num) < 0) {

        fprintf(stderr, "Format i2c bus name error!\n");
        return -3;
    }//bus_name
///open bus, i.e. fd
    if ((bus = i2c_open(bus_name)) == -1) {

        fprintf(stderr, "Open i2c bus:%s error!\n", bus_name);
        return -3;
    }//open
    
addr=0x18, iaddr=0x05, num_bytes=2;
    /* Init i2c device */
///setup device desc.
    I2CDevice device;
    memset(&device, 0, sizeof(device));
    //default init.
    i2c_init_device(&device);
    //specific init., e.g. from CLI
    device.bus = bus;
    device.addr = addr & 0x3ff;
    device.page_bytes = page_bytes;
    device.iaddr_bytes = iaddr_bytes;

///show device desc.
    /* Print i2c device description */
    fprintf(stdout, "%s\n", i2c_get_device_desc(&device, i2c_dev_desc, sizeof(i2c_dev_desc)));
    fprintf(stdout, "internal register address=0x%02x\n", iaddr);
    fprintf(stdout, "reading %d bytes\n", num_bytes);

    ssize_t ret = 0;
    unsigned char buf[16];
    size_t buf_size = sizeof(buf);

    /* Read */
    memset(buf, 0, buf_size);

///read data
    ret = i2c_read_handle(&device, iaddr, buf, num_bytes);
    if (ret == -1 || (size_t)ret != num_bytes)
    {

        fprintf(stderr, "Read i2c error!\n");
        return -5;
    }

    /* Print read result */
    fprintf(stdout, "Read data:\n");
    print_i2c_data(buf, num_bytes);

    i2c_close(bus);

  std::cout<<"Device   list: "<<DeviceFactory::List()  <<std::endl;
  std::cout<<"Register list: "<<RegisterFactory::List()<<std::endl;

  return 0;
}//main
