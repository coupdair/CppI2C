#ifndef CONTENT_H
#define CONTENT_H

#include <cppcms/view.h>
#include <string>
#include "device.hpp"

namespace content
{
  struct master: public cppcms::base_content
  {
    std::string title;
  };//master
  struct page: public master
  {
    std::string page_title, page_content;
  };//page
  struct devices: public master
  {
    std::string content_head;
    std::list<std::string> device_list;
  };//devices

  //forms
  struct info_form_MC2SA_gain : public cppcms::form
  {
//    cppcms::widgets::select resolution;
//    cppcms::widgets::submit submit;
    cppcms::widgets::checkbox gain1;
    cppcms::widgets::checkbox gain2;
    cppcms::widgets::checkbox gain4;
    cppcms::widgets::checkbox gain8;
    cppcms::widgets::checkbox gain11;
    cppcms::widgets::checkbox gain16;
    info_form_MC2SA_gain()
    {
		///widget titles
//        resolution.message("select todo");resolution.help(" units : help todo");
//        submit.value("apply");
//        gain.message("capacitors for gain");
        gain1.message(""); gain1.name("gain1");  gain1.help("1 pF");
        gain2.message(""); gain2.name("gain2");  gain2.help("2 pF");
        gain4.message(""); gain4.name("gain4");  gain4.help("4 pF");
        gain8.message(""); gain8.name("gain8");  gain8.help("8 pF");
        gain11.message("");gain11.name("gain11");gain11.help("11 pF");
        gain16.message("");gain16.name("gain16");gain16.help("16 pF");
        ///order widgets
//        add(resolution);
//        add(submit);
//        add(gain);
        add(gain1);
        add(gain2);
        add(gain4);
        add(gain8);
        add(gain11);
        add(gain16);
        ///values and behavious
/** /
        resolution.add("5","5");//"label", "value"
        resolution.add("10","10");
        resolution.add("15","15");
        resolution.add("20","20");
        resolution.selected_id("5");
/**/
//        name.non_empty();
//        age.range(0,123);
    }//constructor
    virtual bool validate()
    {
        if(!form::validate()) 
            return false;
/*
        if(resolution.selected_id()!="single")
        {
            marital.valid(false);
            return false;
        }
*/
        return true;
    }//validate
  };//info_form_MC2SA_gain
  struct info_form_MC2SA_resistor : public cppcms::form
  {
    cppcms::widgets::checkbox resistor2;
    cppcms::widgets::checkbox resistor3;
    cppcms::widgets::checkbox resistor5;
    info_form_MC2SA_resistor()
    {
		///widget titles
        resistor2.message("");resistor2.name("resistor2");resistor2.help("200k Ohm");
        resistor3.message("");resistor3.name("resistor3");resistor3.help("300k Ohm");
        resistor5.message("");resistor5.name("resistor5");resistor5.help("500k Ohm");
        ///order widgets
        add(resistor2);
        add(resistor3);
        add(resistor5);
        ///values and behavious
    }//constructor
    virtual bool validate()
    {
        if(!form::validate()) 
            return false;
        return true;
    }//validate
  };//info_form_MC2SA_resistor
  struct info_form_MC2SA_level : public cppcms::form
  {
    cppcms::widgets::numeric<float> level;
//    cppcms::widgets::range level;
    info_form_MC2SA_level()
    {
		///widget titles
        level.message("discri level");level.name("level_amplitude");level.help(" V : range [0 .. 3.3]V");
        ///order widgets
        add(level);
        ///values and behavious
        level.non_empty();
        level.range(0.0,3.3);
    }//constructor
    virtual bool validate()
    {
        if(!form::validate()) 
            return false;
        return true;
    }//validate
  };//info_form_MC2SA_level
  struct info_form_temperature : public cppcms::form
  {
    cppcms::widgets::numeric<float> temperature;
    cppcms::widgets::select resolution;
    info_form_temperature()
    {
		///widget titles
 		//! \todo [low] set help messsage as ToolTip (note: <i>italic</i> not working as translated)
		temperature.message("Temperature");temperature.name("temperature");temperature.help(" °C : ambiant temperature in degree Celcius");//temperature.error_message("*");
        resolution.message("Resolution");resolution.name("temperature_resolution");resolution.help(" °C : temperature resolution in degree Celcius (and speed, e.g. 4Hz at 0.0625°C)");
        ///order widgets
        add(temperature);
        add(resolution);
        ///values and behavious
        TemperatureDevice *temperatureDev=(TemperatureDevice *)DeviceFactory::NewDevice("TemperatureDevice"); if(temperatureDev==NULL) exit(-9);
        temperature.value(temperatureDev->get_Celcius());
        temperature.readonly(true);temperature.disabled(true);	
        resolution.add("0.5","0.5");//"label", "value"
        resolution.add("0.25","0.25");
        resolution.add("0.125","0.125");
        resolution.add("0.0625","0.0625");
        resolution.selected_id("0.0625");
    }//constructor
    virtual bool validate()
    {
        if(!form::validate()) 
            return false;
        return true;
    }//validate
  };//info_form_temperature
  struct info_form_apply : public cppcms::form
  {
    cppcms::widgets::submit submit;
    info_form_apply()
    {
		///widget titles
        submit.value("apply");submit.name("apply_all");
        ///order widgets
        add(submit);
    }//constructor
    virtual bool validate()
    {
        if(!form::validate()) 
            return false;
        return true;
    }//validate
  };//info_form_apply
  struct message : public master
  {
    //data
    ///temperature
    float temperature;
    float resolution;
    ///MC2SA
    char gain;
    char resistor;
    int  discri;
    //GUI (HMTL)
    info_form_MC2SA_gain infoMC2SAgain;
    info_form_MC2SA_resistor infoMC2SAresistor;
    info_form_MC2SA_level infoMC2SAdiscri;
    info_form_temperature infoTemperature;
    info_form_apply infoApply;
    //value list
    std::string value_list(void)
    {
      std::ostringstream s;
      s<<"message data:";
      s<<"\n - temperature="<<temperature;
      s<<"\n - resolution=" <<resolution;
      std::bitset<6> bs(gain);
      s<<"\n - gain="<<bs.to_ulong()<<", i.e. "<<bs.to_string();
      bs=resistor;
      s<<"\n - resistor="<<bs.to_ulong()<<", i.e. "<<bs.to_string();
      s<<"\n - discri="<<discri;
      s<<std::endl;
      return s.str();
    }//value_list
  };//message
}//content

#endif //CONTENT_H
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
