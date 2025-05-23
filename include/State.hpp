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

	virtual void Update(float deltaTime,Vector2 pos,bool sign = false) = 0;

	virtual void Render(float deltaTime) = 0;
	
	virtual void Trigger() = 0;
	virtual void onUnloadState() = 0;
	
	std::unique_ptr<State> next()
	{
		return std::move( m_next );
	}

protected:
	StateManager &stateManager;
	std::unique_ptr<State> m_next;
};

class StateManager{
public:
	StateManager() = default;
	~StateManager(){
		std::cout << "StateManager destroyed" << std::endl;
	};

	std::stack<std::unique_ptr<State>> stateStack;
	bool resume = false;
	bool forceExit = false;

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
	
	// use next state to create new state push state
	// last state to go back to last state pop state
	void switchState(){
		if(resume){
			if(!stateStack.empty()){
				stateStack.pop();	
			}	
			if(!stateStack.empty()){
				stateStack.top()->Trigger();
			}
			resume = false;
		}	

		if(!stateStack.empty()){
			auto tn = stateStack.top()->next();
			if(tn != nullptr){
				tn->Trigger();
				stateStack.push(std::move(tn));
			}
		}	
	}

	void Update(float deltaTime,Vector2 pos,bool ishit)
	{
		switchState();
		if(!stateStack.empty()){
			stateStack.top()->Update(deltaTime,pos,ishit);		
		}
	}

	void Render(float deltaTime)
	{
		if(!stateStack.empty()){
			stateStack.top()->Render(deltaTime);
		}
	}
};