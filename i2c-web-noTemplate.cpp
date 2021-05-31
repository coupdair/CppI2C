/**
 * \file i2c-web.cpp
 * \brief i2c info on HTTP
 * \author Sebastien COUDERT
**/

//ArgP
#include <error.h>
#include <argp.h>
#include <stdlib.h>

//STL
#include <string.h>
#include <string>
#include <sstream>  // std::ostringstream
#include <iostream> // std::ios, std::istream, std::cout
#include <fstream>  // std::filebuf
#include <vector>   // std::vector

//CppCMS
#include <cppcms/application.h>
#include <cppcms/applications_pool.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <iostream>

//I2C lib.
#include "i2c_tools.hpp"
#include "os_tools.hpp"

#define VERSION "v0.0.8d"

//Program option/documentation
//{argp
//! [argp] version of program
const char *argp_program_version=VERSION;
//! [argp] author of program
const char *argp_program_bug_address="sebastien.coudert@ganil.fr";
//! [argp] documentation of program
static char doc[]=
   "i2c-web: I2C bus and its devices on HTTP\
\n  i2c-web."VERSION"\
\n\
examples:\n\
  i2c-web --help\n\
  i2c-web -v\n\
  i2c-web -v -i 1 -s XYZ\n\
  i2c-web -V\n\
  i2c-web --usage";

//! [argp] A description of the arguments
static char args_doc[] = "";

//! [argp] The options and its description
static struct argp_option options[]=
{
  {"version",  'V', 0, 0,           "Produce version output and exit" },
  {"verbose",  'v', 0, 0,           "Produce verbose output" },
  {"integer",  'i', "VALUE", 0,     "bus index, e.g. 1" },
  {"string",   's', "STRING", 0,    "get string (unsed)" },
  {"config",   'c', "STRING", 0,    "configuration file in JSON for C++CMS web server, e.g. config.js" },
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
    case 'i':
      arguments->integer=atoi(arg);
      break;
    case 's':
      arguments->string=arg;
      break;
    case 'c': //JSON configuration file for C++CMS web server
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }//switch
  return 0;
}//parse_option


//! [argp] print argument values
void print_args(struct arguments *p_arguments)
{
  printf (".version=%s\n.verbose=%s\n.integer=%d\n.string=%s\n"
  , p_arguments->version?"yes":"no"
  , p_arguments->verbose?"yes":"no"
  , p_arguments->integer
  , p_arguments->string
  );
}//print_args

//! [argp] setup argp parser
static struct argp argp = { options, parse_option, args_doc, doc };

//}argp

//HTTP service using C++CMS
class http_service: public cppcms::application {
public:
    http_service(cppcms::service &srv) :
        cppcms::application(srv) 
    {
    }//constructor
    virtual void main(std::string url);
};//http_service

void http_service::main(std::string /*url*/)
{
  int verbose=1;//[0-2]
//HTML head
  response().out()<<
    "<html>\n"
    "<body>\n";
//I2C bus
  std::string s;
  i2c_bus_list(s);
  response().out()
  <<"  <h1>I2C bus(es)</h1>\n"
    "  <pre>\n"
  <<s
  <<"  </pre>\n";

//I2C devices
  ///list of I2C devices
  std::vector<int> device_addresses;
  i2c_device_list(1,device_addresses,s,(verbose>1));

//I2C table
  response().out()
  <<"  <h1>I2C bus #1</h1>\n"
    "  <h2>as table</h2>\n"
    "  I2C devices table:\n"
    "  <pre>\n"
  <<s
  <<"  </pre>\n";

//devices
  response().out()<<
    "  <h2>as list</h2>\n";
  ///vector of I2C devices
  std::vector<int>::iterator it=device_addresses.begin();
  //output
  response().out()
  <<"\nI2C devices address(es):\n"
    "  <pre>\n";
  if(verbose>0) std::cout<<"\nI2C devices address(es):";
  //format hex
  std::ostringstream hex;
  hex<<"0x";
  hex.setf(std::ios::hex,std::ios::basefield);//set hex as the basefield
  hex.width(2);hex.fill('0');
  hex<<*it;
  //output
  response().out()       <<hex.str();
  if(verbose>0) std::cout<<hex.str();
  for(++it;it!=device_addresses.end();++it)
  {
    //format hex
    hex=std::ostringstream();
    hex<<"0x";
    hex.setf(std::ios::hex,std::ios::basefield);//set hex as the basefield
    hex.width(2);hex.fill('0');
    hex<<*it;
    //output
    response().out()       <<", "<<hex.str();
    if(verbose>0) std::cout<<", "<<hex.str();
  }//device loop
  std::cout<<'.'<<std::endl;
  response().out()
  <<".\n"
    "  </pre>\n";

//Linux
  os_pretty_name(s);
  response().out()
  <<"  <h1>Linux OS</h1>\n"
    "  <h2>distribution</h2>\n"
    "  <pre>\n"
  <<s
  <<"  </pre>\n";

//HTML tail
  response().out()<<
    "</body>\n"
    "</html>\n";
}//http_service::main

//}C++CMS

//! main function
int main(int argc,char ** argv)
{
    //CLI arguments
  struct arguments arguments;
  arguments.version=0;
  arguments.verbose=0;
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
    std::cout<<"os-tools." <<VERSION_OS_TOOLS <<std::endl;
    return 0;
  }//version

  ///print option values
  if(arguments.verbose)
  {
    printf("command line option values:\n");
    print_args(&arguments);
  }//print default option values

  //web service
  std::cout<<argv[0]<<" is running (Ctrl+C to stop), have look to http://localhost:8080/i2c-bus."<<std::endl;
    try {
        cppcms::service srv(argc,argv);
        srv.applications_pool().mount(cppcms::applications_factory<http_service>());
        srv.run();
    }
    catch(std::exception const &e) {
        std::cerr<<e.what()<<std::endl;
    }
}//main
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
