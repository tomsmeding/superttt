#include <iostream>
#include <sstream>
#include <vector>
#include <utility>
#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <sys/time.h>
#include <cassert>

#define NONE (0)
#define ME (1)
#define OTHER (2)

#define MAXDEPTH (7)

using namespace std;

struct Move{
	short x=0,y=0;
};

bool haswonsmallDidQueryCache=false;

void printboard(uint8_t *board){
	int x,y;
	cerr<<"+-------+-------+-------+"<<endl;
	for(y=0;y<9;y++){
		cerr<<"| ";
		for(x=0;x<9;x++){
			cerr<<(board[9*y+x]==NONE?'.':board[9*y+x]==ME?'X':board[9*y+x]==OTHER?'O':'?')<<' ';
			if(x%3==2)cerr<<"| ";
		}
		cerr<<endl;
		if(y%3==2)cerr<<"+-------+-------+-------+"<<endl;
	}
}

bool validatemove(Move &move,uint8_t *board){return 0<=move.x&&move.x<9&&0<=move.y&&move.y<9&&board[9*move.y+move.x]==NONE;}

int nonant(Move &move){
	return move.y/3*3+move.x/3;
}

int smallnonant(Move &move){
	return move.y%3*3+move.x%3;
}

int haswonsmall(uint8_t *board,int non,bool updateStatic){
	int x,y,p;
	static short wonvalues[9]={NONE};
	assert(0<=non&&non<9);
	if(wonvalues[non]!=NONE){
		haswonsmallDidQueryCache=true;
		return wonvalues[non];
	}
	haswonsmallDidQueryCache=false;
	//horizontal
	x=non%3*3;
	for(y=non/3*3;y<non/3*3+3;y++){
		p=board[9*y+x];
		if(p!=NONE&&board[9*y+x+1]==p&&board[9*y+x+2]==p){
			if(updateStatic)wonvalues[non]=p;
			return p;
		}
	}
	//vertical
	y=non/3*3;
	for(x=non%3*3;x<non%3*3+3;x++){
		p=board[9*y+x];
		if(p!=NONE&&board[9*(y+1)+x]==p&&board[9*(y+2)+x]==p){
			if(updateStatic)wonvalues[non]=p;
			return p;
		}
	}
	x=non%3*3;
	// \.
	p=board[9*y+x];
	if(p!=NONE&&board[9*(y+1)+x+1]==p&&board[9*(y+2)+x+2]==p){
		if(updateStatic)wonvalues[non]=p;
		return p;
	}
	// /
	p=board[9*y+x+2];
	if(p!=NONE&&board[9*(y+1)+x+1]==p&&board[9*(y+2)+x]==p){
		if(updateStatic)wonvalues[non]=p;
		return p;
	}
	return NONE;
}

void updateAllHaswonsmallStatics(uint8_t *board){
	int non,won;
	for(non=0;non<9;non++){
		won=haswonsmall(board,non,true);
		if(won==ME)cerr<<'X';
		else if(won==OTHER)cerr<<'O';
		else cerr<<'.';
		if(non%3==2)cerr<<endl;
	}
}

int haswon(uint8_t *board){
	int p;
	//horizontal
	p=haswonsmall(board,0,false);
	if(p!=NONE&&haswonsmall(board,1,false)==p&&haswonsmall(board,2,false)==p)return p;
	p=haswonsmall(board,3,false);
	if(p!=NONE&&haswonsmall(board,4,false)==p&&haswonsmall(board,5,false)==p)return p;
	p=haswonsmall(board,6,false);
	if(p!=NONE&&haswonsmall(board,7,false)==p&&haswonsmall(board,8,false)==p)return p;
	//vertical
	p=haswonsmall(board,0,false);
	if(p!=NONE&&haswonsmall(board,3,false)==p&&haswonsmall(board,6,false)==p)return p;
	p=haswonsmall(board,1,false);
	if(p!=NONE&&haswonsmall(board,4,false)==p&&haswonsmall(board,6,false)==p)return p;
	p=haswonsmall(board,2,false);
	if(p!=NONE&&haswonsmall(board,5,false)==p&&haswonsmall(board,8,false)==p)return p;
	// \.
	p=haswonsmall(board,0,false);
	if(p!=NONE&&haswonsmall(board,4,false)==p&&haswonsmall(board,8,false)==p)return p;
	// /
	p=haswonsmall(board,2,false);
	if(p!=NONE&&haswonsmall(board,4,false)==p&&haswonsmall(board,6,false)==p)return p;
	return NONE;
}

bool boardfull(uint8_t *board){
	int x,y;
	for(y=0;y<9;y++)for(x=0;x<9;x++)if(board[9*y+x]==NONE)return false;
	return true;
}

bool nonantfull(uint8_t *board,int non){
	int x,y;
	for(y=non/3*3;y<non/3*3+3;y++)for(x=non%3*3;x<non%3*3+3;x++)if(board[9*y+x]==NONE)return false;
	return true;
}

void moveIntoNonant(Move &move,int non){ //edits in-place
	move.x+=non%3*3;
	move.y+=non/3*3;
}

Move domove(uint8_t *board,Move othermove){
	Move lefttop,m;
	int non,idx;
	vector<pair<int,int>> empty;
	non=smallnonant(othermove);
	lefttop.x=lefttop.y=0;
	moveIntoNonant(lefttop,non);
	cerr<<"lefttop={"<<lefttop.x<<","<<lefttop.y<<"}"<<endl;
	if(nonantfull(board,non)){
		for(m.y=0;m.y<9;m.y++)
			for(m.x=0;m.x<9;m.x++)
				if(board[9*m.y+m.x]==NONE)empty.push_back({m.x,m.y});
	} else {
		for(m.y=lefttop.y;m.y<lefttop.y+3;m.y++)
			for(m.x=lefttop.x;m.x<lefttop.x+3;m.x++)
				if(board[9*m.y+m.x]==NONE)empty.push_back({m.x,m.y});
	}
	if(empty.size()==0){
		m.x=m.y=-2;
		return m;
	}
	idx=rand()%empty.size();
	m.x=empty[idx].first;
	m.y=empty[idx].second;
	return m;
}

int main(void){
	string line;
	Move move;
	int won;
	srand(time(NULL));
	uint8_t *board=new uint8_t[81]();
	getline(cin,line);
	if(line=="go"){
		cout<<"4 4"<<endl;
		board[9*4+4]=ME;
		printboard(board);
	} else if(line!="nogo"){
		cout<<"no0b"<<endl;
	}
	while(true){
		cin>>move.x>>move.y;
		if(!validatemove(move,board))cout<<"no0b, read {"<<move.x<<","<<move.y<<'}'<<endl;
		else board[9*move.y+move.x]=OTHER;
		if(boardfull(board)){
			cerr<<"Board is full due to other player!"<<endl;
			break;
		}
		move=domove(board,move);
		if(validatemove(move,board)){
			board[9*move.y+move.x]=ME;
			cout<<move.x<<' '<<move.y<<endl;
		} else {
			cerr<<"domove did an invalid move, namely {"<<move.x<<","<<move.y<<"}!"<<endl;
			cout<<"no0b"<<endl;
		}
		printboard(board);
		updateAllHaswonsmallStatics(board);
		won=haswon(board);
		if(won!=NONE){
			cerr<<"Player "<<won<<" won the game!"<<endl;
			break;
		}
		if(boardfull(board)){
			cerr<<"Board is full due to me!"<<endl;
			break;
		}
	}
	delete[] board;
	return 0;
}
