#!/usr/bin/env bash
for p1 in ./stttfirst ./stttrandom ./stttrecur; do
	for p2 in ./stttfirst ./stttrandom ./stttrecur; do
		if [[ $p1 == $p2 ]]; then
			continue
		fi
		printf "%s\n%s\n" $p1 $p2 >competition.txt
		./competition.py
	done
done
