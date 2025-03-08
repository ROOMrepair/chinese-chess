#pragma once

#include "raylib.h"
#include "Asset.hpp"

class Piece
{
public:
	Camp camp_type;
	PieceType piece_type;

	bool isDead;	
	int x;
	int y;

	Piece(Camp _camp, PieceType _ptype, int pIndex) : camp_type(_camp), piece_type(_ptype)
	{
		
	}

	~Piece() = default;

	void drawPiece(int x, int y);

	void move(int x, int y);
	void canMove(int x, int y);
};
