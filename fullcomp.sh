#!/usr/bin/env bash
find competitions -type f -delete

for p1 in ./stttfirst ./stttrandom ./stttrecur ./stttswag; do
	for p2 in ./stttfirst ./stttrandom ./stttrecur ./stttswag; do
		if [[ $p1 == $p2 ]]; then
			continue
		fi
		printf "%s\n%s\n" $p1 $p2 >competition.txt
		./competition.py
		status=$?
		if [[ $status != 0 ]]; then
			echo "NON-ZERO EXIT STATUS ${status}!"
			exit 1
		fi
	done
done

./fullcompstats.py
