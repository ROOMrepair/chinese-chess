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
#include "MenuState.hpp"
#include "Asset.hpp"
#include "Notify.hpp"
#include "Event.hpp"
#include "util.hpp"
#include "pregen.hpp"
#include "search.hpp"

#include <cstdlib>  // for rand(), srand()
#include <ctime> 

const char *resource_dir = "resources";
const char *conf_file = "./resources/source.conf";
const char *log_eval = "./board_eval.txt";
const char *zobrist_log= "./zobristLog.txt";


#ifdef SHOW_EVAL
	std::ofstream logout(log_eval);
#endif

#ifdef SHOW_ZB
	std::ofstream zobristout(zobrist_log);
#endif

int main()
{
	SetConsoleOutputCP(CP_UTF8);
	// signal handler
	
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	SetTargetFPS(60);

	srand(static_cast<unsigned int>(time(nullptr)));

	bool exitWindow = false;
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "chinese-chess");

	auto &asset = Asset::getInstance();
	if (!asset.loadAssets(resource_dir, conf_file))
	{
		std::cerr << "Failed to load assets! Unload Assets" << std::endl;
		return 1;
	}

	genZobrist();
	genShortRangeMove();
	genLongRangeMove();
	
	StateManager sm;	

	uint64_t limitedThinkingTime = 100000000;
	NewHashTable(24);
	
	MouseClickState mcs;
	Timer t;
	std::shared_ptr<Board> board = std::make_shared<Board>(mcs,t);

	if (!board->loadFromFen(cszStartFen))
    {
		std::cerr << "Failed to load fen" << std::endl;
		return 1;
    }

	auto gst = StateManager::makeState<GameState>(sm,mcs,t,board,limitedThinkingTime);
	auto mst = StateManager::makeState<MenuState>(sm,mcs,board);

	if(!mst)
	{
		std::cerr << "Failed to create MenuState" << std::endl;
		return 1;
	}

	if(!gst)
	{
		std::cerr << "Failed to create GameState" << std::endl;
		return 1;
	}

	// start
	sm.stateStack.push(std::move(gst));
	sm.stateStack.push(std::move(mst));
	sm.stateStack.top()->Trigger();

	DEBUG_("size of Board ",sizeof(Board));
	
#ifdef SHOW_EVAL
	if (!logout.is_open()){
		std::cerr << "Failed to open output file: " << log_eval << std::endl;
		return 1;
	}
#endif

#ifdef SHOW_ZB
	if (!zobristout.is_open()){
		std::cerr << "Failed to open output file: " << zobrist_log << std::endl;
		return 1;
	}
	
	zobristout << "player[dwKey, dwLock0, dwLock1]" 
				<< std::setw(11) << std::setfill(' ')
				<< std::to_string(zobrPlayer.dwKey) << " "
				<< std::to_string(zobrPlayer.dwLock0) << " "
				<< std::to_string(zobrPlayer.dwLock1) << "\n";

	for(int i = 0; i < 1;i ++){
		for(int y = rowOffset ; y <= rowOffsetTo;++y){
			for(int x = colOffset;x <= colOffsetTo; ++x){
				zobristout	<< std::setw(11) << std::setfill(' ') 
							<< std::to_string(zobrTable[i][SQPOS_xy(x,y)].dwKey) << " "
							<< std::setw(11) << std::setfill(' ')
							<< std::to_string(zobrTable[i][SQPOS_xy(x,y)].dwLock0) << " "
							<< std::setw(11) << std::setfill(' ')
							<< std::to_string(zobrTable[i][SQPOS_xy(x,y)].dwLock1) << "\n";
			}
		}
	}
#endif

	while (!exitWindow)
	{
		if(WindowShouldClose() 
			|| IsKeyPressed(KEY_ESCAPE)
			|| sm.forceExit) exitWindow = true;

		float frameTime = GetFrameTime();
		if (frameTime > 0.05f) frameTime = 0.05f;
		Vector2 pos = GetMousePosition();
		
		bool isHitButton = false;
		isHitButton = ButtonManager::getInstance().Update(pos);
		CounterManager::getInstance().Update(pos);
		NotifyManager::getInstance().Update(frameTime);

		sm.Update(frameTime,pos,isHitButton);

		BeginDrawing();
		sm.Render(frameTime);

		ButtonManager::getInstance().Draw();
		CounterManager::getInstance().DrawAll();
		NotifyManager::getInstance().Draw();

		EndDrawing();
	}

	// DelHashTable();
	CloseWindow();

#ifdef SHOW_EVAL
	logout.close();
#endif

#ifdef SHOW_ZB
	zobristout.close();
#endif
	return 0;
}
