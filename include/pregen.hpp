#pragma once

#include <cstdint>
#include "constant.hpp"
#include "util.hpp"

void genShortRangeMove();  
void genLongRangeMove();

extern const bool cbcInBoard[256];
extern const bool cbcInFort[256];

// 0 表示下右 1 表示上左
struct SlideMoveStruct{
  uint8_t SlideMove[2];
  uint8_t CannonCap[2];
  uint8_t RookCap[2];
};

extern SlideMoveStruct SlideMoveCol[10][1024];
extern SlideMoveStruct SlideMoveRow[9][512];

extern uint8_t KnightMoves[256][12];
extern uint8_t KnightPins[256][8];

extern uint8_t BishopMoves[256][8];
extern uint8_t BishopPins[256][4];

extern uint8_t KingMoves[256][8];
extern uint8_t AdvisorMoves[256][8];
extern uint8_t PawnMoves[2][256][4];


inline bool IN_BOARD(int sq) {
  return cbcInBoard[sq];
}

inline bool IN_FORT(int sq) {
  return cbcInFort[sq];
}

inline bool SAME_HALF(int sqSrc,int sqDst){
  return ((sqSrc ^ sqDst) & 0x80) == 0;
}

inline bool DIFF_HALF(int sqSrc, int sqDst) {
  return ((sqSrc ^ sqDst) & 0x80) != 0;
}

inline bool AWAY_HALF(int sq, int half) {
  return (sq & 0x80) == (half << 7);
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