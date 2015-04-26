#!/usr/bin/env bash
BINARIES="./stttfirst ./stttrandom ./stttrecur ./stttswag"

if [[ ! -e competitions ]]; then
	mkdir competitions || exit 1
fi
if [[ ! -e competitionstubs ]]; then
	mkdir competitionstubs || exit 1
fi

find competitions/ -type f -delete
find competitionstubs/ -type f -delete

FIFONAME=fullcompMT_output_fifo.fifo
[[ -e $FIFONAME ]] && rm $FIFONAME
mkfifo fullcompMT_output_fifo.fifo

tail -F $FIFONAME &
TAILPID=$!

trap "rm $FIFONAME; kill $TAILPID; ./fullcompstats.py" EXIT


DATE_FMT="%Y-%m-%d %H:%M:%S"



#SOURCE: competition.sh, by Wilmer van der Gaast
# Don't count the number of logical cores/threads. Although a competition
# finishes slightly faster when using all (hyper)threads, it's only a 10-20%
# improvement instead of the ~100% you'd expect. You just get threads tied
# up waiting for execution units very often. Nice when testing, but it ruins
# the time limits/etc.
if [ -z "$num_cores" ] && [ -e /proc/cpuinfo ]; then
	num_cores=$(grep ^'core id\b' /proc/cpuinfo | sort | uniq | wc -l)
fi

if [ -z "$num_cores" ] || [ "$num_cores" -lt "1" ]; then
	# OS X, src:
	# http://stackoverflow.com/questions/1715580/how-to-discover-number-of-cores-on-mac-os-x
	# http://www.opensource.apple.com/source/xnu/xnu-792.13.8/libkern/libkern/sysctl.h
	num_cores=$(sysctl -n hw.physicalcpu || echo 0)
fi

if [ -z "$num_cores" ] || [ "$num_cores" -lt "1" ]; then
	num_cores=2
	echo "Couldn't figure out number of cores, will guess $num_cores."
else
	echo "Number of cores (w/o Hyper-Threading): $num_cores."
fi



function green {
	printf '\x1B[33m%s\x1B[0m' $1
}



for p1 in $BINARIES; do
	for p2 in $BINARIES; do
		if [[ $p1 == $p2 ]]; then
			continue
		fi
		p1pretty=$(echo "$p1" | sed 's/[^a-zA-Z0-9 ]//g')
		p2pretty=$(echo "$p2" | sed 's/[^a-zA-Z0-9 ]//g')
		COMPFILE="competitionstubs/game_${p1pretty}_vs_${p2pretty}.sh"
		cat >"$COMPFILE" << EOF
#!/usr/bin/env bash
printf "%s\n%s\n" $p1 $p2 | ./competition.py -q -
status=$?
if [[ \$status != 0 ]]; then
	echo "$p1 - $p2 : ERROR $status     (0-0)" >$FIFONAME
	exit 1
fi
lastline=\$(tail -n1 "competitions/game_${p1pretty}_vs_${p2pretty}.txt")
if [[ "\$lastline" == "P1 won" ]]; then
	echo \$(date +"$DATE_FMT") $(green "$p1") "- $p2 : WIN - LOSS (3-1)" >$FIFONAME
elif [[ "\$lastline" == "P2 won" ]]; then
	echo \$(date +"$DATE_FMT") "$p1 -" $(green $p2) ": LOSS - WIN (1-3)" >$FIFONAME
elif [[ "\$lastline" == "Tie" ]]; then
	echo \$(date +"$DATE_FMT") "$p1" $(green -) "$p2 :    TIE     (1-1)" >$FIFONAME
fi
EOF
		chmod +x $COMPFILE
	done
done

find competitionstubs -type f -name 'game_*' | xargs -P$num_cores -n1 -- bash
