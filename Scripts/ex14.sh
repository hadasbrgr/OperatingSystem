#Hadas berger

#!/bin/bash
grep -h $1 $2
grep -h -s $1 $2 > m.txt
awk '{n += $3}; END{print "Total Balabce: " n}' m.txt
rm m.txt