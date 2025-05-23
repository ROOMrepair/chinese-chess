#pragma once

#include <iostream>

#include "raylib.h"
#include "Board.hpp"
#include "Event.hpp"
#include "Button.hpp"
#include "State.hpp"
#include "Counter.hpp"

class MenuState: public State
{
public:

	MenuState(StateManager &sm,MouseClickState &_mcs,std::shared_ptr<Board> b);
	~MenuState(){
		std::cout << "MenuState destroyed" << std::endl;
	};

	MouseClickState &mcs;
	std::shared_ptr<Board> tempBoard;
	// Timer &t;
	// radius:20 32  10 40 10 40 10 40 10 40 10  40 * 8 + 9 * 10 = 410 
	// index = (num / 4) + (num % 4) - 1
	Rectangle discardedArea = {550,160,210,410};
	
	// temp BoardStates for borad
	bool isRedSide = true;
	bool useAi = false;
	int depth = 0;

	void DrawTips();
	void DrawDiscardedMarker(bool isDragging = false);
	void DrawDiscardedPieces(bool isDragging = false);


	void Render(float frameTime);
	void Update(float frameTime,Vector2 pos,bool ishit = false);
	void Trigger();
	void onUnloadState();

	bool checkDisCardedClick(Vector2 pos,int *activated);
};
