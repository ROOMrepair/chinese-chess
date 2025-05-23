#include "pregen.hpp"

const bool cbcInBoard[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const uint8_t validPosAdvisorBishop[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 8, 8, 2, 1, 8, 1, 2, 8, 8, 0, 0, 0, 0,
  0, 0, 0, 8, 8, 8, 8, 1, 8, 8, 8, 8, 0, 0, 0, 0,
  0, 0, 0, 2, 8, 8, 1, 2, 1, 8, 8, 2, 0, 0, 0, 0,
  0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0,
  0, 0, 0, 8, 8, 2, 8, 8, 8, 2, 8, 8, 0, 0, 0, 0,
  0, 0, 0, 8, 8, 2, 8, 8, 8, 2, 8, 8, 0, 0, 0, 0,
  0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0,
  0, 0, 0, 2, 8, 8, 1, 2, 1, 8, 8, 2, 0, 0, 0, 0,
  0, 0, 0, 8, 8, 8, 8, 1, 8, 8, 8, 8, 0, 0, 0, 0,
  0, 0, 0, 8, 8, 2, 1, 8, 1, 2, 8, 8, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const uint8_t validPosPawn[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 6, 1, 6, 1, 6, 1, 6, 1, 6, 0, 0, 0, 0,
  0, 0, 0, 6, 1, 6, 1, 6, 1, 6, 1, 6, 0, 0, 0, 0,
  0, 0, 0, 6, 1, 6, 1, 6, 1, 6, 1, 6, 0, 0, 0, 0,
  0, 0, 0, 6, 1, 6, 1, 6, 1, 6, 1, 6, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const bool cbcInFort[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const int8_t ccLegalSpanTab[512] = {
                       0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0
};

const int8_t ccKnightPinTab[512] = {
                               0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,-16,  0,-16,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0, -1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0, -1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0, 16,  0, 16,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0
};

// 坐标的镜像(左右对称)
const uint8_t cucsqMirrorTab[256] = {
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
  0, 0, 0, 0x3b, 0x3a, 0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0, 0, 0, 0,
  0, 0, 0, 0x4b, 0x4a, 0x49, 0x48, 0x47, 0x46, 0x45, 0x44, 0x43, 0, 0, 0, 0,
  0, 0, 0, 0x5b, 0x5a, 0x59, 0x58, 0x57, 0x56, 0x55, 0x54, 0x53, 0, 0, 0, 0,
  0, 0, 0, 0x6b, 0x6a, 0x69, 0x68, 0x67, 0x66, 0x65, 0x64, 0x63, 0, 0, 0, 0,
  0, 0, 0, 0x7b, 0x7a, 0x79, 0x78, 0x77, 0x76, 0x75, 0x74, 0x73, 0, 0, 0, 0,
  0, 0, 0, 0x8b, 0x8a, 0x89, 0x88, 0x87, 0x86, 0x85, 0x84, 0x83, 0, 0, 0, 0,
  0, 0, 0, 0x9b, 0x9a, 0x99, 0x98, 0x97, 0x96, 0x95, 0x94, 0x93, 0, 0, 0, 0,
  0, 0, 0, 0xab, 0xaa, 0xa9, 0xa8, 0xa7, 0xa6, 0xa5, 0xa4, 0xa3, 0, 0, 0, 0,
  0, 0, 0, 0xbb, 0xba, 0xb9, 0xb8, 0xb7, 0xb6, 0xb5, 0xb4, 0xb3, 0, 0, 0, 0,
  0, 0, 0, 0xcb, 0xca, 0xc9, 0xc8, 0xc7, 0xc6, 0xc5, 0xc4, 0xc3, 0, 0, 0, 0,
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
};

static const int KingMoveTab[4]    = {-0x10, -0x01, +0x01, +0x10};
static const int AdvisorMoveTab[4] = {-0x11, -0x0f, +0x0f, +0x11};
static const int BishopMoveTab[4]  = {-0x22, -0x1e, +0x1e, +0x22};

static const int KnightMoveTab[8]  = {-0x21, -0x1f, -0x12, -0x0e, +0x0e, +0x12, +0x1f, +0x21};
static const int KnightPinTabs[8]  = {-0x10, -0x10, -0x01, +0x01, -0x01, +0x01, +0x10, +0x10};

SlideMoveStruct SlideMoveCol[10][1024] = {};
SlideMoveStruct SlideMoveRow[9][512] = {};

SlideMaskStruct SlideMaskCol[10][1024] = {};
SlideMaskStruct SlideMaskRow[9][512] = {};

uint8_t KnightMoves[256][12] = {};
uint8_t KnightPins[256][8] = {};

uint8_t BishopMoves[256][8] = {};
uint8_t BishopPins[256][4] = {};

uint8_t KingMoves[256][8] = {};
uint8_t AdvisorMoves[256][8] = {};
uint8_t PawnMoves[2][256][4] = {};


ZobristStruct zobrPlayer = {};
ZobristStruct zobrTable[14][256] = {}; // 0 - 6 表示红方


void genZobrist(){
  int i,j;
  RC4Struct rc4;
  rc4.InitRand();
  zobrPlayer.InitRC4(rc4);
  for (i = 0; i < 14; i ++) {
    for (j = 0; j < 256; j ++) {
      zobrTable[i][j].InitRC4(rc4);
    }
  }
}

void genShortRangeMove(){
  int sqSrc,sqDst,n,i,j;
  for(sqSrc = 0;sqSrc < 256;++sqSrc){
    if(IN_BOARD(sqSrc)){

      // king moves
      n = 0; 
      for(i = 0; i < 4; ++i){
        sqDst = sqSrc + KingMoveTab[i];           
        if(IN_FORT(sqDst)){
          KingMoves[sqSrc][n] = sqDst;
          n++;
        }
      }
      assert(n <= 4); 
      KingMoves[sqSrc][n] = 0;

      // advisor moves
      n = 0;
      for(i = 0; i < 4; ++i){
        sqDst = sqSrc + AdvisorMoveTab[i];           
        if(IN_FORT(sqDst)){
          AdvisorMoves[sqSrc][n] = sqDst;
          n++;
        }
      }
      assert(n <= 4); 
      AdvisorMoves[sqSrc][n] = 0;
     
      // bitshop moves
      n = 0;
      for (i = 0; i < 4; ++i) {
        sqDst = sqSrc + BishopMoveTab[i];
        if (IN_BOARD(sqDst) && SAME_HALF(sqSrc, sqDst)) {
          BishopMoves[sqSrc][n] = sqDst;
          BishopPins[sqSrc][n] = BISHOP_PIN(sqSrc, sqDst);
          n++;
        }
      }
      assert(n <= 4);
      BishopMoves[sqSrc][n] = 0;

      // knight moves
      n = 0;
      for (i = 0; i < 8; ++i) {
        sqDst = sqSrc + KnightMoveTab[i];
        if (IN_BOARD(sqDst)) {
          KnightMoves[sqSrc][n] = sqDst;
          KnightPins[sqSrc][n] = sqSrc + KnightPinTabs[i]; 
          n++;
        }
      }
      assert(n <= 8);
      KnightMoves[sqSrc][n] = 0;
      
      // pawn moves i = 0 表示棋盘的下半部分
      for(i = 0;i < 2;++i){
        n = 0;
        sqDst = sqSrc + (i == 0 ? -16 : 16);
        if(IN_BOARD(sqDst)){
          PawnMoves[i][sqSrc][n] = sqDst;
          n++;
        } 
        if(AWAY_HALF(sqSrc,i)){
          for(j = -1; j <= 1;j += 2){
            sqDst = sqSrc + j;
            if(IN_BOARD(sqDst)){
              PawnMoves[i][sqSrc][n] = sqDst;
              n++;
            }
          } 
        }
        assert(n <= 3);
        PawnMoves[i][sqSrc][n] = 0; 
      }
    } 
  }
}

//! slidemovecol 表示的是col方向的移动，实际使用的是 rowOffset
void genLongRangeMove(){
  int i,j,k;
  // col
  for (i = 0; i < 10;++i){
    for (j = 0; j < 1024;++j){
      SlideMoveCol[i][j].CannonCap[0] = SlideMoveCol[i][j].RookCap[0] = 
      SlideMoveCol[i][j].SlideMove[0] = SlideMoveCol[i][j].SuperCap[0] = 
      SlideMoveCol[i][j].CannonCap[1] = SlideMoveCol[i][j].RookCap[1] =
      SlideMoveCol[i][j].SlideMove[1] = SlideMoveCol[i][j].SuperCap[1] = i + rowOffset;

      SlideMaskCol[i][j].wNonCap = SlideMaskCol[i][j].wRookCap = SlideMaskCol[i][j].wCannonCap =
      SlideMaskCol[i][j].wSuperCap = 0;

      // i = 0 第一行  0000010001 
      //? 不用管 sq 原始位 j = 000010001 或 000010101 时对应的值是一样的
      // down 
      for(k = i + 1; k < 10; ++k){
        if((1 << k) & j){
          SlideMoveCol[i][j].RookCap[0] = rowOffset + k;
          SlideMaskCol[i][j].wRookCap |= (1 << k);
          break; 
        }
        SlideMoveCol[i][j].SlideMove[0] = rowOffset + k;
        SlideMaskCol[i][j].wNonCap |= (1 << k);
      }
      
      for(k = k + 1; k < 10; ++k){
        if((1 << k) & j){
          SlideMoveCol[i][j].CannonCap[0] = rowOffset + k;
          SlideMaskCol[i][j].wCannonCap |= (1 << k);
          break;
        }
      }

      for(k = k + 1; k < 10; ++k){
        if((1 << k) & j){
          SlideMoveCol[i][j].SuperCap[0] = rowOffset + k;
          SlideMaskCol[i][j].wSuperCap |= (1 << k);
          break;
        }
      }

      // up
      for(k = i - 1; k >= 0 ;--k){
        if((1 << k) & j){
          SlideMoveCol[i][j].RookCap[1] = rowOffset + k;
          SlideMaskCol[i][j].wRookCap |= (1 << k);
          break;
        }
        SlideMoveCol[i][j].SlideMove[1] = rowOffset + k;
        SlideMaskCol[i][j].wNonCap |= (1 << k);
      }       
      
      for(k = k - 1; k >= 0; --k){
        if((1 << k) & j){
          SlideMoveCol[i][j].CannonCap[1] = rowOffset + k;
          SlideMaskCol[i][j].wCannonCap |= (1 << k);
          break;
        }
      }
      for (k --; k >= 0; k --) {
        if ((j & (1 << k)) != 0) {
          SlideMoveCol[i][j].SuperCap[1] = rowOffset + k;
          SlideMaskCol[i][j].wSuperCap |= 1 << k;
          break;
        }
      }
    }
  }  
  // row 
  for(i = 0;i < 9; ++i){
    for(j = 0;j < 512;++j){
      SlideMoveRow[i][j].CannonCap[0] = SlideMoveRow[i][j].RookCap[0] = 
      SlideMoveRow[i][j].SlideMove[0] = SlideMoveRow[i][j].SuperCap[0] =
       SlideMoveRow[i][j].CannonCap[1] = SlideMoveRow[i][j].RookCap[1] = 
       SlideMoveRow[i][j].SlideMove[1] = SlideMoveRow[i][j].SuperCap[1] = i + colOffset;

      SlideMaskRow[i][j].wNonCap = SlideMaskRow[i][j].wRookCap = SlideMaskRow[i][j].wCannonCap = 
      SlideMaskRow[i][j].wSuperCap = 0;
      // right
      for(k = i + 1; k < 9;++k){
        if((1 << k) & j){
          SlideMoveRow[i][j].RookCap[0] = colOffset + k; // 实际的列坐标
          SlideMaskRow[i][j].wRookCap |= (1 << k);
          break; 
        }
        SlideMoveRow[i][j].SlideMove[0] = colOffset + k;
        SlideMaskRow[i][j].wNonCap |= (1 << k);
      }       
      
      for(k = k + 1; k < 9; ++k){
        if((1 << k) & j){
          SlideMoveRow[i][j].CannonCap[0] = colOffset + k;
          SlideMaskRow[i][j].wCannonCap |= (1 << k);
          break; 
        }
      }

      for(k = k + 1; k < 9; ++k){
        if((1 << k) & j){
          SlideMoveRow[i][j].SuperCap[0] = colOffset + k;
          SlideMaskRow[i][j].wSuperCap |= (1 << k);
          break; 
        }
      }

      // left
      for(k = i - 1; k >= 0;--k){
        if((1 << k) & j){
          SlideMoveRow[i][j].RookCap[1] = colOffset + k;
          SlideMaskRow[i][j].wRookCap |= (1 << k);
          break; 
        }
        SlideMoveRow[i][j].SlideMove[1] = colOffset + k;
        SlideMaskRow[i][j].wNonCap |= (1 << k);
      }       
     
      for(k = k - 1; k >= 0; --k){
        if((1 << k) & j){
          SlideMoveRow[i][j].CannonCap[1] = colOffset + k;
          SlideMaskRow[i][j].wCannonCap |= (1 << k);
          break; 
        }
      }

      for(k = k - 1; k >= 0; --k){
        if((1 << k) & j){
          SlideMoveRow[i][j].SuperCap[1] = colOffset + k;
          SlideMaskRow[i][j].wSuperCap |= (1 << k);
          break; 
        }
      }
    }
  } 
}