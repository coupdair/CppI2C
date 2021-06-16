#ifndef CONTENT_H
#define CONTENT_H

#include <cppcms/view.h>
#include <string>

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

  //form
  struct info_form : public cppcms::form
  {
    cppcms::widgets::numeric<float> temperature;
    cppcms::widgets::select resolution;
    cppcms::widgets::submit submit;
//    cppcms::widgets::select_multiple gain;
    info_form()
    {
		///widget titles
		//! \todo [medium] _ add read reg.
/*
        TemperatureDevice *temperatureDev=(TemperatureDevice *)DeviceFactory::NewDevice("TemperatureDevice"); if(temperatureDev==NULL) exit(-9);
        short t=temperatureDev->get_Celcius();
        temperature.value(t);
*/
        temperature.value(23.45);
 		//! \todo [low] set help messsage as ToolTip (note: <i>italic</i> not working as translated)
		temperature.message("Temperature");temperature.help(" °C : ambiant temperature in degree Celcius");//temperature.error_message("*");
        resolution.message("Resolution");resolution.help(" °C : temperature resolution in degree Celcius (and speed, e.g. 4Hz at 0.0625°C)");
        submit.value("apply");
//        gain.message("Your gain(s)");
        ///order widgets
        add(temperature);
        add(resolution);
        add(submit);
//        add(gain);
        ///values and behavious
        temperature.readonly(true);temperature.disabled(true);	
        resolution.add("0.5","0");//"label", 0x"value"
        resolution.add("0.25","1");
        resolution.add("0.125","2");
        resolution.add("0.0625","3");
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
  };//info_form
  struct message : public master
  {
    //data
    float temperature;
    int resolution;
    //GUI (HMTL)
    info_form info;
    //value list
    std::string value_list(void)
    {
      std::ostringstream s;
      s<<"message data:";
      s<<"\n - temperature="<<temperature;
      s<<"\n - resolution=0x" <<resolution;
      s<<std::endl;
      return s.str();
    }//value_list
  };//message
}//content

#endif //CONTENT_H
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
