#include "Board.hpp"

//  * MVV(LVA)指的是：如果被吃子无保护，那么取值MVV，否则取值MVV-LVA。
//  * 由于ElephantEye的MVV(LVA)值在计算完毕后再加了1，并且有其它考虑，因此有以下几种含义：
//  * a. MVV(LVA)大于1，说明被吃子价值大于攻击子(表面上是赚的)，这种吃子将首先搜索，静态搜索也将考虑这种吃子；
//  * b. MVV(LVA)等于1，说明被吃子有一定价值(吃车马炮或吃过河兵卒，即便表面上是亏的，也值得一试)，静态搜索也将考虑这种吃子；
//  * c. MVV(LVA)等于0，说明被吃子没有价值，静态搜索将不考虑这种吃子。
//  *
//  * MVV价值表"SIMPLE_VALUE"是按照帅(将)=5、车=4、马炮=3、兵(卒)=2、仕(士)相(象)=1设定的；

int Board::MvvLva(int sqDst, int pcCaptured, int nLva)
{ // nLva 表示攻击子的子力值
	int nMvv, nLvaAdjust;
	int sdPlayer = isRedTurn ? 0 : 1;
	nMvv = SIMPLE_VALUE(pcCaptured); // 如果被捉子的值 4
	// 吃子被对方保护
	nLvaAdjust = (Protected(OPP_SIDE(sdPlayer), sqDst) ? nLva : 0);
	if (nMvv >= nLvaAdjust)
	{ // 正常攻击 -0 或者被保护但是攻击子子力小，可以牺牲
		return nMvv - nLvaAdjust + 1;
	}
	else
	{ // 被捉子有价值，或者该子对己方构成威胁，硬捉
		return (nMvv >= 3 || HOME_HALF(isRedSide, sqDst, sdPlayer)) ? 1 : 0;
	}
}

int Board::GenAllMoves(MoveStruct *lpmvsCap, MoveStruct *lpmvsNocap)
{
	int capCount, nocapCount;
	int i, pt, sqSrc, sqDst, sqDstpt;
	int nSideTag;
	// bool bCanPromote;
	MoveStruct *lpmvsCapCurr, *lpmvsNocapCurr;

	// 数组前移指针
	lpmvsCapCurr = lpmvsCap;
	lpmvsNocapCurr = lpmvsNocap;

	nSideTag = SIDE_TAG(isRedTurn ? 0 : 1);

	//! use my piece cap

	// 1 生成将的着法
	pt = nSideTag + KING;
	sqSrc = Pieces[pt].alive;

	if (sqSrc != 0)
	{
		__ASSERT_SQUARE(sqSrc);
		pieceCap(pt, sqSrc, [&](int bx, int by, int debug)
				 {
			sqDst = SQpos(by, bx);				
			sqDstpt = Squares[sqDst];
			if(debug) return;
			if(sqSrc == sqDst) return;
			if(sqDstpt == 0){
				lpmvsNocapCurr->wmv = MOVE(sqSrc,sqDst); 					
				lpmvsNocapCurr ++;
			} else if((sqDstpt & (1 << 5)) != (pt & (1 << 5))){ // cap
				lpmvsCapCurr->wmv = MOVE(sqSrc,sqDst);
				lpmvsCapCurr->wvl = MvvLva(sqDst,sqDstpt,5); // 帅(将)的价值是 5
				lpmvsCapCurr ++;
			} });
	}

	// 2. 生成仕(士)的着法
	for (i = ADVISOR_FROM; i <= ADVISOR_TO; i++)
	{
		pt = nSideTag + i;
		sqSrc = Pieces[pt].alive;
		if (sqSrc != 0)
		{
			__ASSERT_SQUARE(sqSrc);
			pieceCap(pt, sqSrc, [&](int bx, int by, int debug)
					 {
				sqDst = SQpos(by, bx);				
				sqDstpt = Squares[sqDst];
				if(debug) return;
				if(sqSrc == sqDst) return;
				if(sqDstpt == 0){
					lpmvsNocapCurr->wmv = MOVE(sqSrc,sqDst); 					
					lpmvsNocapCurr ++;
				} else if((sqDstpt & (1 << 5)) != (pt & (1 << 5))){ // cap
					lpmvsCapCurr->wmv = MOVE(sqSrc,sqDst);
					lpmvsCapCurr->wvl = MvvLva(sqDst,sqDstpt,1); // 士的价值是 1
					lpmvsCapCurr ++;
				} });
		}
	}

	// 3. 生成相(象)的着法
	for (i = BISHOP_FROM; i <= BISHOP_TO; i++)
	{
		pt = nSideTag + i;
		sqSrc = Pieces[nSideTag + i].alive;
		if (sqSrc != 0)
		{
			__ASSERT_SQUARE(sqSrc);

			pieceCap(pt, sqSrc, [&](int bx, int by, int debug)
					 {
				sqDst = SQpos(by, bx);				
				sqDstpt = Squares[sqDst];
				if(debug) return;
				if(sqSrc == sqDst) return;

				if(sqDstpt == 0){
					lpmvsNocapCurr->wmv = MOVE(sqSrc,sqDst); 					
					lpmvsNocapCurr ++;
				}else if((sqDstpt & (1 << 5)) != (pt & (1 << 5))){ // cap
					lpmvsCapCurr->wmv = MOVE(sqSrc,sqDst);
					lpmvsCapCurr->wvl = MvvLva(sqDst,sqDstpt,1); //象的价值是 1
					lpmvsCapCurr ++;
				} });
		}
	}

	// 4. 生成马的着法
	for (i = KNIGHT_FROM; i <= KNIGHT_TO; i++)
	{
		pt = nSideTag + i;
		sqSrc = Pieces[nSideTag + i].alive;
		if (sqSrc != 0)
		{
			__ASSERT_SQUARE(sqSrc);

			pieceCap(pt, sqSrc, [&](int bx, int by, int debug)
					 {
				sqDst = SQpos(by, bx);				
				sqDstpt = Squares[sqDst];
				if(debug) return;
				if(sqSrc == sqDst) return;

				if(sqDstpt == 0){
					lpmvsNocapCurr->wmv = MOVE(sqSrc,sqDst); 					
					lpmvsNocapCurr ++;
				}else if((sqDstpt & (1 << 5)) != (pt & (1 << 5))){ // cap
					lpmvsCapCurr->wmv = MOVE(sqSrc,sqDst);
					lpmvsCapCurr->wvl = MvvLva(sqDst,sqDstpt,3); //马的价值是 3
					lpmvsCapCurr ++;
				} });
		}
	}

	// 5. 生成车的着法
	for (i = ROOK_FROM; i <= ROOK_TO; i++)
	{
		pt = nSideTag + i;
		sqSrc = Pieces[nSideTag + i].alive;
		if (sqSrc != 0)
		{
			__ASSERT_SQUARE(sqSrc);

			pieceCap(pt, sqSrc, [&](int bx, int by, int debug)
					 {
				sqDst = SQpos(by, bx);				
				sqDstpt = Squares[sqDst];
				if(debug) return;
				if(sqSrc == sqDst) return;

				if(sqDstpt == 0){
					lpmvsNocapCurr->wmv = MOVE(sqSrc,sqDst); 					
					lpmvsNocapCurr ++;
				}else if((sqDstpt & (1 << 5)) != (pt & (1 << 5))){ // cap
					lpmvsCapCurr->wmv = MOVE(sqSrc,sqDst);
					lpmvsCapCurr->wvl = MvvLva(sqDst,sqDstpt,4); // 车的价值是 4
					lpmvsCapCurr ++;
				} });
		}
	}

	// 6. 生成炮的着法
	for (i = CANNON_FROM; i <= CANNON_TO; i++)
	{
		pt = nSideTag + i;
		sqSrc = Pieces[nSideTag + i].alive;
		if (sqSrc != 0)
		{
			__ASSERT_SQUARE(sqSrc);

			pieceCap(pt, sqSrc, [&](int bx, int by, int debug)
					 {
				sqDst = SQpos(by, bx);				
				sqDstpt = Squares[sqDst];
				if(debug) return;
				if(sqSrc == sqDst) return;

				if(sqDstpt == 0){
					lpmvsNocapCurr->wmv = MOVE(sqSrc,sqDst); 					
					lpmvsNocapCurr ++;
				}else if((sqDstpt & (1 << 5)) != (pt & (1 << 5))){ // cap
					lpmvsCapCurr->wmv = MOVE(sqSrc,sqDst);
					lpmvsCapCurr->wvl = MvvLva(sqDst,sqDstpt,3); // 炮的价值是 3
					lpmvsCapCurr ++;
				} });
		}
	}

	// 7 生成兵的着法
	for (i = PAWN_FROM; i <= PAWN_TO; i++)
	{
		pt = nSideTag + i;
		sqSrc = Pieces[nSideTag + i].alive;
		if (sqSrc != 0)
		{
			__ASSERT_SQUARE(sqSrc);

			pieceCap(pt, sqSrc, [&](int bx, int by, int debug)
					 {
				sqDst = SQpos(by, bx);				
				sqDstpt = Squares[sqDst];
				if(debug) return;
				if(sqSrc == sqDst) return;

				if(sqDstpt == 0){
					lpmvsNocapCurr->wmv = MOVE(sqSrc,sqDst); 					
					lpmvsNocapCurr ++;
				}else if((sqDstpt & (1 << 5)) != (pt & (1 << 5))){ // cap
					lpmvsCapCurr->wmv = MOVE(sqSrc,sqDst);
					lpmvsCapCurr->wvl = MvvLva(sqDst,sqDstpt,2); // 兵的价值是 2
					lpmvsCapCurr ++;
				} });
		}
	}

	capCount = lpmvsCapCurr - lpmvsCap;
	nocapCount = lpmvsNocapCurr - lpmvsNocap;

	assert(capCount <= 128 && nocapCount <= 128);

	return (capCount << 8) | nocapCount;
}

void Board::GetAllMoves(MoveSortStruct *msvs)
{
	int nMoveNum = GenAllMoves(msvs->mvsCap, msvs->mvsNocap);
	int capCount = nMoveNum >> 8, nocapCount = nMoveNum & 0xff;
	msvs->capNum = capCount;
	msvs->nocapNum = nocapCount;
	msvs->allMoveNum = capCount + nocapCount;
	msvs->nMoveIndex = 0;

	std::memcpy(msvs->mvsAll, msvs->mvsCap, capCount * sizeof(MoveStruct));
	std::memcpy(msvs->mvsAll + capCount, msvs->mvsNocap, nocapCount * sizeof(MoveStruct));

	// std::memcpy(msvs->mvsAll, msvs->mvsNocap, nocapCount * sizeof(MoveStruct));
	// std::memcpy(msvs->mvsAll + nocapCount ,msvs->mvsCap, capCount * sizeof(MoveStruct));
}

int Board::MakeMove(int sqSrc, int sqDst)
{
	int pt;
	int sqSrcpt = Squares[sqSrc];
	int sqDstpt = Squares[sqDst];

	Pieces[sqSrcpt].alive = sqDst;
	Squares[sqDst] = sqSrcpt;

	if (sqDstpt == 0)
	{
		// move
		wBitCols[FILE_X(sqDst)] ^= ColMask(sqDst);
		wBitRows[RANK_Y(sqDst)] ^= RowMask(sqDst);
	}
	else
	{
		// capture
		// 目标位置还是有棋子,状态不变,不修改wbit
		Pieces[sqDstpt].alive = 0;
		BitPiece ^= BIT_PIECE(sqDstpt);

		// 使用被吃子更新子力价值
		pt = PIECE_TYPE(sqDstpt);
		if (sqDstpt < 32)
		{
			this->evRed -= PreEval.ucvlRedPieces[pt][sqDst];
		}
		else
		{
			this->evBlack -= PreEval.ucvlBlackPieces[pt][sqDst];
			pt += 7;
		}

		__ASSERT_BOUND(0, pt, 13);
		//! 这里先去掉被吃子的影响
		this->zobr.Xor(zobrTable[pt][sqDst]);
	}

	Squares[sqSrc] = 0;
	wBitCols[FILE_X(sqSrc)] ^= ColMask(sqSrc);
	wBitRows[RANK_Y(sqSrc)] ^= RowMask(sqSrc);

	//! 使用移动棋子更新
	pt = PIECE_TYPE(sqSrcpt);
	if (sqSrcpt < 32)
	{
		// 更新局面分
		this->evRed += (PreEval.ucvlRedPieces[pt][sqDst] - PreEval.ucvlRedPieces[pt][sqSrc]);
	}
	else
	{
		this->evBlack += (PreEval.ucvlBlackPieces[pt][sqDst] - PreEval.ucvlBlackPieces[pt][sqSrc]);
		pt += 7;
	}
	__ASSERT_BOUND(0, pt, 13);
	//! 更新 zobrist：去掉原来位置加上新位置, 先后顺序无影响
	this->zobr.Xor(zobrTable[pt][sqSrc], zobrTable[pt][sqDst]);

	return sqDstpt;
}

void Board::SaveStatus()
{ // 保存状态
	RollbackStruct *lprbs;
	lprbs = rbsList + rbMoveNum;
	lprbs->zobr = zobr;
	lprbs->evRed = evRed;
	lprbs->evBlack = evBlack;
}

void Board::Rollback()
{ // 用 rblist 快速回滚
	RollbackStruct *lprbs;
	lprbs = rbsList + rbMoveNum;
	zobr = lprbs->zobr;
	evRed = lprbs->evRed;
	evBlack = lprbs->evBlack;
}

void Board::RollbackMove(int sqSrc, int sqDst, int sqSrcpt, int sqDstpt)
{

	Squares[sqSrc] = sqSrcpt;
	Squares[sqDst] = sqDstpt;
	Pieces[sqSrcpt].alive = sqSrc;

	if (sqDstpt == 0)
	{
		// 复原空点位
		wBitCols[FILE_X(sqDst)] ^= ColMask(sqDst);
		wBitRows[RANK_Y(sqDst)] ^= RowMask(sqDst);
	}
	else
	{
		// 复原 吃子
		Pieces[sqDstpt].alive = sqDst;
		BitPiece ^= BIT_PIECE(sqDstpt);
	}

	wBitCols[FILE_X(sqSrc)] ^= ColMask(sqSrc);
	wBitRows[RANK_Y(sqSrc)] ^= RowMask(sqSrc);
}

bool Board::SimulateMove(int mv)
{
	int sqSrc, sqDst, sqSrcpt, sqDstpt, pcCaptured;
	uint32_t dwOldZobristKey;
	RollbackStruct *lprbs;

	if (rbMoveNum == MAX_MOVE_NUM)
	{
		return false;
	}
	assert(rbMoveNum < MAX_MOVE_NUM);

	sqSrc = SRC(mv);
	sqDst = DST(mv);
	sqSrcpt = Squares[sqSrc];
	sqDstpt = Squares[sqDst];

	// dwOldZobristKey = this->zobr.dwKey; //?为什么还要保存
	this->SaveStatus();
	pcCaptured = this->MakeMove(sqSrc, sqDst);
	assert(pcCaptured == sqDstpt);

	if (checkMate())
	{														// 将军
		this->RollbackMove(sqSrc, sqDst, sqSrcpt, sqDstpt); //? 恢复Square wbitcol/row Piece bitpiece
		this->Rollback();									//? 恢复 evblack/red zobr
		return false;
	}

	this->changeSideSimulation();

	// 原来的Zobrist键值记录到检测重复的迷你置换表中
	//   if (this->ucRepHash[dwOldZobristKey & REP_HASH_MASK] == 0) {
	//     this->ucRepHash[dwOldZobristKey & REP_HASH_MASK] = this->nMoveNum;
	//   }

	// 保存到历史着法表中
	//! 这个checkmate 一定是对方被checkmate,(自己不能被checkmate上面过滤了)
	//! 即返回的是本方的子(或noking)或0,
	//! mv移动的一定是本方的子
	lprbs = rbsList + rbMoveNum;
	lprbs->mvs.wmv = mv;
	lprbs->mvs.ChkChs = checkMate(); //? 正表示将军子
	lprbs->Srcpt = sqSrcpt;
	lprbs->Dstpt = sqDstpt;

	if (pcCaptured == 0)
	{ // 没有吃子时可以记录和棋预期数
		if (lprbs->mvs.ChkChs == 0)
		{									   // 也没有对对方将军
											   // 捉到的对方的子的类型,所属的阵营为mv的对面
			lprbs->mvs.ChkChs = -chasedBy(mv); //? 移动后造成的局面影响,为什么是负的?负表示被捉子
		}
		// if (LastMove().CptDrw == -100) {
		// 	lprbs->mvs.CptDrw = -100;
		// } else {
		// 	lprbs->mvs.CptDrw = std::min((int) LastMove().CptDrw, 0) - (lprbs->mvs.ChkChs > 0 || LastMove().ChkChs > 0 ? 0 : 1);
		// }
		// __ASSERT_BOUND(-100, lprbs->mvs.CptDrw, 0);
		lprbs->mvs.CptDrw = 0;
	}
	else
	{
		lprbs->mvs.CptDrw = pcCaptured;
		__ASSERT_PIECE(pcCaptured); // 记录被吃子
	}

	rbMoveNum++; //! 此处共用后需要清除
	return true;
}

void Board::UndoSimulateMove()
{
	assert(rbMoveNum >= 1);
	int sqSrc, sqDst, sqSrcpt, sqDstpt;
	RollbackStruct *lprbs;
	rbMoveNum--;
	lprbs = rbsList + rbMoveNum;
	sqSrc = SRC(lprbs->mvs.wmv);
	sqDst = DST(lprbs->mvs.wmv);
	sqSrcpt = lprbs->Srcpt;
	sqDstpt = lprbs->Dstpt;

	assert(Squares[sqDst] == sqSrcpt); // 检查此时的状态应该恰好能够回退
	assert(lprbs->mvs.CptDrw == sqDstpt);
	this->RollbackMove(sqSrc, sqDst, sqSrcpt, sqDstpt);
	this->Rollback();

	this->isRedTurn = !this->isRedTurn;
	// 重置置换表
}


int Board::chasedBy(int mv)
{ // 返回捉到的子類別
	int i, sd, nSideTag, pcMoved, pcCaptured, kingPos;
	int sqSrc, sqDst, x, y, px, py;
	uint8_t *lpucsqDst, *lpucsqPin;
	SlideMoveStruct *lpsmv;

	//! 此时确保已经移动后,sqSrc 到 dst， chaseby 是移动后检测刚才 mv 的后果的
	sqSrc = DST(mv); // 上一步对方移动的子
	pcMoved = this->Squares[sqSrc];
	// 此时的移动方(对方)
	sd = this->isRedTurn ? 0 : 1;
	nSideTag = SIDE_TAG(sd);
	kingPos = this->Pieces[nSideTag + KING].alive;
	__ASSERT_SQUARE(kingPos);
	__ASSERT_SQUARE(sqSrc);
	__ASSERT_PIECE(pcMoved);
	__ASSERT_BOUND(0, pcMoved - OPP_SIDE_TAG(sd), 15);

	// “捉”的判断包括以下几部分内容：
	switch (pcMoved - OPP_SIDE_TAG(sd))
	{ // 上一步的走子类型

	// 1. 走了马，判断是否捉车或捉有根的炮兵(卒)
	case KNIGHT_FROM:
	case KNIGHT_TO:
		// 逐一检测马踩的八个位置
		lpucsqDst = KnightMoves[sqSrc];
		lpucsqPin = KnightPins[sqSrc];
		sqDst = *lpucsqDst;
		while (sqDst != 0)
		{
			__ASSERT_SQUARE(sqDst);
			if (Squares[*lpucsqPin] == 0)
			{
				pcCaptured = this->Squares[sqDst]; // 捉的目标位
				if ((pcCaptured & nSideTag) != 0)
				{							// 捉到本方的子
					pcCaptured -= nSideTag; // 捉子的类别
					__ASSERT_BOUND(0, pcCaptured, 15);
					// 技巧：优化兵种判断的分枝
					if (pcCaptured <= ROOK_TO)
					{
						// 马捉仕(士)、相(象)和马的情况不予考虑
						if (pcCaptured >= ROOK_FROM)
						{
							// 马捉到了车
							return pcCaptured;
						}
					}
					else
					{
						if (pcCaptured <= CANNON_TO)
						{
							// 马捉到了炮，要判断炮是否受保护
							if (!Protected(sd, sqDst))
							{
								return pcCaptured;
							}
						}
						else
						{
							// 马捉到了兵(卒)，要判断兵(卒)是否过河并受保护
							assert(DIFF_HALF(sqDst, kingPos) == !HOME_HALF(this->isRedSide, sqDst, sd));
							if (DIFF_HALF(sqDst, kingPos) && !Protected(sd, sqDst))
							{
								return pcCaptured;
							}
						}
					}
				}
			}
			lpucsqDst++;
			lpucsqPin++;
			sqDst = *lpucsqDst;
		}
		break;

	// 2. 走了车，判断是否捉有根的马炮兵(卒)
	case ROOK_FROM:
	case ROOK_TO:
		x = FILE_X(sqSrc);
		y = RANK_Y(sqSrc);
		if (((SRC(mv) ^ sqSrc) & 0xf) == 0)
		{
			// 如果车纵向移动了，则判断车横向吃到的子
			lpsmv = RankMovePtr(x, y);
			for (i = 0; i < 2; i++)
			{
				px = lpsmv->RookCap[i];
				assert(px >= 0);
				sqDst = SQPOS_xy(px, y);
				// sqDst = lpsmv->RookCap[i] + RANK_DISP(y);
				__ASSERT_SQUARE(sqDst);
				if (sqDst != sqSrc)
				{
					pcCaptured = this->Squares[sqDst];
					if ((pcCaptured & nSideTag) != 0)
					{
						pcCaptured -= nSideTag;
						__ASSERT_BOUND(0, pcCaptured, 15);
						// 技巧：优化兵种判断的分枝
						if (pcCaptured <= ROOK_TO)
						{
							// 车捉仕(士)、相(象)的情况不予考虑
							if (pcCaptured >= KNIGHT_FROM)
							{
								if (pcCaptured <= KNIGHT_TO)
								{
									// 车捉到了马，要判断马是否受保护
									if (!Protected(sd, sqDst))
									{
										return pcCaptured;
									}
								}
								// 车捉车的情况不予考虑
							}
						}
						else
						{
							if (pcCaptured <= CANNON_TO)
							{
								// 车捉到了炮，要判断炮是否受保护
								if (!Protected(sd, sqDst))
								{
									return pcCaptured;
								}
							}
							else
							{
								// 车捉到了兵(卒)，要判断兵(卒)是否过河并受保护
								assert(DIFF_HALF(sqDst, kingPos) == !HOME_HALF(this->isRedSide, sqDst, sd));
								if (DIFF_HALF(sqDst, kingPos) && !Protected(sd, sqDst))
								{
									return pcCaptured;
								}
							}
						}
					}
				}
			}
		}
		else
		{
			// 如果车横向移动了，则判断车纵向吃到的子
			lpsmv = FileMovePtr(x, y);
			for (i = 0; i < 2; i++)
			{
				py = lpsmv->RookCap[i];
				assert(py >= 0);
				sqDst = SQPOS_xy(x, py);
				// sqDst = lpsmv->ucRookCap[i] + FILE_DISP(x);
				__ASSERT_SQUARE(sqDst);
				if (sqDst != sqSrc)
				{
					pcCaptured = this->Squares[sqDst];
					if ((pcCaptured & nSideTag) != 0)
					{
						pcCaptured -= nSideTag;
						__ASSERT_BOUND(0, pcCaptured, 15);
						// 技巧：优化兵种判断的分枝
						if (pcCaptured <= ROOK_TO)
						{
							// 车捉仕(士)、相(象)的情况不予考虑
							if (pcCaptured >= KNIGHT_FROM)
							{
								if (pcCaptured <= KNIGHT_TO)
								{
									// 车捉到了马，要判断马是否受保护
									if (!Protected(sd, sqDst))
									{
										return pcCaptured;
									}
								}
								// 车捉车的情况不予考虑
							}
						}
						else
						{
							if (pcCaptured <= CANNON_TO)
							{
								// 车捉到了炮，要判断炮是否受保护
								if (!Protected(sd, sqDst))
								{
									return pcCaptured;
								}
							}
							else
							{
								// 车捉到了兵(卒)，要判断兵(卒)是否过河并受保护
								assert(DIFF_HALF(sqDst, kingPos) == !HOME_HALF(this->isRedSide, sqDst, sd));
								if (DIFF_HALF(sqDst, kingPos) && !Protected(sd, sqDst))
								{
									return pcCaptured;
								}
							}
						}
					}
				}
			}
		}
		break;

	// 3. 走了炮，判断是否捉车或捉有根的马兵(卒)
	case CANNON_FROM:
	case CANNON_TO:
		x = FILE_X(sqSrc);
		y = RANK_Y(sqSrc);
		if (((SRC(mv) ^ sqSrc) & 0xf) == 0)
		{
			// 如果炮纵向移动了，则判断炮横向吃到的子
			lpsmv = RankMovePtr(x, y);
			for (i = 0; i < 2; i++)
			{
				px = lpsmv->CannonCap[i];
				assert(px >= 0);
				sqDst = SQPOS_xy(px, y);
				// sqDst = lpsmv->ucCannonCap[i] + RANK_DISP(y);
				__ASSERT_SQUARE(sqDst);
				if (sqDst != sqSrc)
				{
					pcCaptured = this->Squares[sqDst];
					if ((pcCaptured & nSideTag) != 0)
					{
						pcCaptured -= nSideTag;
						__ASSERT_BOUND(0, pcCaptured, 15);
						// 技巧：优化兵种判断的分枝
						if (pcCaptured <= ROOK_TO)
						{
							// 炮捉仕(士)、相(象)的情况不予考虑
							if (pcCaptured >= KNIGHT_FROM)
							{
								if (pcCaptured <= KNIGHT_TO)
								{
									// 炮捉到了马，要判断马是否受保护
									if (!Protected(sd, sqDst))
									{
										return pcCaptured;
									}
								}
								else
								{
									// 炮捉到了车
									return pcCaptured;
								}
							}
						}
						else
						{
							// 炮捉炮的情况不予考虑
							if (pcCaptured >= PAWN_FROM)
							{
								// 炮捉到了兵(卒)，要判断兵(卒)是否过河并受保护
								assert(DIFF_HALF(sqDst, kingPos) == !HOME_HALF(this->isRedSide, sqDst, sd));
								if (DIFF_HALF(sqDst, kingPos) && !Protected(sd, sqDst))
								{
									return pcCaptured;
								}
							}
						}
					}
				}
			}
		}
		else
		{
			// 如果炮横向移动了，则判断炮纵向吃到的子
			lpsmv = FileMovePtr(x, y);
			for (i = 0; i < 2; i++)
			{
				py = lpsmv->CannonCap[i];
				assert(py >= 0);
				sqDst = SQPOS_xy(x, py);
				// sqDst = lpsmv->ucCannonCap[i] + FILE_DISP(x);
				__ASSERT_SQUARE(sqDst);
				if (sqDst != sqSrc)
				{
					pcCaptured = this->Squares[sqDst];
					if ((pcCaptured & nSideTag) != 0)
					{
						pcCaptured -= nSideTag;
						__ASSERT_BOUND(0, pcCaptured, 15);
						// 技巧：优化兵种判断的分枝
						if (pcCaptured <= ROOK_TO)
						{
							// 炮捉仕(士)、相(象)的情况不予考虑
							if (pcCaptured >= KNIGHT_FROM)
							{
								if (pcCaptured <= KNIGHT_TO)
								{
									// 炮捉到了马，要判断马是否受保护
									if (!Protected(sd, sqDst))
									{
										return pcCaptured;
									}
								}
								else
								{
									// 炮捉到了车
									return pcCaptured;
								}
							}
						}
						else
						{
							// 炮捉炮的情况不予考虑
							if (pcCaptured >= PAWN_FROM)
							{
								// 炮捉到了兵(卒)，要判断兵(卒)是否过河并受保护
								assert(DIFF_HALF(sqDst, kingPos) == !HOME_HALF(this->isRedSide, sqDst, sd));
								if (DIFF_HALF(sqDst, kingPos) && !Protected(sd, sqDst))
								{
									return pcCaptured;
								}
							}
						}
					}
				}
			}
		}
		break;
	}

	return 0;
}
