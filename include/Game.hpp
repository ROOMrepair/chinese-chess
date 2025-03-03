#pragma once
#include "StateMachine.hpp"

class Game
{
private:
	StateMachine* machine;	
	
public:
	Game();
	~Game();
	

	void run();
};

