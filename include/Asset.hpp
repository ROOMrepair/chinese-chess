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

// enum PieceType
// {
// 	BING = 0,
// 	PAO,
// 	CHE,
// 	MA,
// 	XIANG,
// 	SHI,
// 	JIANG,
// };

// enum Camp
// {
// 	Red = 0,
// 	Black,
// };

// template <typename T>
// constexpr const char* EnumMap(T m_key) noexcept {
//     constexpr std::array<std::pair<T, const char*>, 7> PieceTypeStrings{{
//         {PieceType::BING, "BING"},
//         {PieceType::PAO, "PAO"},
//         {PieceType::CHE, "CHE"},
//         {PieceType::MA, "MA"},
//         {PieceType::XIANG, "XIANG"},
//         {PieceType::SHI, "SHI"},
//         {PieceType::JIANG, "JIANG"}
//     }};
//     constexpr std::array<std::pair<Camp, const char*>, 2> CampStrings{{
//         {Camp::Red, "RED"},
//         {Camp::Black, "BLACK"}
//     }};

//     if constexpr (std::is_same_v<T, PieceType>) {
//         for (const auto& [k, v] : PieceTypeStrings) {
//             if (k == m_key) return v;
//         }
//     }
//     else if constexpr (std::is_same_v<T, Camp>) {
//         for (const auto& [k, v] : CampStrings) {
//             if (k == m_key) return v;
//         }
//     } 

//     return "";
// }

// inline const char* BOARD = "BOARD";
// inline const char* BOARD_BG = "BOARD_BG";

struct PieceTexture{
    std::string id;
    Texture2D texture;
};

class Asset
{
private:
    static std::shared_ptr<Asset> instance;
    Asset(): assetNum(0){}; 
    int assetNum;

public:
    static std::shared_ptr<Asset>& getInstance()
    {
        // 静态局部变量，静态储存区内分配储存单元。在整个程序运行期间都不释放
        static std::shared_ptr<Asset> instance(new Asset());
		return instance;
    }


    // 禁用拷贝构造函数和赋值运算符
    Asset(const Asset&) = delete;
    Asset& operator=(const Asset&) = delete;

    static void destroyInstance()
    {
        instance.reset();   
    } 
   
    int getNum(){ return assetNum; }

    ~Asset()
    {

        std::cout << "asset destroyed" << std::endl;
        unloadAssets();
    }

    std::array<PieceTexture, AssetCapacity> pieceTextures;
		
	bool loadAssets(const char* assetDir,const char* fileName);
	void unloadAssets(); 
};
