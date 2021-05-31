#ifndef CONTENT_H
#define CONTENT_H

#include <cppcms/view.h>
#include <string>

namespace content
{
  struct master : public cppcms::base_content
  {
    std::string title;
  };//master
  struct page : public master
  {
    std::string page_title, page_content;
  };//page
  struct news : public master
  {
    std::string content_head;
    std::list<std::string> news_list;
  };//news
}//content

#endif //CONTENT_H
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
