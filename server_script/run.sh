#!/bin/sh
dir=`dirname ${0}`
cd ${dir}


ret=`python ./android.py /home/wwwroot/hosts/web/android/`
if [ "$ret" ];then
    echo 'error'
    echo "Hosts(android) sync error\!" | mail -s "mail form iotwrt" jacob-chen@iotwrt.com
    exit
fi

ret=`python ./ios.py /home/wwwroot/hosts/web/ios/`
if [ "$ret" ];then
    echo 'error'
    echo "Hosts(ios) sync error\!"  | mail -s "mail form iotwrt" jacob-chen@iotwrt.com
    exit
fi