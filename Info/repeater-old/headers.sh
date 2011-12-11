#!/bin/bash

echo $1 > .headers
	
while read line
do
	if [ -f $line ]; then
		if [ "$line" != "$1" ]; then
			#echo "[$line]"
			#grep -w "$2" < $line
			ack -H "$2" $line $3
		fi
		grep -E "^#include <.*\.h>" $line | sed -e 's/^#include <\([^\<]*\)>.*$/\/usr\/include\/\1/' | while read param
		do
			while read l
			do
				if [ "$l" == "$param" ]; then
					continue 2
				fi
			done < .headers
			echo $param >> .headers
		done
	fi
done < .headers

rm .headers
