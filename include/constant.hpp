#pragma once

#include "raylib.h"

// check move type
#define SUCCESS 0
#define CHECKMATED 1
#define NOKING 100

								
// font 
const int DEFAULT_FONT_SIZE = 32;

// screen render
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// notify

// board render
const int BOARD_PADDING_X = 40;
const int BOARD_PADDING_Y = 40;
const int BOARD_BORDER_WIDTH = 10;

const Color BOARD_LINE_COLOR = {171,93,22,255};
const Color BORDER_COLOR = {218,185,151,255};

// piece
const Color RING_COLOR_Purple = {175, 45, 245, 255};
const Color RING_COLOR_Orange = {245, 45, 45, 255};

// board info
const int B_WIDTH = 9;
const int B_HEIGHT = 10;

const int KING_TYPE = 0;
const int ADVISOR_TYPE = 1;
const int BISHOP_TYPE = 2;
const int KNIGHT_TYPE = 3;
const int ROOK_TYPE = 4;
const int CANNON_TYPE = 5;
const int PAWN_TYPE = 6;

const int KING = 0;
const int ADVISOR_FROM = 1;
const int ADVISOR_TO = 2;
const int BISHOP_FROM = 3;
const int BISHOP_TO = 4;
const int KNIGHT_FROM = 5;
const int KNIGHT_TO = 6;
const int ROOK_FROM = 7;
const int ROOK_TO = 8;
const int CANNON_FROM = 9;
const int CANNON_TO = 10;
const int PAWN_FROM = 11;
const int PAWN_TO = 15;

const int KING_BITPIECE = 1 << KING; // 1
const int ADVISOR_BITPIECE = (1 << ADVISOR_FROM) | (1 << ADVISOR_TO); // 110
const int BISHOP_BITPIECE = (1 << BISHOP_FROM) | (1 << BISHOP_TO); // 11000
const int KNIGHT_BITPIECE = (1 << KNIGHT_FROM) | (1 << KNIGHT_TO);
const int ROOK_BITPIECE = (1 << ROOK_FROM) | (1 << ROOK_TO);
const int CANNON_BITPIECE = (1 << CANNON_FROM) | (1 << CANNON_TO);
const int PAWN_BITPIECE = (1 << PAWN_FROM) | (1 << (PAWN_FROM + 1)) |
	(1 << (PAWN_FROM + 2)) | (1 << (PAWN_FROM + 3)) | (1 << PAWN_TO);
const int ATTACK_BITPIECE = KNIGHT_BITPIECE | ROOK_BITPIECE | CANNON_BITPIECE | PAWN_BITPIECE;
const int PieceToList[7] = {KING,ADVISOR_TO,BISHOP_TO,KNIGHT_TO,ROOK_TO,CANNON_TO,PAWN_TO};

const int cnPieceTypes[48] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 6,
  0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 6
};

// 棋子的简单分值，只在简单比较时作参考
const int cnSimpleValues[48] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  5, 1, 1, 1, 1, 3, 3, 4, 4, 3, 3, 2, 2, 2, 2, 2,
  5, 1, 1, 1, 1, 3, 3, 4, 4, 3, 3, 2, 2, 2, 2, 2,
};

// 大写为 red 
const char *const cszStartFen = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r";

const char *const cszPieceBytes = "KABNRCP";

const int rowOffset = 3;
const int colOffset = 3;

const int colCenter = 7;

const int rowOffsetTo = 12;
const int colOffsetTo = 11;