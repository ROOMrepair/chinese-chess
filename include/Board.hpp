#pragma once

#include <cstring>
#include "Asset.hpp"
#include "constant.hpp"
#include "pregen.hpp"
#include <functional>

// 0 / 1 red / black
inline int SIDE_TAG(int sd)
{
	return 16 + (sd << 4);
}

struct PieceType
{
	int alive;
	int textureIndex;
};

struct BoardPos
{
	int x;
	int y;
};

struct BoardInfo
{
	// board
	float boardWidth_t;
	float boardHeight_t;

	float bgWidth_t;
	float bgHeight_t;

	float boardWidth;
	float boardHeight;

	float bgWidth;
	float bgHeight;	

	float gridWidth;
	float gridHeight;

	// piece	
	float pieceScale = 1.0f;	
	float radius = 20.0f;
	// 画面上的board的左上角坐标
	float BoardOriginX = 0;
	float BoardOriginY = 0;
};

class Board
{
public:
	Board(bool isredside);
	~Board();

	bool isRedTurn;
	bool isRedSide;
	bool isExchangeSide;

	BoardInfo binfo;
	BoardPos activatedPos;	
		
	Vector2 dragStartPos;
	Vector2 dragMousePos;

	uint8_t Squares[256]; // 每个格子放的棋子，0表示没有棋子 从此处获取坐标?
	PieceType Pieces[48]; // 棋子标识，0表示被吃  16 - 47

	// 这两个数组相当于拆分记录了棋盘数组的行列位置
	// 每一步操作都需要更新
  	uint16_t wBitRows[16];   // 位行数组，注意用法是"wBitRanks[RANK_Y(sq)]"
  	uint16_t wBitCols[16];   // 位列数组，注意用法是"wBitFiles[FILE_X(sq)]"
							  
	static int PieceStrMap(char c);
	static int PieceTextureMap(int piece);

	// init 
	bool loadFromFen(const char *szFen);
	bool addPiece(int x, int y, int piece);
	bool caculateBoardInfo();
	void clearBoard();

	bool checkClickActivation(Vector2 pos,BoardPos &bp);	
	void handleDragEvent(Vector2 pos);
	void handleReleaseEvent(Vector2 pos);

	// board render 
	void drawBackground();
	void drawPieces(bool isDragging = false);
	void drawMarker(bool isDragging = false);
	Vector2 screenXY(int x,int y);
	// void filpBoard();
	
	// 
	void move();	
	void pieceCap(int pt,int pos,std::function<void(int,int)> cb);

	//  debug
	void printPieces();
	void printSquares();
	void printBoard();
};