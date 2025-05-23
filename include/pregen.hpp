#pragma once

#include <cstdint>
#include "constant.hpp"
#include "util.hpp"
#include "rc4prng.h"

void genShortRangeMove();  
void genLongRangeMove();
void genZobrist();

extern const bool cbcInBoard[256];
extern const bool cbcInFort[256];

extern const uint8_t cucsqMirrorTab[256];
extern const uint8_t validPosAdvisorBishop[256];
extern const uint8_t validPosPawn[256];

extern const int8_t ccLegalSpanTab[512];   // 合理着法跨度表
extern const int8_t ccKnightPinTab[512];   // 马脚预制表

// 0 表示下右 1 表示上左
struct SlideMoveStruct{
  uint8_t SlideMove[2];
  uint8_t CannonCap[2];
  uint8_t RookCap[2];
  uint8_t SuperCap[2];  // 超级炮(隔两子吃子)能走到的最大一格/最小一格, 为某些棋形准备
};

struct SlideMaskStruct {
  uint16_t wNonCap, wRookCap, wCannonCap,wSuperCap;
}; // sms

extern SlideMoveStruct SlideMoveCol[10][1024];
extern SlideMoveStruct SlideMoveRow[9][512];

extern SlideMaskStruct SlideMaskCol[10][1024]; // 81,920 字节
extern SlideMaskStruct SlideMaskRow[9][512];   // 36,864 字节

extern uint8_t KnightMoves[256][12];
extern uint8_t KnightPins[256][8];

extern uint8_t BishopMoves[256][8];
extern uint8_t BishopPins[256][4];

extern uint8_t KingMoves[256][8];
extern uint8_t AdvisorMoves[256][8];
extern uint8_t PawnMoves[2][256][4];

struct ZobristStruct {
  uint32_t dwKey, dwLock0, dwLock1;
  void InitZero(void) {
    dwKey = dwLock0 = dwLock1 = 0;
  }
  void InitRC4(RC4Struct &rc4) {
    dwKey = rc4.NextLong();
    dwLock0 = rc4.NextLong();
    dwLock1 = rc4.NextLong();
  }
  void Xor(const ZobristStruct &zobr) {
    dwKey ^= zobr.dwKey;
    dwLock0 ^= zobr.dwLock0;
    dwLock1 ^= zobr.dwLock1;
  }
  void Xor(const ZobristStruct &zobr1, const ZobristStruct &zobr2) {
    dwKey ^= zobr1.dwKey ^ zobr2.dwKey;
    dwLock0 ^= zobr1.dwLock0 ^ zobr2.dwLock0;
    dwLock1 ^= zobr1.dwLock1 ^ zobr2.dwLock1;
  }
}; // zobr

extern ZobristStruct zobrPlayer;
extern ZobristStruct zobrTable[14][256]; // 0 - 6 表示红方

// 0 / 1 red / black
inline int SIDE_TAG(int sd) {
   return 16 + (sd << 4);
}

inline int OPP_SIDE_TAG(int sd) {
  return 32 - (sd << 4);
}

inline int OPP_SIDE(int sd) {
  return 1 - sd;
}

inline int PIECE_TYPE(int pc) {
  return cnPieceTypes[pc];
}

inline int PIECE_INDEX(int pc) {
  return pc & 15;
}

inline int SQUARE_FLIP(int sq) {
  return 254 - sq;
}

inline int FILE_FLIP(int x) {
  return 14 - x;
}

inline int RANK_FLIP(int y) {
  return 15 - y;
}

inline uint8_t SQUARE_MIRROR(int sq) {
  return cucsqMirrorTab[sq];
}

// uint32_t BitPiece; 32位的棋子位，0到31位依次表示序号为16到47的棋子是否还在棋盘上
inline uint32_t BIT_PIECE(int pt) {
  return 1 << (pt - 16);
}

inline uint32_t BOTH_BITPIECE(int nBitPiece) {
  return nBitPiece + (nBitPiece << 16);
}

inline bool IN_BOARD(int sq) {
  return cbcInBoard[sq];
}

inline bool IN_FORT(int sq) {
  return cbcInFort[sq];
}

// only for pawn pregen
inline bool AWAY_HALF(int sq, int half) {
  return (sq & 0x80) == (half << 7);
}

inline bool SAME_HALF(int sqSrc,int sqDst){
  return ((sqSrc ^ sqDst) & 0x80) == 0;
}

inline bool DIFF_HALF(int sqSrc, int sqDst) {
  return ((sqSrc ^ sqDst) & 0x80) != 0;
}

// (IsredSide) (sq & 0x80 == 0 board上半部) (sd==0 当前为红) -> is sq sd same side
// 0 0 0 -> 1  !(IsredSide ^ (sq & 0x80) ^ (sd << 7))
// 0 0 1 -> 0
// 0 1 0 -> 0
// 0 1 1 -> 1
// 1 0 0 -> 0
// 1 0 1 -> 1
// 1 1 0 -> 1
// 1 1 1 -> 0
// is sq 在当前执子方半部 sd = isRedTurn ? 0 : 1;
inline bool HOME_HALF(bool isRedSide,int sq,int sd){
  return !(isRedSide ^ (((sq & 0x80) >> 7) ^ sd));
}

// 前进/后
inline int SQUARE_FORWARD(bool isRedSide,int sq, int sd) {
  return  sq - 16 + (!(sd ^ isRedSide) << 5);
}

inline int SQUARE_BACKWARD(bool isRedSide,int sq, int sd) {
  return sq + 16 - (!(sd ^ isRedSide) << 5);
}

inline int BISHOP_PIN(int sqSrc, int sqDst) {
  return ( sqSrc + sqDst ) >> 1;
}

//? 列值 刚好由低4位决定 行值由高4位
inline int FILE_X(int sq) {
  return sq & 15;
}

inline int RANK_Y(int sq) {
  return sq >> 4;
}

inline int BOARD_X(int sq){
  return FILE_X(sq) - colOffset;
}

inline int BOARD_Y(int sq){
  return RANK_Y(sq) - rowOffset;
}

// 列 mask
inline int ColMask(int sq){
  return 1 << (RANK_Y(sq) - rowOffset);
}

// 行 mask
inline int RowMask(int sq){
  return 1 << (FILE_X(sq) - colOffset);
}

// 注意 row col 和 实际的 board x 和 board y 相反
inline int SQpos(int y,int x){
  return 16 * (y + rowOffset) + x + colOffset;
}

inline int SQPOS_xy(int x,int y){
  return 16 * y + x;
}

inline bool KING_SPAN(int sqSrc, int sqDst) {
  assert(sqDst - sqSrc + 256 >= 0 && sqDst - sqSrc + 256 < 512);
  return ccLegalSpanTab[sqDst - sqSrc + 256] == 1;
}

inline bool ADVISOR_SPAN(int sqSrc, int sqDst) {
  assert(sqDst - sqSrc + 256 >= 0 && sqDst - sqSrc + 256 < 512);
  return ccLegalSpanTab[sqDst - sqSrc + 256] == 2;
}

inline bool BISHOP_SPAN(int sqSrc, int sqDst) {
  assert(sqDst - sqSrc + 256 >= 0 && sqDst - sqSrc + 256 < 512);
  return ccLegalSpanTab[sqDst - sqSrc + 256] == 3;
}

// src 为马的位置
inline int KNIGHT_PIN(int sqSrc, int sqDst) {
  assert(sqDst - sqSrc + 256 >= 0 && sqDst - sqSrc + 256 < 512);
  return  sqSrc +  ccKnightPinTab[sqDst - sqSrc + 256];
}

inline int SRC(int mv) { // 得到着法的起点
  return mv & 255;
}

inline int DST(int mv) { // 得到着法的终点
  return mv >> 8;
}

inline int MOVE(int sqSrc, int sqDst) {   // 由起点和终点得到着法
  return sqSrc + (sqDst << 8);
}

inline int SIMPLE_VALUE(int pc) {
  return cnSimpleValues[pc];
}

inline Color PIECE_COLOR(int pt){
  return (pt & (1 << 5)) ? BLACK : RED;
}

inline int PIECE_BYTE(int pt){
  return cszPieceBytes[pt];
}

