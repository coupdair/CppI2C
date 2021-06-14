LD_LIBRARY_PATH=LD_LIBRARY_PATH="../CppCMS/cppcms/build;../CppCMS/cppcms/build/booster"

#all: i2c help version  web web-help web-version
#all: web web-help web-version
all: i2c help version

code:
	geany ReadMe.md Makefile i2c_tools.hpp os_tools.hpp i2c.cpp config.js master.tmpl main.tmpl devices.tmpl  page.tmpl i2c-web.cpp web-capture.sh &

i2c: i2c.cpp i2c_tools.hpp
	g++ -I../i2c-tools/include -I../i2c-tools/tools -DUSE_I2C_TOOLS_CODE -fpermissive i2c.cpp -o i2c
#	g++ i2c.cpp -o i2c
	./i2c --list | tee factory_lists.txt

help: i2c
	./i2c --help | tee i2c.help.output

version: i2c
	./i2c --version | tee VERSION

run: i2c
	echo;echo "factory:"
	./i2c -i 1 | tee i2c-bus.txt

web: i2c-web.cpp config.js i2c_tools.hpp
	../CppCMS/cppcms/bin/cppcms_tmpl_cc master.tmpl main.tmpl page.tmpl devices.tmpl -o web_skin.cpp
	g++ -I../CppCMS/cppcms/ -I../CppCMS/cppcms/booster -I../CppCMS/cppcms/build -I../CppCMS/cppcms/build/booster  -O2 -Wall -g i2c-web.cpp web_skin.cpp -o i2c-web -L../CppCMS/cppcms/build -L../CppCMS/cppcms/build/booster -lcppcms -lbooster

web-help: web
	$(LD_LIBRARY_PATH) ./i2c-web --help | tee i2c-web.help.output

web-version: web
	$(LD_LIBRARY_PATH) ./i2c-web --version | tee VERSION

run-web: web
	#sleep a while then get HTML and stop service
	./web-capture.sh &
	#run i2c-web
	$(LD_LIBRARY_PATH) ./i2c-web -c config.js -i 1 | tee i2c-web.txt
