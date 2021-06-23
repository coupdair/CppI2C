#!/bin/bash

echo 'web capture will start in a while ...'
sleep 6

p=i2c-web
p=fake-web
p=$1

#get HTML content
wb=i2c-bus
list=
for d in $wb $wb/bus $wb/devices $wb/setup/0 $wb/system
do
  f=`basename $d`
  if [ "$f" == "0" ] ; then fo="setup"; else fo=$f; fi
  echo 'get '$f
  #clean
  rm $f $f.? $f.?? $fo.html
  #get web page
  wget http://localhost:8080/$d
  mv $f $fo.html
  list=$list' '$fo.html
done

sleep 1
#stop web service
killall $p

for f in $list
do
  echo $f':'
  cat $f
done
