#pragma once

#include <stdint.h>
#include <vector>

union MoveStruct
{
	uint32_t dwmv;
	struct
	{
		uint16_t wmv, wvl;
	};
	struct
	{
		uint8_t Src, Dst;	   // 起始格和目标格
		int8_t CptDrw, ChkChs; // 被吃子(+)/和棋着法数(-)、将军子(+)/被捉子(-)
	};
}; // mvs

struct RollbackStruct {
  int evRed, evBlack; // 红方和黑方的子力价值
  MoveStruct mvs;       // 着法
};

void makeMove(MoveStruct &mv);
void undoMove(MoveStruct &mv);


// 局面评估，静态搜索
int SearchQuiescEvaluate();

int AlphaBeta(int depth,int alpha,int beta);

//                        -infinity, +infinity
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
// 	return alpha;
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
