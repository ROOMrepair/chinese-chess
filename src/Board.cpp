#include "Board.hpp"

Board::Board(MouseClickState &ms, bool isredside) : mcs(ms)
{
	this->isRedTurn = true;
	this->isRedSide = isredside;
	this->isExchangeSide = false;

	this->evBlack = 0;
	this->evRed = 0;

	this->clearBoard();
	this->caculateBoardInfo();
	this->activatedPos = {-1, -1};
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

bool Board::loadFromFen(const char *szFen)
{
	const char *p = szFen;
	const char *tmp = p;

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
			if (isRedSide)
			{
				isExchangeSide = false;
			}
			else
			{
				isExchangeSide = true;
			}
			break;
		}
		else if (*tmp == 'K')
		{
			if (isRedSide)
			{
				isExchangeSide = true;
			}
			else
			{
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
			/*  这里因为固定了每个棋子的下标 pawn from~to 导致不能
			 *  在 fen 串中添加任意棋子
			 */
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

	return true;
}

bool Board::addPiece(int row, int col, int piece)
{
	// todo 这里暂时在 addpiece 中同步修改 wbitmask
	// todo 在每一步对棋盘的更改处,都需要对 wbitmask 更改
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
	int textureIndex = PieceTextureMap(piece);
	if (textureIndex < 0)
	{
		return false;
	}
	Pieces[piece].textureIndex = textureIndex;
	return true;
}

void Board::clearBoard()
{
	std::memset(Squares, 0, 256);
	std::memset(Pieces, 0, sizeof(PieceType) * 48);
	std::memset(wBitCols, 0, sizeof(uint16_t) * 16);
	std::memset(wBitRows, 0, sizeof(uint16_t) * 16);
}

bool Board::caculateBoardInfo()
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

	return true;
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

					// circleXY.x += dragMousePos.x - dragStartPos.x;
					// circleXY.y += dragMousePos.y - dragStartPos.y;

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
					DrawTextEx(asset.fonts[0].font, std::to_string(pt & 0x0f).c_str(), {textXY.x + cx * 2 - 5, textXY.y - 5}, 30, 0, GREEN);
					DrawTextEx(asset.fonts[0].font, std::to_string(sqPos).c_str(), {textXY.x + cx * 2 - 5, textXY.y - 30}, 30, 0, GREEN);
					if(Proteced(isRedTurn ? 0 : 1,sqPos,0) && (((pt & (1 << 5)) >> 5) ^ isRedTurn)){
						DrawRing(circleXY,
					 		this->binfo.radius,
					 		this->binfo.radius + 5, 0, 360, 36, BLUE);		
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

		// 画目标点
		pieceCap(pt, pos, [this](int bx, int by)
				 {
			int sqSrc = SQpos(activatedPos.y,activatedPos.x);
			int sqDst = SQpos(by, bx);					

			int sqSrcPiece = Squares[sqSrc];
			int sqDstPiece = Squares[sqDst];

			if(sqDstPiece == 0){
				DrawCircleV(screenXY(bx,by), 5, PURPLE);			
			}else if((sqDstPiece & ( 1 << 5)) != (sqSrcPiece & (1 << 5))){
				// DrawCircleV(screenXY(bx,by), 5, GREEN);			
				DrawRing(screenXY(bx,by),
					 this->binfo.radius,
					 this->binfo.radius + 5, 0, 360, 36, GREEN);
			} });
	}
}

void Board::pieceCap(int pt, int pos, std::function<void(int, int)> cb)
{
	int i, bx, by, col_mask, row_mask, p, j;
	SlideMoveStruct col_move, row_move;

	switch (pt & 0x0f)
	{
	case KING:
		i = 0;
		while (KingMoves[pos][i] != 0)
		{
			int x = BOARD_X(KingMoves[pos][i]), y = BOARD_Y(KingMoves[pos][i]);
			cb(x, y);
			i++;
		}
		break;
	case ADVISOR_FROM ... ADVISOR_TO:
		i = 0;
		while (AdvisorMoves[pos][i] != 0)
		{
			int x = BOARD_X(AdvisorMoves[pos][i]), y = BOARD_Y(AdvisorMoves[pos][i]);
			cb(x, y);
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
				cb(x, y);
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
				cb(x, y);
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
		col_mask = wBitCols[FILE_X(pos)];
		row_mask = wBitRows[RANK_Y(pos)];
		col_move = SlideMoveCol[by][col_mask];
		row_move = SlideMoveRow[bx][row_mask];

		// col move
		for (p = col_move.SlideMove[1] - rowOffset; p <= by - 1; ++p)
		{
			cb(bx, p);
		}

		for (p = by + 1; p <= col_move.SlideMove[0] - rowOffset; ++p)
		{
			cb(bx, p);
		}

		// row move
		for (p = row_move.SlideMove[1] - colOffset; p <= bx - 1; ++p)
		{
			cb(p, by);
		}

		for (p = bx + 1; p <= row_move.SlideMove[0] - colOffset; ++p)
		{
			cb(p, by);
		}

		// cap col
		for (j = 0; j < 2; ++j)
		{
			p = col_move.RookCap[j] - rowOffset;
			if (p >= 0)
			{
				cb(bx, p);
			}
		}

		// cap row
		for (j = 0; j < 2; ++j)
		{
			p = row_move.RookCap[j] - colOffset;
			if (p >= 0)
			{
				cb(p, by);
			}
		}

		break;
	case CANNON_FROM ... CANNON_TO:
		bx = FILE_X(pos) - colOffset;
		by = RANK_Y(pos) - rowOffset;
		col_mask = wBitCols[FILE_X(pos)];
		row_mask = wBitRows[RANK_Y(pos)];
		col_move = SlideMoveCol[by][col_mask];
		row_move = SlideMoveRow[bx][row_mask];
		// col move
		for (p = col_move.SlideMove[1] - rowOffset; p <= by - 1; ++p)
		{
			cb(bx, p);
		}

		for (p = by + 1; p <= col_move.SlideMove[0] - rowOffset; ++p)
		{
			cb(bx, p);
		}

		// row move
		for (p = row_move.SlideMove[1] - colOffset; p <= bx - 1; ++p)
		{
			cb(p, by);
		}

		for (p = bx + 1; p <= row_move.SlideMove[0] - colOffset; ++p)
		{
			cb(p, by);
		}

		// cap col
		for (j = 0; j < 2; ++j)
		{
			p = col_move.CannonCap[j] - rowOffset;
			if (p >= 0)
			{
				cb(bx, p);
			}
		}

		// cap row
		for (j = 0; j < 2; ++j)
		{
			p = row_move.CannonCap[j] - colOffset;
			if (p >= 0)
			{
				cb(p, by);
			}
		}

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
					cb(x, y);
					i++;
				}
			}
			else
			{
				while (PawnMoves[0][pos][i] != 0)
				{
					int x = BOARD_X(PawnMoves[0][pos][i]), y = BOARD_Y(PawnMoves[0][pos][i]);
					cb(x, y);
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
					cb(x, y);
					i++;
				}
			}
			else
			{
				while (PawnMoves[1][pos][i] != 0)
				{
					int x = BOARD_X(PawnMoves[1][pos][i]), y = BOARD_Y(PawnMoves[1][pos][i]);
					cb(x, y);
					i++;
				}
			}
		}

		break;
	default:
		break;
	}
}

void Board::printPieces()
{
	for (int i = 0; i < 48; ++i)
	{
		DEBUG_(Pieces[i].textureIndex);
	}
	DEBUG_("\n");
}

void Board::printSquares()
{
	for (int i = 0; i < 16; ++i)
	{
		for (int j = 0; j < 16; ++j)
		{
			DEBUG_("%d ", Squares[16 * i + j]);
		}
		DEBUG_("\n");
	}
}

void Board::printBoard()
{
	for (int i = rowOffset; i <= rowOffsetTo; ++i)
	{
		for (int j = colOffset; j <= colOffsetTo; ++j)
		{
			DEBUG_("%d ", Squares[16 * i + j]);
		}
		DEBUG_("\n");
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
					//! silly
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
	this->activatedPos.x = -1;
	this->activatedPos.y = -1;
	this->mcs.state = MouseStateType::Idle;
	this->isRedTurn = !this->isRedTurn;
}

int Board::capture(int sqSrc, int sqDst)
{
	if (!this->checkMateMove(sqSrc, sqDst, true))
	{
		return CHECKMATED;
	}
	this->activateAnimation(sqSrc, sqDst, true);
	return SUCCESS;
}

int Board::move(int sqSrc, int sqDst)
{
	if (!this->checkMateMove(sqSrc, sqDst, false))
	{
		return CHECKMATED;
	}
	this->activateAnimation(sqSrc, sqDst, false);
	return SUCCESS;
}

// move
// 每一次移动前/后需要进行检查
// pre:
// 1.被将军检测，是否会造成/已造成本方被将军，会则禁止移动，并提示
// post:
// 1. 将死检测，对两侧的将都需要检测,检测后需要对目前所有的将军线路进行判断，是否能够通过移动阻隔脱离将军状态
//
// (如果没有这样的一条线路)
//
// checkmatemove 是对本方，为 premove check
int Board::checkMateMove(int sqSrc, int sqDst, bool isCapture)
{
	int piece_i_src = Squares[sqSrc]; // 棋子类型
	int piece_i_dst = Squares[sqDst];

	// trymove
	Pieces[piece_i_src].alive = sqDst;
	Squares[sqDst] = Squares[sqSrc];
	if (!isCapture)
	{
		wBitCols[FILE_X(sqDst)] ^= ColMask(sqDst);
		wBitRows[RANK_Y(sqDst)] ^= RowMask(sqDst);
	}
	else
	{
		// 目标位置还是有棋子,棋盘状态不变,但是需要更改目标位置的棋子状态
		Pieces[piece_i_dst].alive = 0;
	}

	Squares[sqSrc] = 0;
	wBitCols[FILE_X(sqSrc)] ^= ColMask(sqSrc);
	wBitRows[RANK_Y(sqSrc)] ^= RowMask(sqSrc);

	// rollback
	auto rollback = [&]()
	{
		Squares[sqSrc] = piece_i_src;
		Squares[sqDst] = piece_i_dst;
		Pieces[piece_i_src].alive = sqSrc;
		Pieces[piece_i_dst].alive = sqDst;

		if (!isCapture)
		{
			wBitCols[FILE_X(sqDst)] ^= ColMask(sqDst);
			wBitRows[RANK_Y(sqDst)] ^= RowMask(sqDst);
		}

		wBitCols[FILE_X(sqSrc)] ^= ColMask(sqSrc);
		wBitRows[RANK_Y(sqSrc)] ^= RowMask(sqSrc);
	};

	// 对本方的将军检测
	int kingSqPos = isRedTurn ? Pieces[SIDE_TAG(0)].alive : Pieces[SIDE_TAG(1)].alive;
	int oppositeSide = isRedTurn ? SIDE_TAG(1) : SIDE_TAG(0);
	int opponentKingSqPos = Pieces[oppositeSide].alive; 

	// 当前将位置
	int kx = FILE_X(kingSqPos);
	int ky = RANK_Y(kingSqPos);

	if (kingSqPos == 0 || opponentKingSqPos == 0)
	{
		rollback();
		return false;
	}

	int i,j,p,attackPos,attackPawnSide;

	// 检测对将,使用车的列着法
	int kbx = FILE_X(kingSqPos) - colOffset;
	int kby = RANK_Y(kingSqPos) - rowOffset;
	int col_mask = wBitCols[FILE_X(kingSqPos)];
	int row_mask = wBitRows[RANK_Y(kingSqPos)];
	SlideMoveStruct col_move, row_move;

	SlideMaskStruct *mskcol = this->FileMaskPtr(kx, ky); 
	SlideMaskStruct *mskrow = this->RankMaskPtr(kx, ky);


	col_move = SlideMoveCol[kby][col_mask];
	row_move = SlideMoveRow[kbx][row_mask];

	if (kx == FILE_X(opponentKingSqPos))
	{
		#ifdef MYDEBUG
		std::cout << "kingpos " << kingSqPos << " opkingpos " << opponentKingSqPos << std::endl;
		std::cout << "wbitcol of " << kbx << " " << kby << " : "<< std::bitset<16>(wBitCols[kx]) << std::endl;
		std::cout << "rookcap of " << kbx << " " << kby << " : "<< std::bitset<16>(mskcol->wRookCap) << std::endl;
		#endif
		if((mskcol->wRookCap & (1 << (RANK_Y(opponentKingSqPos) - rowOffset))) != 0){
				Notify n = {"CHECKMATED"};
				n.color = RED;
				n.fontSize = 48;
				NotifyManager::getInstance().Add(std::move(n));
				rollback();
				return false;
		}
		// for (j = 0; j < 2; ++j)
		// {
		// 	p = col_move.RookCap[j] - rowOffset;
		// 	if (p == RANK_Y(opponentKingSqPos) - rowOffset)
		// 	{
		// 		Notify n = {"CHECKMATED"};
		// 		n.color = RED;
		// 		n.fontSize = 48;
		// 		NotifyManager::getInstance().Add(std::move(n));
		// 		rollback();
		// 		return false;
		// 	}
		// }
	}


	// 检测马将
	for(i = KNIGHT_FROM;i <= KNIGHT_TO;++i){
		attackPos = Pieces[i + oppositeSide].alive;

		if(attackPos == 0) continue;	
		j = 0;	
		while (KnightMoves[attackPos][j] != 0)
		{
			if (Squares[KnightPins[attackPos][j]] == 0)
			{

				if(kingSqPos == KnightMoves[attackPos][j]){
					Notify n = {"CHECKMATED"};
					n.color = RED;
					n.fontSize = 48;
					NotifyManager::getInstance().Add(std::move(n));
					rollback();
					return false;
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

	for(i = PAWN_FROM;i <= PAWN_TO;++i){
		attackPos = Pieces[i + oppositeSide].alive;

		if(attackPos == 0) continue;	
		j = 0;	
		while (PawnMoves[attackPawnSide][attackPos][j] != 0)
		{
			if(kingSqPos == PawnMoves[attackPawnSide][attackPos][j]){
				Notify n = {"CHECKMATED"};
				n.color = RED;
				n.fontSize = 48;
				NotifyManager::getInstance().Add(std::move(n));
				rollback();
				return false;
			}
			j++;
		}
	}
	// 检测车将
	for(i = ROOK_FROM; i <= ROOK_TO;++i){
		attackPos = Pieces[i + oppositeSide].alive;
		if(attackPos == 0) continue;	
		
		if (kx == FILE_X(attackPos)){
			for( j = 0; j < 2;++j){
				p = col_move.RookCap[j] - rowOffset;
				if (p == RANK_Y(attackPos) - rowOffset)
				{
					Notify n = {"CHECKMATED"};
					n.color = RED;
					n.fontSize = 48;
					NotifyManager::getInstance().Add(std::move(n));
					rollback();
					return false;
				}		
			}
		}
		if (ky == RANK_Y(attackPos)){
			for( j = 0; j < 2;++j){
				p = row_move.RookCap[j] - colOffset;
				if (p == FILE_X(attackPos) - colOffset)
				{
					Notify n = {"CHECKMATED"};
					n.color = RED;
					n.fontSize = 48;
					NotifyManager::getInstance().Add(std::move(n));
					rollback();
					return false;
				}			
			}
		}
	}
	
	// 检测炮将
	for(i = CANNON_FROM;i <= CANNON_TO;++i){
		attackPos = Pieces[i + oppositeSide].alive;
		if(attackPos == 0) continue;	
		
		if(kx == FILE_X(attackPos)){
			for( j = 0; j < 2;++j){
				p = col_move.CannonCap[j] - rowOffset;
				if (p == RANK_Y(attackPos) - rowOffset)
				{
					Notify n = {"CHECKMATED"};
					n.color = RED;
					n.fontSize = 48;
					NotifyManager::getInstance().Add(std::move(n));
					rollback();
					return false;
				}		
			}
		}

		if(ky == RANK_Y(attackPos)){
			for( j = 0; j < 2;++j){
				p = row_move.CannonCap[j] - colOffset;
				if (p == FILE_X(attackPos) - colOffset)
				{
					Notify n = {"CHECKMATED"};
					n.color = RED;
					n.fontSize = 48;
					NotifyManager::getInstance().Add(std::move(n));
					rollback();
					return false;
				}			
			}
		}

	}

	rollback();
	return true;
}



// 将死
int Board::checkMated(int sqSrc,int sqDst,bool isCapture){
	(void)sqSrc;
	(void)sqDst;
	(void)isCapture;	
	return 0;
}

void Board::postMove()
{
	int sqSrc = moveAnim.sqSrc;
	int sqDst = moveAnim.sqDst;

	// 0 ^ 1 = 1 添加目标位置
	Pieces[Squares[sqSrc]].alive = sqDst;
	Squares[sqDst] = Squares[sqSrc];
	wBitCols[FILE_X(sqDst)] ^= ColMask(sqDst);
	wBitRows[RANK_Y(sqDst)] ^= RowMask(sqDst);

	// 1 ^ 1 = 0 删除原位置
	Squares[sqSrc] = 0;
	wBitCols[FILE_X(sqSrc)] ^= ColMask(sqSrc);
	wBitRows[RANK_Y(sqSrc)] ^= RowMask(sqSrc);

	moveAnim.isAnimating = false;

	this->changeSide();
}

void Board::postCapture()
{
	int sqSrc = moveAnim.sqSrc;
	int sqDst = moveAnim.sqDst;

	Pieces[Squares[sqDst]].alive = 0;
	Pieces[Squares[sqSrc]].alive = sqDst;

	Squares[sqDst] = Squares[sqSrc];
	// 目标位置还是有棋子,状态不变
	// wBitCols[FILE_X(sqDst)] ^= ColMask(sqDst);
	// wBitRows[RANK_Y(sqDst)] ^= RowMask(sqDst);

	// 1 ^ 1 = 0 删除原位置
	Squares[sqSrc] = 0;
	wBitCols[FILE_X(sqSrc)] ^= ColMask(sqSrc);
	wBitRows[RANK_Y(sqSrc)] ^= RowMask(sqSrc);

	moveAnim.isAnimating = false;

	this->changeSide();
}

void Board::preMove(bool fromDrag)
{
	if (activatedPos.x != -1 && activatedPos.y != -1)
	{
		int pos = SQpos(activatedPos.y, activatedPos.x);
		int pt = Squares[pos];

		bool isValidPos = false;
		bool error = false;

		pieceCap(pt, pos, [this, &isValidPos, &error](int bx, int by)
				 {
					 float cx = this->binfo.BoardOriginX + bx * this->binfo.gridWidth;
					 float cy = this->binfo.BoardOriginY + by * this->binfo.gridHeight;
					 if (CheckCollisionPointCircle(this->mcs.dragMousePos, {cx, cy}, this->binfo.radius))
					 {
						 isValidPos = true;
						 int sqSrc = SQpos(activatedPos.y, activatedPos.x);
						 int sqDst = SQpos(by, bx);

						 int sqSrcPiece = Squares[sqSrc];
						 int sqDstPiece = Squares[sqDst];

						 if (sqDstPiece == 0)
						 {
							if(move(sqSrc, sqDst) != SUCCESS){
								error = true;
							}
						 }
						 else if ((sqDstPiece & (1 << 5)) != (sqSrcPiece & (1 << 5)))
						 {
							if(capture(sqSrc,sqDst) != SUCCESS){
								error = true;
							}
						 }
						 else{
							// 同色和原点，这里先不处理
								error = true;
						 }
					 } });

		if (!isValidPos)
		{
			NotifyManager::getInstance().Add({"这不是一个有效位置"});
		}

		if ((!isValidPos || error) && fromDrag)
		{
			this->mcs.state = MouseStateType::Selected;
		}
	}
}

void Board::activateAnimation(int sqSrc, int sqDst, bool isCapture)
{
	moveAnim.progress = 0;
	moveAnim.elapsedTime = 0;
	moveAnim.isAnimating = true;

	moveAnim.sqSrc = sqSrc;
	moveAnim.sqDst = sqDst;

	moveAnim.startPos = screenXY(BOARD_X(sqSrc), BOARD_Y(sqSrc));
	moveAnim.endPos = screenXY(BOARD_X(sqDst), BOARD_Y(sqDst));

	if (isCapture)
	{
		moveAnim.onComplete = [this]()
		{
			this->postCapture();
		};
	}
	else
	{
		moveAnim.onComplete = [this]()
		{
			this->postMove();
		};
	}
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
