#pragma once

#include "Asset.hpp"
#include "Board.hpp"
#include "Event.hpp"

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
	void update();
};


