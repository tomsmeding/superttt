#include <iostream>
#include <fstream>
#include <limits>
#include <cstdint>
#include <cassert>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define SYMBOLFOR(p) ((p)==P1?'X':(p)==P2?'O':(p)==PNONE?'.':'?')

#define PNONE (0)
#define P1 (1)
#define P2 (2)

using namespace std;

uint8_t board[81]={PNONE};

void dosleep(int ms){
#ifdef _WIN32
	Sleep(ms);
#else
	usleep(ms*1e3);
#endif
}

void printboard(void){
	int x,y;
	cout<<"+-------+-------+-------+"<<endl;
	for(y=0;y<9;y++){
		cout<<"| ";
		for(x=0;x<9;x++){
			cout<<SYMBOLFOR(board[9*y+x])<<' ';
			if(x%3==2)cout<<"| ";
		}
		cout<<endl;
		if(y%3==2)cout<<"+-------+-------+-------+"<<endl;
	}
}

int haswonsmall(int non){
	int x,y,p;
	static short wonvalues[9]={PNONE};
	assert(0<=non&&non<9);
	if(wonvalues[non]!=PNONE)return wonvalues[non];
	//horizontal
	x=non%3*3;
	for(y=non/3*3;y<non/3*3+3;y++){
		p=board[9*y+x];
		if(p!=PNONE&&board[9*y+x+1]==p&&board[9*y+x+2]==p){
			wonvalues[non]=p;
			return p;
		}
	}
	//vertical
	y=non/3*3;
	for(x=non%3*3;x<non%3*3+3;x++){
		p=board[9*y+x];
		if(p!=PNONE&&board[9*(y+1)+x]==p&&board[9*(y+2)+x]==p){
			wonvalues[non]=p;
			return p;
		}
	}
	x=non%3*3;
	// \.
	p=board[9*y+x];
	if(p!=PNONE&&board[9*(y+1)+x+1]==p&&board[9*(y+2)+x+2]==p){
		wonvalues[non]=p;
		return p;
	}
	// /
	p=board[9*y+x+2];
	if(p!=PNONE&&board[9*(y+1)+x+1]==p&&board[9*(y+2)+x]==p){
		wonvalues[non]=p;
		return p;
	}
	return PNONE;
}

int main(int argc,char **argv){
	if(argc==1){
		cout<<"Pass the file name of the competition log as a command-line parameter."<<endl;
		return 1;
	} else if(argc>2){
		cout<<"Multiple command-line arguments were passed."<<endl;
		return 1;
	}
	ifstream in(argv[1]);
	if(!in.good()){
		cout<<"Cannot open file '"<<argv[1]<<"'."<<endl;
		return 1;
	}
	string p1name,p2name;
	int player,x,y;
	char c;
	in.get(); in.get(); in.get(); in.get(); //"P1: "
	getline(in,p1name);
	in.get(); in.get(); in.get(); in.get(); //"P2: "
	getline(in,p2name);
	cout<<"\x1B[2J\x1B[1;1HCompetition between:\n"
	      "P1 (X): "<<p1name<<"\n"
	      "P2 (O): "<<p2name<<endl;
	if(!in.good()){
		cout<<"Error in log file format."<<endl;
		return 1;
	}
	cout<<"\x1B[4;27HPress enter to play next move"<<flush;
	cout<<"\x1B[4;1H"<<flush;
	printboard();
	while(in.good()){
		c=in.get(); //"P" for a move line or a win line, "T" for a tie line
		if(c=='P'){
			player=in.get()-'0';
			c=in.get(); //":" for a move line, " " for a win line
			if(c==':'){
				in.get();
				x=in.get()-'0';
				in.get(); //" "
				y=in.get()-'0';
				in.ignore(numeric_limits<streamsize>::max(),'\n');
				board[9*y+x]=player;
				cout<<"\x1B["<<(5+y/3+y)<<';'<<(3+x/3*2+x*2)<<'H'<<SYMBOLFOR(player)
				    <<"\x1B[5;27H"<<SYMBOLFOR(player)<<": "<<x<<' '<<y
				    <<"\x1B[7;27H"<<SYMBOLFOR(haswonsmall(0))<<SYMBOLFOR(haswonsmall(1))<<SYMBOLFOR(haswonsmall(2))
					<<"\x1B[8;27H"<<SYMBOLFOR(haswonsmall(3))<<SYMBOLFOR(haswonsmall(4))<<SYMBOLFOR(haswonsmall(5))
					<<"\x1B[9;27H"<<SYMBOLFOR(haswonsmall(6))<<SYMBOLFOR(haswonsmall(7))<<SYMBOLFOR(haswonsmall(8))<<flush;
				c=cin.get();
				if(c!='\n')cin.ignore(numeric_limits<streamsize>::max(),'\n');
			} else if(c==' '){
				cout<<"\x1B[17;1HThe victor is player "<<player<<"!"<<endl;
				in.close();
				dosleep(1500);
				return 0;
			} else {
				cout<<"\x1B[17;1HError in log file format."<<endl;
				return 1;
			}
		} else if(c=='T'){
			cout<<"\x1B[17;1HThe game resulted in a tie."<<endl;
			in.close();
			dosleep(1500);
			return 0;
		}
	}
	cout<<"\x1B[17;1HLog file ended prematurely."<<endl;
	return 1;
}
