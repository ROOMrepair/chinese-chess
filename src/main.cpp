#include "raylib.h"
#include "resource_dir.hpp"	// utility header for SearchAndSetResourceDir
#include "GameState.hpp"
#include "Asset.hpp"
#include <iostream>

const int screenWidth = 800;
const int screenHeight = 600;

const char* resource_dir = "resources";
// 相对于工作目录
const char* conf_file = "./resources/source.conf";

int main ()
{
	// signal handler
	
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	InitWindow(screenWidth, screenHeight, "Hello Raylib");

	auto asset = Asset::getInstance();
	if(!asset->loadAssets(resource_dir,conf_file)){
		std::cerr << "Failed to load assets! Unload Assets" << std::endl;			
		return 1;
	}
	// generate h

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
