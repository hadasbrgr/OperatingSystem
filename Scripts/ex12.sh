#!/bin/bash
#echo "Enter your path:"
#read first
var1="is a directory"
var2="is a file"
for i in $(ls $1)
do
	if test -d $i 
		then
			echo $i $var1
		else
 			echo $i $var2
 	fi
done