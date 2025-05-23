#include "Board.hpp"

char szFenState[256] = {};

Board::Board(MouseClickState &ms,Timer &t, bool isredside,bool useai,int depth,bool isInnerBoard) :
	mcs(ms),tr(t),isRedSide(isredside),useAi(useai),ndepth(depth){
	this->isRedTurn = true;

	this->activatedPos = {-1, -1};
	this->BitPiece = 0;
	this->isExchangeSide = false;
	this->isThinking.store(false);

	if(!isInnerBoard){
		this->clearBoard();
	}
	this->caculateBoardInfo();
}

Board::Board(MouseClickState &ms,Timer &t,bool isInnerBoard):
 	mcs(ms),tr(t){

	this->activatedPos = {-1, -1};
	this->BitPiece = 0;
	this->isExchangeSide = false;
	this->isThinking.store(false);

	if(!isInnerBoard){
		this->clearBoard();
	}
	this->caculateBoardInfo();
}

Board::~Board()
{
	std::cout << "Board destroyed" << std::endl;
}

int Board::PieceStrMap(char c)
{
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

int Board::PieceTextureMap(int piece)
{
	if (piece & (1 << 5))
	{
		// black
		switch (piece & 0x0f)
		{
		case KING:
			return KING_BLACK_INDEX;
		case ADVISOR_FROM ... ADVISOR_TO:
			return ADVISOR_BLACK_INDEX;
		case BISHOP_FROM ... BISHOP_TO:
			return BISHOP_BLACK_INDEX;
		case KNIGHT_FROM ... KNIGHT_TO:
			return KNIGHT_BLACK_INDEX;
		case ROOK_FROM ... ROOK_TO:
			return ROOK_BLACK_INDEX;
		case CANNON_FROM ... CANNON_TO:
			return CANNON_BLACK_INDEX;
		case PAWN_FROM ... PAWN_TO:
			return PAWN_BLACK_INDEX;
		default:
			return -1;
		}
	}
	else
	{
		// red
		switch (piece & 0x0f)
		{
		case KING:
			return KING_RED_INDEX;
		case ADVISOR_FROM ... ADVISOR_TO:
			return ADVISOR_RED_INDEX;
		case BISHOP_FROM ... BISHOP_TO:
			return BISHOP_RED_INDEX;
		case KNIGHT_FROM ... KNIGHT_TO:
			return KNIGHT_RED_INDEX;
		case ROOK_FROM ... ROOK_TO:
			return ROOK_RED_INDEX;
		case CANNON_FROM ... CANNON_TO:
			return CANNON_RED_INDEX;
		case PAWN_FROM ... PAWN_TO:
			return PAWN_RED_INDEX;
		default:
			return -1;
		}
	}
}

void Board::boardToFen(char *szFen){
  	char *lpFen = szFen;
	for(int y = rowOffset;y <= rowOffsetTo;y++){
		int k = 0;
		for(int x = colOffset;x <= colOffsetTo;x++){
			int pt = Squares[SQPOS_xy(x,y)]; 
			if(pt != 0){
				if(k > 0){
					// 记录之前空格	
					*lpFen = k + '0';
					lpFen ++;
					k = 0;
				}
				*lpFen = PIECE_BYTE(PIECE_TYPE(pt)) + (pt < 32 ? 0 : 'a' - 'A');
				lpFen++;
			}else{
				k++;	
			}			
		}
		if(k > 0){
      		*lpFen = k + '0';
      		lpFen ++;
		}
    	*lpFen = '/';
    	lpFen ++;
	}						
  	*(lpFen - 1) = ' '; // 把最后一个'/'替换成' '

  	*lpFen = (this->isRedTurn ? 'r' : 'b');
  	lpFen ++;
  	*lpFen = '\0';
}

bool Board::loadFromFen(const char *szFen)
{
	const char *p = szFen;
	const char *tmp = p;

	const char *temp = szFen;
	while(*temp != '\0'){
		std::cout << *temp;
		temp++;		
	}
	std::cout << std::endl;

	int pieceBlack[7];
	int pieceRed[7];

	pieceRed[KING_TYPE] = SIDE_TAG(0) + KING;			 // 16
	pieceRed[ADVISOR_TYPE] = SIDE_TAG(0) + ADVISOR_FROM; // 17
	pieceRed[BISHOP_TYPE] = SIDE_TAG(0) + BISHOP_FROM;
	pieceRed[KNIGHT_TYPE] = SIDE_TAG(0) + KNIGHT_FROM;
	pieceRed[ROOK_TYPE] = SIDE_TAG(0) + ROOK_FROM;
	pieceRed[CANNON_TYPE] = SIDE_TAG(0) + CANNON_FROM;
	pieceRed[PAWN_TYPE] = SIDE_TAG(0) + PAWN_FROM;

	for (int i = 0; i < 7; ++i)
	{
		pieceBlack[i] = pieceRed[i] + 16;
	}

	int row = 0, col = 0;

	// k 在上半部(前三行) && isRedSide 不交换
	// k 在上半部(前三行) && !isRedSide 交换
	// K 在上半部(前三行) && isRedSide	交换
	// K 在上半部(前三行) && !isRedSide 不交换
	// "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/7N1/RNBAKAB1R w";
	// 暂时认为先出现的在上方
	// i == 0 为 pawn 在下半部（在 square 数组的下半部）
	while (*tmp != ' ')
	{
		if (*tmp == 'k')
		{
			if(!ISMENUMODE){
				if (isRedSide)
					isExchangeSide = false;
				else
					isExchangeSide = true;
			}
			break;
		}
		else if (*tmp == 'K')
		{
			if(!ISMENUMODE){
				if (isRedSide)
					isExchangeSide = true;
				else
					isExchangeSide = false;
			}
			break;
		}
		tmp++;
	}

	while (*p != ' ')
	{
		if (*p == '/')
		{
			row++;
			col = 0;
			p++;
			continue;
		}

		if (col >= 9 || col < 0 || row >= 10 || row < 0)
		{
			DEBUG_("out of bound\n");
			return false;
		}

		if (*p >= '1' && *p <= '9')
		{
			int skip = *p - '0';
			col += skip;
		}
		else if (*p >= 'A' && *p <= 'Z')
		{
			// red
			int pt = PieceStrMap(*p);
			if (pt < 0)
				return false;
			if (Pieces[pieceRed[pt]].alive == 1)
			{
				DEBUG_("piece red alive error\n");
				return false;
			}
			if (!addPiece(row, col, pieceRed[pt]))
			{
				DEBUG_("Add piece red error\n");
				return false;
			}
			pieceRed[pt]++;

			col++;
		}
		else if (*p >= 'a' && *p <= 'z')
		{
			// black
			int pt = PieceStrMap(*p - 'a' + 'A');
			if (pt < 0)
				return false;
			if (Pieces[pieceBlack[pt]].alive == 1)
			{
				DEBUG_("piece black alive error\n");
				return false;
			}
			if (!addPiece(row, col, pieceBlack[pt]))
			{
				DEBUG_("Add piece black error\n");
				return false;
			}
			pieceBlack[pt]++;

			col++;
		}

		p++;
	}

	while(*p != '\0'){
		if(*p == 'r'){
			this->isRedTurn = true;
		}else if(*p == 'b'){
			this->isRedTurn = false;
		}
		p++;
	}
	// add to discarded piece
	// ! 这些弃子没有重新加到 Square和 Pieces 中，需要手动添加
	for(int i = 0; i < 7;++i){
		int redptFrom = pieceRed[i];
		int redptTo = SIDE_TAG(0) + PieceToList[i];
		while(redptFrom <= redptTo){
			// DEBUG_("red dicarded: ",redptFrom);
			disCardedPieces.push_back(redptFrom);
			Pieces[redptFrom].alive = 0;
			Pieces[redptFrom].textureIndex = PieceTextureMap(redptFrom);
			redptFrom++;
		}
	}	
		
	for(int i = 0; i < 7; ++i){
		int blackptFrom = pieceBlack[i];
		int blackptTo = SIDE_TAG(1) + PieceToList[i];
		while(blackptFrom <= blackptTo){
			// DEBUG_("black dicarded: ",blackptFrom);
			disCardedPieces.push_back(blackptFrom);
			Pieces[blackptFrom].alive = 0;
			Pieces[blackptFrom].textureIndex = PieceTextureMap(blackptFrom);
			blackptFrom++;
		}
	}	


	for(auto i:disCardedPieces){
		std::cout << "discarded: " << i << std::endl;
	}
	return true;
}

bool Board::addPiece(int row, int col, int piece)
{
	int pt;
	// todo 这里暂时在 addpiece 中同步修改 wbitmask
	// todo 在每一步对棋盘的更改处,都需要对 wbitmask 更改
	// 这个决定了最终的棋子分布
	if (isExchangeSide)
	{
		row = 9 - row;
		col = 8 - col;
	}

	int sq = SQpos(row, col);
	Squares[sq] = piece;

	wBitCols[FILE_X(sq)] ^= ColMask(sq);
	wBitRows[RANK_Y(sq)] ^= RowMask(sq);

	Pieces[piece].alive = sq;
	BitPiece ^= BIT_PIECE(piece);

	int textureIndex = PieceTextureMap(piece);
	if (textureIndex < 0)
	{
		return false;
	}
	Pieces[piece].textureIndex = textureIndex;

	// 叠加所有棋子的信息
	pt = PIECE_TYPE(piece);
	if(piece >= 32){
		pt += 7;	
	}
  	this->zobr.Xor(zobrTable[pt][sq]);
	return true;
}

void Board::clearBoard()
{
    zobr.InitZero();
	ClearRollbackList();
	this->lastmoveCur = 0;
	ClearHash();
	evRed = evBlack = 0;
	BitPiece = 0;
	std::memset(Squares, 0, 256);
	std::memset(Pieces, 0, sizeof(PieceType) * 48);
	std::memset(wBitCols, 0, sizeof(uint16_t) * 16);
	std::memset(wBitRows, 0, sizeof(uint16_t) * 16);
	this->disCardedPieces.clear();
}

void Board::caculateBoardInfo()
{
	auto &asset = Asset::getInstance();
	auto &bt = asset.pieceTextures[BOARD_INDEX].texture;
	auto &bgt = asset.pieceTextures[BOARD_BG_INDEX].texture;

	int tHeight = bt.height, tWidth = bt.width;
	int gtHeight = bgt.height, gtWidth = bgt.width;

	this->binfo.boardWidth_t = tWidth;
	this->binfo.boardHeight_t = tHeight;

	this->binfo.bgWidth_t = gtWidth;
	this->binfo.bgHeight_t = gtHeight;

	float scaleY = (float)(SCREEN_HEIGHT - BOARD_PADDING_Y * 2) / tHeight;
	float scaleX = (float)(SCREEN_WIDTH - BOARD_PADDING_X * 2) / tWidth;
	float scale = scaleY > scaleX ? scaleX : scaleY;

	// use this to caculcate piece pos
	this->binfo.boardHeight = tHeight * scale;
	this->binfo.boardWidth = tWidth * scale;

	this->binfo.bgHeight = SCREEN_HEIGHT;
	this->binfo.bgWidth = SCREEN_WIDTH;

	this->binfo.gridHeight = this->binfo.boardHeight / 9;
	this->binfo.gridWidth = this->binfo.boardWidth / 8;

	this->binfo.pieceScale = 0.75f;
	this->binfo.BoardOriginX = BOARD_PADDING_X;
	this->binfo.BoardOriginY = BOARD_PADDING_Y;
}

Vector2 Board::screenXY(int x, int y)
{
	float cx = this->binfo.BoardOriginX + x * this->binfo.gridWidth;
	float cy = this->binfo.BoardOriginY + y * this->binfo.gridHeight;
	return (Vector2){cx, cy};
}

void Board::drawBackground()
{
	auto &asset = Asset::getInstance();
	auto &bt = asset.pieceTextures[BOARD_INDEX].texture;
	auto &bgt = asset.pieceTextures[BOARD_BG_INDEX].texture;

	Rectangle bt_src = {0, 0, this->binfo.boardWidth_t, this->binfo.boardHeight_t};
	Rectangle bt_dest = {(float)0 + BOARD_PADDING_X, (float)0 + BOARD_PADDING_Y, this->binfo.boardWidth, this->binfo.boardHeight};

	Rectangle bgt_src = {(float)0, (float)0, this->binfo.bgWidth_t, this->binfo.bgHeight_t};
	Rectangle bgt_dest = {(float)0, (float)0, this->binfo.bgWidth, this->binfo.bgHeight};

	DrawTexturePro(bgt, bgt_src, bgt_dest, {0, 0}, 0, WHITE);
	DrawTexturePro(bt, bt_src, bt_dest, {0, 0}, 0, BOARD_LINE_COLOR);
	DrawRectangleRoundedLinesEx({(float)BOARD_PADDING_X - 10,
								 (float)BOARD_PADDING_Y - 10,
								 this->binfo.boardWidth + 20,
								 this->binfo.boardHeight + 20},
								0.1f, 10, 5, BOARD_LINE_COLOR);
}

void Board::drawPieces(bool isDragging)
{
	// 激活的点位在最下面
	if (isDragging && activatedPos.x != -1 && activatedPos.y != -1)
	{
		Vector2 activePiece = screenXY(activatedPos.x, activatedPos.y);
		DrawCircleV(activePiece, 10, {66, 133, 244, 255});
	}

	float activeCircleX, activeCircleY, activeTx, activeTy;

	auto &asset = Asset::getInstance();
	for (int i = 0; i < B_HEIGHT; ++i)
	{
		for (int j = 0; j < B_WIDTH; ++j)
		{
			int sqPos = SQpos(i, j);
			int pt = Squares[sqPos];
			if (pt & 63)
			{
				Color c = PIECE_COLOR(pt);
				auto &p_texture = asset.pieceTextures[Pieces[pt].textureIndex].texture;

				// 0.5 texture 长宽 文字的中心位置
				float cx = p_texture.width * this->binfo.pieceScale * 0.5;
				float cy = p_texture.height * this->binfo.pieceScale * 0.5;

				// 背景中心
				Vector2 circleXY = screenXY(j, i);

				if (isDragging && activatedPos.x == j && activatedPos.y == i)
				{
					// 棋子跟随鼠标移动
					circleXY.x += this->mcs.dragMousePos.x - this->mcs.dragStartPos.x;
					circleXY.y += this->mcs.dragMousePos.y - this->mcs.dragStartPos.y;

					activeCircleX = circleXY.x;
					activeCircleY = circleXY.y;

					activeTx = circleXY.x - cx;
					activeTy = circleXY.y - cy;
				}

				if (moveAnim.isAnimating && activatedPos.x == j && activatedPos.y == i)
				{
					float progress = moveAnim.progress >= 1.0f ? 1.0f : moveAnim.progress;

					circleXY.x = moveAnim.startPos.x + (moveAnim.endPos.x - moveAnim.startPos.x) * progress;
					circleXY.y = moveAnim.startPos.y + (moveAnim.endPos.y - moveAnim.startPos.y) * progress;

					activeCircleX = circleXY.x;
					activeCircleY = circleXY.y;

					activeTx = circleXY.x - cx;
					activeTy = circleXY.y - cy;
				}

				// texture rect 左上角
				Vector2 textXY = {circleXY.x - cx, circleXY.y - cy};

				// 右上角画标记
				//
				DrawCircleV(
					circleXY,
					this->binfo.radius,
					c);
				DrawTextureEx(
					p_texture,
					textXY,
					0,
					this->binfo.pieceScale,
					WHITE);


#ifdef MYDEBUG
				if(showTags){
					DrawTextEx(asset.fonts[0].font, std::to_string(pt).c_str(), {textXY.x + cx * 2 - 5, textXY.y - 5}, 30, 0, GREEN);
					DrawTextEx(asset.fonts[0].font, std::to_string(sqPos).c_str(), {textXY.x + cx * 2 - 5, textXY.y - 30}, 30, 0, GREEN);

					// draw protected
					if(!ISMENUMODE){
						if (Protected(isRedTurn ? 0 : 1, sqPos, 0) && (((pt & (1 << 5)) >> 5) ^ isRedTurn))
						{
							DrawRing(circleXY,
							 		this->binfo.radius,
							 		this->binfo.radius + 5, 0, 360, 36, BLUE);
						}
						
					}
				}
#endif
			}
		}
	}

	// 将激活的棋子放在最上方
	if ((isDragging || moveAnim.isAnimating) &&
		activatedPos.x != -1 && activatedPos.y != -1)
	{
		int pt = Squares[SQpos(activatedPos.y, activatedPos.x)];
		Color c = PIECE_COLOR(pt);
		auto &p_texture = asset.pieceTextures[Pieces[pt].textureIndex].texture;

		DrawCircleV({activeCircleX, activeCircleY}, this->binfo.radius, c);
		DrawTextureEx(p_texture, {activeTx, activeTy}, 0, this->binfo.pieceScale, WHITE);
	}
}

void Board::PieceFreePos(int pt,std::function<void(int,int,int)> cb){
	bool isred = !((1 << 5) & pt);
	__ASSERT_PIECE(pt);

	auto checkSideValid = [this,isred](int ptype,int sqDst){
		switch (ptype & 0x0f)
		{
		case KING:
		case ADVISOR_FROM...ADVISOR_TO:
		case BISHOP_FROM...BISHOP_TO:
		case PAWN_FROM...PAWN_TO:
			if(isred){
				if(isRedSide){
					//红在 board 下方,检查目标位置是否在同一边
					return SAME_HALF(sqDst,0xc7);	
				}else{
					//红在 board 上方
					return SAME_HALF(sqDst,0x37);
				}
			}else{
				if(isRedSide){
					//红在 board 下方
					return SAME_HALF(sqDst,0x37);
				}else{
					//红在 board 上方
					return SAME_HALF(sqDst,0xc7);	
				}
			}	
		default:
			return true;
		}	
	};

	switch (cnPieceTypes[pt])
	{
	case KING:
		for(int i = 0; i < B_HEIGHT;++i){
			for(int j = 0; j < B_WIDTH; ++j){
				int sqDstPos = SQpos(i,j);
				if(IN_FORT(sqDstPos) && Squares[sqDstPos] == 0 && checkSideValid(pt,sqDstPos)){
					cb(j,i,0);
				}
			}	
		}
		break;
	case ADVISOR_TYPE:
		for(int i = 0; i < B_HEIGHT;++i){
			for(int j = 0; j < B_WIDTH; ++j){
				int sqDstPos = SQpos(i,j);
				if(IN_FORT(sqDstPos) && Squares[sqDstPos] == 0 
					&& checkSideValid(pt,sqDstPos) 
					&& validPosAdvisorBishop[sqDstPos] == ADVISOR_TYPE){
					cb(j,i,0);
				}
			}	
		}
		break;
	case BISHOP_TYPE:
		for(int i = 0; i < B_HEIGHT;++i){
			for(int j = 0; j < B_WIDTH; ++j){
				int sqDstPos = SQpos(i,j);
				if(Squares[sqDstPos] == 0 
					&& checkSideValid(pt,sqDstPos) 
					&& validPosAdvisorBishop[sqDstPos] == BISHOP_TYPE){
					cb(j,i,0);
				}
			}	
		}
		break;
	case PAWN_TYPE:
		for(int i = 0; i < B_HEIGHT;++i){
			for(int j = 0; j < B_WIDTH; ++j){
				int sqDstPos = SQpos(i,j);
				if(Squares[sqDstPos] != 0)
					continue;
				// 目标位置是同一边
				if(checkSideValid(pt,sqDstPos)){
					if(validPosPawn[sqDstPos] != PAWN_TYPE){
						continue;	
					}
						// 此时在前进一个就为对方则判断处于边线位置?
					if(DIFF_HALF(SQUARE_FORWARD(isRedSide,sqDstPos,!isred),sqDstPos)){
						// 边线后面一格
						int ptback = Squares[SQUARE_BACKWARD(isRedSide,sqDstPos,!isred)];
						if(!ptback){
							cb(j,i,0);
							continue;
						}
						bool isbackred = !((1 << 5) & ptback);
						int backtype = cnPieceTypes[ptback];
						// 如果是背后是空,自己,或者 非本方的兵,则是合法的
						// 这里分为不是自己方和不是兵，因为自己方的话，第二个检测过不了
						if((ptback == pt) || (isbackred ^ isred) || (backtype != PAWN_TYPE)){
							cb(j,i,0);
						}
					}else{
						// 此时目标为兵原点所在行,此时保证前一个不是本方兵
						int ptforw = Squares[SQUARE_FORWARD(isRedSide,sqDstPos,!isred)];
						if(!ptforw){
							cb(j,i,0);
							continue;
						}
						bool isforwred = !((1 << 5) & ptforw);
						int backtype = cnPieceTypes[ptforw];
						if((ptforw == pt) || (isforwred ^ isred) || (backtype != PAWN_TYPE)){
							cb(j,i,0);
						}
					}
				}else{
					// 对边不需检查位置
					cb(j,i,0);
				}
			}	
		}
		break;
	case KNIGHT_TYPE:
	case ROOK_TYPE:
	case CANNON_TYPE:
		for(int i = 0; i < B_HEIGHT;++i){
			for(int j = 0; j < B_WIDTH; ++j){
				int sqDstPos = SQpos(i,j);
				if(Squares[sqDstPos] == 0){
					cb(j,i,0);
				}
			}	
		}
		break;
	default:
		break;
	}
}

void Board::drawMarker(bool isDragging)
{
	if (activatedPos.x != -1 && activatedPos.y != -1 && !moveAnim.isAnimating)
	{
		int pos = SQpos(activatedPos.y, activatedPos.x);
		int pt = Squares[pos];
		Color ringColor = pt & (1 << 5) ? RING_COLOR_Orange : RING_COLOR_Purple;

		Vector2 activePiece = screenXY(activatedPos.x, activatedPos.y);

		if (!isDragging)
		{
			DrawRing(activePiece,
					 this->binfo.radius,
					 this->binfo.radius + 5, 0, 360, 36, ringColor);
		}

		if(ISMENUMODE){
			this->PieceFreePos(pt,[this](int bx,int by,[[maybe_unused]]int debug){
				DrawCircleV(screenXY(bx,by), 5, PURPLE);			
			});		
			return;
		}
		// 画目标点
		pieceCap(pt, pos, [this](int bx, int by,int debug)
			{
				int sqSrc = SQpos(activatedPos.y,activatedPos.x);
				int sqDst = SQpos(by, bx);					

				int sqSrcPiece = Squares[sqSrc];
				int sqDstPiece = Squares[sqDst];

				if(sqDst == sqSrc){
					return;
				}

				if(debug){
					if(showTags){
						DrawRing(screenXY(bx,by),
					 		this->binfo.radius,
					 		this->binfo.radius + 5, 0, 360, 36, GOLD);
					}
					return;
				}else{
					if(sqDstPiece == 0){
						DrawCircleV(screenXY(bx,by), 5, PURPLE);			
					}else if((sqDstPiece & ( 1 << 5)) != (sqSrcPiece & (1 << 5))){
						DrawRing(screenXY(bx,by),
					 		this->binfo.radius,
					 		this->binfo.radius + 5, 0, 360, 36, GREEN);
					}
				}
			});
	}else if(activatedDisCardedPos >= 0){
		int pt = disCardedPieces[static_cast<size_t>(activatedDisCardedPos)];
		if(ISMENUMODE){
			this->PieceFreePos(pt,[this](int bx,int by,[[maybe_unused]]int debug){
				DrawCircleV(screenXY(bx,by), 5, PURPLE);			
			});
			return;
		}
	}

	// 绘制 lastmove
	if(!ISMENUMODE && !moveAnim.isAnimating){
		if(lastmoveCur > 0 && IN_BOARD(rbsList[lastmoveCur - 1].mvs.Src) && IN_BOARD(rbsList[lastmoveCur - 1].mvs.Dst)){
			Color rc = Squares[rbsList[lastmoveCur - 1].mvs.Src] & (1 << 5) ? RING_COLOR_Orange : RING_COLOR_Purple;
			DrawRing(screenXY(BOARD_X(rbsList[lastmoveCur - 1].mvs.Dst),BOARD_Y(rbsList[lastmoveCur - 1].mvs.Dst)),
						this->binfo.radius,
						this->binfo.radius + 5, 0, 360, 36, rc);

			DrawCircleV(screenXY(BOARD_X(rbsList[lastmoveCur - 1].mvs.Src),BOARD_Y(rbsList[lastmoveCur - 1].mvs.Src)), 5, rc);
		}
	}
}

void Board::pieceCap(int pt, int pos, std::function<void(int, int,[[maybe_unused]] int)> cb) const
{
	int i, bx, by, p, j;
	// SlideMoveStruct col_move, row_move;
	SlideMoveStruct *Colmove,* Rowmove;

	switch (pt & 0x0f)
	{
	case KING:
		i = 0;
		while (KingMoves[pos][i] != 0)
		{
			int x = BOARD_X(KingMoves[pos][i]), y = BOARD_Y(KingMoves[pos][i]);
			cb(x, y, 0);
			i++;
		}
		break;
	case ADVISOR_FROM ... ADVISOR_TO:
		i = 0;
		while (AdvisorMoves[pos][i] != 0)
		{
			int x = BOARD_X(AdvisorMoves[pos][i]), y = BOARD_Y(AdvisorMoves[pos][i]);
			cb(x, y,0);
			i++;
		}
		break;
	case BISHOP_FROM ... BISHOP_TO:
		i = 0;
		while (BishopMoves[pos][i] != 0)
		{
			if (Squares[BishopPins[pos][i]] == 0)
			{
				int x = BOARD_X(BishopMoves[pos][i]), y = BOARD_Y(BishopMoves[pos][i]);
				cb(x, y,0);
			}
			i++;
		}
		break;
	case KNIGHT_FROM ... KNIGHT_TO:
		i = 0;
		while (KnightMoves[pos][i] != 0)
		{
			if (Squares[KnightPins[pos][i]] == 0)
			{
				int x = BOARD_X(KnightMoves[pos][i]), y = BOARD_Y(KnightMoves[pos][i]);
				cb(x, y,0);
			}
			i++;
		}
		break;
	case ROOK_FROM ... ROOK_TO:
		// pos = 16 * (activatedPos.y + rowOffset) + activatedPos.x + colOffset;
		// bx =  FILE_X(pos) - colOffset;
		// by =  RANK_Y(pos) - rowOffset;

		// uint16_t wbitCol[16]; 只有 9 * 10 的空间有效
		// 000000001000  colmsk = 1 << RANK_Y(pos) - rowOffset
		// (只能改变这一列的 pos 位置的信息,不改变其他位置,上面表示这一列的pos位置)
		// wbitCol[File_X(pos)] ^= colmask 用异或是为了添加删除
		// 0 ^ 1 添加 1 ^ 0 保持 0 ^ 0 保持  1 ^ 1 删除
		// j = wbitCol[FILE_X(pos)]
		// SlideMoveCol[by][j] 棋子在列中的位置用 行坐标 by
		bx = FILE_X(pos) - colOffset;
		by = RANK_Y(pos) - rowOffset;
		// col_mask = wBitCols[FILE_X(pos)];
		// row_mask = wBitRows[RANK_Y(pos)];
		// col_move = SlideMoveCol[by][col_mask];
		// row_move = SlideMoveRow[bx][row_mask];

		Colmove = FileMovePtr(FILE_X(pos),RANK_Y(pos));
		Rowmove = RankMovePtr(FILE_X(pos),RANK_Y(pos));
		// col move
		for (p = Colmove->SlideMove[1] - rowOffset; p <= by - 1; ++p)
		{
			cb(bx, p, 0);
		}

		for (p = by + 1; p <= Colmove->SlideMove[0] - rowOffset; ++p)
		{
			cb(bx, p, 0);
		}

		// row move
		for (p = Rowmove->SlideMove[1] - colOffset; p <= bx - 1; ++p)
		{
			cb(p, by, 0);
		}

		for (p = bx + 1; p <= Rowmove->SlideMove[0] - colOffset; ++p)
		{
			cb(p, by, 0);
		}

		// cap col
		for (j = 0; j < 2; ++j)
		{
			p = Colmove->RookCap[j] - rowOffset;
			if (p >= 0)
			{
				cb(bx, p, 0);
			}
		}

		// cap row
		for (j = 0; j < 2; ++j)
		{
			p = Rowmove->RookCap[j] - colOffset;
			if (p >= 0)
			{
				cb(p, by,0);
			}
		}
		break;
	case CANNON_FROM ... CANNON_TO:

		bx = FILE_X(pos) - colOffset;
		by = RANK_Y(pos) - rowOffset;
		// col_mask = wBitCols[FILE_X(pos)];
		// row_mask = wBitRows[RANK_Y(pos)];
		// col_move = SlideMoveCol[by][col_mask];
		// row_move = SlideMoveRow[bx][row_mask];

		Colmove = FileMovePtr(FILE_X(pos),RANK_Y(pos));
		Rowmove = RankMovePtr(FILE_X(pos),RANK_Y(pos));
		// col move
		for (p = Colmove->SlideMove[1] - rowOffset; p <= by - 1; ++p)
		{
			cb(bx, p,0);
		}

		for (p = by + 1; p <= Colmove->SlideMove[0] - rowOffset; ++p)
		{
			cb(bx, p,0);
		}

		// row move
		for (p = Rowmove->SlideMove[1] - colOffset; p <= bx - 1; ++p)
		{
			cb(p, by,0);
		}

		for (p = bx + 1; p <= Rowmove->SlideMove[0] - colOffset; ++p)
		{
			cb(p, by,0);
		}

		// cap col
		for (j = 0; j < 2; ++j)
		{
			p = Colmove->CannonCap[j] - rowOffset;
			if (p >= 0)
			{
				cb(bx, p,0);
			}
		}

		// cap row
		for (j = 0; j < 2; ++j)
		{
			p = Rowmove->CannonCap[j] - colOffset;
			if (p >= 0)
			{
				cb(p, by,0);
			}
		}

	#ifdef SUPERCAP // for super cap
		for( j = 0;j < 2; ++j){
			p = Colmove->SuperCap[j] - rowOffset;
			if (p >= 0){
				cb(bx, p, 1);
			}
		}

		for( j = 0;j < 2; ++j){
			p = Rowmove->SuperCap[j] - colOffset;
			if (p >= 0){
				cb(p, by, 1);
			}
		}
	#endif

		break;
	case PAWN_FROM ... PAWN_TO:
		i = 0;
		if (isRedSide)
		{
			// 0 表示所在棋盘的下半部
			// black red side
			if (pt & (1 << 5))
			{
				while (PawnMoves[1][pos][i] != 0)
				{
					int x = BOARD_X(PawnMoves[1][pos][i]), y = BOARD_Y(PawnMoves[1][pos][i]);
					cb(x, y,0);
					i++;
				}
			}
			else
			{
				while (PawnMoves[0][pos][i] != 0)
				{
					int x = BOARD_X(PawnMoves[0][pos][i]), y = BOARD_Y(PawnMoves[0][pos][i]);
					cb(x, y,0);
					i++;
				}
			}
		}
		else
		{
			// black black side
			if (pt & (1 << 5))
			{
				while (PawnMoves[0][pos][i] != 0)
				{
					int x = BOARD_X(PawnMoves[0][pos][i]), y = BOARD_Y(PawnMoves[0][pos][i]);
					cb(x, y,0);
					i++;
				}
			}
			else
			{
				while (PawnMoves[1][pos][i] != 0)
				{
					int x = BOARD_X(PawnMoves[1][pos][i]), y = BOARD_Y(PawnMoves[1][pos][i]);
					cb(x, y,0);
					i++;
				}
			}
		}

		break;
	default:
		break;
	}
}

bool Board::checkClickActivation(Vector2 pos, BoardPos &bp)
{
	for (int i = 0; i < B_HEIGHT; ++i)
	{
		for (int j = 0; j < B_WIDTH; ++j)
		{
			int piece_i = Squares[16 * (i + rowOffset) + j + colOffset];
			if (piece_i & 63) //? 当前位置有棋子才检测
			{
				float cx = this->binfo.BoardOriginX + j * this->binfo.gridWidth;
				float cy = this->binfo.BoardOriginY + i * this->binfo.gridHeight;
				if (CheckCollisionPointCircle(pos, {cx, cy}, this->binfo.radius))
				{
					if(ISMENUMODE){
						bp.x = j;
						bp.y = i;
						return true;	
					}
					if(useAi){
						// 不是本方直接不允许点击
						if((isRedSide && (piece_i & (1 << 5))) || (!isRedSide && !(piece_i & (1 << 5)))){
							if(activatedPos.x < 0 || activatedPos.y < 0){
								NotifyManager::getInstance().Add({"不能操控对方棋子"});
							}
							return false;
						}
					}
					if (((piece_i & (1 << 5)) && isRedTurn) || (!(piece_i & (1 << 5)) && !isRedTurn))
					{
						if (activatedPos.x < 0 || activatedPos.y < 0)
						{
							NotifyManager::getInstance().Add({"不是你的回合"});
						}
						return false;
					}
					bp.x = j;
					bp.y = i;
					return true;
				}
			}
		}
	}
	bp.x = -1;
	bp.y = -1;
	return false;
}

void Board::changeSide()
{
	this->changeSideSimulation();
	this->triggerSearch();
}

void Board::changeSideSimulation(){
	this->activatedPos = {-1,-1};
	this->mcs.state = MouseStateType::Idle;
	this->isRedTurn = !this->isRedTurn;

    this->zobr.Xor(zobrPlayer);
}

int Board::move(int sqSrc, int sqDst,bool isCheck)
{
	int tp;
	int sqSrcpt = Squares[sqSrc];
	int sqDstpt = Squares[sqDst]; // 如果非capture，为 0
	if (!this->ISMENUMODE && isCheck)
	{
		// trymove
		this->SaveStatus(); //? 存储 evblack/red zobr
		this->MakeMove(sqSrc,sqDst); //? 修改了Square wbitcol/row Piece bitpiece evblack/red zobr 
		if(tp = checkMate(),tp != 0 && tp != NOKING){
			this->RollbackMove(sqSrc,sqDst,sqSrcpt,sqDstpt); //? 恢复Square wbitcol/row Piece bitpiece
			this->Rollback(); //? 恢复 evblack/red zobr 
			return CHECKMATED;
		}

		this->RollbackMove(sqSrc,sqDst,sqSrcpt,sqDstpt); //? 恢复Square wbitcol/row Piece bitpiece
		this->Rollback(); //? 恢复 evblack/red zobr 
	}
	this->activateAnimation(sqSrc, sqDst,[this](){
		this->postMove();
	});
	return SUCCESS;
}

void Board::UndoLastMove(){

	if(lastmoveCur < 1){
		NotifyManager::getInstance().Add({
			"不能回退,无记录"	
		});
		return;
	}
	// 如果是 ai，在我方回合触发: 退两步 
	// 				开局时：我方先手，此时回退数组大小为0，不用考虑
	//           			我方后手，此时大小为1，不能触发。
	//			 在对方回合触发: 退一步
	// 不是 ai, （本系统假设只能退一步，只在对手回合退）
	
	if(useAi){
		if(isRedSide ^ isRedTurn){
			// 对方思考, 回退一次
			this->stopSearch(true);
			assert(rbMoveNum == lastmoveCur);
			this->UndoSimulateMove();
			lastmoveCur--;

		}else{
			// 本方
			if(lastmoveCur < 2){
				NotifyManager::getInstance().Add({
					"不能回退,无记录"	
				});
				return;
			}

			this->UndoSimulateMove();
			lastmoveCur--;

			this->UndoSimulateMove();
			lastmoveCur--;
		}
	}else{
		this->UndoSimulateMove();
		lastmoveCur--;
	}

	DEBUG_("[悔棋后 Zobrist: ",zobr.dwKey, " ",zobr.dwLock0," ",zobr.dwLock1," ]");
}

int Board::checkMate() // 不要在内部移动,这个只检测当前局面
{
	// 对本方的将军检测
	int kingSqPos = isRedTurn ? Pieces[SIDE_TAG(0)].alive : Pieces[SIDE_TAG(1)].alive;
	int oppositeSide = isRedTurn ? SIDE_TAG(1) : SIDE_TAG(0);
	int opponentKingSqPos = Pieces[oppositeSide].alive;

	// 当前将位置
	int kx = FILE_X(kingSqPos);
	int ky = RANK_Y(kingSqPos);

	if (kingSqPos == 0 || opponentKingSqPos == 0)
	{
		return NOKING;
	}

	int i, j, attackPos, attackPawnSide;

	// 检测对将,使用车的列着法
	SlideMaskStruct *mskcol = this->FileMaskPtr(kx, ky);
	SlideMaskStruct *mskrow = this->RankMaskPtr(kx, ky);

	if (kx == FILE_X(opponentKingSqPos))
	{
		if ((mskcol->wRookCap & (1 << (RANK_Y(opponentKingSqPos) - rowOffset))) != 0)
		{
			return oppositeSide + KING;
		}
	}

	// 检测马将
	for (i = KNIGHT_FROM; i <= KNIGHT_TO; ++i)
	{
		attackPos = Pieces[i + oppositeSide].alive;

		if (attackPos == 0)
			continue;
		j = 0;
		while (KnightMoves[attackPos][j] != 0)
		{
			if (Squares[KnightPins[attackPos][j]] == 0)
			{

				if (kingSqPos == KnightMoves[attackPos][j])
				{
					return oppositeSide + i;
				}
			}
			j++;
		}
	}

	// 检测兵将
	// 0 表示所在棋盘的下半部
	// isredside isredturn
	// 1 1 -> 0
	// 1 0 -> 1
	// 0 1 -> 1
	// 0 0 -> 0
	attackPawnSide = !(isRedSide ^ isRedTurn);

	for (i = PAWN_FROM; i <= PAWN_TO; ++i)
	{
		attackPos = Pieces[i + oppositeSide].alive;

		if (attackPos == 0)
			continue;
		j = 0;
		while (PawnMoves[attackPawnSide][attackPos][j] != 0)
		{
			if (kingSqPos == PawnMoves[attackPawnSide][attackPos][j])
			{
				return oppositeSide + i;
			}
			j++;
		}
	}
	// 检测车将
	//
	for (i = ROOK_FROM; i <= ROOK_TO; ++i)
	{
		attackPos = Pieces[i + oppositeSide].alive;
		if (attackPos == 0)
			continue;

		if (kx == FILE_X(attackPos))
		{
			if ((mskcol->wRookCap & (1 << (RANK_Y(attackPos) - rowOffset))) != 0)
			{
				return oppositeSide + i;
			}
		}
		if (ky == RANK_Y(attackPos))
		{
			if ((mskrow->wRookCap & (1 << (FILE_X(attackPos) - colOffset))) != 0)
			{
				return oppositeSide + i;
			}
		}
	}

	// 检测炮将
	for (i = CANNON_FROM; i <= CANNON_TO; ++i)
	{
		attackPos = Pieces[i + oppositeSide].alive;
		if (attackPos == 0)
			continue;

		if (kx == FILE_X(attackPos))
		{
			if ((mskcol->wCannonCap & (1 << (RANK_Y(attackPos) - rowOffset))) != 0)
			{
				return oppositeSide + i;
			}
		}

		if (ky == RANK_Y(attackPos))
		{
			if ((mskrow->wCannonCap & (1 << (FILE_X(attackPos) - colOffset)))!= 0)
			{
				return oppositeSide + i;
			}
		}
	}

	return 0;
}

// 将死
bool Board::checkMated()
{
	int sqSrc,sqDst,sqSrcpt,sqDstpt,tp;
	bool ischecked = true;
	MoveSortStruct msvs;
	this->GetAllMoves(&msvs);

	for(int i = 0;i < msvs.allMoveNum;++i){
		sqSrc = (int)msvs.mvsAll[i].Src;
		sqDst = (int)msvs.mvsAll[i].Dst;	

		sqSrcpt = Squares[sqSrc];
		sqDstpt = Squares[sqDst]; // 如果非capture，为 0

		this->SaveStatus(); //? 存储 evblack/red zobr
		this->MakeMove(sqSrc,sqDst); //? 修改了Square wbitcol/row Piece bitpiece evblack/red zobr 
		if(tp = checkMate(),tp != 0 && tp != NOKING){
			this->RollbackMove(sqSrc,sqDst,sqSrcpt,sqDstpt); //? 恢复Square wbitcol/row Piece bitpiece
			this->Rollback(); //? 恢复 evblack/red zobr 
		}else{
			ischecked = false;	
			this->RollbackMove(sqSrc,sqDst,sqSrcpt,sqDstpt); //? 恢复Square wbitcol/row Piece bitpiece
			this->Rollback(); //? 恢复 evblack/red zobr 
			break;
		}
	}

	return ischecked;
}

void Board::postMove()
{
	int pcCap,mv;
	int sqSrc = moveAnim.sqSrc;
	int sqDst = moveAnim.sqDst;
	RollbackStruct *rbs;

	this->SaveStatus();
	mv = MOVE(sqSrc,sqDst);
	pcCap = this->MakeMove(sqSrc,sqDst);
	moveAnim.isAnimating = false;

	if(ISMENUMODE){
		this->activatedPos = {-1,-1};
		this->mcs.state = MouseStateType::Idle;
		return;
	}

	this->changeSideSimulation();

	//! 这个是最终的行走,目前和搜索共用rblist,要确保搜索后清除所有记录
	//! 只能先交换后再记录
	rbs = rbsList + rbMoveNum;
	rbs->mvs.Src = (uint8_t)sqSrc;
	rbs->mvs.Dst = (uint8_t)sqDst;
	rbs->mvs.ChkChs = checkMate();
	rbs->Srcpt = Squares[sqDst]; // 此时已经是在 dst 位置
	rbs->Dstpt = pcCap;
	// DEBUG_((int)(rbs->mvs.ChkChs)," ",pcCap);
  	if (pcCap == 0) { // 没有吃子
    	if (rbs->mvs.ChkChs == 0) { // 也没有对对方将军
			// 捉到的对方的子的类型,所属的阵营为mv的对面
	      	rbs->mvs.ChkChs = -chasedBy(mv); //? 移动后造成的局面影响,为什么是负的?负表示被捉子
    	}
    	// if (LastMove().CptDrw == -100) {
      	// 	lprbs->mvs.CptDrw = -100;
    	// } else {
      	// 	lprbs->mvs.CptDrw = std::min((int) LastMove().CptDrw, 0) - (lprbs->mvs.ChkChs > 0 || LastMove().ChkChs > 0 ? 0 : 1);
    	// }
    	// __ASSERT_BOUND(-100, lprbs->mvs.CptDrw, 0);
		rbs->mvs.CptDrw = 0;
  	} else {
    	rbs->mvs.CptDrw = pcCap;
    	__ASSERT_PIECE(pcCap); // 记录被吃子
		disCardedPieces.push_back(pcCap);
  	}
	
	DEBUG_("[Zobrist: ",zobr.dwKey, " ",zobr.dwLock0," ",zobr.dwLock1," ]");
	rbMoveNum++;
	lastmoveCur++;

	// DEBUG_("rbmoveNUm & lastmoveIndex ",rbMoveNum," ",lastmoveCur);

	// 目前和搜索共用rblist,要确保搜索后清除所有记录
	assert(rbMoveNum == lastmoveCur);

	if(isGameOver()){
		return;
	}

	this->triggerSearch();
}

void Board::preMove(bool fromDrag,Rectangle rec)
{
	if (activatedPos.x != -1 && activatedPos.y != -1)
	{
		int pos = SQpos(activatedPos.y, activatedPos.x);
		int pt = Squares[pos];

		bool isValidPos = false;
		bool errorType = SUCCESS;

		float originX = this->binfo.BoardOriginX + activatedPos.x * this->binfo.gridWidth;
		float originY = this->binfo.BoardOriginY + activatedPos.y * this->binfo.gridHeight;

		// 原点
		if(fromDrag && CheckCollisionPointCircle(this->mcs.dragMousePos, {originX, originY}, this->binfo.radius)){
			this->mcs.state = MouseStateType::Selected;
			return;
		}

		if(ISMENUMODE){
			// todo 检测是否拖入了弃子区
			if(CheckCollisionPointRec(this->mcs.dragMousePos,rec)){
				// menuState 只需要管 Squares,其他状态都不需要
				Squares[pos] = 0;
				Pieces[pt].alive = 0;
				disCardedPieces.push_back(pt);
				activatedPos = {-1,-1};
				this->mcs.state = MouseStateType::Idle;
				return;
			}

			PieceFreePos(pt,[this,&isValidPos](int bx,int by,[[maybe_unused]] int debug){
				float cx = this->binfo.BoardOriginX + bx * this->binfo.gridWidth;
				float cy = this->binfo.BoardOriginY + by * this->binfo.gridHeight;

				if (CheckCollisionPointCircle(this->mcs.dragMousePos, {cx, cy}, this->binfo.radius)){
					isValidPos = true;
					int sqSrc = SQpos(activatedPos.y, activatedPos.x);
					int sqDst = SQpos(by, bx);
					move(sqSrc,sqDst);
				}
			});


			if (!isValidPos && fromDrag)
			{
				this->mcs.state = MouseStateType::Selected;
			}
			return;	
		}

		pieceCap(pt, pos, [this,&errorType,&isValidPos](int bx, int by,int debug){
					float cx = this->binfo.BoardOriginX + bx * this->binfo.gridWidth;
					float cy = this->binfo.BoardOriginY + by * this->binfo.gridHeight;
					if (CheckCollisionPointCircle(this->mcs.dragMousePos, {cx, cy}, this->binfo.radius))
					{
						isValidPos = true;
						int sqSrc = SQpos(activatedPos.y, activatedPos.x);
						int sqDst = SQpos(by, bx);

						int sqSrcPiece = Squares[sqSrc];
						int sqDstPiece = Squares[sqDst];

						if(debug){
							isValidPos = false;
							return;
						}else{
						 	if (sqDstPiece == 0)
						 	{
								errorType = move(sqSrc, sqDst);
								if(errorType != SUCCESS){
									isValidPos = false;
								}
						 	}
						 	else if ((sqDstPiece & (1 << 5)) != (sqSrcPiece & (1 << 5)))
						 	{
								errorType = move(sqSrc, sqDst);
								if(errorType != SUCCESS){
									isValidPos = false;
								}
						 	}
						 	else{
								// 同色和原点
								isValidPos = false;
						 	}
						}
					}
				});

		if(errorType != SUCCESS){
			if(errorType == CHECKMATED){
				Notify n = {"CHECKMATED"};
				n.color = RED;
				n.fontSize = 36;
				NotifyManager::getInstance().Add(std::move(n));
			}
		}else if(!isValidPos)
		{
			NotifyManager::getInstance().Add({"这不是一个有效位置"});
		}

		if (!isValidPos && fromDrag)
		{
			this->mcs.state = MouseStateType::Selected;
		}

	}else if(this->activatedDisCardedPos >= 0){
		if(ISMENUMODE){
			int pt = disCardedPieces[static_cast<size_t>(this->activatedDisCardedPos)]; 
			bool isValidPos = false;

			PieceFreePos(pt,[this,pt,&isValidPos](int bx,int by,[[maybe_unused]] int debug){
				float cx = this->binfo.BoardOriginX + bx * this->binfo.gridWidth;
				float cy = this->binfo.BoardOriginY + by * this->binfo.gridHeight;

				if (CheckCollisionPointCircle(this->mcs.dragMousePos, {cx, cy}, this->binfo.radius)){
					isValidPos = true;
					int sqDst = SQpos(by, bx);

					Squares[sqDst] = pt;
					Pieces[pt].alive = sqDst;
					disCardedPieces.erase(disCardedPieces.begin() + activatedDisCardedPos);
					activatedDisCardedPos = -1;
					this->mcs.state = MouseStateType::Idle;
					return;
				}
			});

			if (!isValidPos && fromDrag)
			{
				this->mcs.state = MouseStateType::Selected;
			}
		}
	}
}

void Board::activateAnimation(int sqSrc, int sqDst,std::function<void(void)> cb)
{
	moveAnim.progress = 0;
	moveAnim.elapsedTime = 0;
	moveAnim.isAnimating = true;

	moveAnim.sqSrc = sqSrc;
	moveAnim.sqDst = sqDst;

	moveAnim.startPos = screenXY(BOARD_X(sqSrc), BOARD_Y(sqSrc));
	moveAnim.endPos = screenXY(BOARD_X(sqDst), BOARD_Y(sqDst));


	moveAnim.onComplete = cb;
	moveAnim.onComplete = [this]()
	{
		this->postMove();
	};
}

void Board::updateMoveAnimation(float frameTime)
{
	if (moveAnim.isAnimating)
	{
		moveAnim.elapsedTime += frameTime;
		moveAnim.progress = moveAnim.elapsedTime / moveAnim.duration;
		if (moveAnim.progress >= 1.0f)
		{
			if (moveAnim.onComplete)
			{
				moveAnim.onComplete();
			}
		}
	}
}

bool Board::isGameOver(){
	int kingSqPos = isRedTurn ? Pieces[SIDE_TAG(0)].alive : Pieces[SIDE_TAG(1)].alive;
	int oppositeSide = isRedTurn ? SIDE_TAG(1) : SIDE_TAG(0);
	int opponentKingSqPos = Pieces[oppositeSide].alive;

	if (kingSqPos == 0 || opponentKingSqPos == 0 || this->checkMated())
	{
		Notify n = {
			.msg = "游戏结束",
			.duration = 1000.0f,
		};
		NotifyManager::getInstance().Add(std::move(n));
		this->gameOver = true;
		this->mcs.state = MouseStateType::Idle;
		return true;
	}
	return false;
}

void Board::discard(){

}