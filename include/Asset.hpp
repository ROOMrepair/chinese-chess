#pragma once

#include "raylib.h"
#include "resource_dir.hpp"
#include <array>
#include <memory>
#include <iostream>
#include <string>
#include <fstream>
#include <string>
#include <cstdlib>
#include "util.hpp"
#include "piece_index.hpp"

const int AssetCapacity = 128;
const int PieceCampNum = 16;

struct PieceTexture{
    std::string id;
    Texture2D texture;
};

class Asset
{
private:
    Asset(): assetNum(0){}; 
    int assetNum;

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
   
    int getNum(){ return assetNum; }

    ~Asset()
    {

        std::cout << "asset destroyed" << std::endl;
        unloadAssets();
    }

    std::array<PieceTexture, AssetCapacity> pieceTextures;
		
	bool loadAssets(const char* assetDir,const char* fileName);
    bool loadFont();
	void unloadAssets(); 
};
