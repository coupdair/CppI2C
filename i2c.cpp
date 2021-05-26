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
#include <sstream>

#define VERSION "v0.0.1"

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
  {"verbose",  'v', 0, 0,           "Produce verbose output" },
  {"integer",  'i', "VALUE", 0,     "bus index, e.g. 1" },
  {"string",   's', "STRING", 0,    "get string (unsed)" },
//default options
  { 0 }
};//options (CLI)

//! [argp] Used by main to communicate with parse_option
struct arguments
{
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
    case 'v':
      arguments->verbose=1;
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
  printf (".verbose=%s\n.integer=%d\n.string=%s\n"
  , p_arguments->verbose?"yes":"no"
  , p_arguments->integer
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

  ///print option values
  if(arguments.verbose)
  {
    printf("command line option values:\n");
    print_args(&arguments);
  }//print default option values

//system calls
  std::ostringstream cmd;
///list I2C buses
  std::string ftb="/dev/shm/i2c_bus_list.txt";
  cmd<<"i2cdetect -l > "<<ftb;
  if(!system(std::string(cmd.str()).c_str()))
  {
    printf("I2C bus(es):\n");
    cmd=std::ostringstream();cmd<<"cat "<<ftb;
    system(std::string(cmd.str()).c_str());
  }
  else {printf("error: while accessing I2C buses, see \"%s\"\n",std::string(cmd.str()).c_str());return 1;}
///list I2C devices
  std::string ftd="/dev/shm/i2c_device_list.txt";
  cmd=std::ostringstream();cmd<<"i2cdetect -y "<<arguments.integer<<" > "<<ftd;
  if(!system(std::string(cmd.str()).c_str()))
  {
    printf("\nI2C devices:\n");
    cmd=std::ostringstream();cmd<<"cat "<<ftd;
    system(std::string(cmd.str()).c_str());
  }
  else {printf("error: while accessing I2C bus, see \"%s\"\n",std::string(cmd.str()).c_str());return 1;}

  return 0;
}//main
