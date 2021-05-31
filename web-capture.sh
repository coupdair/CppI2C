#!/bin/bash

echo 'web capture will start in a while ...'
sleep 6

#get HTML content
wb=i2c-bus
list=
for d in $wb $wb/system $wb/news
do
  f=`basename $d`
  echo 'get '$f
  #clean
  rm $f $f.? $f.?? $f.html
  #get web page
  wget http://localhost:8080/$d
  mv $f $f.html
  list=$list' '$f.html
done

sleep 1
#stop web service
killall i2c-web

for f in $list
do
  echo $f':'
  cat $f
done
