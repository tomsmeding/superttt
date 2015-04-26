#!/usr/bin/env python3
"""
Usage: ./competition.py [options] [competitionfile]

The <competitionfile> should contain two lines, which are the two commands to
execute as players.

The script writes a competitionlog to competitions/game_p1_vs_p2.txt (with p1
and p2 replaced by their respective commands).

Options:
  -C   Do not write a Competition log
  -h   Help. What you're looking at
  -q   Quiet. Don't print so much
  -v   View the competition afterwards using ./viewcompetition
"""

import os,sys,subprocess,shlex,re,time

#Function definitions

def parsemove(line):
	if not (len(line)==3 and re.match(r"^[0-8] [0-8]$",line)):
		return False
	return [int(line[0]),int(line[2])]

def isFree(move):
	return board[move[1]][move[0]]==0

def nonant(move):
	return move[1]//3*3+move[0]//3

def smallnonant(move):
	return move[1]%3*3+move[0]%3

def nonantfull(non):
	for y in range(non//3*3,non//3*3+3):
		for x in range(non%3*3,non%3*3+3):
			if board[y][x]==0: return False
	return True

def applymove(move,player):
	board[move[1]][move[0]]=player

haswonsmallcache=[0]*9

def haswonsmall(non):
	assert(0<=non and non<9)
	if haswonsmallcache[non]!=0: return haswonsmallcache[non]
	#horizontal
	x=non%3*3
	for y in range(non//3*3,non//3*3+3):
		p=board[y][x]
		if p!=0 and board[y][x+1]==p and board[y][x+2]==p:
			haswonsmallcache[non]=p
			return p
	#vertical
	y=non//3*3
	for x in range(non%3*3,non%3*3+3):
		p=board[y][x]
		if p!=0 and board[y+1][x]==p and board[y+2][x]==p:
			haswonsmallcache[non]=p
			return p

	x=non%3*3
	# \.
	p=board[y][x]
	if p!=0 and board[y+1][x+1]==p and board[y+2][x+2]==p:
		haswonsmallcache[non]=p
		return p
	# /
	p=board[y][x+2]
	if p!=0 and board[y+1][x+1]==p and board[y+2][x]==p:
		haswonsmallcache[non]=p
		return p

	return 0

def haswon():
	won=[haswonsmall(x) for x in range(0,9)]
	#horizontal
	p=won[0]
	if p!=0 and won[1]==p and won[2]==p: return p
	p=won[3]
	if p!=0 and won[4]==p and won[5]==p: return p
	p=won[6]
	if p!=0 and won[7]==p and won[8]==p: return p
	#vertical
	p=won[0]
	if p!=0 and won[3]==p and won[6]==p: return p
	p=won[1]
	if p!=0 and won[4]==p and won[6]==p: return p
	p=won[2]
	if p!=0 and won[5]==p and won[8]==p: return p
	# \.
	p=won[0]
	if p!=0 and won[4]==p and won[8]==p: return p
	# /
	p=won[2]
	if p!=0 and won[4]==p and won[6]==p: return p
	return 0

def printboard():
	print("+-------+-------+-------+ ",end="");
	for non in range(0,3):
		won=haswonsmall(non)
		print("." if won==0 else "X" if won==1 else "O" if won==2 else "?",end="")
	print("")

	for y in range(0,9):
		print("| ",end="");
		for x in range(0,9):
			print(("." if board[y][x]==0 else "X" if board[y][x]==1 else "O" if board[y][x]==2 else "?")+" ",end="");
			if x%3==2:
				print("| ",end="");
		if y<2:
			for non in range(3+3*y,6+3*y):
				won=haswonsmall(non)
				print("." if won==0 else "X" if won==1 else "O" if won==2 else "?",end="")
		print("");
		if y%3==2:
			print("+-------+-------+-------+");

#Getting entries and flags

fname=""
quiet=False
viewcompetition=False
complog=True
if len(sys.argv)==1: #no args
	fname="competition.txt"
else:
	for arg in sys.argv[1:]: #skip script name
		if len(arg)>1 and arg[0]=="-":
			for c in arg[1:]: #skip "-"
				if c=="C":
					complog=False
				elif c=="h":
					print(__doc__)
					sys.exit(0)
				elif c=="q": quiet=True
				elif c=="v": viewcompetition=True
				else:
					print("Unrecognised flag '"+c+"'.")
					print(__doc__)
					sys.exit(1)
		elif fname=="":
			fname=arg
		else:
			print("Unrecognised argument '"+arg+"'; the competition file name was already given as '"+fname+"'.")
			sys.exit(1)

if fname=="-":
	if not quiet: print("Getting entries from stdin.")
	p1fname=""
	p2fname=""
	while p1fname=="": p1fname=input().strip()
	while p2fname=="": p2fname=input().strip()
else:
	if fname=="": fname="competition.txt"
	if not quiet: print("Getting entries from file '"+fname+"'.")
	try:
		f=open(fname,mode="r")
	except:
		print("Could not open file '"+fname+"'.")
		sys.exit(1)

	p1fname=f.readline().strip()
	if p1fname=="":
		print("Too few lines in file.");
		sys.exit(1)
	p2fname=f.readline().strip()
	if p2fname=="":
		print("Too few lines in file.");
		sys.exit(1)
	f.close()

#Set up player logs

if not os.path.exists("playerlogs"):
	try:
		os.mkdir("playerlogs")
	except:
		print("Error: could not create log directory 'playerlogs'.")
		sys.exit(1)
elif not os.path.isdir("playerlogs"):
	#Apparently, there's a file named "playerlogs". Bastard.
	print("Error: an existing file prohibits creation of log directory 'playerlogs'.")
	sys.exit(1)

try:
	logfname="playerlogs/"+re.sub(r"[^a-zA-Z0-9 ]","",p1fname)+"_white_vs_"+re.sub(r"[^a-zA-Z0-9 ]","",p2fname)+".txt"
	p1errlog=open(logfname,mode="w")
	logfname="playerlogs/"+re.sub(r"[^a-zA-Z0-9 ]","",p2fname)+"_black_vs_"+re.sub(r"[^a-zA-Z0-9 ]","",p1fname)+".txt"
	p2errlog=open(logfname,mode="w")
except:
	print("Error: could not open log file '"+logfname+"'.")
	sys.exit(1)

#Start programs

if not quiet:
	print("Running this competition with:")
	print("P1 (X): '"+p1fname+"'")
	print("P2 (O): '"+p2fname+"'")

try:
	p1proc=subprocess.Popen(shlex.split(p1fname),stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=p1errlog,bufsize=1,shell=True) #line-buffered
except Exception as e:
	print("Could not execute command '"+p1fname+"'.")
	raise e
(p1in,p1out)=(p1proc.stdin,p1proc.stdout)

try:
	p2proc=subprocess.Popen(shlex.split(p2fname),stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=p2errlog,bufsize=1,shell=True) #line-buffered
except Exception as e:
	print("Could not execute command '"+p2fname+"'.")
	raise e
(p2in,p2out)=(p2proc.stdin,p2proc.stdout)

#Set up competition log

if not os.path.exists("competitions"):
	try:
		os.mkdir("competitions")
	except:
		print("Error: could not create log directory 'competitions'.")
		sys.exit(1)
elif not os.path.isdir("competitions"):
	#Apparently, there's a file named "competitions". Bastard.
	print("Error: an existing file prohibits creation of log directory 'competitions'.")
	sys.exit(1)

try:
	logfname="competitions/game_"+re.sub(r"[^a-zA-Z0-9 ]","",p1fname)+"_vs_"+re.sub(r"[^a-zA-Z0-9 ]","",p2fname)+".txt"
	logfile=open(logfname,mode="w")
	logfile.write("P1: "+p1fname+"\nP2: "+p2fname+"\n")
except:
	print("Error: could not open log file '"+logfname+"'.")
	sys.exit(1)

#Start competition

board=[[0]*9 for _ in range(0,9)]
nextnonant=-1
nummoves=0
endgame=False

p1totaltime=0
p2totaltime=0

p1in.write(b"go\n");
try:
	p1in.flush()
except IOError as e:
	if e.errno==os.errno.EINVAL:
		print("Program P1 quit prematurely.")
		try: p2proc.terminate()
		except: pass
		sys.exit(1)
	else: raise e
p2in.write(b"nogo\n");
try:
	p2in.flush()
except IOError as e:
	if e.errno==os.errno.EINVAL:
		print("Program P2 quit prematurely.")
		try: p1proc.terminate()
		except: pass
		sys.exit(1)
	else: raise e
while True:
	for (pAin,pAout,pBin,pBout,player) in [(p1in,p1out,p2in,p2out,1),(p2in,p2out,p1in,p1out,2)]:
		while True:
			line=pAout.readline().decode("ascii").strip()
			if len(line)>0:
				break
			time.sleep(0.1)
			timeoutfail=False
			if player==1:
				p1totaltime+=0.1
				if p1totaltime>=50:
					timeoutfail=True
			else:
				p2totaltime+=0.1
				if p2totaltime>=50:
					timeoutfail=True
			if timeoutfail:
				print("P"+str(player)+" timed out!")
				try: p1proc.terminate()
				except: pass
				try: p2proc.terminate()
				except: pass
				sys.exit(1)
		move=parsemove(line)
		if move==False:
			print("P"+str(player)+" made an invalid move: '"+line+"'.")
			endgame=True
			break
		if nextnonant!=-1 and nonant(move)!=nextnonant:
			print("P"+str(player)+" made a move outside of its forced nonant: '"+line+"' should have been in nonant "+str(nextnonant)+".")
			endgame=True
			break
		applymove(move,player)
		nextnonant=smallnonant(move)
		if nonantfull(nextnonant):
			nextnonant=-1
		pBin.write((str(move[0])+" "+str(move[1])+"\n").encode("ascii"))
		try:
			pBin.flush()
		except IOError as e:
			if e.errno==os.errno.EINVAL:
				print("Program P"+str(3-player)+" quit prematurely.")
				try:
					if player==1: p2proc.terminate()
					else: p1proc.terminate()
				except: pass
				sys.exit(1)
			else: raise e
		if not quiet:
			print("P"+str(player)+": "+str(move[0])+" "+str(move[1])+"  ",end="")
			sys.stdout.flush()
		if complog:
			logfile.write("P"+str(player)+": "+str(move[0])+" "+str(move[1])+"\n")
		nummoves+=1
		if nummoves==81:
			if not quiet:
				print("")
				printboard()
			won=haswon()
			if won!=0:
				if not quiet:
					print("\nP"+str(won)+" has won this game!")
				if complog:
					logfile.write("P"+str(won)+" won\n")
			else:
				if not quiet:
					print("\nThe board is full, a tie results; the game shall now be terminated.")
				if complog:
					logfile.write("Tie\n")
			endgame=True
			break
		won=haswon()
		if won!=0:
			if not quiet:
				print("")
				printboard()
				print("")
				print("P"+str(won)+" has won this game!")
			if complog:
				logfile.write("P"+str(won)+" won\n")
			endgame=True
			break
	if endgame:
		break
	if not quiet:
		print("")
		printboard()
		print("")

#Clean up

if complog:
	logfile.close()
p1errlog.close()
p2errlog.close()

try: p1proc.terminate()
except: pass
try: p2proc.terminate()
except: pass

if viewcompetition:
	os.system("."+os.path.sep+"viewcompetition "+logfname)
