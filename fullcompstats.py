#!/usr/bin/env python3
from os import listdir
from os.path import isfile,join
from re import match
from sys import exit
files=[fnm for fnm in listdir("competitions") if isfile(join("competitions",fnm)) and fnm[:5]=="game_"]
scores=dict()
playernames=[]
for fname in files:
	f=open(join("competitions",fname))
	lines=f.readlines()
	p1name=lines[0][4:].strip()
	p2name=lines[1][4:].strip()
	result=lines[-1].strip()
	if not match(r"P. won|Tie",result):
		print("(Unterminated log file "+fname+")")
		continue #both get 0 score
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
maxplayerlen=max(6,max([len(sc[0]) for sc in scores]))+1
print("PLAYER "+" "*(maxplayerlen-7)+"| SCORE")
print("-"*maxplayerlen+"+------")
for score in scores:
	print(score[0]+" "*(maxplayerlen-len(score[0]))+"| "+str(score[1]))
