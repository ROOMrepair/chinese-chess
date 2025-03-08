#pragma once

#include <cstring>
#include "Asset.hpp"
#include "constant.hpp"

// 0 / 1 red / black
inline int SIDE_TAG(int sd) {
  return 16 + (sd << 4);
}

class Board{
	public :
		Board();
		~Board();

		bool isRedTurn;	
		 
  		uint8_t Squares[256]; // 每个格子放的棋子，0表示没有棋子
  		uint8_t Pieces[48];   // 每个棋子放的位置，0表示被吃  16 - 47

		bool loadFromFen(const char* szFen);
		int PieceStrMap(char c);
		void drawBoard(int x,int y);
		void clearBoard();
					
};

