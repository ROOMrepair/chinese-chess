#pragma once

#include <stack>
#include "State.hpp"

class StateMachine
{
private:
	std::stack<State*> states;

public:
	StateMachine();
	~StateMachine();

	State* createState();
};