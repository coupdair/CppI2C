#!/bin/bash

echo 'web capture will start in a while ...'
sleep 6

#clean
rm i2c-bus*
#get HTML content
wget http://localhost:8080/i2c-bus

sleep 1
#stop web service
killall i2c-web
