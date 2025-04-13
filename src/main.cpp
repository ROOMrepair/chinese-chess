#include <iostream>

#if defined(_WIN32)           

	#define NOGDI             // All GDI defines and routines
	#define NOUSER            // All USER defines and routines

	#define MMNOSOUND

	typedef struct tagMSG *LPMSG;						  
	#include <windows.h>

#endif

#include "raylib.h"
#include "resource_dir.hpp" // utility header for SearchAndSetResourceDir
#include "GameState.hpp"
#include "Asset.hpp"
#include "Notify.hpp"
#include "Event.hpp"
#include "util.hpp"
#include "pregen.hpp"

const char *resource_dir = "resources";
const char *conf_file = "./resources/source.conf";

int main()
{
	SetConsoleOutputCP(CP_UTF8);

	// signal handler
	
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	SetTargetFPS(60);

	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Raylib");

	auto &asset = Asset::getInstance();
	if (!asset.loadAssets(resource_dir, conf_file))
	{
		std::cerr << "Failed to load assets! Unload Assets" << std::endl;
		return 1;
	}

	genShortRangeMove();
	genLongRangeMove();
	
	// use state manager
	StateManager sm;	
	bool isRedSide = true;

	auto gst = StateManager::makeState<GameState>(sm,isRedSide);
	if(!gst)
	{
		std::cerr << "Failed to create GameState" << std::endl;
		return 1;
	}

	// start
	sm.stateStack.push(std::move(gst));
	
	#ifdef MYDEBUG
		bool redside = true;	
		int sd = 0; // redside
		std::cout << "redside: " << redside << " sd " << sd << std::endl;
		for(int sq = 125; sq <= 130;sq ++){
			std::cout << SQUARE_FORWARD(redside,sq,sd) << std::endl; 	
		}
		sd = 1;
		std::cout << "redside: " << redside << " sd " << sd << std::endl;
		for(int sq = 125; sq <= 130;sq ++){
			std::cout << SQUARE_FORWARD(redside,sq,sd) << std::endl; 	
		}
		redside = false,sd = 0;
		std::cout << "redside: " << redside << " sd " << sd << std::endl;
		for(int sq = 125; sq <= 130;sq ++){
			std::cout << SQUARE_FORWARD(redside,sq,sd) << std::endl; 	
		}
		sd = 1;
		std::cout << "redside: " << redside << " sd " << sd << std::endl;
		for(int sq = 125; sq <= 130;sq ++){
			std::cout << SQUARE_FORWARD(redside,sq,sd) << std::endl; 	
		}
	#endif

	while (!WindowShouldClose())
	{
		float frameTime = GetFrameTime();
		Vector2 pos = GetMousePosition();
		
		sm.Update(frameTime,pos);
		
		ButtonManager::getInstance().Update(pos);
		NotifyManager::getInstance().Update(frameTime);

		BeginDrawing();
		sm.Render(frameTime);

		ButtonManager::getInstance().Draw();
		NotifyManager::getInstance().Draw();

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
