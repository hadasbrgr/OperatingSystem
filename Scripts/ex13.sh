#!/bin/bash

# echo $#- all the arguments- number
# pwd $1- print the path
directory="s‫‪afe_rm_dir‬‬"
if [ "$#" -ne 1 ]
 then
    #echo "‫‪error:‬‬ ‫‪only‬‬ ‫‪one‬‬ ‫‪argument‬‬ ‫‪is‬‬ ‫‪allowed‬‬"
    echo "error: only one argument is allowed"

else if test -e $1
	then
		if [ ! -d "$directory" ]
			then
				mkdir $directory
			fi
				cp $1 $directory
				rm $1
				echo "done!"
	else 
		echo "error: there is no such file"
	fi
fi
