#pragma once

#include <cstring>
#include <functional>
#include <bitset>

#include "Asset.hpp"
#include "constant.hpp"
#include "pregen.hpp"
#include "Notify.hpp"
#include "Event.hpp"
#include "search.hpp"

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

struct MovePieceAnimation{
	bool isAnimating = false;
	Vector2 startPos;
	Vector2 endPos;

	int sqSrc;
	int sqDst;

	float duration = 0.5f;
	float progress = 0.0f;
	float elapsedTime = 0.0f;
	
	std::function<void()> onComplete = nullptr;
};

class Board
{
public:
	Board(MouseClickState &ms, bool isredside);
	~Board();

	MouseClickState &mcs;
	MovePieceAnimation moveAnim;

	bool isRedTurn; // 谁的回合
	bool isRedSide; // todo 玩家执子方,主视点位置。如果中途切换了呢?
	bool isExchangeSide;

	BoardInfo binfo;
	BoardPos activatedPos;	
		
	uint8_t Squares[256];
	PieceType Pieces[48]; // 棋子标识，0表示被吃  16 - 47 非0标记当前位置

  	uint16_t wBitRows[16];   // 位行数组，注意用法是"wBitRanks[RANK_Y(sq)]"
  	uint16_t wBitCols[16];   // 位列数组，注意用法是"wBitFiles[FILE_X(sq)]"
					
	// evaluate 
	// 子力评价
	int evBlack;
	int evRed;
	
							  
	static int PieceStrMap(char c);
	static int PieceTextureMap(int piece);

  	SlideMaskStruct *RankMaskPtr(int x, int y) const {
    	return SlideMaskRow[x - colOffset] + wBitRows[y];
  	}
  	SlideMaskStruct *FileMaskPtr(int x, int y) const {
    	return SlideMaskCol[y - rowOffset] + wBitCols[x];
  	}

	// init 
	bool loadFromFen(const char *szFen);
	bool addPiece(int x, int y, int piece);
	bool caculateBoardInfo();
	void clearBoard();

	// event
	bool checkClickActivation(Vector2 pos,BoardPos &bp);	
	void handleDragEvent(Vector2 pos);
	void handleReleaseEvent(Vector2 pos);

	// board render 
	void drawBackground();
	void drawPieces(bool isDragging = false);
	void drawMarker(bool isDragging = false);
	Vector2 screenXY(int x,int y);
	
	// move 
	void preMove(bool fromDrag); // use dragstartpos and dragmovepos
	int checkMateMove(int sqSrc,int sqDst,bool isCapture);
	int checkMated(int sqSrc,int sqDst,bool isCapture);

	int move(int sqSrc,int sqDst);
	void postMove();	

	int capture(int sqSrc,int sqDst);
	void postCapture();
						
	void pieceCap(int pt,int pos,std::function<void(int,int)> cb);
	void changeSide();
	void unDo();

	// animation
	void updateMoveAnimation(float frameTime);
	void activateAnimation(int sqSrc, int sqDst,bool isCapture);

	// todo evaluate 
	bool Proteced(int sd,int sqSrc,int sqExcept);
	int AdvisorShape();
	int evaluateLevel1();

	//  debug
	void printPieces();
	void printSquares();
	void printBoard();
};