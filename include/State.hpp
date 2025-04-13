#pragma once

#include <stack>
#include <memory>
#include <stdexcept>

class StateManager;

class State
{
public:
	State(StateManager &m):stateManager(m){};

	State(const State&) = delete;
	State& operator=(const State&) = delete;

	virtual	~State() = default;

	virtual void Update(float deltaTime,Vector2 pos) = 0;

	virtual void Render(float deltaTime) = 0;
	
protected:
	StateManager &stateManager;
};

class StateManager{
public:
	StateManager() = default;
	~StateManager() = default;

	std::stack<std::unique_ptr<State>> stateStack;

	// make new state
	template<typename T,typename... Args>
	static std::unique_ptr<T> makeState(Args&&... args)
	{
		auto state = std::unique_ptr<T>{nullptr};

		try
		{
			state = std::make_unique<T>(std::forward<Args>(args)...);	
		}
		catch(std::runtime_error& e)
		{
			std::cout << "Creation of new state was unsuccessful\n";
			std::cerr << e.what() << '\n';
		}
		
		return state;
	}
	
	void Update(float deltaTime,Vector2 pos)
	{
		stateStack.top()->Update(deltaTime,pos);		
	}

	void Render(float deltaTime)
	{
		stateStack.top()->Render(deltaTime);
	}
};