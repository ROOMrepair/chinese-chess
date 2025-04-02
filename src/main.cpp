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

	DEBUG_("Hello Raylib",1,3.13,"\n");

	SetConsoleOutputCP(CP_UTF8);

	// signal handler
	
	// Tell the window to use vsync and work on high DPI displays
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
	
	bool isRedSide = false;

	MouseClickState mcs;
	GameState gst(mcs,isRedSide);

	if (!gst.board.loadFromFen(cszStartFen))
	{
		std::cout << "Failed to load from fen" << std::endl;
		return 1;
	}

	// DEBUG_("is exchangeside %d\n",gst.board.isExchangeSide);
	// DEBUG_("is redside %d\n",gst.board.isRedSide);

	// gst.board.printPieces();
	// DEBUG_("-----\n");
	// gst.board.printBoard();

	while (!WindowShouldClose())
	{
		float frameTime = GetFrameTime();
		Vector2 pos = GetMousePosition();
		
		// if(IsKeyPressed('N')){
		// 	NotifyManager::getInstance().Add(
		// 		{
		// 			u8"此处是一个非法目标",
		// 		});
		// }

		gst.update(pos);
		ButtonManager::getInstance().Update(pos);
		NotifyManager::getInstance().Update(frameTime);

		BeginDrawing();

		gst.renderScene();
		ButtonManager::getInstance().Draw();
		NotifyManager::getInstance().Draw();

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
