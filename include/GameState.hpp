#pragma once

#include "Asset.hpp"
#include "Board.hpp"
#include "Event.hpp"
#include "Button.hpp"

class GameState
{
public:
	Board board;
	MouseClickState &mcs;

	GameState(MouseClickState &m,bool isRedSide);
	~GameState(){
		std::cout << "Gamestate destroyed" << std::endl;
	};
	
	void renderScene();
	void update(Vector2 pos);
	
	// game
	void SaveBoard();
};


