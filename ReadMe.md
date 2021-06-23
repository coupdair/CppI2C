I2C

- i2c:      I2C info on terminal
- i2c-web:  I2C info on HTTP

# compile

## MC2SA board on I2C

`make`
`make && make run-web`

## no MC2SA board connected

`make fake-web run-fake-web`

# use

`make run`

# misc.

- `wget http://193.48.111.15:8080/hello` fail
- `wget http://localhost:8080/hello` done

# TODO

priotity

## high

- . register: RW, RO, WO
- module: fake to I2C devices

## medium

- `fake`and `fake-web`: fake module MC2SA by compiling -DFAKE_MODULE

## low

- dynamic module: discover I2C devices before GUI
