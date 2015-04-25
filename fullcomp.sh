#!/usr/bin/env bash
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

#CANNOT USE TABS BELOW -- SORRY!
python3 <<EOF
from os import listdir
from os.path import isfile,join
from re import match
from sys import exit
files=[fnm for fnm in listdir("competitions") if isfile(join("competitions",fnm)) and fnm[:5]=="game_"]
scores=dict()
playernames=[]
for file in files:
   f=open(join("competitions",file))
   lines=f.readlines()
   p1name=lines[0][4:].strip()
   p2name=lines[1][4:].strip()
   result=lines[-1].strip()
   if not match(r"P. won|Tie",result):
      print("Unterminated log file "+file)
      exit(1)
   try:
      playernames.index(p1name)
   except:
      playernames.append(p1name)
      scores[p1name]=0
   try:
      playernames.index(p2name)
   except:
      playernames.append(p2name)
      scores[p2name]=0
   if result=="Tie":
      scores[p1name]+=1
      scores[p2name]+=1
   elif result=="P1 won":
      scores[p1name]+=3
      scores[p2name]+=1
   elif result=="P1 won":
      scores[p1name]+=1
      scores[p2name]+=3
scores=sorted(scores.items(),key=lambda item:-item[1]) #sort reverse on scores
for score in scores:
   print(score[0]+": "+str(score[1]))
EOF
