#include "Board.hpp"

Board::Board(int bIndex,int bgIndex){
	boardIndex = bIndex;
	boardBgIndex = bgIndex;
}

Board::~Board(){
	std::cout << "Board destroyed" << std::endl;
}

void Board::drawBoard(int x,int y){
	auto asset = Asset::getInstance();
	auto t = asset->pieceTextures[boardIndex].texture;
	DrawTextureEx( t,{(float)x, (float)y},0,0.2f,WHITE);
}