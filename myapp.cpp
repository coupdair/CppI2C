#include <cppcms/application.h>
#include <cppcms/applications_pool.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <cppcms/url_dispatcher.h>
#include <iostream>

#include "content.h"

class myapp  : public cppcms::application {
public:
    myapp(cppcms::service &s): cppcms::application(s)
    {
        dispatcher().assign("",&myapp::intro,this);
        mapper().assign("");

        dispatcher().assign("/news",&myapp::news,this);
        mapper().assign("news","/news");

        dispatcher().assign("/page",&myapp::page,this);
        mapper().assign("page","/page");

        dispatcher().assign("/integer",&myapp::integerLink,this);
        mapper().assign("integer","/integer");

        dispatcher().assign("^/integer/(\\d+)$",&myapp::integer,this,1);
        mapper().assign("integer","/integer");

        mapper().root("/i2cweb");
    }//constructor
    void ini(content::master &c)
    {
        c.title = "Web inherit.";
    }//ini
    void intro()
    {
        content::master c;
        ini(c);
        render("intro",c);
    }//intro
    void page()
    {
        content::page c;
        ini(c);
        std::cout<<__FILE__<<"/"<<__func__<<std::endl;
        c.page_title = "About cout";
        c.page_content = "<p>A page about this web site</p>";
        render("page",c);
    }//page
    void integerLink()
    {
        content::page c;
        ini(c);
        std::cout<<__FILE__<<"/"<<__func__<<std::endl;
        c.page_title = "integer link ex.";
        c.page_content = "<p><a href='integer/12'>12</a></p><br/><p><a href='integer/123'>123</a></p>";
        render("page",c);
    }//integerLink
    void integer(std::string i)
    {
        content::integer c;
        ini(c);
        std::cout<<__FILE__<<"/"<<__func__<<std::endl;
        c.page_title = "integer";
        c.i="<p>dynamic integer: "+i+"</p>";
        c.page_content = "<p>static content</p>";
        render("integer",c);
    }//integer
    void news()
    {
        content::news c;
        ini(c);
        c.news_list.push_back("This is the latest message!");
        c.news_list.push_back("This is the next message.");
        c.news_list.push_back("This is the last message!");
        render("news",c);
    }//news
};//myapp



int main(int argc,char ** argv)
{
    try {
        cppcms::service srv(argc,argv);
        srv.applications_pool().mount(cppcms::applications_factory<myapp>());
        srv.run();
    }
    catch(std::exception const &e) {
        std::cerr<<e.what()<<std::endl;
    }
}//main
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

