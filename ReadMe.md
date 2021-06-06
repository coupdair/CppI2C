I2C

- i2c:      I2C info on terminal
- i2c-web:  I2C info on HTTP

# compile

`make`
`make && make run-web`

# use

`make run`

# misc.

- `wget http://193.48.111.15:8080/hello` fail
- `wget http://localhost:8080/hello` done


# WiP: i2c-tools

~~~ { .bash }
make i2c
ld error: in function `i2c_bus_list2(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >&, bool)':
i2c.cpp:(.text+0x18): undefined reference to `gather_i2c_busses()'
/usr/bin/ld: i2c.cpp:(.text+0xec): undefined reference to `free_adapters(i2c_adap*)'
~~~
