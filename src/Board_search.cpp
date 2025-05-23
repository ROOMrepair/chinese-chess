#include "Board.hpp"

void Board::resetSearchBoard(const Board& other){
	this->isRedTurn = other.isRedTurn;
	this->isRedSide = other.isRedSide;

	this->ndepth = other.ndepth;
	this->evBlack = other.evBlack;
	this->evRed = other.evRed;
	this->lastmoveCur = other.lastmoveCur;
	this->bKnowledge = other.bKnowledge;

	std::copy(std::begin(other.Squares), std::end(other.Squares), std::begin(this->Squares));
	std::copy(std::begin(other.Pieces), std::end(other.Pieces), std::begin(this->Pieces));
	std::copy(std::begin(other.wBitRows), std::end(other.wBitRows), std::begin(this->wBitRows));
	std::copy(std::begin(other.wBitCols), std::end(other.wBitCols), std::begin(this->wBitCols));

	this->BitPiece = other.BitPiece;
	this->zobr = other.zobr;
}

void Board::doSearch(bool isDelay){
	
	if(!this->searchBoard){
		this->searchBoard = std::make_unique<Board>(
			this->mcs,
			this->tr,
			this->isRedSide,
			false,	//! 搜索的board 内部不应该触发 dosearch,只调用局面搜索
			this->ndepth,
			true
		);
	}
	
	if(this->search_thread_.joinable()){
		this->search_thread_.join();
	}
	
	this->searchBoard->resetSearchBoard(*this);
	ClearHash();

	auto self = shared_from_this();
	this->search_thread_ = std::thread([self,isDelay](){

			if(isDelay){
				std::this_thread::sleep_for(std::chrono::seconds(2));	
			}
			self->tr.start_record();
			self->isThinking.store(true);
			self->searchBoard->isThinking.store(true);

			int bestMove = self->searchBoard->SearchMain(self->searchBoard->ndepth);
			self->tr.end_record();

			self->postSearch(bestMove);
	});

}

void Board::postSearch(int bestmove){
	this->isThinking.store(false);
	this->searchBoard->isThinking.store(false);

	if(bestmove == -1 || disCardSearchRes){
		return;	
	}

	int sqSrc = SRC(bestmove);
	int sqDst = DST(bestmove);

	__ASSERT_SQUARE(sqSrc);
	__ASSERT_SQUARE(sqDst);
	
	this->activatedPos.x = BOARD_X(sqSrc);
	this->activatedPos.y = BOARD_Y(sqSrc);

	if(this->move(sqSrc,sqDst,false) != SUCCESS){
		DEBUG_("invalid move from ",sqSrc," to ",sqDst);
	}
}

void Board::stopSearch(bool discard){
	this->isThinking.store(false);	
	if(this->searchBoard){
		this->searchBoard->isThinking.store(false);
	}
	if(this->search_thread_.joinable()){
		disCardSearchRes = discard;
		this->search_thread_.join();
		disCardSearchRes = false;
	}
}