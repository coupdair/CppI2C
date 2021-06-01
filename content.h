#ifndef CONTENT_H
#define CONTENT_H

#include <cppcms/view.h>
#include <cppcms/form.h>
#include <string>

namespace content
{
//page
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
    cppcms::widgets::text name;
    cppcms::widgets::radio sex;
    cppcms::widgets::select marital;
    cppcms::widgets::numeric<double> age;
    cppcms::widgets::submit submit;
    cppcms::widgets::checkbox board;
    info_form()
    {
		//widget titles
        name.message("Your Name");
        sex.message("Sex");
        marital.message("Marital Status");
        age.message("Your Age");
        submit.value("set");
        board.message("You have a board");
        //order widgets
        add(name);
        add(sex);
        add(marital);
        add(age);
        add(board);
        add(submit);
        //values and behavious
        sex.add("Male","male");
        sex.add("Female","female");
        marital.add("Single","single");
        marital.add("Married","married");
        marital.add("Divorced","divorced");
        name.non_empty();
        sex.non_empty();
        age.range(0,123);
    }//constructor
    virtual bool validate()
    {
        if(!form::validate()) 
            return false;
        if(marital.selected_id()!="single" && age.value()<18)
        {
            marital.valid(false);
            return false;
        }
        return true;
    }//validate
  };//info_form
  struct message : public cppcms::base_content
  {
    //data
    std::string name,state,sex;
    double age;
    bool board;
    //GUI (HMTL)
    info_form info;
    //value list
    std::string value_list(void)
    {
      std::ostringstream s;
      s<<"message data:";
      s<<"\n - name="<<name;
      s<<"\n - state="<<state;
      s<<"\n - sex="<<sex;
      s<<"\n - age="<<age;
      s<<"\n - "<<std::string((board)?"have a":"havn't")<<" board";
      s<<std::endl;
      return s.str();
    }//value_list
  };//message

}//content

#endif //CONTENT_H
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
