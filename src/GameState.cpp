#include "GameState.hpp"


GameState::GameState()
{

}

GameState::~GameState()
{
	std::cout << "Gamestate destroyed" << std::endl;
}

void GameState::renderScene()
{
	board.drawBoard(0,0);
	// drawPieces();
}

void GameState::update()
{
	
}