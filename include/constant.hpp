#pragma once

#include "raylib.h"

// check move type
#define SUCCESS 0
#define CHECKMATED 1

const int MAX_GEN_MOVES = 128;  // 搜索的最大着法数，中国象棋的任何局面都不会超过120个着法
								
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

// 大写为 red 
// init: "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r";
const char *const cszStartFen = "1nbakab1r/9/1c5c1/p1p3p1p/9/4p1n2/P1P1P1P1P/1Cr4C1/7N1/RNBAKAB1R r";

const char *const cszPieceBytes = "KABNRCP";

const int rowOffset = 3;
const int colOffset = 3;

const int colCenter = 7;

const int rowOffsetTo = 12;
const int colOffsetTo = 11;

inline Color PIECE_COLOR(int pt){
	return (pt & (1 << 5)) ? BLACK : RED;
}
