#pragma once

#include "Asset.hpp"
#include "raylib.h"

const int P_WIDTH = 9;
const int P_HEIGHT = 9;

class Board{
	public :
		Board(int bIndex,int bgIndex);
		~Board();
		
		int boardIndex;
		int boardBgIndex;

		void drawBoard(int x,int y);

	private:
};