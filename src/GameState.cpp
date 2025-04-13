#include "GameState.hpp"

GameState::GameState(StateManager &sm,bool isRedSide):
	State(sm),	
	board(mcs,isRedSide){

	// init board 
	if (!board.loadFromFen(cszStartFen))
    {
        throw std::runtime_error("Failed to load from FEN");
    }

	// init buttons 
	auto &bm = ButtonManager::getInstance();	
	Button changeButton({700,100},"交换");
	changeButton.bindOnClick([this](){
		DEBUG_("交换");
		this->board.changeSide();
	});

	Button undoButton({700,200},"悔棋");
	undoButton.bindOnClick([this](){
		DEBUG_("悔棋");
	});

	bm.Add(std::move(changeButton));
	bm.Add(std::move(undoButton));


	// bind mouse event
	mcs.bindOnClick([this](MouseStateType pre, Vector2 pos){
		this->mcs.dragMousePos = pos;
		BoardPos bp;	

		if(this->board.checkClickActivation(pos,bp)){
			this->mcs.dragStartPos = pos;
			if(pre == MouseStateType::Idle){

				this->board.activatedPos = bp;
				this->mcs.state = MouseStateType::CheckDragFromIdle;

			}else if(pre == MouseStateType::Selected){
				// 区分对选中的棋子进行单击或是拖动
				// 如果是同一个棋子，检测拖动或单击取消
				if(this->board.activatedPos.x == bp.x && this->board.activatedPos.y == bp.y){
					this->mcs.state = MouseStateType::checkDragFromSelected;
				}else{
					this->board.activatedPos = bp;	
					this->mcs.state = MouseStateType::CheckDragFromIdle;
				}
			}
		}else{
			// 判断移动位置是否合法
			this->board.preMove(false);	
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
			this->board.activatedPos = {-1,-1};
			this->mcs.state = MouseStateType::Idle;
		}else if(pre == MouseStateType::CheckDragFromIdle){
			this->mcs.state = MouseStateType::Selected;
		}else if(pre == MouseStateType::Dragging){
			// 判断移动位置是否合法
			this->board.preMove(true);
		}
	});
}

void GameState::DrawTips(){
    auto &asset = Asset::getInstance();
	auto &font = asset.fonts[0].font;
	// todo
    DrawTextEx(font,"现在轮到:",{600,20},DEFAULT_FONT_SIZE,0,BLACK);
	if(board.isRedTurn){
		DrawRectangleRounded({720,10,50,50},8,8,RED);	
	}else{
		DrawRectangleRounded({720,10,50,50},8,8,BLACK);	
	}
}

void GameState::Render(float frameTime)
{
	// todo 将激活的棋子放在最上面
	bool isDragging = mcs.state == MouseStateType::Dragging;

	board.drawBackground();
	board.updateMoveAnimation(frameTime);

	board.drawMarker(isDragging);
	board.drawPieces(isDragging);

	DrawTips();	
}

void GameState::Update( [[maybe_unused]] float deltaTime,Vector2 pos)
{
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