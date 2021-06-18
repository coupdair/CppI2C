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
#include <cppcms/url_dispatcher.h>
#include <iostream>
//./build/cppcms/config.h CPPCMS_PACKAGE_VERSION

//Devices (and related)
#include "module.hpp"
///I2C lib.
#include "i2c_tools.hpp"
#include "os_tools.hpp"

//CppCMS data
#include "content.h"

#define VERSION "v0.1.4g"

//Program option/documentation
//{argp
//! [argp] version of program
const char *argp_program_version=VERSION;
//! [argp] author of program
const char *argp_program_bug_address="sebastien.coudert@ganil.fr";
//! [argp] documentation of program
static char doc[] =
   "i2c-web: I2C bus and its devices on HTTP\
\n  i2c-web." VERSION "\
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
    dispatcher().assign("",&http_service::main,this);
    mapper().assign("");

//    dispatcher().assign("/no_template",&http_service::no_template,this);
//    mapper().assign("no_template","/no_template");

    dispatcher().assign("/bus",&http_service::bus,this);
    mapper().assign("bus","/bus");

    dispatcher().assign("/devices",&http_service::devices,this);
    mapper().assign("devices","/devices");

    dispatcher().assign("/setup",&http_service::device_setup,this);
    mapper().assign("setup","/setup");

    dispatcher().assign("/system",&http_service::system,this);
    mapper().assign("system","/system");

    mapper().root("/i2c-bus");
  }//constructor
  void ini(content::master &c)
  {
      c.title = "PiPoE for IC2";
  }//ini
  void main()
  {
    content::master c;
    ini(c);
    render("main",c);
  }//main
  void system()
  {
    content::page c;
    ini(c);
    c.page_title = "system";
    ///date
    //TZ
    booster::locale::generator g;
    std::locale loc=g("en_US.UTF-8");
    std::locale::global(loc);
    std::string tz("GMT");
    booster::locale::time_zone::global(tz);
    //time
    booster::locale::date_time time_point;
    //time_point=booster::locale::period::year(1970) + booster::locale::period::february() + booster::locale::period::day(5);
    time_point=booster::locale::date_time(std::time(NULL));
    //format
    std::ostringstream ss;
    ss.imbue(loc);
    ss<<booster::locale::as::time_zone(tz);
    ss<<booster::locale::as::ftime("%Y-%m-%d %H:%M:%S") << time_point;

    ///versions
    std::string s, vc,vi,vo, vm,vd,vr;
    ///program and lib.
    s=VERSION;
    vc="C++CMS.v" CPPCMS_PACKAGE_VERSION;
    i2c_pretty_version(vi);
    os_pretty_version(vo);
    register_pretty_version<char>(vr);
    device_pretty_version<char>(vd);
    module_pretty_version(vm);
    c.page_content=
      "  <h1>server</h1>\n"
      "  <h2>program</h2>\n"
      "  <pre>\n"
    + s
    + "\n  </pre>\n"
      "  <h2>libraries</h2>\n"
      "  <pre>\n"
    + vc
    + "\n  </pre>\n"
      "  <pre>\n"
    + vi
    + "\n  </pre>\n"
      "  <pre>\n"
    + vo
    + "\n  </pre>\n"
      "  <pre>\n"
    + vr
    + "\n  </pre>\n"
      "  <pre>\n"
    + vd
    + "\n  </pre>\n"
      "  <pre>\n"
    + vm
    + "\n  </pre>\n";
    ///Linux
    os_pretty_name(s);
    c.page_content+=
      "  <h1>Linux OS</h1>\n"
      "  <h2>distribution</h2>\n"
      "  <pre>\n"
    + s
    + "\n  </pre>\n";
    ///time
    c.page_content+=
      "  <hr />\n"
      "  system time:\n"
      "  <pre>\n"
    + ss.str()
    + "\n  </pre>\n";
//    response().out()<<cppcms::filters::datetime(time(0)) << "<br>\n";
    ///duration
    //booster::locale::date_time_duration duration(time_point,booster::locale::date_time(std::time(NULL)));
    booster::locale::date_time time_pt(booster::locale::date_time(std::time(NULL)));
    std::ostringstream si;
    int i=123;
    //i=booster::locale::period::second(duration);
    i=booster::locale::period::second(time_point)-booster::locale::period::second(time_pt);
std::cout<<std::endl<<"dur="<<i<<std::endl<<std::endl;
    if(i==0)  si<<"<1s"; else si<<i<<"s";
    c.page_content+=
      "  system access duration:\n"
      "  <pre>\n"
    + si.str()
    + "\n  </pre>\n";

    render("page",c);
  }//system

  void devices()
  {
    int verbose=1;//[0-2]
    content::devices c;
    ini(c);
    std::string s;
        
    //I2C devices
    ///list of I2C devices
    std::vector<int> device_addresses;
    i2c_device_list(1,device_addresses,s,(verbose>1));

    //I2C table
    c.content_head
    = "  <h1>I2C bus #1</h1>\n"
      "  <h2>as table</h2>\n"
      "  I2C devices table:\n"
      "  <pre>\n"
    + s
    + "  </pre>\n"
    ;

    //devices
    c.content_head
    +="  <h2>as list</h2>\n"
      "\nI2C devices address(es):\n"
    ;
    ///vector of I2C devices
    std::vector<int>::iterator it=device_addresses.begin();
    //output
    if(verbose>0) std::cout<<"\nI2C devices address(es):";
    //format hex
    std::ostringstream hex;
    hex<<"0x";
    hex.setf(std::ios::hex,std::ios::basefield);//set hex as the basefield
    hex.width(2);hex.fill('0');
    hex<<*it;
    //output
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
      c.device_list.push_back(hex.str());
      if(verbose>0) std::cout<<", "<<hex.str();
    }//device loop
    std::cout<<'.'<<std::endl;
    render("devices",c);
  }//devices

  void device_setup()
  {std::cout<<__func__<<" page"<<std::endl;
    int verbose=1;//[0-1]
        content::message c;
        if(request().request_method()=="POST")
        {
            c.infoMC2SA.load(context());
            c.infoTemperature.load(context());
            c.infoApply.load(context());
            //setup reg.
            TemperatureDevice *temperatureDev=(TemperatureDevice *)DeviceFactory::NewDevice("TemperatureDevice"); if(temperatureDev==NULL) exit(-9);
            c.infoTemperature.temperature.value(temperatureDev->get_Celcius());
            //content transfer GUI to core
            if(c.infoMC2SA.validate())
            {	///MC2SA
				std::istringstream(c.infoMC2SA.resolution.selected_id())>>c.resolution_todo;//string>>float
            	if(verbose>0) std::cout<<"MC2SA apply "<<c.value_list()<<std::flush;
            	//setup reg.
                //! \todo set reg.
            }//valid
            //content transfer GUI to core
            if(c.infoTemperature.validate())
            {	///temperature
				c.temperature=c.infoTemperature.temperature.value();
				std::istringstream(c.infoTemperature.resolution.selected_id())>>c.resolution;//string>>float
             	if(verbose>0) std::cout<<"Temperature apply "<<c.value_list()<<std::flush;
            	//setup reg.
            	temperatureDev->set_Celcius(c.resolution);
                Register *reg=(temperatureDev->find("TemperatureResolution"))->second;
                int r=reg->read();
                std::cout<<"resolution mode="<<r<<std::endl;
                /**/
            }//valid
            //c.infoApply.validate();
        }//POST
        render("message",c);
  }//device_setup

  virtual void bus();
  virtual void no_template();
};//http_service

void http_service::bus()
{
//  int verbose=1;//[0-2]
  content::page c;
  ini(c);
  c.page_title = "I2C bus(es)";
  //I2C bus
  std::string s;
  i2c_bus_list(s);
  c.page_content
  = "  <pre>\n"
  + s
  + "  </pre>\n";
  render("page",c);
}//http_service::bus

void http_service::no_template()
{
//  int verbose=1;//[0-2]
//HTML head
  response().out()<<
    "<html>\n"
    "<body>\n";
    
//message
  std::string s("no (content) template");
  response().out()
  <<"  <h1>no template</h1>\n"
    "  <pre>\n"
  <<s
  <<"  </pre>\n";

//HTML tail
  response().out()<<
    "</body>\n"
    "</html>\n";
}//http_service::no_template

//}C++CMS

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
    std::cout<<"os-tools." <<VERSION_OS_TOOLS <<std::endl;
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
