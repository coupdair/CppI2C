all: i2c

i2c: i2c.cpp
	g++ i2c.cpp -o i2c
