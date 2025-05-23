#pragma once

#include <vector>
#include <memory>
#include "Asset.hpp"
#include "Board.hpp"
#include "Event.hpp"
#include "Button.hpp"
#include "Notify.hpp"
#include "State.hpp"
#include "search.hpp"
#include "MenuState.hpp"


class GameState: public State
{
public:

	MouseClickState &mcs;
	Timer &t;
	std::shared_ptr<Board> board;

	uint64_t limited_t;

	GameState(StateManager &sm,MouseClickState &_mcs,Timer &_t,std::shared_ptr<Board> b,uint64_t limitedTime);

	~GameState();

	void DrawTips();
	void Render(float frameTime);
	void Update(float frameTime,Vector2 pos,bool ishit = false);
	void Trigger();
	void onUnloadState();
	
	// game
	void SaveBoard();
	void gameOver();
};


