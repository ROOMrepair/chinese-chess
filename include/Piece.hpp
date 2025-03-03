#pragma once

#include "raylib.h"
#include "Asset.hpp"

class Piece
{
public:
	Camp camp_type;
	PieceType piece_type;

	int pieceIndex;

	Piece(Camp _camp, PieceType _ptype, int pIndex) : camp_type(_camp), piece_type(_ptype), pieceIndex(pIndex)
	{
	}

	virtual ~Piece() = default;

	void drawPiece(int x, int y);

	virtual void move(int x, int y) = 0;
	virtual void canMove(int x, int y) = 0;
};

class Bing : public Piece
{
	Bing(Camp _camp, PieceType _ptype, int pIndex) : Piece(_camp, _ptype, pIndex) {}
};

class Pao : public Piece
{
	Pao(Camp _camp, PieceType _ptype, int pIndex) : Piece(_camp, _ptype, pIndex) {}
};

class Che : public Piece
{
	Che(Camp _camp, PieceType _ptype, int pIndex) : Piece(_camp, _ptype, pIndex) {}
};

class Ma : public Piece
{
	Ma(Camp _camp, PieceType _ptype, int pIndex) : Piece(_camp, _ptype, pIndex) {}
};

class Xiang : public Piece
{
	Xiang(Camp _camp, PieceType _ptype, int pIndex) : Piece(_camp, _ptype, pIndex) {}
};

class Shi : public Piece
{
	Shi(Camp _camp, PieceType _ptype, int pIndex) : Piece(_camp, _ptype, pIndex) {}
};

class Jiang : public Piece
{
	Jiang(Camp _camp, PieceType _ptype, int pIndex) : Piece(_camp, _ptype, pIndex) {}
};