#pragma once

#include <stdint.h>
#include <memory>
#include <vector>
#include <cstring>

#include "pregen.hpp"

const int MAX_GEN_MOVES = 128;  // 搜索的最大着法数，中国象棋的任何局面都不会超过120个着法
const int MAX_MOVE_NUM = 2048;
const int REP_HASH_MASK = 4095;

const int MATE_VALUE = 10000;
const int WIN_VALUE = MATE_VALUE - 200;

const int SORT_VALUE_MAX = 65535;
const int LIMIT_DEPTH = 64;

const int HASH_BETA = 1;
const int HASH_ALPHA = 2;
const int HASH_PV = HASH_ALPHA | HASH_BETA;

static const int cnShellStep[8] = {0, 1, 4, 13, 40, 121, 364, 1093};

union MoveStruct
{
	uint32_t dwmv;
	struct
	{
		// wvl 表示着法的朴素 mvv 得分
		// 不吃子着法似乎没有分
		uint16_t wmv, wvl; // wmv 着法/着法mvv分数
	};
	struct
	{
		uint8_t Src, Dst;	   // 起始格和目标格
		int8_t CptDrw, ChkChs; // 被吃子(+)/和棋着法数(-)、将军子(+)/被捉子(-)
		//? chkchs 表示被哪一个子将军，注意检测noking 
	};
}; // mvs

// 回滚结构
struct RollbackStruct {
  ZobristStruct zobr;   // Zobrist
  int evRed, evBlack; // 红方和黑方的子力价值
  int8_t Srcpt, Dstpt; // 临时放这里，不能和union 其他属性冲突
  MoveStruct mvs;       // 着法
}; // rbs

struct MoveSortStruct{
	int capNum,nocapNum,allMoveNum;
	int nMoveIndex = 0;
	MoveStruct mvsCap[MAX_GEN_MOVES];
	MoveStruct mvsNocap[MAX_GEN_MOVES];
	MoveStruct mvsAll[2 * MAX_GEN_MOVES];

	void ShellSort(MoveStruct *mvs,int len){
		int i, j, nStep, nStepLevel;
  		MoveStruct mvsBest;
  		nStepLevel = 1;

  		while (cnShellStep[nStepLevel] < len) {
    		nStepLevel ++;
  		}
  		nStepLevel --;
  		while (nStepLevel > 0) {
    		nStep = cnShellStep[nStepLevel]; // 40
    		for (i = nStep; i < len; i++) {
      		mvsBest = mvs[i];
      		j = i - nStep; // 0 1 2 3...39(moveNum - 1 - step)
      		while (j >= 0 && mvsBest.wvl > mvs[j].wvl) {
        		mvs[j + nStep] = mvs[j];
        		j -= nStep;
      		}
      		mvs[j + nStep] = mvsBest; // j 用来间隔 step 选取最大
    		}
    		nStepLevel --;
  		}
	}

	void InitRoot() {
  		int i;
  		nMoveIndex = 0;
  		// 为什么根结点要将分数都置为1
  		for (i = 0; i < allMoveNum; i ++) {
    		mvsAll[i].wvl = 1;
  		}
  		ShellSort(mvsAll,allMoveNum);
	}

  	int NextRoot(void) {
    	if (nMoveIndex < allMoveNum - 1) {
    	  	nMoveIndex ++;
    		return mvsAll[nMoveIndex].wmv;
    	} else {
      		return 0;
    	}
  	}

	void UpdateRoot(int mv) {
  		int i;
  		for (i = 0; i < allMoveNum; i++) {
    		if (mvsAll[i].wmv == mv) {
      			mvsAll[i].wvl = SORT_VALUE_MAX;
    		} else if (mvsAll[i].wvl > 0) {
      			mvsAll[i].wvl --;      
    		}
  		}
	}
};

struct HashStruct {
  uint32_t dwZobristLock0;           // Zobrist校验锁，第一部分
  uint16_t wmv;                      // 最佳着法
  uint8_t ucAlphaDepth, ucBetaDepth; // 深度(上边界和下边界)
  int16_t svlAlpha, svlBeta;         // 分值(上边界和下边界)
  uint32_t dwZobristLock1;           // Zobrist校验锁，第二部分
}; // hsh

// 置换表信息
extern int nHashMask;              // 置换表的大小
// extern HashStruct *hshItems;
extern std::unique_ptr<HashStruct[]> hshItems;

extern RollbackStruct rbsList[MAX_MOVE_NUM]; 
extern int rbMoveNum;
extern MoveSortStruct rtmsvs; // 每层覆盖 msvs? 还是只用一个?
extern uint16_t BestMoveList[LIMIT_DEPTH];

inline void ClearRollbackList(){
	memset(rbsList, 0, MAX_MOVE_NUM * sizeof(RollbackStruct));
	rbMoveNum = 0;
}

inline void ClearHash() {         // 清空置换表
	memset(hshItems.get(), 0, (nHashMask + 1) * sizeof(HashStruct));
}

inline void NewHashTable(int nHashScale){
	nHashMask = ((1 << nHashScale) / sizeof(HashStruct)) - 1;
	// hshItems = new HashStruct[nHashMask + 1];
	
	hshItems = std::make_unique<HashStruct[]>(nHashMask + 1); 
	ClearHash();
}

inline void DelHashTable(){
	// delete []hshItems;				
	hshItems.reset();
}

// when n = 2^k, hash_value % n == hash_value & (n - 1)
inline HashStruct &HASH_ITEM(const ZobristStruct &zb, int nLayer = 0) {
	return hshItems[(zb.dwKey + nLayer) & nHashMask];
}

inline bool HASH_POS_EQUAL(const HashStruct &hsh, const ZobristStruct &zb) {
  return hsh.dwZobristLock0 == zb.dwLock0 && hsh.dwZobristLock1 == zb.dwLock1;
}

inline int ProbeHash(const ZobristStruct &zb,int nDepth,int vlApha,int vlBeta){
	int i;
	// for(i = 0;i < 2;++i){
	HashStruct hsh = HASH_ITEM(zb);		
	if(HASH_POS_EQUAL(hsh,zb)){
		// if(nDepth >= 5){
			// DEBUG_("probe hit at ndepth = ",nDepth, " alpha: ",vlApha," beta: ",vlBeta);
		// }
  		if (hsh.ucBetaDepth > 0) {
			if(hsh.ucBetaDepth >= nDepth && hsh.svlBeta >= vlBeta){
				return hsh.svlBeta;
			}
		}
		if(hsh.ucAlphaDepth > 0){
			if(hsh.ucAlphaDepth >= nDepth && hsh.svlAlpha <= vlApha){
				return hsh.svlAlpha;
			}
		}
	}
	// }
	// no record
	return -MATE_VALUE - 1;
}

inline void RecordHash(const ZobristStruct &zb,int nFlag,int mv,int nDepth,int vl){
	int i,nHashDepth;
	
	// int nMinDepth = 512;
	// int nMinLayer = 0;
	// 是否已有记录
	// for(i = 0; i < 2;++i){
    HashStruct hsh = HASH_ITEM(zb, 0);
	if(HASH_POS_EQUAL(hsh,zb)){
		// if(nDepth >= 5){
			// DEBUG_("record hit at ndepth = ", nDepth);
		// }
      	if ((nFlag & HASH_ALPHA) != 0 && (hsh.ucAlphaDepth <= nDepth || hsh.svlAlpha >= vl)) {
        	hsh.ucAlphaDepth = nDepth;
        	hsh.svlAlpha = vl;
      	}

      	if ((nFlag & HASH_BETA) != 0 && (hsh.ucBetaDepth <= nDepth || hsh.svlBeta <= vl) ) {
        	hsh.ucBetaDepth = nDepth;
        	hsh.svlBeta = vl;
      	}
      	if (mv != 0) {
        	hsh.wmv = mv;
      	}
      	HASH_ITEM(zb, 0) = hsh;
      	return;
	}

    	// nHashDepth = std::max((hsh.ucAlphaDepth == 0 ? 0 : hsh.ucAlphaDepth + 256),
        // 	(hsh.wmv == 0 ? hsh.ucBetaDepth : hsh.ucBetaDepth + 256));
    	// assert(nHashDepth < 512);
    	// if (nHashDepth < nMinDepth) {
      	// 	nMinDepth = nHashDepth;
      	// 	nMinLayer = i;
    	// }
	// }

	// if(nDepth >= 5){
	// 	DEBUG_("record at ndepth = ", nDepth);
	// }
	hsh.dwZobristLock0 = zb.dwLock0;		
	hsh.dwZobristLock1 = zb.dwLock1;
  	hsh.wmv = mv;

    if ((nFlag & HASH_ALPHA) != 0 ) {
		hsh.ucAlphaDepth = nDepth;	
  		hsh.svlAlpha = vl;
	}

	if((nFlag & HASH_BETA) != 0 ){
		hsh.ucBetaDepth = nDepth;
  		hsh.svlBeta = vl;
	}
	HASH_ITEM(zb,0) = hsh;
}

// 局面评估，静态搜索
// int SearchQuiescEvaluate();

//                      init -infinity, +infinity
// int AlphaBeta(int depth, int alpha, int beta)
// {
// 	if (depth == 0)
// 	{
// 		return Evaluate();
// 	}
// 	GenerateLegalMoves();
//	while (MovesLeft())
// 	{
// 		MakeNextMove();
// 		val = -AlphaBeta(depth - 1, -beta, -alpha);
// 		UnmakeMove();
// 		if (val >= beta)
// 		{
// 			return beta;
// 		}
// 		if (val > alpha)
// 		{
// 			alpha = val;
// 		}
// 	}
// 	return alpha; alpha是当前视角（节点）下的期望值，beta 为上界 a <= v <= b
//  每个节点要maxmize,（最大化自己的分数）对不同层就是最小化了 
// }

// int MinMax(int depth) {
// 　if (SideToMove() == WHITE) {　// 白方是“最大”者
// 　　return Max(depth);
// 　} else {　　　　　　　　　　　// 黑方是“最小”者
// 　　return Min(depth);
// 　}
// }
// 　
// int Max(int depth) {
// 　int best = -INFINITY;
// 　if (depth <= 0) {
// 　　return Evaluate();
// 　}
// 　GenerateLegalMoves();
// 　while (MovesLeft()) {
// 　　MakeNextMove();
// 　　val = Min(depth - 1);
// 　　UnmakeMove();
// 　　if (val > best) {
// 　　　best = val;
// 　　}
// 　}
// 　return best;
// }
// 　
// int Min(int depth) {
// 　int best = INFINITY;　// 注意这里不同于“最大”算法
// 　if (depth <= 0) {
// 　　return Evaluate();
// 　}
// 　GenerateLegalMoves();
// 　while (MovesLeft()) {
// 　　MakeNextMove();
// 　　val = Max(depth - 1);
// 　　UnmakeMove();
// 　　if (val < best) { 　// 注意这里不同于“最大”算法
// 　　　best = val;
// 　　}
// 　}
// 　return best;
// }
//  black depth 2 
//  red   depth 1 
// int NegaMax(int depth) {
// 　int best = -INFINITY;
// 　if (depth <= 0) {
// 　　return Evaluate();
// 　}
// 　GenerateLegalMoves();
// 　while (MovesLeft()) {
// 　　MakeNextMove();
// 　　val = -NegaMax(depth - 1); // 注意这里有个负号。
// 　　UnmakeMove();
// 　　if (val > best) {
// 　　　best = val;
// 　　}
// 　}
// 　return best;
// }
