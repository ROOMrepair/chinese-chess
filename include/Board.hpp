#pragma once

#include <cstring>
#include <iomanip>
#include <functional>
#include <atomic>
#include <thread>
#include <memory>
#include <iostream>

#include "Asset.hpp"
#include "constant.hpp"
#include "pregen.hpp"
#include "Notify.hpp"
#include "Event.hpp"
#include "search.hpp"
#include "preeval.hpp"


extern char szFenState[256];

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

class Board: public std::enable_shared_from_this<Board>
{
public:
	Board(MouseClickState &ms,Timer &t, bool isredside,bool useai,int depth,bool isInnerBoard = false);
	Board(MouseClickState &ms,Timer &t,bool isInnerBoard = false);

	~Board();

	bool ISMENUMODE = false; 

	MouseClickState &mcs;
	Timer &tr;
	MovePieceAnimation moveAnim;

	bool isRedTurn = true; // 谁的回合
	bool isRedSide = false; // todo 玩家执子方,主视点位置。如果中途切换了呢?
	bool isExchangeSide = false;
	bool useAi = false;

	bool showTags = false;
	bool bKnowledge = true;
	bool gameOver = false;

	//! multi thread
	std::atomic_bool isThinking;
	std::thread search_thread_;		

	bool disCardSearchRes = false;
	int ndepth = 0;
	size_t searchNodes = 0;

	BoardInfo binfo;
	BoardPos activatedPos;	
	int activatedDisCardedPos = -1;
		
	uint8_t Squares[256];
	PieceType Pieces[48]; // 棋子标识，0表示被吃  16 - 47 非0标记当前位置
	
	// Pieces[piece_i_dst].alive = 0;
	// BitPiece ^= BIT_PIECE(piece_i_dst); 这两个的变化要同步
	uint32_t BitPiece;  // 32位的棋子位，0到31位依次表示序号为16到47的棋子是否还在棋盘上

  	uint16_t wBitRows[16];   // 位行数组，注意用法是"wBitRanks[RANK_Y(sq)]"
  	uint16_t wBitCols[16];   // 位列数组，注意用法是"wBitFiles[FILE_X(sq)]"
	std::vector<int> disCardedPieces = {};

  	ZobristStruct zobr;
	int lastmoveCur = 0;

	// evaluate 
	int evBlack;
	int evRed;
	
	int RedSidePiecePos(int sqPos) const{
		return isRedSide ? sqPos : SQUARE_FLIP(sqPos); 	
	}

	int BlackSidePiecePos(int sqPos) const{
		return isRedSide ? SQUARE_FLIP(sqPos) : sqPos;
	}
							  
	static int PieceStrMap(char c);
	static int PieceTextureMap(int piece);

	void FileBoard(){
		for(int i = 0;i < 255;i++){
			int j = SQUARE_FLIP(i);
			if( i < j ){
				std::swap(Squares[i],Squares[SQUARE_FLIP(i)]);
			}
		}				
		for(int i = 16;i < 48;++i){
			if(Pieces[i].alive > 0){
				Pieces[i].alive = SQUARE_FLIP(Pieces[i].alive);				
			}
		}
	}

  	SlideMoveStruct *RankMovePtr(int x, int y) const {
    	//? wbitRank[y] 取值为 0~511? y 表示棋子所在行
    	return SlideMoveRow[x - colOffset] + wBitRows[y];
  	}
  	SlideMoveStruct *FileMovePtr(int x, int y) const {
    	return SlideMoveCol[y - rowOffset] + wBitCols[x];
  	}
  	SlideMaskStruct *RankMaskPtr(int x, int y) const {
    	return SlideMaskRow[x - colOffset] + wBitRows[y];
  	}
  	SlideMaskStruct *FileMaskPtr(int x, int y) const {
    	return SlideMaskCol[y - rowOffset] + wBitCols[x];
  	}

	// init 
	void boardToFen(char* szFen);
	bool loadFromFen(const char *szFen);
	bool addPiece(int x, int y, int piece);
	void caculateBoardInfo();
	void clearBoard();
	bool isGameOver();

	// event
	bool checkClickActivation(Vector2 pos,BoardPos &bp);	

	// board render/update
	void drawBackground();
	void drawPieces(bool isDragging = false);
	void drawMarker(bool isDragging = false);
	Vector2 screenXY(int x,int y);
	
	// move 
	void preMove(bool fromDrag,Rectangle rec = {0,0,0,0}); 
	int move(int sqSrc,int sqDst,bool isCheck = true);
	void postMove();	
	
	void discard();

	int checkMate();
	bool checkMated();
	int chasedBy(int mv);
						
	void PieceFreePos(int pt,std::function<void(int,int,int)> cb);
	void pieceCap(int pt,int pos,std::function<void(int,int,int)> cb) const;
	void changeSide();
	void changeSideSimulation();

	// animation
	void updateMoveAnimation(float frameTime);
	void activateAnimation(int sqSrc, int sqDst,std::function<void()> cb);

	// todo evaluate 
	bool Protected(int sd,int sqSrc,int sqExcept = 0) const;
	void PreEvaluate();
	// ! 所有返回的评价值，都是直接针对当前的走子方
	int AdvisorShape() const;
	int StringHold(void) const;	
	int RookMobility(void) const;
	int KnightTrap(void) const;

	int Evaluate();
	int Evaluate(int vlAlpha, int vlBeta);
	int EvaluateLevel1(int vlAlpha, int vlBeta) const;
	int Material(void) const { // 子力平衡，包括先行权因素
	    return SIDE_VALUE(isRedTurn ? 0 : 1, evRed - evBlack) + PreEval.vlAdvanced;
	}
	
	// generate moves
	int MvvLva(int sqDst, int pcCaptured, int nLva);
  	int GenAllMoves(MoveStruct *lpmvsCap,MoveStruct *lpmvsNocap);
	void GetAllMoves(MoveSortStruct *msvs);

	bool SimulateMove(int mv);
	void UndoSimulateMove();


	int MakeMove(int sqSrc,int sqDst);
	void RollbackMove(int sqSrc,int sqDst,int sqSrcpt,int sqDstpt);

	void UndoLastMove();

  	void SaveStatus();
  	void Rollback();
  	MoveStruct LastMove() const { 
		//! 当前下标为 rbMoveNum, 直到makemove最后才会 更新 rbMOveNum
		//! 处理第一个位置,本方步数也需要存入
    	return rbsList[rbMoveNum - 1].mvs;
  	}

	void triggerSearch(){
		if(useAi && (isRedSide ^ isRedTurn)){
			doSearch(false);
		}
	}
	void doSearch(bool isDelay);
	int SearchMain(int nDepth);
	int SearchRoot(int nDepth);
	int Alpha_Beta_Raw(int ndepth,int alpha,int beta);
	int Minimax_Raw(int nDepth, bool isMaximizingPlayer);

	std::unique_ptr<Board> searchBoard;
	void resetSearchBoard(const Board& other);
	void postSearch(int bestmove);
	void stopSearch(bool discard = false);
};