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
  struct info_form_MC2SA : public cppcms::form
  {
    cppcms::widgets::select resolution;
//    cppcms::widgets::submit submit;
//    cppcms::widgets::select_multiple gain;
    info_form_MC2SA()
    {
		///widget titles
        resolution.message("select todo");resolution.help(" units : help todo");
//        submit.value("apply");
//        gain.message("Your gain(s)");
        ///order widgets
        add(resolution);
//        add(submit);
//        add(gain);
        ///values and behavious
        resolution.add("5","5");//"label", "value"
        resolution.add("10","10");
        resolution.add("15","15");
        resolution.add("20","20");
        resolution.selected_id("5");
/** /
        gain.add("2");
        gain.add("4");
        gain.add("6");
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
  };//info_form_MC2SA
  struct info_form_temperature : public cppcms::form
  {
    cppcms::widgets::numeric<float> temperature;
    cppcms::widgets::select resolution;
    cppcms::widgets::submit submit;
    info_form_temperature()
    {
		///widget titles
 		//! \todo [low] set help messsage as ToolTip (note: <i>italic</i> not working as translated)
		temperature.message("Temperature");temperature.help(" °C : ambiant temperature in degree Celcius");//temperature.error_message("*");
        resolution.message("Resolution");resolution.help(" °C : temperature resolution in degree Celcius (and speed, e.g. 4Hz at 0.0625°C)");
        submit.value("apply");
        ///order widgets
        add(temperature);
        add(resolution);
        add(submit);
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
  struct message : public master
  {
    //data
    ///temperature
    float temperature;
    float resolution;
    ///MC2SA
    float resolution_todo;
    //GUI (HMTL)
    info_form_MC2SA infoMC2SA;
    info_form_temperature infoTemperature;
    //value list
    std::string value_list(void)
    {
      std::ostringstream s;
      s<<"message data:";
      s<<"\n - temperature="<<temperature;
      s<<"\n - resolution=" <<resolution;
      s<<"\n - resolution_todo=" <<resolution_todo;
      s<<std::endl;
      return s.str();
    }//value_list
  };//message
}//content

#endif //CONTENT_H
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
