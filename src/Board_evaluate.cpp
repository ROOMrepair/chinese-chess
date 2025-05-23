#include "Board.hpp"

PreEvalStructEx PreEvalEx;
PreEvalStruct PreEval;

static const char* preOutput = "./evaluate.txt";
// 视点在表下方，处于对手阵营时，分高
// 中局的马
static const uint8_t cucvlKnightMidgame[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0, 90, 90, 90, 96, 90, 96, 90, 90, 90,  0,  0,  0,  0,
  0,  0,  0, 90, 96,103, 97, 94, 97,103, 96, 90,  0,  0,  0,  0,
  0,  0,  0, 92, 98, 99,103, 99,103, 99, 98, 92,  0,  0,  0,  0,
  0,  0,  0, 93,108,100,107,100,107,100,108, 93,  0,  0,  0,  0,
  0,  0,  0, 90,100, 99,103,104,103, 99,100, 90,  0,  0,  0,  0,
  0,  0,  0, 90, 98,101,102,103,102,101, 98, 90,  0,  0,  0,  0,
  0,  0,  0, 92, 94, 98, 95, 98, 95, 98, 94, 92,  0,  0,  0,  0,
  0,  0,  0, 93, 92, 94, 95, 92, 95, 94, 92, 93,  0,  0,  0,  0,
  0,  0,  0, 85, 90, 92, 93, 78, 93, 92, 90, 85,  0,  0,  0,  0,
  0,  0,  0, 88, 85, 90, 88, 90, 88, 90, 85, 88,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 残局的马
static const uint8_t cucvlKnightEndgame[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0, 92, 94, 96, 96, 96, 96, 96, 94, 92,  0,  0,  0,  0,
  0,  0,  0, 94, 96, 98, 98, 98, 98, 98, 96, 94,  0,  0,  0,  0,
  0,  0,  0, 96, 98,100,100,100,100,100, 98, 96,  0,  0,  0,  0,
  0,  0,  0, 96, 98,100,100,100,100,100, 98, 96,  0,  0,  0,  0,
  0,  0,  0, 96, 98,100,100,100,100,100, 98, 96,  0,  0,  0,  0,
  0,  0,  0, 94, 96, 98, 98, 98, 98, 98, 96, 94,  0,  0,  0,  0,
  0,  0,  0, 94, 96, 98, 98, 98, 98, 98, 96, 94,  0,  0,  0,  0,
  0,  0,  0, 92, 94, 96, 96, 96, 96, 96, 94, 92,  0,  0,  0,  0,
  0,  0,  0, 90, 92, 94, 92, 92, 92, 94, 92, 90,  0,  0,  0,  0,
  0,  0,  0, 88, 90, 92, 90, 90, 90, 92, 90, 88,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 开中局、有进攻机会的帅(将)和兵(卒)，参照“梦入神蛋”
static const uint8_t cucvlKingPawnMidgameAttacking[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  9,  9,  9, 11, 13, 11,  9,  9,  9,  0,  0,  0,  0,
  0,  0,  0, 39, 49, 69, 84, 89, 84, 69, 49, 39,  0,  0,  0,  0,
  0,  0,  0, 39, 49, 64, 74, 74, 74, 64, 49, 39,  0,  0,  0,  0,
  0,  0,  0, 39, 46, 54, 59, 61, 59, 54, 46, 39,  0,  0,  0,  0,
  0,  0,  0, 29, 37, 41, 54, 59, 54, 41, 37, 29,  0,  0,  0,  0,
  0,  0,  0,  7,  0, 13,  0, 16,  0, 13,  0,  7,  0,  0,  0,  0,
  0,  0,  0,  7,  0,  7,  0, 15,  0,  7,  0,  7,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0, 11, 15, 11,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 残局、有进攻机会的帅(将)和兵(卒)
static const uint8_t cucvlKingPawnEndgameAttacking[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0, 10, 10, 10, 15, 15, 15, 10, 10, 10,  0,  0,  0,  0,
  0,  0,  0, 50, 55, 60, 85,100, 85, 60, 55, 50,  0,  0,  0,  0,
  0,  0,  0, 65, 70, 70, 75, 75, 75, 70, 70, 65,  0,  0,  0,  0,
  0,  0,  0, 75, 80, 80, 80, 80, 80, 80, 80, 75,  0,  0,  0,  0,
  0,  0,  0, 70, 70, 65, 70, 70, 70, 65, 70, 70,  0,  0,  0,  0,
  0,  0,  0, 45,  0, 40, 45, 45, 45, 40,  0, 45,  0,  0,  0,  0,
  0,  0,  0, 40,  0, 35, 40, 40, 40, 35,  0, 40,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  5,  5, 15,  5,  5,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  3,  3, 13,  3,  3,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  1,  1, 11,  1,  1,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 2. 开中局、没有进攻机会的帅(将)和兵(卒)
static const uint8_t cucvlKingPawnMidgameAttackless[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  9,  9,  9, 11, 13, 11,  9,  9,  9,  0,  0,  0,  0,
  0,  0,  0, 19, 24, 34, 42, 44, 42, 34, 24, 19,  0,  0,  0,  0,
  0,  0,  0, 19, 24, 32, 37, 37, 37, 32, 24, 19,  0,  0,  0,  0,
  0,  0,  0, 19, 23, 27, 29, 30, 29, 27, 23, 19,  0,  0,  0,  0,
  0,  0,  0, 14, 18, 20, 27, 29, 27, 20, 18, 14,  0,  0,  0,  0,
  0,  0,  0,  7,  0, 13,  0, 16,  0, 13,  0,  7,  0,  0,  0,  0,
  0,  0,  0,  7,  0,  7,  0, 15,  0,  7,  0,  7,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0, 11, 15, 11,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 4. 残局、没有进攻机会的帅(将)和兵(卒)
static const uint8_t cucvlKingPawnEndgameAttackless[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0, 10, 10, 10, 15, 15, 15, 10, 10, 10,  0,  0,  0,  0,
  0,  0,  0, 10, 15, 20, 45, 60, 45, 20, 15, 10,  0,  0,  0,  0,
  0,  0,  0, 25, 30, 30, 35, 35, 35, 30, 30, 25,  0,  0,  0,  0,
  0,  0,  0, 35, 40, 40, 45, 45, 45, 40, 40, 35,  0,  0,  0,  0,
  0,  0,  0, 25, 30, 30, 35, 35, 35, 30, 30, 25,  0,  0,  0,  0,
  0,  0,  0, 25,  0, 25, 25, 25, 25, 25,  0, 25,  0,  0,  0,  0,
  0,  0,  0, 20,  0, 20, 20, 20, 20, 20,  0, 20,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  5,  5, 13,  5,  5,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  3,  3, 12,  3,  3,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  1,  1, 11,  1,  1,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 5. 没受威胁的仕(士)和相(象)
static const uint8_t cucvlAdvisorBishopThreatless[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0, 20,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0, 18,  0,  0, 20, 23, 20,  0,  0, 18,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0, 23,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0, 20, 20,  0, 20, 20,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 5'. 可升变的，没受威胁的仕(士)和相(象) 什么是可升变的？
static const uint8_t cucvlAdvisorBishopPromotionThreatless[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0, 30,  0,  0,  0, 30,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0, 28,  0,  0, 30, 33, 30,  0,  0, 28,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0, 33,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0, 30, 30,  0, 30, 30,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 6. 受到威胁的仕(士)和相(象)，参照“梦入神蛋”
static const uint8_t cucvlAdvisorBishopThreatened[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0, 40,  0,  0,  0, 40,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0, 38,  0,  0, 40, 43, 40,  0,  0, 38,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0, 43,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0, 40, 40,  0, 40, 40,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 9. 开中局的车，参照“梦入神蛋”
static const uint8_t cucvlRookMidgame[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,206,208,207,213,214,213,207,208,206,  0,  0,  0,  0,
  0,  0,  0,206,212,209,216,233,216,209,212,206,  0,  0,  0,  0,
  0,  0,  0,206,208,207,214,216,214,207,208,206,  0,  0,  0,  0,
  0,  0,  0,206,213,213,216,216,216,213,213,206,  0,  0,  0,  0,
  0,  0,  0,208,211,211,214,215,214,211,211,208,  0,  0,  0,  0,
  0,  0,  0,208,212,212,214,215,214,212,212,208,  0,  0,  0,  0,
  0,  0,  0,204,209,204,212,214,212,204,209,204,  0,  0,  0,  0,
  0,  0,  0,198,208,204,212,212,212,204,208,198,  0,  0,  0,  0,
  0,  0,  0,200,208,206,212,200,212,206,208,200,  0,  0,  0,  0,
  0,  0,  0,194,206,204,212,200,212,204,206,194,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 10. 残局的车
static const uint8_t cucvlRookEndgame[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,182,182,182,184,186,184,182,182,182,  0,  0,  0,  0,
  0,  0,  0,184,184,184,186,190,186,184,184,184,  0,  0,  0,  0,
  0,  0,  0,182,182,182,184,186,184,182,182,182,  0,  0,  0,  0,
  0,  0,  0,180,180,180,182,184,182,180,180,180,  0,  0,  0,  0,
  0,  0,  0,180,180,180,182,184,182,180,180,180,  0,  0,  0,  0,
  0,  0,  0,180,180,180,182,184,182,180,180,180,  0,  0,  0,  0,
  0,  0,  0,180,180,180,182,184,182,180,180,180,  0,  0,  0,  0,
  0,  0,  0,180,180,180,182,184,182,180,180,180,  0,  0,  0,  0,
  0,  0,  0,180,180,180,182,184,182,180,180,180,  0,  0,  0,  0,
  0,  0,  0,180,180,180,182,184,182,180,180,180,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 11. 开中局的炮，参照“梦入神蛋”
static const uint8_t cucvlCannonMidgame[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,100,100, 96, 91, 90, 91, 96,100,100,  0,  0,  0,  0,
  0,  0,  0, 98, 98, 96, 92, 89, 92, 96, 98, 98,  0,  0,  0,  0,
  0,  0,  0, 97, 97, 96, 91, 92, 91, 96, 97, 97,  0,  0,  0,  0,
  0,  0,  0, 96, 99, 99, 98,100, 98, 99, 99, 96,  0,  0,  0,  0,
  0,  0,  0, 96, 96, 96, 96,100, 96, 96, 96, 96,  0,  0,  0,  0,
  0,  0,  0, 95, 96, 99, 96,100, 96, 99, 96, 95,  0,  0,  0,  0,
  0,  0,  0, 96, 96, 96, 96, 96, 96, 96, 96, 96,  0,  0,  0,  0,
  0,  0,  0, 97, 96,100, 99,101, 99,100, 96, 97,  0,  0,  0,  0,
  0,  0,  0, 96, 97, 98, 98, 98, 98, 98, 97, 96,  0,  0,  0,  0,
  0,  0,  0, 96, 96, 97, 99, 99, 99, 97, 96, 96,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 12. 残局的炮
static const uint8_t cucvlCannonEndgame[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,100,100,100,100,100,100,100,100,100,  0,  0,  0,  0,
  0,  0,  0,100,100,100,100,100,100,100,100,100,  0,  0,  0,  0,
  0,  0,  0,100,100,100,100,100,100,100,100,100,  0,  0,  0,  0,
  0,  0,  0,100,100,100,102,104,102,100,100,100,  0,  0,  0,  0,
  0,  0,  0,100,100,100,102,104,102,100,100,100,  0,  0,  0,  0,
  0,  0,  0,100,100,100,102,104,102,100,100,100,  0,  0,  0,  0,
  0,  0,  0,100,100,100,102,104,102,100,100,100,  0,  0,  0,  0,
  0,  0,  0,100,100,100,102,104,102,100,100,100,  0,  0,  0,  0,
  0,  0,  0,100,100,100,104,106,104,100,100,100,  0,  0,  0,  0,
  0,  0,  0,100,100,100,104,106,104,100,100,100,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// 空头炮的威胁分值，大体上空头炮位置越高威胁越大。进入残局时，该值要相应减少。
// 注意向右是主视角(行号),靠近本家时分数高
static const int cvlHollowThreat[16] = {
   0,  0,  0,  0,  0,  0, 60, 65, 70, 75, 80, 80, 80,  0,  0,  0
};

// 炮镇窝心马的威胁分值，指标同上，大体上高度越低威胁越大，没有窝心马时可取四分之一。进入残局时，取值似乎不应变化。
static const int cvlCentralThreat[16] = {
   0,  0,  0,  0,  0,  0, 50, 45, 40, 35, 30, 30, 30,  0,  0,  0
};

//? 沉底炮的威胁分值，(列号)，大体上越靠近边线威胁越大。威胁减少时，该值要相应减少。
static const int cvlBottomThreat[16] = {
   0,  0,  0, 40, 30,  0,  0,  0,  0,  0, 30, 40,  0,  0,  0,  0
};

bool Board::Protected(int sd,int sqSrc,int sqExcept) const{

	int i,sqDst,nSideTag,pc,sqPin,x,y;

	nSideTag = SIDE_TAG(sd); // 16 or 32 

	// sq 是否是在当前执子方(sd)半部
	if(HOME_HALF(this->isRedSide,sqSrc,sd)){
		if(IN_FORT(sqSrc)){

			// 受到将的保护 
			sqDst = Pieces[nSideTag + KING].alive;		
			if(sqDst != 0 && sqDst != sqExcept){ // sqExcept 位置受牵制不能移动，不能保护其他子
				if(KING_SPAN(sqSrc,sqDst)){
					return true;
				}
			}
			// 受到士的保护
      		for (i = ADVISOR_FROM; i <= ADVISOR_TO; i ++) {
        		sqDst = Pieces[nSideTag + i].alive;
        		if (sqDst != 0 && sqDst != sqExcept) {
          			if (ADVISOR_SPAN(sqSrc, sqDst)) {
            			return true;
          			}
        		}
			}
		}	
			// 受到象的保护
		for (i = BISHOP_FROM; i <= BISHOP_TO; i ++) {
			sqDst = Pieces[nSideTag + i].alive;
			if (sqDst != 0 && sqDst != sqExcept) {
		  		if (BISHOP_SPAN(sqSrc, sqDst) && Squares[BISHOP_PIN(sqSrc, sqDst)] == 0) {
					return true;
		  		}
			}
	  	}
	}else{
		
    	// 判断受到过河兵(卒)横向的保护
    	for (sqDst = sqSrc - 1; sqDst <= sqSrc + 1; sqDst += 2) {
      		if (sqDst != sqExcept) {
        		pc = Squares[sqDst];
        		if ((pc & nSideTag) != 0 && PIECE_INDEX(pc) >= PAWN_FROM) {
          		return true;
        		}
      		}
    	}
	
	}
	
	// 受到兵的纵向保护
  	sqDst = SQUARE_BACKWARD(this->isRedSide, sqSrc, sd);
	if(sqDst != sqExcept){
		pc = Squares[sqDst];
		if((pc & nSideTag) != 0 && PIECE_INDEX(pc) >= PAWN_FROM){
			return true;
		}	
	}

  	// 判断受到马的保护
  	for (i = KNIGHT_FROM; i <= KNIGHT_TO; i ++) {
    	sqDst = Pieces[nSideTag + i].alive;
    	if (sqDst != 0 && sqDst != sqExcept) {
      	sqPin = KNIGHT_PIN(sqDst, sqSrc); // 注意，sqSrc和sqDst是反的,sqDst是马的起始位置
      		if (sqPin != sqDst && Squares[sqPin] == 0) {
        		return true;
      		}
    	}
  	}
	
  	x = FILE_X(sqSrc);
  	y = RANK_Y(sqSrc);

	SlideMaskStruct* msk;

	// todo 受到对将的保护

	// 受到车的保护
	for( i = ROOK_FROM; i <= ROOK_TO; i++){
		sqDst = Pieces[nSideTag + i].alive;	
		if(sqDst != 0 && sqDst != sqExcept && sqDst != sqSrc){ // 前面都会自动排除相同点，这里需要加上
			if(x == FILE_X(sqDst)){
				msk = this->FileMaskPtr(x,y);
				if((msk->wRookCap & (1 << (RANK_Y(sqDst) - rowOffset))) != 0){
					return true;
				}
			}else if(y == RANK_Y(sqDst)){
				msk = this->RankMaskPtr(x,y);
				if((msk->wRookCap & (1 << (FILE_X(sqDst) - colOffset))) != 0){
					return true;
				}
			}
		}
	}
	
	// 受到炮的保护
	for( i = CANNON_FROM; i <= CANNON_TO; i++){
		sqDst = Pieces[nSideTag + i].alive;	
		if(sqDst != 0 && sqDst != sqExcept && sqDst != sqSrc){ // 前面都会自动排除相同点，这里需要加上
			if(x == FILE_X(sqDst)){
				msk = this->FileMaskPtr(x,y);
				if((msk->wCannonCap & (1 << (RANK_Y(sqDst) - rowOffset))) != 0){
					return true;
				}
			}else if(y == RANK_Y(sqDst)){
				msk = this->RankMaskPtr(x,y);
				if((msk->wCannonCap & (1 << (FILE_X(sqDst) - colOffset))) != 0){
					return true;
				}
			}
		}
	}	

	return false;
}

void Board::PreEvaluate(){
	int i,nMidgameValue,sq;
	int nRedAttacks, nBlackAttacks;
	int nRedSimpleValue,nBlackSimpleValue;

  	uint8_t ucvlPawnPiecesAttacking[256], ucvlPawnPiecesAttackless[256];

    for (i = 0; i < 65536; i ++) {
      PreEvalEx.cPopCnt16[i] = PopCnt16(i); // 统计 0-2 ^ 16 - 1 每个中1的个数
    }

	// 计算子力
  	nMidgameValue = PopCnt32(this->BitPiece & BOTH_BITPIECE(ADVISOR_BITPIECE | BISHOP_BITPIECE | PAWN_BITPIECE)) * OTHER_MIDGAME_VALUE;
  	nMidgameValue += PopCnt32(this->BitPiece & BOTH_BITPIECE(KNIGHT_BITPIECE | CANNON_BITPIECE)) * KNIGHT_CANNON_MIDGAME_VALUE;
  	nMidgameValue += PopCnt32(this->BitPiece & BOTH_BITPIECE(ROOK_BITPIECE)) * ROOK_MIDGAME_VALUE;

  	// 使用二次函数，子力很少时才认为接近残局 
	// (2 * M - x) * x / M = 2x - x^2 / M [0,66]
  	nMidgameValue = (2 * TOTAL_MIDGAME_VALUE - nMidgameValue) * nMidgameValue / TOTAL_MIDGAME_VALUE;
  	__ASSERT_BOUND(0, nMidgameValue, TOTAL_MIDGAME_VALUE);
	// 对不同子力阶段下的评估做分段或切换
  	PreEval.vlAdvanced = (TOTAL_ADVANCED_VALUE * nMidgameValue + TOTAL_ADVANCED_VALUE / 2) / TOTAL_MIDGAME_VALUE;
  	__ASSERT_BOUND(0, PreEval.vlAdvanced, TOTAL_ADVANCED_VALUE);

  	for (sq = 0; sq < 256; sq ++) {
    	if (IN_BOARD(sq)) {
			if(isRedSide){ // red 为主视角
				// king
      			PreEval.ucvlRedPieces[0][sq] = PreEval.ucvlBlackPieces[0][SQUARE_FLIP(sq)] = (uint8_t)
          			((cucvlKingPawnMidgameAttacking[sq] * nMidgameValue + cucvlKingPawnEndgameAttacking[sq] * (TOTAL_MIDGAME_VALUE - nMidgameValue)) / TOTAL_MIDGAME_VALUE);
				// knight
      			PreEval.ucvlRedPieces[3][sq] = PreEval.ucvlBlackPieces[3][SQUARE_FLIP(sq)] = (uint8_t)
          			((cucvlKnightMidgame[sq] * nMidgameValue + cucvlKnightEndgame[sq] * (TOTAL_MIDGAME_VALUE - nMidgameValue)) / TOTAL_MIDGAME_VALUE);
				// rook
      			PreEval.ucvlRedPieces[4][sq] = PreEval.ucvlBlackPieces[4][SQUARE_FLIP(sq)] = (uint8_t)
          			((cucvlRookMidgame[sq] * nMidgameValue + cucvlRookEndgame[sq] * (TOTAL_MIDGAME_VALUE - nMidgameValue)) / TOTAL_MIDGAME_VALUE);
				// cannon
      			PreEval.ucvlRedPieces[5][sq] = PreEval.ucvlBlackPieces[5][SQUARE_FLIP(sq)] = (uint8_t)
          			((cucvlCannonMidgame[sq] * nMidgameValue + cucvlCannonEndgame[sq] * (TOTAL_MIDGAME_VALUE - nMidgameValue)) / TOTAL_MIDGAME_VALUE);
				// pawn
				// ! 注意这两个数组没有区分红黑方,以正常的视角看,在下方确立了
      			ucvlPawnPiecesAttacking[sq] = PreEval.ucvlRedPieces[0][sq];
      			ucvlPawnPiecesAttackless[sq] = (uint8_t)
          			((cucvlKingPawnMidgameAttackless[sq] * nMidgameValue + cucvlKingPawnEndgameAttackless[sq] * (TOTAL_MIDGAME_VALUE - nMidgameValue)) / TOTAL_MIDGAME_VALUE);
    		}else{  // black 为主视角
				// king
      			PreEval.ucvlRedPieces[0][SQUARE_FLIP(sq)] = PreEval.ucvlBlackPieces[0][sq] = (uint8_t)
          			((cucvlKingPawnMidgameAttacking[sq] * nMidgameValue + cucvlKingPawnEndgameAttacking[sq] * (TOTAL_MIDGAME_VALUE - nMidgameValue)) / TOTAL_MIDGAME_VALUE);
				// knight
      			PreEval.ucvlRedPieces[3][SQUARE_FLIP(sq)] = PreEval.ucvlBlackPieces[3][sq] = (uint8_t)
          			((cucvlKnightMidgame[sq] * nMidgameValue + cucvlKnightEndgame[sq] * (TOTAL_MIDGAME_VALUE - nMidgameValue)) / TOTAL_MIDGAME_VALUE);
				// rook
      			PreEval.ucvlRedPieces[4][SQUARE_FLIP(sq)] = PreEval.ucvlBlackPieces[4][sq] = (uint8_t)
          			((cucvlRookMidgame[sq] * nMidgameValue + cucvlRookEndgame[sq] * (TOTAL_MIDGAME_VALUE - nMidgameValue)) / TOTAL_MIDGAME_VALUE);
				// cannon
      			PreEval.ucvlRedPieces[5][SQUARE_FLIP(sq)] = PreEval.ucvlBlackPieces[5][sq] = (uint8_t)
          			((cucvlCannonMidgame[sq] * nMidgameValue + cucvlCannonEndgame[sq] * (TOTAL_MIDGAME_VALUE - nMidgameValue)) / TOTAL_MIDGAME_VALUE);
				// pawn
      			ucvlPawnPiecesAttacking[sq] = PreEval.ucvlBlackPieces[0][sq];
      			ucvlPawnPiecesAttackless[sq] = (uint8_t)
          			((cucvlKingPawnMidgameAttackless[sq] * nMidgameValue + cucvlKingPawnEndgameAttackless[sq] * (TOTAL_MIDGAME_VALUE - nMidgameValue)) / TOTAL_MIDGAME_VALUE);
   			}
	  	}
	}
	
	// 空头炮和炮镇窝心马的威胁值
  	for (i = 0; i < 16; i ++) {
		//  空头炮
    	PreEvalEx.vlHollowThreat[i] = cvlHollowThreat[i] * (nMidgameValue + TOTAL_MIDGAME_VALUE) / (TOTAL_MIDGAME_VALUE * 2);
    	__ASSERT_BOUND(0, PreEvalEx.vlHollowThreat[i], cvlHollowThreat[i]);
    	PreEvalEx.vlCentralThreat[i] = cvlCentralThreat[i];
  	}

	int redKingPos = this->Pieces[SIDE_TAG(0)].alive;
	assert(redKingPos != 0);

  	// 然后判断各方是否处于进攻状态，方法是计算各种过河棋子的数量，按照车马2炮兵1相加。
  	nRedAttacks = nBlackAttacks = 0;
	// for red
  	for (i = SIDE_TAG(0) + KNIGHT_FROM; i <= SIDE_TAG(0) + ROOK_TO; i ++) {
		if(this->Pieces[i].alive != 0 && DIFF_HALF(redKingPos,this->Pieces[i].alive)){
      		nRedAttacks += 2;
    	}
  	}
  	for (i = SIDE_TAG(0) + CANNON_FROM; i <= SIDE_TAG(0) + PAWN_TO; i ++) {
    	if (this->Pieces[i].alive != 0 && DIFF_HALF(redKingPos,this->Pieces[i].alive)) {
	      	nRedAttacks ++;
    	}
  	}
	// for black
  	for (i = SIDE_TAG(1) + KNIGHT_FROM; i <= SIDE_TAG(1) + ROOK_TO; i ++) {
		if(this->Pieces[i].alive != 0 && SAME_HALF(redKingPos,this->Pieces[i].alive)){
      		nBlackAttacks += 2;
    	}
  	}
  	for (i = SIDE_TAG(1) + CANNON_FROM; i <= SIDE_TAG(1) + PAWN_TO; i ++) {
    	if (this->Pieces[i].alive != 0 && SAME_HALF(redKingPos,this->Pieces[i].alive)) {
      	nBlackAttacks ++;
    	}
  	}

	uint16_t BitPieceRed = 0xffff & this->BitPiece; 
	uint16_t BitPieceBlack = (this->BitPiece >> 16) & 0xffff;

  	// 如果本方轻子数比对方多，那么每多一个轻子(车算2个轻子)威胁值加2。威胁值最多不超过8。
	nRedSimpleValue = PopCnt16(BitPieceRed & ROOK_BITPIECE) * 2 + PopCnt16(BitPieceRed & (KNIGHT_BITPIECE | CANNON_BITPIECE));
  	nBlackSimpleValue = PopCnt16(BitPieceBlack & ROOK_BITPIECE) * 2 + PopCnt16(BitPieceBlack & (KNIGHT_BITPIECE | CANNON_BITPIECE));
  	if (nRedSimpleValue > nBlackSimpleValue) {
    	nRedAttacks += (nRedSimpleValue - nBlackSimpleValue) * 2;
  	} else {
   		nBlackAttacks += (nBlackSimpleValue - nRedSimpleValue) * 2;
  	}
  	nRedAttacks = nRedAttacks > TOTAL_ATTACK_VALUE ? TOTAL_ATTACK_VALUE : nRedAttacks;
  	nBlackAttacks = nBlackAttacks > TOTAL_ATTACK_VALUE ? TOTAL_ATTACK_VALUE : nBlackAttacks;

  	//? 缺士的状况
  	PreEvalEx.vlBlackAdvisorLeakage = TOTAL_ADVISOR_LEAKAGE * nRedAttacks / TOTAL_ATTACK_VALUE;
  	PreEvalEx.vlRedAdvisorLeakage = TOTAL_ADVISOR_LEAKAGE * nBlackAttacks / TOTAL_ATTACK_VALUE;
  	__ASSERT_BOUND(0, nRedAttacks, TOTAL_ATTACK_VALUE);
  	__ASSERT_BOUND(0, nBlackAttacks, TOTAL_ATTACK_VALUE);
  	__ASSERT_BOUND(0, PreEvalEx.vlBlackAdvisorLeakage, TOTAL_ADVISOR_LEAKAGE);
  	__ASSERT_BOUND(0, PreEvalEx.vlBlackAdvisorLeakage, TOTAL_ADVISOR_LEAKAGE);

  	for (sq = 0; sq < 256; sq ++) {
    	if (IN_BOARD(sq)) {
			if(isRedSide){ // red 为主视角
				// 使用对方的 attack 值评价
      			PreEval.ucvlRedPieces[1][sq] = PreEval.ucvlRedPieces[2][sq] = (uint8_t) ((cucvlAdvisorBishopThreatened[sq] * nBlackAttacks +
          			 cucvlAdvisorBishopThreatless[sq] * (TOTAL_ATTACK_VALUE - nBlackAttacks)) / TOTAL_ATTACK_VALUE);

      			PreEval.ucvlBlackPieces[1][sq] = PreEval.ucvlBlackPieces[2][sq] = (uint8_t) ((cucvlAdvisorBishopThreatened[SQUARE_FLIP(sq)] * nRedAttacks +
          			cucvlAdvisorBishopThreatless[SQUARE_FLIP(sq)] * (TOTAL_ATTACK_VALUE - nRedAttacks)) / TOTAL_ATTACK_VALUE);

      			PreEval.ucvlRedPieces[6][sq] = (uint8_t) ((ucvlPawnPiecesAttacking[sq] * nRedAttacks +
          			ucvlPawnPiecesAttackless[sq] * (TOTAL_ATTACK_VALUE - nRedAttacks)) / TOTAL_ATTACK_VALUE);

      			PreEval.ucvlBlackPieces[6][sq] = (uint8_t) ((ucvlPawnPiecesAttacking[SQUARE_FLIP(sq)] * nBlackAttacks +
          			ucvlPawnPiecesAttackless[SQUARE_FLIP(sq)] * (TOTAL_ATTACK_VALUE - nBlackAttacks)) / TOTAL_ATTACK_VALUE);
			}else{	// black 为主视角
      			PreEval.ucvlRedPieces[1][sq] = PreEval.ucvlRedPieces[2][sq] = (uint8_t) ((cucvlAdvisorBishopThreatened[SQUARE_FLIP(sq)] * nBlackAttacks +
          			cucvlAdvisorBishopThreatless[SQUARE_FLIP(sq)] * (TOTAL_ATTACK_VALUE - nBlackAttacks)) / TOTAL_ATTACK_VALUE);

      			PreEval.ucvlBlackPieces[1][sq] = PreEval.ucvlBlackPieces[2][sq] = (uint8_t) ((cucvlAdvisorBishopThreatened[sq] * nRedAttacks +
          			cucvlAdvisorBishopThreatless[sq] * (TOTAL_ATTACK_VALUE - nRedAttacks)) / TOTAL_ATTACK_VALUE);

      			PreEval.ucvlRedPieces[6][sq] = (uint8_t) ((ucvlPawnPiecesAttacking[SQUARE_FLIP(sq)] * nRedAttacks +
          			ucvlPawnPiecesAttackless[SQUARE_FLIP(sq)] * (TOTAL_ATTACK_VALUE - nRedAttacks)) / TOTAL_ATTACK_VALUE);

      			PreEval.ucvlBlackPieces[6][sq] = (uint8_t) ((ucvlPawnPiecesAttacking[sq] * nBlackAttacks +
          			ucvlPawnPiecesAttackless[sq] * (TOTAL_ATTACK_VALUE - nBlackAttacks)) / TOTAL_ATTACK_VALUE);

			}
    	}
  	}
	//? 沉底炮和对方攻击值相关
  	for (i = 0; i < 16; i ++) {
    	PreEvalEx.vlRedBottomThreat[i] = cvlBottomThreat[i] * nBlackAttacks / TOTAL_ATTACK_VALUE;
    	PreEvalEx.vlBlackBottomThreat[i] = cvlBottomThreat[i] * nRedAttacks / TOTAL_ATTACK_VALUE;
  	}

  // 检查预评价是否对称
#ifndef NDEBUG
  	for (sq = 0; sq < 256; sq ++) {
    	if (IN_BOARD(sq)) {
      	for (i = 0; i < 7; i ++) {
        	assert(PreEval.ucvlRedPieces[i][sq] == PreEval.ucvlRedPieces[i][SQUARE_MIRROR(sq)]);
        	assert(PreEval.ucvlBlackPieces[i][sq] == PreEval.ucvlBlackPieces[i][SQUARE_MIRROR(sq)]);
      	}
    	}
  	}
  	for (i = colOffset; i <= colOffsetTo; i ++) {
    	assert(PreEvalEx.vlRedBottomThreat[i] == PreEvalEx.vlRedBottomThreat[FILE_FLIP(i)]);
    	assert(PreEvalEx.vlBlackBottomThreat[i] == PreEvalEx.vlBlackBottomThreat[FILE_FLIP(i)]);
  	}
#endif

  	// 调整不受威胁方少掉的仕(士)相(象)分值
  	this->evRed = ADVISOR_BISHOP_ATTACKLESS_VALUE * (TOTAL_ATTACK_VALUE - nBlackAttacks) / TOTAL_ATTACK_VALUE;
  	this->evBlack = ADVISOR_BISHOP_ATTACKLESS_VALUE * (TOTAL_ATTACK_VALUE - nRedAttacks) / TOTAL_ATTACK_VALUE;
  	// 如果允许升变，那么不受威胁的仕(士)相(象)分值就少了一半
  	// 最后重新计算子力位置分
  	for (i = 16; i < 32; i ++) {
    	sq = this->Pieces[i].alive;
    	if (sq != 0) {
      		this->evRed += PreEval.ucvlRedPieces[PIECE_TYPE(i)][sq];
    	}
  	}
  	for (i = 32; i < 48; i ++) {
    	sq = this->Pieces[i].alive;
    	if (sq != 0) {
    	//   __ASSERT_SQUARE(sq);
      		this->evBlack += PreEval.ucvlBlackPieces[PIECE_TYPE(i)][sq];
    	}
  	}

#ifdef SHOW_EVAL
  	// 0 king 1 advisor 2 bishop 3 knight 4 rook 5 cannchen 6 pawn
	SearchAndSetResourceDir("chinese-chess");
	std::ofstream preEvalLog(preOutput);
	if (!preEvalLog.is_open()){
		std::cerr << "Failed to open output file: " << preOutput << std::endl;
	}else{
		preEvalLog << "[ nMidgameValue: " << nMidgameValue << " vlAdvanced: " << PreEval.vlAdvanced << " ]\n";	
		preEvalLog << "[ isRedSide: "<< isRedSide  << " evred: "<< evRed << " evblack: " << evBlack << " ]\n"; 
		for(int a = 0; a < 7; ++a){
			preEvalLog << "--------------------\n";
			for(int t = 0; t < 2; ++t){
				if(t == 0){
					preEvalLog << pieceStr(a) << " red\n";
					for(int y = 0; y < 16; ++y){
						for(int x = 0; x < 16;++x){
							preEvalLog << std::setw(3) << std::setfill(' ')
								<< (int)(PreEval.ucvlRedPieces[a][SQPOS_xy(x,y)])
								<< " ";
						}
						preEvalLog << '\n';
					}
				}else{
					preEvalLog << pieceStr(a) << " black\n";
					for(int y = 0; y < 16; ++y){
						for(int x = 0; x < 16;++x){
							preEvalLog << std::setw(3) << std::setfill(' ') 
								<< (int)(PreEval.ucvlBlackPieces[a][SQPOS_xy(x,y)])
								<<  " ";
						}
						preEvalLog << '\n';
					}
				}
			}
		}

		preEvalLog.close();	
	}
#endif

}

//  * 1. 帅(将)在原位，双仕(士)都在底线，编为1号，这种情况要判断空头炮和炮镇窝心马；
//  * 2. 帅(将)在原位，双仕(士)从左边包围帅(将)，编为2号，这种情况要判断右边的沉底炮和车封右边的帅(将)门；
//  * 3. 帅(将)在原位，双仕(士)从右边包围帅(将)，编为3号，这种情况要判断左边的沉底炮和车封左边的帅(将)门；
//  * 4. 其他情况，包括帅(将)不在原位或缺仕(士)，都编号0。

// part 1
int Board::AdvisorShape() const{
  	int redPenalty = 0, blackPenalty = 0;
	int nshape,sqAdv1,sqAdv2,pcCannon,
		pcRook,sq,x,y,kx,ky;

	int redSideTag = SIDE_TAG(0),blackSideTag = SIDE_TAG(1);
	int redKingInitPos = RedSidePiecePos(0xc7), blackKingInitPos = BlackSidePiecePos(0xc7);
	int portCenterRed = RedSidePiecePos(0xb7),portCenterBlack = BlackSidePiecePos(0xb7);

	SlideMaskStruct *lpsms;

	uint16_t BitPieceRed = 0xffff & this->BitPiece; 
	uint16_t BitPieceBlack = (this->BitPiece >> 16) & 0xffff;

	// red
	kx = FILE_X(redKingInitPos),ky = RANK_Y(redKingInitPos);
	if((BitPieceRed & ADVISOR_BITPIECE) == ADVISOR_BITPIECE){
		// 将在原点
		if(Pieces[redSideTag + KING].alive == redKingInitPos){
			sqAdv1 = this->Pieces[redSideTag + ADVISOR_FROM].alive;
			sqAdv2 = this->Pieces[redSideTag + ADVISOR_TO].alive;
			if(false){
			}else if(sqAdv1 == RedSidePiecePos(0xc6)){ // left 
				nshape = (sqAdv2 == RedSidePiecePos(0xc8)) ? SHAPE_CENTER : (sqAdv2 == portCenterRed) ? SHAPE_LEFT : SHAPE_NONE;
			}else if(sqAdv1 == RedSidePiecePos(0xc8)){
				nshape = (sqAdv2 == RedSidePiecePos(0xc6)) ? SHAPE_CENTER : (sqAdv2 == portCenterRed) ? SHAPE_RIGHT : SHAPE_NONE;
			}else if(sqAdv1 == portCenterRed){
				nshape = (sqAdv2 == RedSidePiecePos(0xc6)) ? SHAPE_LEFT : (sqAdv2 == RedSidePiecePos(0xc8)) ? SHAPE_RIGHT : SHAPE_NONE;
			}else{
				nshape = SHAPE_NONE;
			}
			switch(nshape){
				case SHAPE_CENTER:
        			for (pcCannon = blackSideTag + CANNON_FROM; pcCannon <= blackSideTag + CANNON_TO; pcCannon++) {
          				sq = this->Pieces[pcCannon].alive;
          				if (sq != 0) {
            				x = FILE_X(sq);
            				if (x == colCenter) {
              					y = RANK_Y(sq);
              					lpsms = this->FileMaskPtr(x, y);
              					if ((lpsms->wRookCap & (1 << (ky - rowOffset))) != 0) {
                					// 计算空头炮的威胁
									if(isRedSide){
										//! 右侧是本方视角，为什么需要flip
										//
				// static const int cvlHollowThreat[16] = {
				//    0,  0,  0,  0,  0,  0, 60, 65, 70, 75, 80, 80, 80,  0,  0,  0
				// };
				//  y = 9                 ---^---  <-  ---^---      靠近本方则分数比较高   本方 ^ 视角,表示这个视角的炮对敌方的威胁
	                					redPenalty += PreEvalEx.vlHollowThreat[RANK_FLIP(y)];
									}else{
										redPenalty += PreEvalEx.vlHollowThreat[y];
									}
              					} else if ((lpsms->wSuperCap & (1 << (ky - rowOffset))) != 0 &&
                  					(this->Squares[portCenterRed] == 21 || this->Squares[portCenterRed] == 22)) {
                					// 计算炮镇窝心马的威胁
									if(isRedSide){
                						redPenalty += PreEvalEx.vlCentralThreat[RANK_FLIP(y)];
									}else{
										redPenalty += PreEvalEx.vlCentralThreat[y];
									}
              					}
            				}
          				}
        			}
					break;
				case SHAPE_LEFT:
				case SHAPE_RIGHT:
        			for (pcCannon = blackSideTag + CANNON_FROM; pcCannon <= blackSideTag + CANNON_TO; pcCannon ++) {
          				sq = this->Pieces[pcCannon].alive;
          				if (sq != 0) {
            				x = FILE_X(sq);
            				y = RANK_Y(sq);
            				if (x == colCenter) {
              					if ((this->FileMaskPtr(x, y)->wSuperCap & (1 << (ky - rowOffset))) != 0) {
                					// 计算一般中炮的威胁，帅(将)门被对方控制的还有额外罚分
									//? 用将门是否被敌方protected来判断是否被控制
									// 可以复用炮镇窝心马的棋形，但是分数不一样
									if(isRedSide){
                						redPenalty += (PreEvalEx.vlCentralThreat[RANK_FLIP(y)] >> 2) +
                    						(this->Protected(1, nshape == SHAPE_LEFT ? RedSidePiecePos(0xc8) :
									 		RedSidePiecePos(0xc6)) ? 20 : 0);
									}else{
                						redPenalty += (PreEvalEx.vlCentralThreat[y] >> 2) +
                    						(this->Protected(1, nshape == SHAPE_LEFT ? RedSidePiecePos(0xc8) :
									 		RedSidePiecePos(0xc6)) ? 20 : 0);
									}
                					// 如果车在底线保护帅(将)，则给予更大的罚分！
                					for (pcRook = redSideTag + ROOK_FROM; pcRook <= redSideTag + ROOK_TO; pcRook ++) {
                  						sq = this->Pieces[pcRook].alive;
                  						if (sq != 0) {
                    						y = RANK_Y(sq);
											// 红方底线
                    						if (y == RANK_Y(RedSidePiecePos(0xc7))) {
                      							x = FILE_X(sq);
                      							if ((this->RankMaskPtr(x, y)->wRookCap & (1 << (kx - colOffset) )) != 0) {
                        							redPenalty += 80;
                      							}
                    						}
                  						}
                					}
              					}
            				} else if (y == RANK_Y(RedSidePiecePos(0xc7))) {
              					if ((this->RankMaskPtr(x, y)->wRookCap & (1 << (kx - colOffset))) != 0) {
                					// 计算沉底炮的威胁 注意是列号,这里有对应视角
                					redPenalty += PreEvalEx.vlRedBottomThreat[x];
              					}
            				}
          				}
        			}
					break;
				case SHAPE_NONE:
					break;
				default:
					break;
			}
		}else if(this->Pieces[redSideTag + KING].alive == portCenterRed){
      		// 有双仕(士)但花心被帅(将)占领，要罚分
      		redPenalty += 20;
		}
	}else{
    	if ((BitPieceBlack & ROOK_BITPIECE) == ROOK_BITPIECE) {
    	  	// 缺仕(士)怕双车，有罚分
    	  	redPenalty += PreEvalEx.vlRedAdvisorLeakage;
    	}
	}
	// for black
	
	kx = FILE_X(blackKingInitPos),ky = RANK_Y(blackKingInitPos);
	if((BitPieceBlack & ADVISOR_BITPIECE) == ADVISOR_BITPIECE){
		// 将在原点
		if(Pieces[blackSideTag + KING].alive == blackKingInitPos){
			sqAdv1 = this->Pieces[blackSideTag + ADVISOR_FROM].alive;
			sqAdv2 = this->Pieces[blackSideTag + ADVISOR_TO].alive;
			if(false){
			}else if(sqAdv1 == BlackSidePiecePos(0xc6)){ // left 
				nshape = (sqAdv2 == BlackSidePiecePos(0xc8)) ? SHAPE_CENTER : (sqAdv2 == portCenterBlack) ? SHAPE_LEFT : SHAPE_NONE;
			}else if(sqAdv1 == BlackSidePiecePos(0xc8)){
				nshape = (sqAdv2 == BlackSidePiecePos(0xc6)) ? SHAPE_CENTER : (sqAdv2 == portCenterBlack) ? SHAPE_RIGHT : SHAPE_NONE;
			}else if(sqAdv1 == portCenterBlack){
				nshape = (sqAdv2 == BlackSidePiecePos(0xc6)) ? SHAPE_LEFT : (sqAdv2 == BlackSidePiecePos(0xc8)) ? SHAPE_RIGHT : SHAPE_NONE;
			}else{
				nshape = SHAPE_NONE;
			}
			switch(nshape){
				case SHAPE_CENTER:
        			for (pcCannon = redSideTag + CANNON_FROM; pcCannon <= redSideTag + CANNON_TO; pcCannon++) {
          				sq = this->Pieces[pcCannon].alive;
          				if (sq != 0) {
            				x = FILE_X(sq);
            				if (x == colCenter) {
              					y = RANK_Y(sq);
              					lpsms = this->FileMaskPtr(x, y);
              					if ((lpsms->wRookCap & (1 << (ky - rowOffset))) != 0) {
                					// 计算空头炮的威胁
									if(isRedSide){
										//! 右侧是本方视角，为什么需要flip
										//
				// static const int cvlHollowThreat[16] = {
				//    0,  0,  0,  0,  0,  0, 60, 65, 70, 75, 80, 80, 80,  0,  0,  0
				// };
				//  y = 9                 ---^---  <-  ---^---         本方 ^ 视角,表示这个视角的炮对敌方的威胁
	                					blackPenalty += PreEvalEx.vlHollowThreat[y];
									}else{
										blackPenalty += PreEvalEx.vlHollowThreat[RANK_FLIP(y)];
									}
              					} else if ((lpsms->wSuperCap & (1 << (ky - rowOffset))) != 0 &&
                  					(this->Squares[portCenterBlack] == 37 || this->Squares[portCenterBlack] == 38)) {
                					// 计算炮镇窝心马的威胁
									if(isRedSide){
                						blackPenalty += PreEvalEx.vlCentralThreat[y];
									}else{
										blackPenalty += PreEvalEx.vlCentralThreat[RANK_FLIP(y)];
									}
              					}
            				}
          				}
        			}
					break;
				case SHAPE_LEFT:
				case SHAPE_RIGHT:
        			for (pcCannon =  redSideTag + CANNON_FROM; pcCannon <= redSideTag + CANNON_TO; pcCannon ++) {
          				sq = this->Pieces[pcCannon].alive;
          				if (sq != 0) {
            				x = FILE_X(sq);
            				y = RANK_Y(sq);
            				if (x == colCenter) {
              					if ((this->FileMaskPtr(x, y)->wSuperCap & (1 << (ky - rowOffset))) != 0) {
                					// 计算一般中炮的威胁，帅(将)门被对方控制的还有额外罚分
									//? 用将门是否被敌方protected来判断是否被控制
									// 可以复用炮镇窝心马的棋形，但是分数不一样
									if(isRedSide){
                						blackPenalty += (PreEvalEx.vlCentralThreat[y] >> 2) +
                    						(this->Protected(0, nshape == SHAPE_LEFT ? BlackSidePiecePos(0xc8) :
									 		BlackSidePiecePos(0xc6)) ? 20 : 0);
									}else{
                						blackPenalty += (PreEvalEx.vlCentralThreat[RANK_FLIP(y)] >> 2) +
                    						(this->Protected(0, nshape == SHAPE_LEFT ? BlackSidePiecePos(0xc8) :
									 		BlackSidePiecePos(0xc6)) ? 20 : 0);
									}
                					// 如果车在底线保护帅(将)，则给予更大的罚分！
                					for (pcRook = blackSideTag + ROOK_FROM; pcRook <= blackSideTag + ROOK_TO; pcRook ++) {
                  						sq = this->Pieces[pcRook].alive;
                  						if (sq != 0) {
                    						y = RANK_Y(sq);
											// 红方底线
                    						if (y == RANK_Y(BlackSidePiecePos(0xc7))) {
                      							x = FILE_X(sq);
                      							if ((this->RankMaskPtr(x, y)->wRookCap & (1 << (kx - colOffset) )) != 0) {
                        							blackPenalty += 80;
                      							}
                    						}
                  						}
                					}
              					}
            				} else if (y == RANK_Y(BlackSidePiecePos(0xc7))) {
              					if ((this->RankMaskPtr(x, y)->wRookCap & (1 << (kx - colOffset))) != 0) {
                					// 计算沉底炮的威胁 注意是列号,这里有对应视角
                					blackPenalty += PreEvalEx.vlBlackBottomThreat[x];
              					}
            				}
          				}
        			}
					break;
				case SHAPE_NONE:
					break;
				default:
					break;
			}
		}else if(this->Pieces[blackSideTag + KING].alive == portCenterBlack){
      		// 有双仕(士)但花心被帅(将)占领，要罚分
      		blackPenalty += 20;
		}
	}else{
    	if ((BitPieceRed & ROOK_BITPIECE) == ROOK_BITPIECE) {
    	  	// 缺仕(士)怕双车，有罚分
    	  	blackPenalty += PreEvalEx.vlBlackAdvisorLeakage;
    	}
	}

	// 此处的逻辑是我方"赢"了对手多少分
  	return SIDE_VALUE(this->isRedTurn ? 0 : 1, blackPenalty - redPenalty);
}

// part 2 牵制的评价
// 常数表"cnValuableStringPieces"用判断牵制是否有价值
// 大于0的项是对于车来说的，牵制马和炮(被牵制)都有价值，大于1的项是对于炮来说只有牵制马才有价值
static const int cnValuableStringPieces[48] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 2, 2, 0, 0, 1, 1, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 2, 2, 0, 0, 1, 1, 0, 0, 0, 0, 0
};

// "ccvlStringValueTab"是类似"KNIGHT_PIN_TAB"的常数表(参阅"pregen.h")，决定牵制价值
// 中间子和被牵制子的距离越近，牵制的价值就越大
static const char ccvlStringValueTab[512] = {
                               0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0, 12,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0, 16,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0, 24,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0, 28,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0, 32,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0, 36,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0, 40,  0,  0,  0,  0,  0,  0,  0,  0,
  12, 16, 20, 24, 28, 32, 36,  0, 36, 32, 28, 24, 20, 16, 12,  0,
   0,  0,  0,  0,  0,  0,  0, 40,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0, 36,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0, 32,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0, 28,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0, 24,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0, 16,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0, 12,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0
};

int Board::StringHold(void) const {
  	int sd, i, j, nDir, sqSrc, sqDst, sqStr;
  	int x, y, nSideTag, nOppSideTag;
	int vlString[2];

  	SlideMoveStruct *lpsmv;

  	for (sd = 0; sd < 2; sd ++) {
    	vlString[sd] = 0;
    	nSideTag = SIDE_TAG(sd), nOppSideTag = OPP_SIDE_TAG(sd);

    	// 考查用车来牵制的情况
    	for (i = ROOK_FROM; i <= ROOK_TO; i ++) {
      		sqSrc = this->Pieces[nSideTag + i].alive; // 本方车
      		if (sqSrc != 0) {
        		__ASSERT_SQUARE(sqSrc);
        		// 考查牵制目标是帅(将)的情况,捉将
        		sqDst = this->Pieces[nOppSideTag + KING].alive;
        		if (sqDst != 0) {
          			__ASSERT_SQUARE(sqDst);
          			x = FILE_X(sqSrc);
          			y = RANK_Y(sqSrc);
		  			// 车与对方将同一列
          			if (x == FILE_X(sqDst)) {
            			lpsmv = this->FileMovePtr(x, y);
						//! 0 表示下右 1 表示上左
            			nDir = (sqSrc < sqDst ? 0 : 1);
            			// 如果车用炮的吃法(炮用超级炮的着法)能吃到目标子"sqDst"，牵制就成立了，下同
            			if (RANK_Y(sqDst) == lpsmv->CannonCap[nDir]) {
              				// 被牵制子"sqStr"是车(炮)本身能吃到的棋子，下同
							sqStr = SQPOS_xy(x,lpsmv->RookCap[nDir]);
              				// sqStr = lpsmv->ucRookCap[nDir] + FILE_DISP(x);
              				__ASSERT_SQUARE(sqStr);
              				// 被牵制子必须是对方的子，下同
              				if ((this->Squares[sqStr] & nOppSideTag) != 0) {
                				// 如果被牵制子是有价值的，而且被牵制子没有保护(被目标子保护不算?)
								// 那么牵制是有价值的，下同
								//  redrook -- black cannon -- black king
                				if (cnValuableStringPieces[this->Squares[sqStr]] > 0 &&
                    				!this->Protected(OPP_SIDE(sd), sqStr, sqDst)) {
                  					vlString[sd] += ccvlStringValueTab[sqDst - sqStr + 256];
                				}
              				}
            			}
          			} else if (y == RANK_Y(sqDst)) { // 车与对方将同一行
            			lpsmv = this->RankMovePtr(x, y);
            			nDir = (sqSrc < sqDst ? 0 : 1);
            			if (FILE_X(sqDst) == lpsmv->CannonCap[nDir]) {
              				sqStr = SQPOS_xy(lpsmv->RookCap[nDir],y);
              				__ASSERT_SQUARE(sqStr);
              				if ((this->Squares[sqStr] & nOppSideTag) != 0) {
                				if (cnValuableStringPieces[this->Squares[sqStr]] > 0 &&
                    				!this->Protected(OPP_SIDE(sd), sqStr, sqDst)) {
                  					vlString[sd] += ccvlStringValueTab[sqDst - sqStr + 256];
                				}
              				}
            			}
          			}
        		}
        		// 考查牵制目标是车的情况
        		for (j = ROOK_FROM; j <= ROOK_TO; j ++) {
          			sqDst = this->Pieces[nOppSideTag + j].alive;
          			if (sqDst != 0) {
            			__ASSERT_SQUARE(sqDst);
            			x = FILE_X(sqSrc);
            			y = RANK_Y(sqSrc);
            			if (x == FILE_X(sqDst)) {
              				lpsmv = this->FileMovePtr(x, y);
              				nDir = (sqSrc < sqDst ? 0 : 1);
							// 车可以用炮的方法吃到dst,捉车
              				if (RANK_Y(sqDst) == lpsmv->CannonCap[nDir]) {
                				sqStr = SQPOS_xy(x,lpsmv->RookCap[nDir]); // 则 sqstr 被牵制，不能移动
                				__ASSERT_SQUARE(sqStr);
								//  redrook -- black cannon -- black rook
                				if ((this->Squares[sqStr] & nOppSideTag) != 0) {
                  					// 目标子是车，不同于帅(将)，要求车也没有保护时才有牵制价值(否则就不能强行捉车?),否则，下同
                  					if (cnValuableStringPieces[this->Squares[sqStr]] > 0 &&
                      					!this->Protected(OPP_SIDE(sd), sqDst) && !this->Protected(OPP_SIDE(sd), sqStr, sqDst)) {
                    					vlString[sd] += ccvlStringValueTab[sqDst - sqStr + 256];
                  					}
                				}
              				}
            			} else if (y == RANK_Y(sqDst)) {
              				lpsmv = this->RankMovePtr(x, y);
              				nDir = (sqSrc < sqDst ? 0 : 1);
              				if (FILE_X(sqDst) == lpsmv->CannonCap[nDir]) {
                				sqStr = SQPOS_xy(lpsmv->RookCap[nDir],y); // 则 sqstr 被牵制，不能移动
                				__ASSERT_SQUARE(sqStr);
                				if ((this->Squares[sqStr] & nOppSideTag) != 0) {
                  					if (cnValuableStringPieces[this->Squares[sqStr]] > 0 &&
                      					!this->Protected(OPP_SIDE(sd), sqDst) && !this->Protected(OPP_SIDE(sd), sqStr, sqDst)) {
                    					vlString[sd] += ccvlStringValueTab[sqDst - sqStr + 256];
                  					}
                				}
              				}
            			}
          			}
        		}
      		}
    	}

    	// 考查用炮来牵制的情况
    	for (i = CANNON_FROM; i <= CANNON_TO; i ++) {
      		sqSrc = this->Pieces[nSideTag + i].alive;
      		if (sqSrc != 0) {
        		__ASSERT_SQUARE(sqSrc);
        		// 考查牵制目标是帅(将)的情况 捉将
        		sqDst = this->Pieces[nOppSideTag + KING].alive;
        		if (sqDst != 0) {
          			__ASSERT_SQUARE(sqDst);
          			x = FILE_X(sqSrc);
          			y = RANK_Y(sqSrc);
          			if (x == FILE_X(sqDst)) {
            			lpsmv = this->FileMovePtr(x, y);
            			nDir = (sqSrc < sqDst ? 0 : 1);
            			if (RANK_Y(sqDst) == lpsmv->SuperCap[nDir]) {
                			sqStr = SQPOS_xy(x,lpsmv->CannonCap[nDir]); // 则 sqstr 被牵制，不能移动
              				__ASSERT_SQUARE(sqStr);
              				if ((this->Squares[sqStr] & nOppSideTag) != 0) {
								// DEBUG_(this->Squares[sqStr]);
								// DEBUG_("cnValuableStringPieces[this->Squares[sqStr]] > 0 ",cnValuableStringPieces[this->Squares[sqStr]] > 0);
								// 炮的情况下，必须是马 > 1 作为牵制的考察，否则两个互相牵制
                				if (cnValuableStringPieces[this->Squares[sqStr]] > 1 &&
                    				!this->Protected(OPP_SIDE(sd), sqStr, sqDst)) {
	                  				vlString[sd] += ccvlStringValueTab[sqDst - sqStr + 256];
                				}
              				}
            			}
          			} else if (y == RANK_Y(sqDst)) {
	            		lpsmv = this->RankMovePtr(x, y);
	            		nDir = (sqSrc < sqDst ? 0 : 1);
            			if (FILE_X(sqDst) == lpsmv->SuperCap[nDir]) {
                			sqStr = SQPOS_xy(lpsmv->CannonCap[nDir],y); // 则 sqstr 被牵制，不能移动
	              			__ASSERT_SQUARE(sqStr);
              				if ((this->Squares[sqStr] & nOppSideTag) != 0) {
                				if (cnValuableStringPieces[this->Squares[sqStr]] > 1 &&
                    				!this->Protected(OPP_SIDE(sd), sqStr, sqDst)) {
	                  				vlString[sd] += ccvlStringValueTab[sqDst - sqStr + 256];
                				}
              				}
            			}
          			}
        		}

        		// 考查牵制目标是车的情况 捉车
        		for (j = ROOK_FROM; j <= ROOK_TO; j ++) {
	          		sqDst = this->Pieces[nOppSideTag + j].alive;
          			if (sqDst != 0) {
            			__ASSERT_SQUARE(sqDst);
            			x = FILE_X(sqSrc);
            			y = RANK_Y(sqSrc);
            			if (x == FILE_X(sqDst)) {
	              			lpsmv = this->FileMovePtr(x, y);
	              			nDir = (sqSrc < sqDst ? 0 : 1);
              				if (RANK_Y(sqDst) == lpsmv->SuperCap[nDir]) {
                				sqStr = SQPOS_xy(x,lpsmv->CannonCap[nDir]); // 则 sqstr 被牵制，不能移动
                				__ASSERT_SQUARE(sqStr);
                				if ((this->Squares[sqStr] & nOppSideTag) != 0) {
									// ?为什么这里不需要检查目标sqdst 也被保护
                  					if (cnValuableStringPieces[this->Squares[sqStr]] > 1 &&
                      					!this->Protected(OPP_SIDE(sd), sqStr, sqDst)) {
                    					vlString[sd] += ccvlStringValueTab[sqDst - sqStr + 256];
                  					}
                				}
              				}
            			} else if (y == RANK_Y(sqDst)) {
	              			lpsmv = this->RankMovePtr(x, y);
	              			nDir = (sqSrc < sqDst ? 0 : 1);
              				if (FILE_X(sqDst) == lpsmv->SuperCap[nDir]) {
								sqStr = SQPOS_xy(lpsmv->CannonCap[nDir],y);
                				__ASSERT_SQUARE(sqStr);
                				if ((this->Squares[sqStr] & nOppSideTag) != 0) {
									// ?为什么这里不需要检查目标sqdst 也被保护
									// DEBUG_("protected",this->Protected(OPP_SIDE(sd), sqStr, sqDst));
                  					if (cnValuableStringPieces[this->Squares[sqStr]] > 1 &&
                      					!this->Protected(OPP_SIDE(sd), sqStr, sqDst)) {
                    					vlString[sd] += ccvlStringValueTab[sqDst - sqStr + 256];
                  					}
                				}
              				}
            			}
          			}
        		}
      		}
    	}
	}
	// #endif
	return SIDE_VALUE(isRedTurn ? 0 : 1, vlString[0] - vlString[1]);
}


// part 3 车的灵活性的评价
int Board::RookMobility(void) const {
  	int sd, i, sqSrc, nSideTag, x, y;
  	int vlRookMobility[2];

  	for (sd = 0; sd < 2; sd ++) {
    	vlRookMobility[sd] = 0;
    	nSideTag = SIDE_TAG(sd);
    	for (i = ROOK_FROM; i <= ROOK_TO; i ++) {
	      	sqSrc = this->Pieces[nSideTag + i].alive;
      		if (sqSrc != 0) {
        		__ASSERT_SQUARE(sqSrc);
        		x = FILE_X(sqSrc);
        		y = RANK_Y(sqSrc);
        		vlRookMobility[sd] += PreEvalEx.cPopCnt16[this->RankMaskPtr(x, y)->wNonCap] +
            		PreEvalEx.cPopCnt16[this->FileMaskPtr(x, y)->wNonCap];
      		}
    	}
    	assert(vlRookMobility[sd] <= 34);
  	}

  	// return SIDE_VALUE(isRedTurn ? 0 : 1, vlRookMobility[0] - vlRookMobility[1]) >> 1;
  	return SIDE_VALUE(isRedTurn ? 0 : 1, vlRookMobility[0] - vlRookMobility[1]);
}

// part 4 马受到阻碍的评价

// 常数表"cbcEdgeSquares"给定了不利于马的位置，处于棋盘边缘和两个花心位置的马都是坏马
static const bool cbcEdgeSquares[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int Board::KnightTrap(void) const {
  	int sd, i, sqSrc, sqDst, nSideTag, nMovable;
  	uint8_t *lpucsqDst, *lpucsqPin;
  	int vlKnightTraps[2];

  	for (sd = 0; sd < 2; sd ++) {
    	vlKnightTraps[sd] = 0;
    	nSideTag = SIDE_TAG(sd);
    	// 考虑马可以走的位置，走到棋盘边缘上，或者走到对方的控制格，都必须排除
    	for (i = KNIGHT_FROM; i <= KNIGHT_TO; i ++) {
	      	sqSrc = this->Pieces[nSideTag + i].alive;
      		if (sqSrc != 0) {
        		__ASSERT_SQUARE(sqSrc);
        		nMovable = 0;
        		lpucsqDst = KnightMoves[sqSrc];
        		lpucsqPin = KnightPins[sqSrc];
        		sqDst = *lpucsqDst;
        		while (sqDst != 0) {
	          		__ASSERT_SQUARE(sqDst);
          			// 以下的判断区别于"genmoves.cpp"中的着法生成器，排除了走到棋盘边缘和走到对方控制格的着法
          			if (!cbcEdgeSquares[sqDst] && this->Squares[sqDst] == 0 &&
              			this->Squares[*lpucsqPin] == 0 && !this->Protected(OPP_SIDE(sd), sqDst)) {
            			nMovable ++;
            			if (nMovable > 1) {
	              			break;
            			}
          			}
          			lpucsqDst ++;
          			sqDst = *lpucsqDst;
          			lpucsqPin ++;
        		}
        		// 没有好的着法的马给予10分罚分，只有一个好的着法的马给予5分罚分
        		if (nMovable == 0) {
          			vlKnightTraps[sd] += 10;
        		} else if (nMovable == 1) {
          			vlKnightTraps[sd] += 5;
        		}
      		}else{
				// 无马
				vlKnightTraps[sd] += 15;
			}

      		// assert(vlKnightTraps[sd] <= 20);
    	}
  	}

  	return SIDE_VALUE(isRedTurn ? 0 : 1, vlKnightTraps[1] - vlKnightTraps[0]);
}

int Board::Evaluate(int vlAlpha, int vlBeta) {
    int vl;
    vl = bKnowledge ? EvaluateLevel1(vlAlpha, vlBeta) : Material();
    //   return vl == pos.DrawValue() ? vl - 1 : vl;
    return vl;
}


int Board::Evaluate() {
    int vl = 0;
	
	int m = this->Material();
	vl += m;

	int a = this->AdvisorShape(); 
	vl += a;

	int s = this->StringHold(); 
  	vl += s; 

	int r = this->RookMobility(); 
  	vl += r; 

	int k = this->KnightTrap(); 
	vl += k; 

#ifdef SHOW_EVAL
	logout << "--------------------\n";
	for(int y = rowOffset; y <= rowOffsetTo; ++y){
		for(int x = colOffset; x <= colOffsetTo; ++x){
			int pt = Squares[SQPOS_xy(x,y)];	
			if(pt == 0){
				logout << "*";
			}else{
				if(pt & (1 << 5)){
					logout << (char)(cszPieceBytes[cnPieceTypes[pt]] - 'A' + 'a');
				}else{
					logout << (char)(cszPieceBytes[cnPieceTypes[pt]]);
				}
			};
		}
		logout << '\n';
	}
	logout	<< "[ " << (isRedTurn ? "red" : "black") << " turn ]\n";
	logout  << "[" 
			<< " Material: " << m
			<< " AdvisorShape: " << a
			<< " StringHold: " << s
			<< " RookMobility: " << r
			<< " KnightTrap: " << k << " ]\n";
	logout 	<< "[ vl: " << vl << " ]\n";
	logout << "--------------------\n";
#endif

    return vl;
}

// 局面评价过程
int Board::EvaluateLevel1(int vlAlpha, int vlBeta) const{
	
  	int vl;

  	vl = this->Material();
  	if (vl + EVAL_MARGIN1 <= vlAlpha) {
    	return vl + EVAL_MARGIN1;
  	} else if (vl - EVAL_MARGIN1 >= vlBeta) {
    	return vl - EVAL_MARGIN1;
  	}

  	vl += this->AdvisorShape();
  	if (vl + EVAL_MARGIN2 <= vlAlpha) {
    	return vl + EVAL_MARGIN2;
  	} else if (vl - EVAL_MARGIN2 >= vlBeta) {
    	return vl - EVAL_MARGIN2;
  	}

  	vl += this->StringHold();
  	if (vl + EVAL_MARGIN3 <= vlAlpha) {
    	return vl + EVAL_MARGIN3;
  	} else if (vl - EVAL_MARGIN3 >= vlBeta) {
    	return vl - EVAL_MARGIN3;
  	}

  	vl += this->RookMobility();
  	if (vl + EVAL_MARGIN4 <= vlAlpha) {
    	return vl + EVAL_MARGIN4;
  	} else if (vl - EVAL_MARGIN4 >= vlBeta) {
    	return vl - EVAL_MARGIN4;
  	}

  	return vl + this->KnightTrap();
}