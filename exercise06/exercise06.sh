#!/bin/bash

echo 26 > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio26/direction

current_value=0
counter=0

while true; do
	new_value=$(cat /sys/class/gpio/gpio26/value)
	if [[ $new_value = 1 && $new_value != $current_value ]] ; then
		let counter+=1
		echo $counter
	fi
	current_value=$new_value
	read -t 0.5 -N 1 input
	if [[ $input = "q" ]]; then
		echo
		echo Stopping...
		break
	fi
done

echo 26 > /sys/class/gpio/unexport
