#include "MenuState.hpp"

int discardPieceNum = 0; 

MenuState::MenuState(StateManager &sm,MouseClickState &_mcs,std::shared_ptr<Board> b)
	:State(sm),mcs(_mcs),tempBoard(std::move(b)){
}

void MenuState::Update([[maybe_unused]] float frameTime,Vector2 pos, [[maybe_unused]] bool ishit){

	if(!this->tempBoard->moveAnim.isAnimating){
		if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
			mcs.handleEvent(MouseEventType::Click,pos);
		}

		if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
			mcs.handleEvent(MouseEventType::Down,pos);
		}

		if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
			mcs.handleEvent(MouseEventType::Release,pos);
		}
	}
	
	this->tempBoard->updateMoveAnimation(frameTime);
}

void MenuState::Render([[maybe_unused]] float frameTime){
	bool isDragging = mcs.state == MouseStateType::Dragging;

	this->tempBoard->drawBackground();
	this->DrawTips();
	if(this->tempBoard->activatedDisCardedPos >= 0){
		this->tempBoard->drawMarker(isDragging);
		this->tempBoard->drawPieces(isDragging);
		this->DrawDiscardedMarker(isDragging);
		this->DrawDiscardedPieces(isDragging);
	}else{
		this->DrawDiscardedMarker(isDragging);
		this->DrawDiscardedPieces(isDragging);
		this->tempBoard->drawMarker(isDragging);
		this->tempBoard->drawPieces(isDragging);
	}
}

void MenuState::DrawDiscardedMarker(bool isDragging){
	if(this->tempBoard->activatedDisCardedPos != -1){
		if(!isDragging){
			int dxi = this->tempBoard->activatedDisCardedPos % 4,dyi = this->tempBoard->activatedDisCardedPos / 4;
			Vector2 dPos = {discardedArea.x + 30 + dxi * 50 ,discardedArea.y + 30 + dyi * 50};
			DrawRing(dPos,this->tempBoard->binfo.radius, this->tempBoard->binfo.radius + 5, 0, 360, 36, BLUE);
		}	
	}	
}

void MenuState::DrawDiscardedPieces(bool isDragging){
	auto &asset = Asset::getInstance();
	float r = this->tempBoard->binfo.radius;
	Vector2 activePos,activeTPos;

	DrawRectangleRoundedLinesEx(discardedArea, 0.1f, 10, 5, BLACK);
	for(size_t i = 0; i < this->tempBoard->disCardedPieces.size();++i){
		int pt = this->tempBoard->disCardedPieces[i];

		Color c = PIECE_COLOR(pt);
		auto &p_texture = asset.pieceTextures[this->tempBoard->Pieces[pt].textureIndex].texture;

		float cx = p_texture.width * this->tempBoard->binfo.pieceScale * 0.5;
		float cy = p_texture.height * this->tempBoard->binfo.pieceScale * 0.5;

		int dxi = i % 4,dyi = i / 4;

		Vector2 dPos = {discardedArea.x + 30 + dxi * 50 ,discardedArea.y + 30 + dyi * 50};
		if(isDragging && this->tempBoard->activatedDisCardedPos >= 0 && i == static_cast<size_t>(this->tempBoard->activatedDisCardedPos)){
			dPos.x += this->mcs.dragMousePos.x - this->mcs.dragStartPos.x;
			dPos.y += this->mcs.dragMousePos.y - this->mcs.dragStartPos.y;
			
			activePos = dPos;
			activeTPos = {dPos.x - cx,dPos.y - cy};
		}

		Vector2 textXY = {dPos.x - cx,dPos.y - cy};
		DrawCircleV(
			dPos,
			r,
			c);
		DrawTextureEx(
			p_texture,
			textXY,
			0,
			this->tempBoard->binfo.pieceScale,
			WHITE);
	}	

	if (isDragging && this->tempBoard->activatedDisCardedPos >= 0)
	{
		int pt = this->tempBoard->disCardedPieces[static_cast<size_t>(this->tempBoard->activatedDisCardedPos)];
		Color c = PIECE_COLOR(pt);
		auto &p_texture = asset.pieceTextures[this->tempBoard->Pieces[pt].textureIndex].texture;

		DrawCircleV(activePos, r, c);
		DrawTextureEx(p_texture,activeTPos, 0, this->tempBoard->binfo.pieceScale, WHITE);
	}
}

void MenuState::Trigger(){
	// isgameover check
	{
		if(this->tempBoard->gameOver){
			this->tempBoard->clearBoard();
			this->tempBoard->gameOver = false;
			if(!this->tempBoard->loadFromFen(cszStartFen)){
				std::cerr << "Failed to load fen" << std::endl;
				this->stateManager.forceExit = true;
				return;
			}
		}
	}

	// set state
	{
		this->tempBoard->ISMENUMODE = true;
		this->tempBoard->showTags = true;
		this->tempBoard->boardToFen(szFenState);

		this->isRedSide = this->tempBoard->isRedSide;
		this->useAi = this->tempBoard->useAi;
		this->depth = this->tempBoard->ndepth;
	}

	// bind mcs event
	{
		mcs.bindOnClick([this](MouseStateType pre, Vector2 pos){
			this->mcs.dragMousePos = pos;
			BoardPos bp;	
			int DisCardPos;

			if(this->tempBoard->checkClickActivation(pos,bp)){
				this->mcs.dragStartPos = pos;
				this->tempBoard->activatedDisCardedPos = -1;
				if(pre == MouseStateType::Idle){
					this->tempBoard->activatedPos = bp;
					this->mcs.state = MouseStateType::CheckDragFromIdle;
				}else if(pre == MouseStateType::Selected){
					// 区分对选中的棋子进行单击或是拖动
					// 如果是同一个棋子，检测拖动或单击取消
					if(this->tempBoard->activatedPos.x == bp.x && this->tempBoard->activatedPos.y == bp.y){
						this->mcs.state = MouseStateType::checkDragFromSelected;
					}else{
						this->tempBoard->activatedPos = bp;	
						this->mcs.state = MouseStateType::CheckDragFromIdle;
					}
				}
			}else if(this->checkDisCardedClick(pos,&DisCardPos)){
				this->mcs.dragStartPos = pos;
				this->tempBoard->activatedPos = {-1,-1};
				// DEBUG_("discardPos ",DisCardPos);
				if(pre == MouseStateType::Idle){
					this->tempBoard->activatedDisCardedPos = DisCardPos;
					this->mcs.state = MouseStateType::CheckDragFromIdle;
				}else if(pre == MouseStateType::Selected){
					if(this->tempBoard->activatedDisCardedPos == DisCardPos){
						this->mcs.state = MouseStateType::checkDragFromSelected;
					}else{
						this->tempBoard->activatedDisCardedPos = DisCardPos;
						this->mcs.state = MouseStateType::CheckDragFromIdle;
					}
				}
			}else{
				// 判断移动位置是否合法
				// 不提供 rec 只在 drag 时提供
				if(pre == MouseStateType::Selected){
					this->tempBoard->preMove(false);
				}
			}
		});

		mcs.bindOnDrag([this](MouseStateType pre,Vector2 pos){
			this->mcs.dragMousePos = pos;
			if(pre == MouseStateType::CheckDragFromIdle || pre == MouseStateType::checkDragFromSelected){
				// down 状态不要设置 dragStartPos
				// this->board.dragStartPos = pos;
				// 超过一定时间才认为是拖动,但是太短单击会被误认为拖动
				// 鼠标移动了则认为是拖动 
				if(this->mcs.dragStartPos.x != pos.x || this->mcs.dragStartPos.y != pos.y){
					this->mcs.state = MouseStateType::Dragging;
					return;
				}	
				if(GetTime() - this->mcs.lastClickTime < checkDragDeltaTime){
					return;
				}
				this->mcs.state = MouseStateType::Dragging;
			}else if(pre == MouseStateType::Dragging){
				// do nothing 
			}
		});

		mcs.bindOnRelease([this](MouseStateType pre ,Vector2 pos){
			this->mcs.dragMousePos = pos;
			if(pre == MouseStateType::checkDragFromSelected){
				this->tempBoard->activatedPos = {-1,-1};
				this->tempBoard->activatedDisCardedPos = -1;

				this->mcs.state = MouseStateType::Idle;
			}else if(pre == MouseStateType::CheckDragFromIdle){
				this->mcs.state = MouseStateType::Selected;
			}else if(pre == MouseStateType::Dragging){
				this->tempBoard->preMove(true,this->discardedArea);
			}
		});
	}

	// bind button event
	{
		auto &bm = ButtonManager::getInstance();	
		auto &cm = CounterManager::getInstance();

		Button redSide({700,40},15,RED);
		redSide.bindOnClick([this](){
			this->isRedSide = true;
		});
		Button blackSide({750,40},15,BLACK);
		blackSide.bindOnClick([this](){
			this->isRedSide = false;
		});
		if(isRedSide){
			bm.AddToMutexGroup(std::move(redSide),1,true);		
			bm.AddToMutexGroup(std::move(blackSide),1);
		}else{
			bm.AddToMutexGroup(std::move(redSide),1);		
			bm.AddToMutexGroup(std::move(blackSide),1,true);
		}

		Button useAiButton({573,70},"使用AI");
		useAiButton.bindOnClick([this](){
			this->useAi = true;
		});
		Button notUseAiButton({684,70},"不使用AI");
		notUseAiButton.bindOnClick([this](){
			this->useAi = false;
		});
		if(useAi){
			bm.AddToMutexGroup(std::move(useAiButton),2,true);
			bm.AddToMutexGroup(std::move(notUseAiButton),2);
		}else{
			bm.AddToMutexGroup(std::move(useAiButton),2);
			bm.AddToMutexGroup(std::move(notUseAiButton),2,true);
		}

		Counter counter1(680, 115, 100, 20, this->depth, 0, 10, 1);
		counter1.BindValue([this](int value){
			this->depth = value;
		});
		cm.Add(std::move(counter1));
		
		Button reSet({{60,288},"重置"});
		reSet.SetPadding(10.0f,0.0f);
		reSet.SetFontSize(24);
		reSet.SetSpacing(10);
		reSet.bindOnClick([this](){
			this->tempBoard->clearBoard();
			if(!this->tempBoard->loadFromFen(cszStartFen)){
				std::cerr << "Failed to load fen" << std::endl;
				this->stateManager.forceExit = true;
				return;
			}
		});
		bm.Add(std::move(reSet));

		Button startGame({{170,288},"开始"});
		startGame.SetPadding(80.0f,0.0f);
		startGame.SetFontSize(24);
		startGame.SetSpacing(10);
		startGame.bindOnClick([this](){
			this->onUnloadState();
		});
		bm.Add(std::move(startGame));
	}
}

void MenuState::onUnloadState(){
	// 重新改为fen,重新再加载一遍
	this->tempBoard->boardToFen(szFenState);
	this->tempBoard->isRedSide = this->isRedSide;
	this->tempBoard->useAi = this->useAi;
	this->tempBoard->ndepth = this->depth;


	ButtonManager::getInstance().Clear();
	CounterManager::getInstance().Clear();

	stateManager.resume = true;
}

void MenuState::DrawTips(){
    auto &asset = Asset::getInstance();
	auto &font = asset.fonts[0].font;
    DrawTextEx(font,"选择执子方:",{540,25},DEFAULT_FONT_SIZE,0,BLACK);
    DrawTextEx(font,"搜索层数:",{540,110},DEFAULT_FONT_SIZE,0,BLACK);
}

bool MenuState::checkDisCardedClick(Vector2 pos,int *activated){
	for(size_t i = 0;i < this->tempBoard->disCardedPieces.size();++i){
		int dxi = i % 4,dyi = i / 4;
		Vector2 dPos = {this->discardedArea.x + 30 + dxi * 50 ,this->discardedArea.y + 30 + dyi * 50};
		if(CheckCollisionPointCircle(pos, dPos, this->tempBoard->binfo.radius)){
			*activated = i;
			return true;
		}
	}	
	return false;
}