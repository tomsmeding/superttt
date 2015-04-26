#!/usr/bin/env bash
BINARIES="./stttfirst ./stttrandom ./stttrecur ./stttswag"

if [[ ! -e competitions ]]; then
	mkdir competitions || exit 1
fi

find competitions -type f -delete

for p1 in $BINARIES; do
	for p2 in $BINARIES; do
		if [[ $p1 == $p2 ]]; then
			continue
		fi
		printf "%s\n%s\n" $p1 $p2 >competition.txt
		./competition.py $@
		status=$?
		if [[ $status != 0 ]]; then
			echo "$p1 - $p2 : ERROR $status     (0-0)"
			continue
		fi
		p1pretty=$(echo "$p1" | sed 's/[^a-zA-Z0-9 ]//g')
		p2pretty=$(echo "$p2" | sed 's/[^a-zA-Z0-9 ]//g')
		complogfile="competitions/game_${p1pretty}_vs_${p2pretty}.txt"
		lastline=$(tail -n1 $complogfile)
		if [[ "$lastline" == "P1 won" ]]; then
			echo "$p1 - $p2 : WIN - LOSS (3-1)"
		elif [[ "$lastline" == "P2 won" ]]; then
			echo "$p1 - $p2 : LOSS - WIN (1-3)"
		elif [[ "$lastline" == "Tie" ]]; then
			echo "$p1 - $p1 :    TIE     (1-1)"
		fi
	done
done

./fullcompstats.py
