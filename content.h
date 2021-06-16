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
		temperature.value(23.45);
		temperature.message("Temperature");
        resolution.message("Resolution");
        submit.value("apply");
//        gain.message("Your gain(s)");
        ///order widgets
        add(temperature);
        add(resolution);
        add(submit);
//        add(gain);
        ///values and behavious
        resolution.add("0.5 °C","0.5");//"label", "value"
        resolution.add("0.25 °C","0.25");
        resolution.add("0.125 °C","0.125");
        resolution.add("0.0625 °C","0.0625");
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
    float resolution;
    //GUI (HMTL)
    info_form info;
    //value list
    std::string value_list(void)
    {
      std::ostringstream s;
      s<<"message data:";
      s<<"\n - temperature="<<temperature;
      s<<"\n - resolution=" <<resolution;
      s<<std::endl;
      return s.str();
    }//value_list
  };//message
}//content

#endif //CONTENT_H
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
