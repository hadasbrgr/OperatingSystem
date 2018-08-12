#Hadas berger

#!/bin/bash
var1="Number of files in the directory that end with .txt is"
num=$( ls $1 | grep ".txt" | wc -l)
echo $var1 $num