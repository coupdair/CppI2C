I2C

- i2c:      I2C info on terminal
- i2c-web:  I2C info on HTTP

# lighttpd

`sudo service lighttpd stop && make web && sudo service lighttpd start && ps aux | grep i2cweb --color`

# compile

## MC2SA board on I2C

`make`
`make && make run-web`

## no MC2SA board connected

`make fake-web web-version run-fake-web`

# use

`make run`

# misc.

- `wget http://193.48.111.15:8080/hello` fail
- `wget http://localhost:8080/hello` done

# TODO

priotity

## high

## medium

## low

- dynamic module: discover I2C devices before GUI
