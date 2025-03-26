#include "GameState.hpp"

GameState::GameState(MouseClickState &m,bool isRedSide):
	mcs(m),board(isRedSide){
	mcs.bindOnClick([this](MouseStateType pre, Vector2 pos){
		this->board.dragMousePos = pos;
		BoardPos bp;	

		if(this->board.checkClickActivation(pos,bp)){

			this->board.dragStartPos = pos;
			if(pre == MouseStateType::Idle){

				this->board.activatedPos = bp;
				this->mcs.state = MouseStateType::CheckDragFromIdle;

			}else if(pre == MouseStateType::Selected){
				// 区分对选中的棋子进行单击或是拖动
				// 如果是同一个棋子，检测拖动或单击取消
				if(this->board.activatedPos.x == bp.x && this->board.activatedPos.y == bp.y){
					this->mcs.state = MouseStateType::checkDragFromSelected;
				}else{
					// 选中新棋子，类似 idle 操作 
					this->board.activatedPos = bp;	
					this->mcs.state = MouseStateType::CheckDragFromIdle;
				}
			}
		}else{
			// todo 检查点击是否是目标点位 	
		}
	});
	mcs.bindOnDrag([this](MouseStateType pre,Vector2 pos){
		this->board.dragMousePos = pos;
		if(pre == MouseStateType::CheckDragFromIdle || pre == MouseStateType::checkDragFromSelected){
			// down 状态不要设置 dragStartPos
			// this->board.dragStartPos = pos;
			// 超过一定时间才认为是拖动,但是太短时单击会被误认为拖动
			// 鼠标移动了则认为是拖动 
			if (this->board.dragStartPos.x != pos.x || this->board.dragStartPos.y != pos.y){
				this->mcs.state = MouseStateType::Dragging;
				return;
			}	
			if(GetTime() - this->mcs.lastClickTime < checkDragDeltaTime){
				return;
			}
			this->mcs.state = MouseStateType::Dragging;
		}else if(pre == MouseStateType::Dragging){
			this->board.handleDragEvent(pos);
		}
	});
	mcs.bindOnRelease([this](MouseStateType pre ,Vector2 pos){
		this->board.dragMousePos = pos;
		if(pre == MouseStateType::checkDragFromSelected){
			this->board.activatedPos = {-1,-1};
			this->mcs.state = MouseStateType::Idle;
		}else if(pre == MouseStateType::CheckDragFromIdle){
			this->mcs.state = MouseStateType::Selected;
		}else if(pre == MouseStateType::Dragging){
			// todo 此处需要判断释放位置是否合法
			this->mcs.state = MouseStateType::Selected;
		}
	});
}

void GameState::renderScene()
{
	// todo 将激活的棋子放在最上面
	bool isDragging = mcs.state == MouseStateType::Dragging;
	board.drawBackground();
	board.drawPieces(isDragging);
	board.drawMarker(isDragging);
}

void GameState::update()
{

	Vector2 pos = GetMousePosition();
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