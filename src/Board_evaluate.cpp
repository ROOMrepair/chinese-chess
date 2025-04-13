#include "Board.hpp"

bool Board::Proteced(int sd,int sqSrc,int sqExcept){

	int i,j,sqDst,nSideTag,pc,sqPin,x,y,bx,by;
	int colmask,rowmask;
	SlideMoveStruct col_move, row_move;

	nSideTag = SIDE_TAG(sd); // 16 or 32 

	// sq 是否是在当前执子方(sd)半部
	if(HOME_HALF(this->isRedSide,sqSrc,sd)){
		if(IN_FORT(sqSrc)){

			// 受到将的保护 
			sqDst = Pieces[nSideTag + KING].alive;		
			if(sqDst != 0 && sqDst != sqExcept){ // sqExcept 位置受牵制不能移动，不能保护其他子
				if(KING_SPAN(sqSrc,sqDst)){
					return true;
				}
			}
			// 受到士的保护
      		for (i = ADVISOR_FROM; i <= ADVISOR_TO; i ++) {
        		sqDst = Pieces[nSideTag + i].alive;
        		if (sqDst != 0 && sqDst != sqExcept) {
          			if (ADVISOR_SPAN(sqSrc, sqDst)) {
            			return true;
          			}
        		}
			}
		}	
			// 受到象的保护
		for (i = BISHOP_FROM; i <= BISHOP_TO; i ++) {
			sqDst = Pieces[nSideTag + i].alive;
			if (sqDst != 0 && sqDst != sqExcept) {
		  		if (BISHOP_SPAN(sqSrc, sqDst) && Squares[BISHOP_PIN(sqSrc, sqDst)] == 0) {
					return true;
		  		}
			}
	  	}
	}else{
		
    	// 判断受到过河兵(卒)横向的保护
    	for (sqDst = sqSrc - 1; sqDst <= sqSrc + 1; sqDst += 2) {
      		if (sqDst != sqExcept) {
        		pc = Squares[sqDst];
        		if ((pc & nSideTag) != 0 && PIECE_INDEX(pc) >= PAWN_FROM) {
          		return true;
        		}
      		}
    	}
	
	}
	
	// 受到兵的纵向保护
  	sqDst = SQUARE_BACKWARD(this->isRedSide, sqSrc, sd);
	if(sqDst != sqExcept){
		pc = Squares[sqDst];
		if((pc & nSideTag) != 0 && PIECE_INDEX(pc) >= PAWN_FROM){
			return true;
		}	
	}

  	// 判断受到马的保护
  	for (i = KNIGHT_FROM; i <= KNIGHT_TO; i ++) {
    	sqDst = Pieces[nSideTag + i].alive;
    	if (sqDst != 0 && sqDst != sqExcept) {
      	sqPin = KNIGHT_PIN(sqDst, sqSrc); // 注意，sqSrc和sqDst是反的,sqDst是马的起始位置
      		if (sqPin != sqDst && Squares[sqPin] == 0) {
        		return true;
      		}
    	}
  	}
	

  	x = FILE_X(sqSrc);
  	y = RANK_Y(sqSrc);
	bx = x - colOffset;
	by = y - rowOffset;

	// 受到车的保护
	for( i = ROOK_FROM; i <= ROOK_TO; i++){
		sqDst = Pieces[nSideTag + i].alive;	
		if(sqDst != 0 && sqDst != sqExcept && sqDst != sqSrc){ // 前面都会自动排除相同点，这里需要加上
			if(x == FILE_X(sqDst)){
				colmask = wBitCols[FILE_X(sqDst)];
				col_move = SlideMoveCol[by][colmask];
				for (j = 0; j < 2; ++j){
					if(col_move.RookCap[j] - rowOffset == RANK_Y(sqDst) - rowOffset){
						return true;
					} 					
				}
			}else if(y == RANK_Y(sqDst)){
				rowmask = wBitRows[RANK_Y(sqDst)];
				row_move = SlideMoveRow[bx][rowmask];
				for (j = 0; j < 2; ++j){
					if(row_move.RookCap[j] - colOffset == FILE_X(sqDst) - colOffset){
						return true;
					} 					
				}
			}
		}
	}
	
	// 受到炮的保护
	for( i = CANNON_FROM; i <= CANNON_TO; i++){
		sqDst = Pieces[nSideTag + i].alive;	
		if(sqDst != 0 && sqDst != sqExcept && sqDst != sqSrc){ // 前面都会自动排除相同点，这里需要加上
			if(x == FILE_X(sqDst)){
				colmask = wBitCols[FILE_X(sqDst)];
				col_move = SlideMoveCol[by][colmask];
				for (j = 0; j < 2; ++j){
					if(col_move.CannonCap[j] - rowOffset == RANK_Y(sqDst) - rowOffset){
						return true;
					} 					
				}
			}else if(y == RANK_Y(sqDst)){
				rowmask = wBitRows[RANK_Y(sqDst)];
				row_move = SlideMoveRow[bx][rowmask];
				for (j = 0; j < 2; ++j){
					if(row_move.CannonCap[j] - colOffset == FILE_X(sqDst) - colOffset){
						return true;
					} 					
				}
			}
		}
	}	

	return false;
}


/* ElephantEye的局面评价内容共4有4部分
 * 1. 跟仕(士)有关的特殊棋型的评价，见"AdvisorShape()"；
 * 2. 车或炮牵制帅(将)或车的棋型的评价，见"StringHold()"；
 * 3. 车的灵活性的评价，见"RookMobility()"；
 * 4. 马受到阻碍的评价，见"KnightTrap()"。
 */

//  * 1. 帅(将)在原位，双仕(士)都在底线，编为1号，这种情况要判断空头炮和炮镇窝心马；
//  * 2. 帅(将)在原位，双仕(士)从左边包围帅(将)，编为2号，这种情况要判断右边的沉底炮和车封右边的帅(将)门；
//  * 3. 帅(将)在原位，双仕(士)从右边包围帅(将)，编为3号，这种情况要判断左边的沉底炮和车封左边的帅(将)门；
//  * 4. 其他情况，包括帅(将)不在原位或缺仕(士)，都编号0。
const int SHAPE_NONE = 0;
const int SHAPE_CENTER = 1;
const int SHAPE_LEFT = 2;
const int SHAPE_RIGHT = 3;

// part 1
int Board::AdvisorShape(){
  	int redPenalty, blackPenalty;
	int nshape,sqAdv1,sqAdv2,pcCannon,sq,x,y;
	int redSideTag = SIDE_TAG(0),blackSideTag = SIDE_TAG(1);
	int redKingInitPos = isRedSide ? 0xc7 : 0x37, blackKingInitPos = isRedSide ? 0x37 : 0xc7;

	int portCenterRed = redKingInitPos == 0xc7 ?  0xb7 : 0x47;	
	int portCenterBlack = blackKingInitPos == 0x37 ? 0x47 : 0xb7;

	// red
	sqAdv1 = this->Pieces[redSideTag + ADVISOR_FROM].alive;
	sqAdv2 = this->Pieces[redSideTag + ADVISOR_TO].alive;
	if(sqAdv1 && sqAdv2){
		if(Pieces[redSideTag + KING].alive == redKingInitPos){
			if(false){
			}else if(sqAdv1 == redKingInitPos - 1){ // left 
				nshape = (sqAdv2 == redKingInitPos + 1) ? SHAPE_CENTER : (sqAdv2 == portCenterRed) ? SHAPE_LEFT : SHAPE_NONE;
			}else if(sqAdv1 == redKingInitPos + 1){
				nshape = (sqAdv2 == redKingInitPos - 1) ? SHAPE_CENTER : (sqAdv2 == portCenterRed) ? SHAPE_RIGHT : SHAPE_NONE;
			}else if(sqAdv1 == portCenterRed){
				nshape = (sqAdv2 == redKingInitPos - 1) ? SHAPE_LEFT : (sqAdv2 == redKingInitPos + 1) ? SHAPE_RIGHT : SHAPE_NONE;
			}else{
				nshape = SHAPE_NONE;
			}
		} 
		switch(nshape){
			case SHAPE_CENTER:
        		// for (pcCannon =blackSideTag + CANNON_FROM; pcCannon <= SIDE_TAG(1) + CANNON_TO; pcCannon ++) {
          		// sq = this->Pieces[pcCannon].alive;
          		// if (sq != 0) {
            	// 	x = FILE_X(sq);
            	// 	if (x == colCenter) {
              	// 	y = RANK_Y(sq);
              	// 	lpsms = this->FileMaskPtr(x, y);
              	// 	if ((lpsms->wRookCap & WHITE_KING_BITFILE) != 0) {
                // 		// 计算空头炮的威胁
                // 		redPenalty += PreEvalEx.vlHollowThreat[RANK_FLIP(y)];
              	// 	} else if ((lpsms->wSuperCap & WHITE_KING_BITFILE) != 0 &&
                //   		(this->ucpcSquares[0xb7] == 21 || this->ucpcSquares[0xb7] == 22)) {
                // 		// 计算炮镇窝心马的威胁
                // 		redPenalty += PreEvalEx.vlCentralThreat[RANK_FLIP(y)];
              	// 	}
            	// 	}
          		// }
        		// }
			case SHAPE_LEFT:
			case SHAPE_RIGHT:
			case SHAPE_NONE:
				break;
			default:
				break;
		}
	}
	return 0;
}

int Board::evaluateLevel1(){
	return 0;	
}