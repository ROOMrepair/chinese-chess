#pragma once

#include "raylib.h"
class State
{
protected:

public:
	State(){
		
	};

	virtual ~State() = default; 
	
	virtual void renderScene() = 0;
	virtual void update() = 0;
};
