#include "search.hpp"
#include "Board.hpp"

MoveSortStruct rtmsvs = {};
RollbackStruct rbsList[MAX_MOVE_NUM] = {}; 
uint16_t BestMoveList[LIMIT_DEPTH] = {};
int rbMoveNum = 0;

// 置换表
int nHashMask;
// HashStruct *hshItems;
std::unique_ptr<HashStruct[]> hshItems;

// 最终是从初始根结点选一步走，单独将它分出来,最终是看根节点的 isredturn处于什么状态
int Board::SearchRoot(int nDepth){
	int nNewDepth, vlBest, vl, mv;

  	// uint16_t wmvPvLine[LIMIT_DEPTH]; 临时变例

  	vlBest = -MATE_VALUE; // 初始值
    // 重置但是目前没有用
	rtmsvs.ShellSort(rtmsvs.mvsAll,rtmsvs.allMoveNum);

	while (mv = rtmsvs.NextRoot(),mv != 0)
	{
		if(this->SimulateMove(mv)){ // 检测是否着法合理
			// 如果上一步已经有将军，减少深度
      		// nNewDepth = this->LastMove().ChkChs > 0 ? nDepth : nDepth - 1;
			nNewDepth = nDepth;
						
			// vl = Minimax_Raw(nNewDepth,false);
			vl = -Alpha_Beta_Raw(nNewDepth, -MATE_VALUE, MATE_VALUE);	
      		this->UndoSimulateMove();

			if(!this->isThinking.load()){
				return vlBest;
			}

			if(vl > vlBest){
				vlBest = vl;	
				BestMoveList[0] = mv;			
				rtmsvs.UpdateRoot(mv);
			}	
		}
	}
	
	return vlBest;
}

int Board::SearchMain(int nDepth){
	int i,vl,mv;
	int vlLast = -MATE_VALUE;
	BestMoveList[0] = 0;

	// root node 使用全局 msvs
	this->GetAllMoves(&rtmsvs);
	rtmsvs.InitRoot();

	// 迭代加深
	for(i = 1; i <= nDepth; i ++){
		this->searchNodes = 0;
		vl = this->SearchRoot(i);

		if(vlLast < vl){
			vlLast = vl;
		}
		DEBUG_("[depth: ",i," searchNodes: ",this->searchNodes,"]");
		if(!this->isThinking.load()){
			vlLast = vl;
			DEBUG_("[quit in previous]");
			break;	
		}
		
    	if (vlLast > WIN_VALUE || vlLast < -WIN_VALUE) {
    	  break;
    	}

		rtmsvs.nMoveIndex = 0;
		// 超时检测
		
		// 有唯一着法判断
	}

	if(BestMoveList[0] != 0){
		DEBUG_("vlLast ",vlLast);
		DEBUG_("bestmove from ",SRC(BestMoveList[0])," to ",DST(BestMoveList[0]));
		return BestMoveList[0];
	}else{
		mv = rtmsvs.mvsAll[rand() % rtmsvs.allMoveNum].wmv; 
		DEBUG_("rand from ",SRC(mv)," to ",DST(mv));
		return mv; 
	}
}

// 朴素静态搜索
int Board::Alpha_Beta_Raw(int nDepth,int alpha,int beta){
	int vl,mv,vlBest,nHashFlag;
	this->searchNodes++;
	if((vl = ProbeHash(this->zobr,nDepth, alpha, beta)) != (-MATE_VALUE - 1)){
		return vl;	
	}
	if(nDepth <= 0){
		vl = Evaluate();
		// RecordHash(this->zobr,HASH_PV,mv,nDepth,vl);
		return vl; 
	}

	nHashFlag = HASH_ALPHA;
	vlBest = -MATE_VALUE;
	MoveSortStruct mvs;
	this->GetAllMoves(&mvs);

	while(mv = mvs.NextRoot(),mv != 0){
		if(this->SimulateMove(mv)){
			vl = -Alpha_Beta_Raw(nDepth - 1, -beta, -alpha);		
      		this->UndoSimulateMove();

			if(!this->isThinking.load()){
				return vlBest;
			}

			if(vl > vlBest){
				vlBest = vl;
				if(vl >= beta){
					nHashFlag = HASH_BETA;
					break;
				}

				if(vl > alpha){
					alpha = vl;
					nHashFlag = HASH_PV;
				}
			}
		}		
	}

	if (vlBest == -MATE_VALUE) {
		return vlBest;
	}else{
		RecordHash(this->zobr,nHashFlag,mv,nDepth,vlBest);
		return vlBest;
	}

}

int Board::Minimax_Raw(int nDepth, bool isMaximizingPlayer) {
	int vl,mv;

	this->searchNodes++;
    if (nDepth == 0) {
		int v = Evaluate(); 
		// DEBUG_("minmax v:",v);
        return v;
    }

    int bestValue = isMaximizingPlayer ? -INFINITY : INFINITY;
    MoveSortStruct mvs;
    this->GetAllMoves(&mvs);

    while ((mv = mvs.NextRoot()) != 0) {
        if (this->SimulateMove(mv)) {
            vl = Minimax_Raw(nDepth - 1, !isMaximizingPlayer);
            this->UndoSimulateMove();

			if(!this->isThinking.load()){
				return bestValue;
			}

            // MAX 层取最大值，MIN 层取最小值
            bestValue = isMaximizingPlayer ? 
                std::max(bestValue, vl) : 
                std::min(bestValue, vl);
        }
    }

    return bestValue;
}