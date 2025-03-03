#include "GameState.hpp"


GameState::GameState()
{
	board = std::make_unique<Board>(BOARD_INDEX,BOARD_BG_INDEX);
}

GameState::~GameState()
{
	std::cout << "Gamestate destroyed" << std::endl;
}

void GameState::renderScene()
{
	board->drawBoard(0,0);
	// drawPieces();
}

void GameState::update()
{
	
}