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

using namespace std;

struct Move{
	short x=0,y=0;
};

int maxdepth=7;

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
	if(p!=NONE&&haswonsmall(board,4,false)==p&&haswonsmall(board,7,false)==p)return p;
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

#define SCORE_WIN (8)
#define SCORE_SMALLWIN (4)
#define SCORE_IDENTITYMAKESTHREE (2)

long long domoveRecurseOther(uint8_t*,int,int);
long long domoveRecurseMe(uint8_t*,int,int);

unsigned long long numHeuristicCalled=0;

long long heuristic(uint8_t *board,int player){
	int won=haswon(board);
	long long pts;
	numHeuristicCalled++;
	if(won==player)pts=SCORE_WIN;
	else if(won==3-player)pts=-SCORE_WIN;
	else pts=0;

#define HEURISTIC_REP_IDENTITYMAKESTHREE_CHECK(non,identity) \
		if(board[identity]==NONE&&haswonsmall(board,non,false)==NONE){ \
			board[identity]=player; \
			if(haswonsmall(board,non,false)==ME&&!haswonsmallDidQueryCache)pts+=SCORE_IDENTITYMAKESTHREE; \
			board[identity]=NONE; \
		}

	HEURISTIC_REP_IDENTITYMAKESTHREE_CHECK(0,0)
	HEURISTIC_REP_IDENTITYMAKESTHREE_CHECK(1,4)
	HEURISTIC_REP_IDENTITYMAKESTHREE_CHECK(2,8)
	HEURISTIC_REP_IDENTITYMAKESTHREE_CHECK(3,36)
	HEURISTIC_REP_IDENTITYMAKESTHREE_CHECK(4,40)
	HEURISTIC_REP_IDENTITYMAKESTHREE_CHECK(5,44)
	HEURISTIC_REP_IDENTITYMAKESTHREE_CHECK(6,72)
	HEURISTIC_REP_IDENTITYMAKESTHREE_CHECK(7,76)
	HEURISTIC_REP_IDENTITYMAKESTHREE_CHECK(8,80)

#undef HEURISTIC_REP_IDENTITYMAKESTHREE_CHECK

	pts+=100*(rand()%3-1);

	return pts;
}
/*long long heuristic(uint8_t *board,int player){
	int won=haswon(board);
	if(won==player)return SCORE_WIN;
	else if(won==3-player)return -SCORE_WIN;
	else return 5*(rand()%3-1);
}*/
//long long heuristic(uint8_t *board,int player){numHeuristicCalled++;return rand()%3-1;}

Move domove(uint8_t *board,Move othermove){
	Move lefttop,m,maxat[63];
	int non,maxatidx,won,searchsize;
	long long pts,max;
	numHeuristicCalled=0;
	non=smallnonant(othermove);
	lefttop.x=lefttop.y=0;
	moveIntoNonant(lefttop,non);
	cerr<<endl<<"maxdepth="<<maxdepth<<endl;
	cerr<<"lefttop={"<<lefttop.x<<","<<lefttop.y<<"}"<<endl;
	for(maxatidx=0;maxatidx<9;maxatidx++)maxat[maxatidx].x=maxat[maxatidx].y=-1;
	maxatidx=0;
	max=-1e9;
	if(nonantfull(board,non)){
		lefttop.x=lefttop.y=0;
		searchsize=9;
		cerr<<"Nonant full, taking full board and less depth"<<endl;
	} else searchsize=3;
	for(m.y=lefttop.y;m.y<lefttop.y+searchsize;m.y++){
		for(m.x=lefttop.x;m.x<lefttop.x+searchsize;m.x++){
			if(board[9*m.y+m.x]==NONE){
				won=haswonsmall(board,non,false);
				board[9*m.y+m.x]=ME;
				pts=haswonsmall(board,non,false);
				if(won!=NONE)pts=0;
				else if(pts==ME)pts=SCORE_SMALLWIN;
				else if(pts==OTHER)pts=-SCORE_SMALLWIN;
				else pts=0;
				pts+=-0.9*domoveRecurseOther(board,smallnonant(m),maxdepth-1-2*(searchsize==9));
				cerr<<"{"<<m.x<<","<<m.y<<"}: pts="<<pts<<endl;
				if(pts==max){
					maxat[maxatidx].x=m.x;
					maxat[maxatidx].y=m.y;
					maxatidx++;
				} else if(pts>max){
					maxat[0].x=m.x;
					maxat[0].y=m.y;
					max=pts;
					maxatidx=1;
				}
				board[9*m.y+m.x]=NONE;
			}
		}
	}
	cerr<<"numHeuristicCalled="<<numHeuristicCalled<<" maxatidx="<<maxatidx<<endl;
	if(maxatidx==0){
		m.x=m.y=-2;
		return m;
	} else if(maxatidx==1){
		return maxat[0];
	} else {
		return maxat[rand()%maxatidx];
	}
}

long long domoveRecurseOther(uint8_t *board,int non,int depth){
	//cerr<<"dmROther("<<depth<<") entry"<<endl;
	Move lefttop,m,maxat;
	long long pts,max;
	int won,searchsize;
	if(depth<=0){
		return heuristic(board,OTHER);
	}
	lefttop.x=lefttop.y=0;
	if(nonantfull(board,non)){
		searchsize=9;
	} else {
		moveIntoNonant(lefttop,non);
		searchsize=3;
	}
	maxat.x=maxat.y=-1;
	max=-1e9;
	for(m.y=lefttop.y;m.y<lefttop.y+3;m.y++){
		for(m.x=lefttop.x;m.x<lefttop.x+3;m.x++){
			if(board[9*m.y+m.x]==NONE){
				won=haswonsmall(board,non,false);
				board[9*m.y+m.x]=OTHER;
				pts=haswonsmall(board,non,false);
				if(won!=NONE)pts=0;
				else if(pts==OTHER)pts=SCORE_SMALLWIN;
				else if(pts==ME)pts=-SCORE_SMALLWIN;
				else pts=0;
				pts+=-0.9*domoveRecurseMe(board,smallnonant(m),depth-1-(searchsize==9));
				if(pts>max){
					maxat.x=m.x;
					maxat.y=m.y;
					max=pts;
				}
				board[9*m.y+m.x]=NONE;
			}
		}
	}
	//cerr<<"dmROther("<<depth<<"): returning max="<<max<<" at maxat={"<<maxat.x<<","<<maxat.y<<"}"<<endl;
	return max;
}
/*long long domoveRecurseOther(uint8_t *board,int non,int depth){
	//cerr<<"dmROther("<<depth<<") entry"<<endl;
	Move lefttop,m;
	long long pts,total=0;
	int num=0;
	if(depth<=0){
		return heuristic(board,OTHER);
	}
	lefttop.x=lefttop.y=0;
	moveIntoNonant(lefttop,non);
	for(m.y=lefttop.y;m.y<lefttop.y+3;m.y++){
		for(m.x=lefttop.x;m.x<lefttop.x+3;m.x++){
			if(board[9*m.y+m.x]==NONE){
				board[9*m.y+m.x]=OTHER;
				pts=haswonsmall(board,non,false);
				if(haswonsmallDidQueryCache)pts=0;
				else if(pts==OTHER)pts=SCORE_SMALLWIN;
				else if(pts==ME)pts=-SCORE_SMALLWIN;
				else pts=0;
				pts+=-0.9*domoveRecurseMe(board,nonant(m),depth-1);
				total+=pts;
				num++;
				board[9*m.y+m.x]=NONE;
			}
		}
	}
	if(num==0)return rand()%3-1;
	return total/num;
}*/

long long domoveRecurseMe(uint8_t *board,int non,int depth){
	//cerr<<"dmRMe("<<depth<<") entry"<<endl;
	Move lefttop,m,maxat;
	long long pts,max;
	int won,searchsize;
	if(depth<=0){
		return heuristic(board,ME);
	}
	lefttop.x=lefttop.y=0;
	if(nonantfull(board,non)){
		searchsize=9;
	} else {
		moveIntoNonant(lefttop,non);
		searchsize=3;
	}
	maxat.x=maxat.y=-1;
	max=-1e9;
	for(m.y=lefttop.y;m.y<lefttop.y+3;m.y++){
		for(m.x=lefttop.x;m.x<lefttop.x+3;m.x++){
			if(board[9*m.y+m.x]==NONE){
				won=haswonsmall(board,non,false);
				board[9*m.y+m.x]=ME;
				pts=haswonsmall(board,non,false);
				if(won!=NONE)pts=0;
				else if(pts==ME)pts=SCORE_SMALLWIN;
				else if(pts==OTHER)pts=-SCORE_SMALLWIN;
				else pts=0;
				pts+=-0.9*domoveRecurseOther(board,smallnonant(m),depth-1-(searchsize==9));
				if(pts>max){
					maxat.x=m.x;
					maxat.y=m.y;
					max=pts;
				}
				board[9*m.y+m.x]=NONE;
			}
		}
	}
	//cerr<<"dmRMe("<<depth<<"): returning max="<<max<<" at maxat={"<<maxat.x<<","<<maxat.y<<"}"<<endl;
	return max;
}

int main(void){
	string line;
	Move move;
	int won,diff_usec;
	char c;
	struct timeval tv_start,tv_end;
	gettimeofday(&tv_start,NULL);
	cerr<<"seed="<<(1000000*tv_start.tv_sec+tv_start.tv_usec)<<endl;
	srand(1000000*tv_start.tv_sec+tv_start.tv_usec);
	//srand(1430043526520409);
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
		c=cin.peek();
		if(c=='q'||c=='Q')break;
		cin>>move.x>>move.y;
		gettimeofday(&tv_start,NULL);
		if(!validatemove(move,board))cout<<"no0b, read {"<<move.x<<","<<move.y<<'}'<<endl;
		else board[9*move.y+move.x]=OTHER;
		if(boardfull(board)){
			cerr<<"Board is full due to other player!"<<endl;
			break;
		}
		move=domove(board,move);
		cerr<<"domove returned move="<<(9*move.y+move.x)<<": {"<<move.x<<","<<move.y<<'}'<<endl;
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
		gettimeofday(&tv_end,NULL);
		/*diff_usec=1e6*(tv_end.tv_sec-tv_start.tv_sec)+tv_end.tv_usec-tv_start.tv_usec;
		if(diff_usec<25e4&&maxdepth<10)maxdepth++; //25e4==0.25e6
		else if(diff_usec>1e6)maxdepth--;*/
	}
	delete[] board;
	return 0;
}
