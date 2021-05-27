all: i2c help version

i2c: i2c.cpp i2c_tools.hpp
	g++ i2c.cpp -o i2c

help: i2c
	./i2c --help | tee i2c.help.output

version: i2c
	./i2c --version | tee VERSION

run: i2c
	./i2c -i 1 | tee i2c.txt

web: i2c-web.cpp config.js i2c_tools.hpp
	g++ -I../CppCMS/cppcms/ -I../CppCMS/cppcms/booster -I../CppCMS/cppcms/build -I../CppCMS/cppcms/build/booster  -O2 -Wall -g i2c-web.cpp -o i2c-web -L../CppCMS/cppcms/build -L../CppCMS/cppcms/build/booster -lcppcms -lbooster

run-web: web
	#wget http://localhost:8080/hello
	/bin/echo -e '#!/bin/bash\n\nsleep 4\n#get HTML content\nwget http://localhost:8080/hello\n\n#clean\nrm capture.sh' > capture.sh; chmod u+x capture.sh; ./capture.sh &
	#run i2c-web
	LD_LIBRARY_PATH="../CppCMS/cppcms/build;../CppCMS/cppcms/build/booster" ./i2c-web -c config.js -i 1 | tee i2c-web.txt
