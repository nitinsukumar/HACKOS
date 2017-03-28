#!/bin/bash

SYS_MAP=`ls /boot/System.map-*-generic`
SYS_CALL_ADDR=0x`cat $SYS_MAP | grep sys_call_table | cut -f1 -d' '`
MP4='/home/nitin/Videos/1minute.mp4'
MP42='/home/nitin/Videos/3minute.mp4'
txt='/home/nitin/Desktop/nitinhack/txt/nitin.txt'
pic1='/home/nitin/Pictures/hacked.jpg'

if [ 'X$SYS_CALL_ADDR' == 'X' ]
then
	echo "Can't find the system call table address.Failed"
	exit 1
fi

echo 'System call table address = $SYS_CALL_ADDR'

echo 'Building the module...'
make &> /dev/null

echo 'Loading the module...'
insmod hijack-syscall.ko syscall_table=$SYS_CALL_ADDR my_path=$MP4 my_path2=$MP42 my_path3=$txt my_path4=$pic1

if [ $? -eq 0 ]
then
	echo "Module loaded!."
fi
