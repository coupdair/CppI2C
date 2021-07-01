LD_LIBRARY_PATH=LD_LIBRARY_PATH="../CppCMS/cppcms/build;../CppCMS/cppcms/build/booster;../libI2C/"

#all: i2c help version  web web-help web-version
#all: web web-help web-version run-web-capture
#all: i2c help version
all: fake-web web-version run-fake-web-capture
#all: web

code:
	geany ReadMe.md Makefile  i2c_tools.hpp os_tools.hpp register.hpp device.hpp module.hpp  i2c.cpp  config.js master.tmpl main.tmpl devices.tmpl setup.tmpl page.tmpl content.h i2c-web.cpp web-capture.sh &

i2c: i2c.cpp i2c_tools.hpp os_tools.hpp device.hpp register.hpp module.hpp
#	g++ -DUSE_I2C_TOOLS_CODE -I../i2c-tools/include -I../i2c-tools/tools -DUSE_I2C_LIB -I../libI2C/include -fpermissive -L../libI2C/ -li2c i2c.cpp -o i2c
	g++ -DUSE_I2C_LIB -I../libI2C/include -fpermissive -L../libI2C/ -li2c i2c.cpp -o i2c
#	g++ i2c.cpp -o i2c
	LD_LIBRARY_PATH=../libI2C/ ./i2c --list | tee factory_lists.txt

help: i2c
	LD_LIBRARY_PATH=../libI2C/ ./i2c --help | tee i2c.help.output

version: i2c
	LD_LIBRARY_PATH=../libI2C/ ./i2c --version | tee VERSION | tee VERSION.i2c

run: i2c
	echo;echo "factory:"
#	LD_LIBRARY_PATH=../libI2C/ ./i2c -i 1 -r 3 | tee i2c-bus.txt
	LD_LIBRARY_PATH=../libI2C/ ./i2c -i 1 -c 1.23    2>&1 | tee    i2c-bus.txt #BAD
	LD_LIBRARY_PATH=../libI2C/ ./i2c -i 1 -c 0.5     2>&1 | tee -a i2c-bus.txt
	LD_LIBRARY_PATH=../libI2C/ ./i2c -i 1 -c 0.0625  2>&1 | tee -a i2c-bus.txt

web: i2c-web.cpp config.js i2c_tools.hpp os_tools.hpp device.hpp register.hpp module.hpp
#	../CppCMS/cppcms/bin/cppcms_tmpl_cc master.tmpl main.tmpl page.tmpl devices.tmpl setup.tmpl -o web_skin.cpp
#	g++ -DUSE_I2C_LIB -I../libI2C/include -fpermissive -L../libI2C/ -li2c  -I../CppCMS/cppcms/ -I../CppCMS/cppcms/booster -I../CppCMS/cppcms/build -I../CppCMS/cppcms/build/booster  -O2 -Wall -g i2c-web.cpp web_skin.cpp -o i2c-web -L../CppCMS/cppcms/build -L../CppCMS/cppcms/build/booster -lcppcms -lbooster
	../CppCMS/cppcms/bin/cppcms_tmpl_cc master.tmpl page.tmpl integer.tmpl news.tmpl intro.tmpl -o my_skin.cpp
	g++ -Wall -I../CppCMS/cppcms/ -I../CppCMS/cppcms/booster -I../CppCMS/cppcms/build -I../CppCMS/cppcms/build/booster myapp.cpp my_skin.cpp -o i2c-web -L../CppCMS/cppcms/build -L../CppCMS/cppcms/build/booster -lcppcms -lbooster

fake-web: i2c-web.cpp config.js i2c_tools.hpp os_tools.hpp device.hpp register.hpp module.hpp
	../CppCMS/cppcms/bin/cppcms_tmpl_cc master.tmpl main.tmpl page.tmpl devices.tmpl setup.tmpl -o web_skin.cpp
	g++ -DFAKE_MC2SA -DUSE_I2C_LIB -I../libI2C/include -fpermissive -L../libI2C/ -li2c -I../CppCMS/cppcms -I../CppCMS/cppcms/booster -I../CppCMS/cppcms/build -I../CppCMS/cppcms/build/booster  -O2 -Wall -g i2c-web.cpp web_skin.cpp -o fake-web -L../CppCMS/cppcms/build -L../CppCMS/cppcms/build/booster -lcppcms -lbooster
#	g++ -DFAKE_MC2SA -DFAKE_TEMPERATURE -I../CppCMS/cppcms -I../CppCMS/cppcms/booster -I../CppCMS/cppcms/build -I../CppCMS/cppcms/build/booster  -O2 -Wall -g i2c-web.cpp web_skin.cpp -o fake-web -L../CppCMS/cppcms/build -L../CppCMS/cppcms/build/booster -lcppcms -lbooster

web-help: web
	$(LD_LIBRARY_PATH) ./i2c-web --help | tee i2c-web.help.output

web-version: web
	$(LD_LIBRARY_PATH) ./i2c-web --version | tee VERSION | tee VERSION.i2c-web

run-web: web
	#run i2c-web
	$(LD_LIBRARY_PATH) ./i2c-web -c config.js -i 1 2>&1 | tee i2c-web-live.txt

run-fake-web: fake-web
	#run fake-web
	$(LD_LIBRARY_PATH) ./fake-web -c config.js -i 1 2>&1 | tee fake-web-live.txt

browse-web:
	firefox --new-instance http://localhost:8080/i2c-bus http://localhost:8080/i2c-bus/bus http://localhost:8080/i2c-bus/devices http://localhost:8080/i2c-bus/setup/0 http://localhost:8080/i2c-bus/system &

run-web-capture: web
	#sleep a while then get HTML and stop service
	./web-capture.sh i2c-web &
	#run i2c-web
	$(LD_LIBRARY_PATH) ./i2c-web -c config.js -i 1 2>&1 | tee i2c-web.txt

run-fake-web-capture: fake-web
	#sleep a while then get HTML and stop service
	./web-capture.sh fake-web &
	#run fake-web
	$(LD_LIBRARY_PATH) ./fake-web -c config.js -i 1 2>&1 | tee fake-web.txt
