#pragma once

#include "raylib.h"
#include "resource_dir.hpp"
#include <array>
#include <iostream>
#include <string>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>

#include "util.hpp"
#include "piece_index.hpp"
#include "constant.hpp"

const int AssetCapacity = 128;
const int FontCapacity = 5;

enum class ResourceType{
    IMAGE,
    FONT,
    UNKNOWN
};

struct PieceTexture{
    std::string id;
    Texture2D texture;
};

struct FontTexture{
    std::string id;
    Font font;
};

class Asset
{
private:
    Asset(): assetNum(0),fontNum(0){}; 

public:
    static Asset& getInstance()
    {
        // 静态局部变量，静态储存区内分配储存单元。在整个程序运行期间都不释放
        static Asset instance;
		return instance;
    }

    // 禁用拷贝构造函数和赋值运算符
    Asset(const Asset&) = delete;
    Asset& operator=(const Asset&) = delete;

    ~Asset()
    {
        std::cout << "asset destroyed" << std::endl;
        unloadAssets();
    }

    std::array<PieceTexture, AssetCapacity> pieceTextures;
    std::array<FontTexture,FontCapacity> fonts;
		
    int assetNum;
    int fontNum;

	bool loadAssets(const char* assetDir,const char* fileName);
	void unloadAssets(); 
};
