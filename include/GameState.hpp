#pragma once

#include "Asset.hpp"
#include "Board.hpp"
#include "Event.hpp"
#include "Button.hpp"
#include "Notify.hpp"
#include "State.hpp"

class GameState: public State
{
public:

	MouseClickState mcs;
	Board board;

	GameState(StateManager &sm,bool isRedSide);
	~GameState(){
		std::cout << "Gamestate destroyed" << std::endl;
	};
	
	void DrawTips();
	void Render(float frameTime);
	void Update(float deltaTime,Vector2 pos);
	
	// game
	void SaveBoard();
};


