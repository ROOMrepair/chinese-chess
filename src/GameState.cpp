#include "GameState.hpp"

GameState::GameState(StateManager &sm,
					MouseClickState &_mcs,
					Timer &_t,
					std::shared_ptr<Board> b,
					uint64_t limitedTime):
	State(sm),mcs(_mcs),t(_t),board(std::move(b)),limited_t(limitedTime){
}

GameState::~GameState(){
	std::cout << "Gamestate destroyed" << std::endl;
	board->stopSearch();	
	DEBUG_("board use count when gamestate decay ",board.use_count());
};

void GameState::DrawTips(){
    auto &asset = Asset::getInstance();
	auto &font = asset.fonts[0].font;
	// todo
    DrawTextEx(font,"现在轮到:",{600,20},DEFAULT_FONT_SIZE,0,BLACK);
	if(board->isRedTurn){
		DrawRectangleRounded({720,10,50,50},8,8,RED);	
	}else{
		DrawRectangleRounded({720,10,50,50},8,8,BLACK);	
	}
	
	std::string formatted_time = this->t.get_formatted_runtime();

	DrawTextEx(font,"运行时间: ",{550,120.0f},30,0,BLACK);
	DrawTextEx(font,formatted_time.c_str(),{700,120.0f},30,0,BLACK);

	auto recent_think = t.get_recent_think_times();

	for(size_t i = 0;i < recent_think.size();++i){
		DrawTextEx(font,recent_think[i].c_str(),{550,150.0f + i * 30},24,0,BLACK);
	}

}

// 每次重新进入状态时运行这个函数
void GameState::Trigger(){
	// set state
	{
		this->board->ISMENUMODE = false;
		this->board->showTags = false;
		this->board->clearBoard();
		this->board->loadFromFen(szFenState);
		this->board->PreEvaluate();
		// todo print zobrist
	}

	// bind click event
	{
		mcs.bindOnClick([this](MouseStateType pre, Vector2 pos){
			this->mcs.dragMousePos = pos;
			BoardPos bp;	

			if(this->board->checkClickActivation(pos,bp)){
				this->mcs.dragStartPos = pos;
				if(pre == MouseStateType::Idle){

					this->board->activatedPos = bp;
					this->mcs.state = MouseStateType::CheckDragFromIdle;

				}else if(pre == MouseStateType::Selected){
					// 区分对选中的棋子进行单击或是拖动
					// 如果是同一个棋子，检测拖动或单击取消
					if(this->board->activatedPos.x == bp.x && this->board->activatedPos.y == bp.y){
						this->mcs.state = MouseStateType::checkDragFromSelected;
					}else{
						this->board->activatedPos = bp;	
						this->mcs.state = MouseStateType::CheckDragFromIdle;
					}
				}
			}else{
				// 判断移动位置是否合法
				if(pre == MouseStateType::Selected){
					this->board->preMove(false);	
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
				this->board->activatedPos = {-1,-1};
				this->mcs.state = MouseStateType::Idle;
			}else if(pre == MouseStateType::CheckDragFromIdle){
				this->mcs.state = MouseStateType::Selected;
			}else if(pre == MouseStateType::Dragging){
				// 判断移动位置是否合法
				this->board->preMove(true);
			}
		});
	}

	// bind button event
	{
		auto &bm = ButtonManager::getInstance();	
		Button changeButton({573,70},"交换");
		changeButton.bindOnClick([this](){
			DEBUG_("交换");
			if(this->board->isThinking.load()){
				DEBUG_("思考中，不可交换");
				return;	
			}
			this->board->changeSide();
		});

		Button undoButton({704,70},"悔棋");
		undoButton.bindOnClick([this](){
			if(this->board->gameOver)return;
			DEBUG_("悔棋");
			this->board->UndoLastMove();
		});

		Button showProtected({638,70},"显示");
		showProtected.bindOnClick([this](){
			this->board->showTags = !this->board->showTags;
		});

		Button menu({{140,288},"调整棋局"});
		menu.SetPadding(80.0f,0.0f);
		menu.SetFontSize(24);
		menu.SetSpacing(10);
		menu.bindOnClick([this](){
			this->onUnloadState();
		});

		bm.Add(std::move(changeButton));
		bm.Add(std::move(undoButton));
		bm.Add(std::move(showProtected));
		bm.Add(std::move(menu));
	}


	this->t.start();
	if(this->board->useAi &&
		((!this->board->isRedSide && this->board->isRedTurn) || 
			(this->board->isRedSide && !this->board->isRedTurn))){
		this->board->doSearch(true);
	}	
}

void GameState::onUnloadState(){

	DEBUG_("before reload ",this->board->disCardSearchRes);
	this->board->stopSearch(true);	
	DEBUG_("after reload ",this->board->disCardSearchRes);
	this->m_next = stateManager.makeState<MenuState>(
		this->stateManager,
		this->mcs,
		this->board
	);

	ButtonManager::getInstance().Clear();
	NotifyManager::getInstance().Clear();

	this->board->activatedPos = {-1,-1};
}

void GameState::Render([[maybe_unused]] float frameTime)
{
	bool isDragging = mcs.state == MouseStateType::Dragging;

	board->drawBackground();
	board->drawMarker(isDragging);
	board->drawPieces(isDragging);
	DrawTips();	
}

void GameState::Update( [[maybe_unused]] float frameTime,Vector2 pos,bool ishit)
{
	if(this->board->gameOver) return;

	if(!this->board->moveAnim.isAnimating){
		if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !ishit){
			mcs.handleEvent(MouseEventType::Click,pos);
		}

		if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
			mcs.handleEvent(MouseEventType::Down,pos);
		}

		if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
			mcs.handleEvent(MouseEventType::Release,pos);
		}
	}

	board->updateMoveAnimation(frameTime);
	if(this->board->isThinking.load() && limited_t != 0 && this->t.record() > limited_t){
		this->board->stopSearch();	
	}
}

void GameState::gameOver(){

}