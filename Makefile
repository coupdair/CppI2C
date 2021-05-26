all: i2c help

i2c: i2c.cpp
	g++ i2c.cpp -o i2c

help: i2c
	./i2c --help
