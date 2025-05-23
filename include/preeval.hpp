#pragma once
#include <cstdint>

#define __ASSERT_BOUND(a, b, c) assert((a) <= (b) && (b) <= (c))
#define __ASSERT_SQUARE(sq) assert(IN_BOARD(sq))
#define __ASSERT_PIECE(pc) assert((pc) >= 16 && (pc) <= 47)

const int SHAPE_NONE = 0;
const int SHAPE_CENTER = 1;
const int SHAPE_LEFT = 2;
const int SHAPE_RIGHT = 3;

const int ROOK_MIDGAME_VALUE = 6;
const int KNIGHT_CANNON_MIDGAME_VALUE = 3;
const int OTHER_MIDGAME_VALUE = 1;
// 全局子力值 66??
const int TOTAL_MIDGAME_VALUE = ROOK_MIDGAME_VALUE * 4 + KNIGHT_CANNON_MIDGAME_VALUE * 8 + OTHER_MIDGAME_VALUE * 18;
const int TOTAL_ADVANCED_VALUE = 4;
const int TOTAL_ATTACK_VALUE = 8;
const int ADVISOR_BISHOP_ATTACKLESS_VALUE = 80;
const int TOTAL_ADVISOR_LEAKAGE = 80;

const int EVAL_MARGIN1 = 160;
const int EVAL_MARGIN2 = 80;
const int EVAL_MARGIN3 = 40;
const int EVAL_MARGIN4 = 20;

extern struct PreEvalStructEx { // 拓展的结构
  int vlBlackAdvisorLeakage, vlRedAdvisorLeakage;
  int vlHollowThreat[16]; // 空头炮的威胁 右侧表示本方视角
  int vlCentralThreat[16]; // 炮镇窝心马的威胁 右侧表示本方视角
  int vlRedBottomThreat[16], vlBlackBottomThreat[16]; // 沉底炮的威胁 右侧表示本方视角
  char cPopCnt16[65536]; // 加速PopCnt16的数组，只需要初始化一次
} PreEvalEx;

extern struct PreEvalStruct {
  int vlAdvanced;
  // 使用了预生成的局面评分
  // 0 king 1 advisor 2 bishop 3 knight 4 rook 5 cannchen 6 pawn
  uint8_t ucvlRedPieces[7][256];  
  uint8_t ucvlBlackPieces[7][256];
} PreEval;

inline int PopCnt16(uint16_t w) {
  int n;
  n = ((w >> 1) & 0x5555) + (w & 0x5555); // 步骤1：每2位统计1的个数
  n = ((n >> 2) & 0x3333) + (n & 0x3333);  // 步骤2：每4位统计1的个数
  n = ((n >> 4) & 0x0f0f) + (n & 0x0f0f);   // 步骤3：每8位统计1的个数
  return (n >> 8) + (n & 0x00ff);           // 步骤4：合并高低8位
}

inline int PopCnt32(uint32_t dw) {
  int n;
  n = ((dw >> 1) & 0x55555555) + (dw & 0x55555555);
  n = ((n >> 2) & 0x33333333) + (n & 0x33333333);
  n = ((n >> 4) & 0x0f0f0f0f) + (n & 0x0f0f0f0f);
  n = ((n >> 8) & 0x00ff00ff) + (n & 0x00ff00ff);
  return (n >> 16) + (n & 0x0000ffff);
}

inline int SIDE_VALUE(int sd, int vl) {
  return (sd == 0 ? vl : -vl);
}

inline static const char* pieceStr(int t){
  switch(t){
   case 0:
     return "king";
   case 1:
     return "advisor";
   case 2:
     return "bishop";
   case 3:
     return "knight";
   case 4:
     return "rook";
   case 5:
     return "cannon";
   case 6:
     return "pawn";
   default:
    return "[unknown]";
  }
}