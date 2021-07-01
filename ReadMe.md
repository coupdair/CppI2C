
#compile

~~~ { .bash }
../../bin/cppcms_tmpl_cc master.tmpl page.tmpl integer.tmpl news.tmpl intro.tmpl -o my_skin.cpp
g++ -Wall -I../.. -I../../booster -I../../build -I../../build/booster myapp.cpp my_skin.cpp -o myapp -L../../build -L../../build/booster -lcppcms -lbooster 
~~~

#run

~~~ { .bash }
LD_LIBRARY_PATH="../../build;../../build/booster" ./myapp -c config.js
~~~          

#view

~~~ { .bash }
firefox http://localhost:8080/myapp http://localhost:8080/myapp/news  http://localhost:8080/myapp/page
~~~

## HTTP server

~~~ { .bash }
sudo service lighttpd start
#see build/fastcgi.sh
~~~

~~~ { .bash }
firefox http://193.48.111.153/myapp http://193.48.111.153/myapp/news http://193.48.111.153/myapp/page http://192.168.1.34/myapp/integer/12 http://192.168.1.34/myapp/integer/123
~~~
