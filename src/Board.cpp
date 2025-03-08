#include "Board.hpp"

Board::Board()
{
}

Board::~Board()
{
	std::cout << "Board destroyed" << std::endl;
}

// const char *const cszStartFen = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w";
bool Board::loadFromFen(const char *szFen)
{
	clearBoard();
	
	const char* p = szFen;
	//? 
	int pieceBlack[7];
	int pieceRed[7];
	
	pieceRed[KING_TYPE] = SIDE_TAG(0) + KING;
	pieceRed[ADVISOR_TYPE] = SIDE_TAG(0) + ADVISOR_FROM;
	pieceRed[BISHOP_TYPE] = SIDE_TAG(0) + BISHOP_FROM;
	pieceRed[KNIGHT_TYPE] = SIDE_TAG(0) + KNIGHT_FROM;
	pieceRed[ROOK_TYPE] = SIDE_TAG(0) + ROOK_FROM;
	pieceRed[CANNON_TYPE] = SIDE_TAG(0) + CANNON_FROM;
	pieceRed[PAWN_TYPE] = SIDE_TAG(0) + PAWN_FROM;

	for(int i = 0;i < 7; ++i){
		pieceBlack[i] = pieceRed[i] + 16;
	}

	// 映射到 square 中
	int row = 0, col = 0;

	while(*p != ' '){
		if( *p == '/' ){
			row++;
			col = 0;
		}else if(*p >= '1' && *p <= '9'){
			int skip = *p - '0';
			col += skip;
		}else if(*p >= 'A' && *p <= 'Z'){
			int pt = PieceStrMap(*p);
			if(pt < 0) return false;
				
													 

		}else if(*p >= 'a' && *p <= 'z'){
			// black
 			int pt = PieceStrMap(*p - 'a' + 'A');
			if(pt < 0) return false;

		}

		p++;
	}

	return true;
}

void Board::clearBoard()
{
	std::memset(Squares, 0, 256);
	std::memset(Pieces, 0, 48);
}

int Board::PieceStrMap(char c){
	switch (c)
	{
	case 'K':
		return KING_TYPE;
	case 'A':
		return ADVISOR_TYPE;
	case 'B':
		return BISHOP_TYPE;
	case 'N':
		return KNIGHT_TYPE;
	case 'R':
		return ROOK_TYPE;
	case 'C':
		return CANNON_TYPE;
	case 'P':
		return PAWN_TYPE;
	default:
		return -1;
	}
}

void Board::drawBoard(int x, int y)
{
	auto asset = Asset::getInstance();
	auto bt = asset->pieceTextures[BOARD_INDEX].texture;
	auto bgt = asset->pieceTextures[BOARD_BG_INDEX].texture;
}