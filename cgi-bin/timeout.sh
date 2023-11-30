#!/usr/bin/env bash

# Get the start time
start_time=`date +%s`
duration=60

# Run the loop until the specified duration is reached
while true; do

	# Get the current time
	current_time=`date +%s`
	elapsed_time=$((current_time - start_time))

	# Check if the duration has been reached
	if [ $elapsed_time -ge $duration ]
	then
		break
	fi

	done
