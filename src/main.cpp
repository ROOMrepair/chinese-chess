#include "raylib.h"
#include "resource_dir.hpp"	// utility header for SearchAndSetResourceDir
#include "GameState.hpp"
#include "Asset.hpp"
#include "constant.hpp"
#include <iostream>

const char* resource_dir = "resources";
const char* conf_file = "./resources/source.conf";

int main ()
{
	// signal handler
	
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	SetTargetFPS(60);

	InitWindow(SCREEN_WIDTH,SCREEN_HEIGHT, "Hello Raylib");

	auto asset = Asset::getInstance();
	if(!asset->loadAssets(resource_dir,conf_file)){
		std::cerr << "Failed to load assets! Unload Assets" << std::endl;			
		return 1;
	}

	GameState g_state;

	while (!WindowShouldClose())
	{
		BeginDrawing();
		g_state.renderScene();
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
