#pragma once

#include <cstdint>
#include "constant.hpp"
#include "util.hpp"

void genShortRangeMove();  
void genLongRangeMove();

extern const bool cbcInBoard[256];
extern const bool cbcInFort[256];

extern const int8_t ccLegalSpanTab[512];   // 合理着法跨度表
extern const int8_t ccKnightPinTab[512];   // 马脚预制表

// 0 表示下右 1 表示上左
struct SlideMoveStruct{
  uint8_t SlideMove[2];
  uint8_t CannonCap[2];
  uint8_t RookCap[2];
};

struct SlideMaskStruct {
  uint16_t wNonCap, wRookCap, wCannonCap;
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

// 0 / 1 red / black
inline int SIDE_TAG(int sd)
{
	return 16 + (sd << 4);
}

inline int PIECE_INDEX(int pc) {
  return pc & 15;
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