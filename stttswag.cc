#include <iostream>
#include <string>
#include <vector>
#include <climits>

using namespace std;

#define DEBUG

#ifdef DEBUG
#include <fstream>
ofstream ferr;
#endif

int getIndex( int alpha, int bx, int by ) {
	return (alpha / 3)*27 + 9*by + (alpha % 3)*3 + bx;
}

struct Move {
	int x, y;
	int getAlphaNonant();
	int getBetaNonant();
	bool isValid();
	Move( int a, int b ) : x(a), y(b) {}
	Move() : Move(-1,-1) {}
};

int Move::getAlphaNonant() {
	return (x / 3) + (y / 3)*3;
}

int Move::getBetaNonant() {
	return (x % 3) + (y % 3)*3;
}

bool Move::isValid() {
	return x >= 0 && y >=0 && x < 9 && y < 9;
}

class Board {
public:
	int move_count;
	int alpha_square[9];
	int beta_square[81];
	int alpha_target;
public:
	void doMove( Move );
	void undoMove( Move, int );
	vector<Move> generateMoves();
	int betaWin( int alpha );
	int alphaWin();
	void print();
	Board();
};

int evaluate( Board& board );
int minimax( Board& board, Move& best_move, int depth, int alpha = INT_MIN/2, int beta = INT_MAX/2 );

void Board::doMove( Move move ) {
	beta_square[move.x+9*move.y] = ( move_count % 2 ) + 1;
	move_count += 1;
	int a = move.getAlphaNonant();
	alpha_square[a] = betaWin( a );
	alpha_target = move.getBetaNonant();
}

void Board::undoMove( Move move, int alpha ) {
	beta_square[move.x+9*move.y] = 0;
	move_count -= 1;
	alpha_square[alpha] = 0;
	alpha_square[alpha] = betaWin( alpha );
	alpha_target = alpha;
}

vector<Move> Board::generateMoves() {
	vector<Move> move_list;
	Move move;
	for( int y = 0; y < 3; y++ )
		for( int x = 0; x < 3; x++ )
			if( beta_square[ getIndex(alpha_target, x, y) ] == 0 ) {
				move_list.push_back( Move( x+(alpha_target%3)*3, y+(alpha_target/3)*3 ));
			}
	if( move_list.empty() ) {
		#ifdef DEBUG
		ferr << "Full beta field" << endl;
		#endif
		for( int y = 0; y < 9; y++ )
			for( int x = 0; x < 9; x++ )
				if( beta_square[x+9*y] == 0 )
					move_list.push_back( Move( x, y ) );
	}
	return move_list;
}

int Board::betaWin( int alpha ) {
	if( alpha_square[alpha] != 0 )
		return alpha_square[alpha];

	int i = getIndex(alpha, 0, 0);
	for( int y = 0; y < 27; y += 9 ) {
		if( ( beta_square[i+y] & beta_square[i+y+1] & beta_square[i+y+2] ) != 0 )
			return beta_square[i+y];
	}
	for( int x = 0; x < 3; x++ ) {
		if( ( beta_square[i+x] & beta_square[i+x+9] & beta_square[i+x+18] ) != 0 )
			return beta_square[i+x];
	}
	if( ( beta_square[i] & beta_square[i+10] & beta_square[i+20] ) != 0 )
		return beta_square[i];
	if( ( beta_square[i+2] & beta_square[i+10] & beta_square[i+18] ) != 0 )
		return beta_square[i+2];
	return 0;
}

int Board::alphaWin() {
	for( int y = 0; y < 9; y += 3 ) {
		if( ( alpha_square[y] & alpha_square[y+1] & alpha_square[y+2] ) != 0 )
			return alpha_square[y];
	}
	for( int x = 0; x < 3; x++ ) {
		if( ( alpha_square[x] & alpha_square[x+3] & alpha_square[x+6] ) != 0 )
			return alpha_square[x];
	}
	if( ( alpha_square[0] & alpha_square[4] & alpha_square[8] ) != 0 )
		return alpha_square[0];
	if( ( alpha_square[2] & alpha_square[4] & alpha_square[6] ) != 0 )
		return alpha_square[2];
	return 0;
}

void Board::print() {
	cerr << "0 | ";
	for( int i = 0; i < 81; ++i ) {
		if( beta_square[i] == 0 )
			cerr << ". ";
		else if( beta_square[i] == 1 )
			cerr << "O ";
		else 
			cerr << "X ";
		if( (i % 9) == 8 ) {
			if( (i % 27 ) == 26 )
				cerr << "\n  +-------+-------+-------";
			if( i < 80 )
				cerr << "\n" << ((i+1) / 9) << " ";
		} 
		if( (i % 3) == 2 && i < 80 )
			cerr << "| ";
	}
	cerr << endl << "    ";
	for( int i = 0 ; i < 9; ++i ) {
		cerr << i << " ";
		if( (i % 3) == 2 )
			cerr << "  ";
	}
	cerr << endl;
	cerr << "score: " << evaluate( *this ) << endl;
}

Board::Board() {
	move_count = 0;
	for( int i = 0; i < 9; i++ )
		alpha_square[i] = 0;
	for( int i = 0; i < 81; i++ )
		beta_square[i] = 0;
	alpha_target = -1;
}

int evaluate( Board& board ) {
	int alpha_count = 0;
	int score = 0;
	for( int i = 0; i < 9; i++ )
		if( board.alpha_square[i] != 0)
			alpha_count += ( 3 - 2*board.alpha_square[i] ) * ( 1 + ( i == 4 ) );
	score += alpha_count * 200;
	for( int i = 0; i < 81; i++ ) 
		if( board.beta_square[i] != 0)
			score += ( 3 - 2*board.beta_square[i] ) * (1 + ((i % 3) == 1) + (((i / 9) % 3) == 1) );
	return ( board.move_count % 2 == 0 ) ? score : -score;
}

int minimax( Board& board, Move& best_move, int depth, int alpha, int beta ) {
	if( board.alphaWin() )
		return INT_MIN/2;
	else if( depth == 0 )
		return evaluate( board );
	else {
		int global_score = INT_MIN;
		int local_score;
		int prev_alpha;
		Move best_move_recursive;
		vector<Move> move_list = board.generateMoves();
		if( move_list.size() == 0 )
			return INT_MAX/2;
		for( Move& move : move_list ) {
			prev_alpha = move.getAlphaNonant();
			board.doMove( move );
			local_score = -minimax( board, best_move_recursive, depth-1, -beta, -alpha );
			board.undoMove( move, prev_alpha );
			if( local_score > global_score ) {
				global_score = local_score;
				best_move = move; 
			}
			if( local_score > alpha )
				alpha = local_score;
			if( beta <= alpha )
				break;
		}
		return global_score;
	}
}

Move importMove() {
	Move move;
	#ifdef VISUAL
	cerr << ">";
	#endif
	cin >> move.x >> move.y;
	return move;
}

void exportMove( Move move ) {
	#ifdef VISUAL
	cerr << ">";
	#endif
	cout << move.x << " " << move.y << endl;
}

int main() {
	Board board;
	Move move;
	string input;

	#ifdef DEBUG
	ferr.open("error.log");
	#endif

	cin >> input;
	if( input == "go" ) {
		#ifdef VISUAL
		board.print();
		#endif
		board.doMove( Move( 4, 4 ) );
		exportMove( Move( 4, 4 ) );
	}
	while( !board.alphaWin() ) {
		#ifdef VISUAL
		board.print();
		#endif
		board.doMove( importMove() );
		#ifdef DEBUG
		for( int i = 0; i < 9; ++i )
			ferr << board.alpha_square[i] << " ";
		ferr << endl;
		#endif
		if( board.alphaWin() )
			break;
		#ifdef VISUAL
		board.print();
		#endif
		minimax( board, move, 9 );
		board.doMove( move );
		#ifdef DEBUG
		for( int i = 0; i < 9; ++i )
			ferr << board.alpha_square[i] << " ";
		ferr << endl;
		#endif
		exportMove( move );
	}
	board.print();
}