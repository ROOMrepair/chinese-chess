#pragma once

#include "Asset.hpp"
#include "Board.hpp"

class GameState
{
public:
	Board board;

	GameState();
	~GameState();

	void renderScene();
	void update();
};


