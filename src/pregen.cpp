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

static const int KingMoveTab[4]    = {-0x10, -0x01, +0x01, +0x10};
static const int AdvisorMoveTab[4] = {-0x11, -0x0f, +0x0f, +0x11};
static const int BishopMoveTab[4]  = {-0x22, -0x1e, +0x1e, +0x22};

static const int KnightMoveTab[8]  = {-0x21, -0x1f, -0x12, -0x0e, +0x0e, +0x12, +0x1f, +0x21};
static const int KnightPinTabs[8]  = {-0x10, -0x10, -0x01, +0x01, -0x01, +0x01, +0x10, +0x10};

SlideMoveStruct SlideMoveCol[10][1024] = {0};
SlideMoveStruct SlideMoveRow[9][512] = {0};

uint8_t KnightMoves[256][12] = {0};
uint8_t KnightPins[256][8] = {0};

uint8_t BishopMoves[256][8] = {0};
uint8_t BishopPins[256][4] = {0};

uint8_t KingMoves[256][8] = {0};
uint8_t AdvisorMoves[256][8] = {0};
uint8_t PawnMoves[2][256][4] = {0};

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
      SlideMoveCol[i][j].SlideMove[0] = SlideMoveCol[i][j].CannonCap[1] = 
      SlideMoveCol[i][j].RookCap[1] = SlideMoveCol[i][j].SlideMove[1] = i + rowOffset;

      // i = 0 第一行  0000010001 
      //? 不用管 sq 原始位 j = 000010001 或 000010101 时对应的值是一样的
      // down 
      for(k = i + 1; k < 10; ++k){
        if((1 << k) & j){
          SlideMoveCol[i][j].RookCap[0] = rowOffset + k;
          break; 
        }
        SlideMoveCol[i][j].SlideMove[0] = rowOffset + k;
      }
      
      for(k = k + 1; k < 10; ++k){
        if((1 << k) & j){
          SlideMoveCol[i][j].CannonCap[0] = rowOffset + k;
          break;
        }
      }
      
      // up
      for(k = i - 1; k >= 0 ;--k){
        if((1 << k) & j){
          SlideMoveCol[i][j].RookCap[1] = rowOffset + k;
          break;
        }
        SlideMoveCol[i][j].SlideMove[1] = rowOffset + k;
      }       
      
      for(k = k - 1; k >= 0; --k){
        if((1 << k) & j){
          SlideMoveCol[i][j].CannonCap[1] = rowOffset + k;
          break;
        }
      }
    }
  }  
  // row 
  for(i = 0;i < 9; ++i){
    for(j = 0;j < 512;++j){
      SlideMoveRow[i][j].CannonCap[0] = SlideMoveRow[i][j].RookCap[0] = 
      SlideMoveRow[i][j].SlideMove[0] = SlideMoveRow[i][j].CannonCap[1] = 
      SlideMoveRow[i][j].RookCap[1] = SlideMoveRow[i][j].SlideMove[1] = i + colOffset;

      // right
      for(k = i + 1; k < 9;++k){
        if((1 << k) & j){
          SlideMoveRow[i][j].RookCap[0] = colOffset + k; // 实际的列坐标
          break; 
        }
        SlideMoveRow[i][j].SlideMove[0] = colOffset + k;
      }       
      
      for(k = k + 1; k < 9; ++k){
        if((1 << k) & j){
          SlideMoveRow[i][j].CannonCap[0] = colOffset + k;
          break; 
        }
      }
      // left
      for(k = i - 1; k >= 0;--k){
        if((1 << k) & j){
          SlideMoveRow[i][j].RookCap[1] = colOffset + k;
          break; 
        }
        SlideMoveRow[i][j].SlideMove[1] = colOffset + k;
      }       
     
      for(k = k - 1; k >= 0; --k){
        if((1 << k) & j){
          SlideMoveRow[i][j].CannonCap[1] = colOffset + k;
          break; 
        }
      }
    }
  } 
}