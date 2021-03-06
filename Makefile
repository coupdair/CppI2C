all: i2c help

i2c: i2c.cpp
	g++ i2c.cpp -o i2c

help: i2c
	./i2c --help | tee i2c.help.output

version: i2c
	./i2c --version | tee VERSION

run: i2c
	./i2c -i 1 | tee i2c.txt
