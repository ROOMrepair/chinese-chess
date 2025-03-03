#pragma once

#include "State.hpp"
#include "Asset.hpp"
#include "Board.hpp"
#include "Piece.hpp"

// 注意多态下的析构
struct PieceState{
	std::unique_ptr<Piece> p;
	bool isDead;	
	int x;
	int y;
};


class GameState: public State
{
private:
public:

	bool isRedTurn;	
	
	PieceState BlackCamp[PieceCampNum];
	PieceState RedCamp[PieceCampNum];

	std::unique_ptr<Board> board;

	GameState();
	~GameState();

	

	void renderScene() override;
	void update() override;
};


