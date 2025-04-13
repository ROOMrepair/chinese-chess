#pragma once

#include "raylib.h"
#include "util.hpp"
#include "functional"

// 这个state 应该反应的是 board 的状态
enum MouseStateType
{
	Idle,
	Selected,
	CheckDragFromIdle,
	checkDragFromSelected,
	Dragging,
};

// 鼠标动作 改变 board 状态
enum MouseEventType
{
	Click,
	Release,
	Down,
};

const double checkDragDeltaTime = 0.10;

class MouseClickState
{
public:

	using OnClick = std::function<void(MouseStateType,Vector2)>; 
	using OnDrag = std::function<void(MouseStateType,Vector2)>;
	using OnRelease = std::function<void(MouseStateType,Vector2)>;

	double lastClickTime;

	MouseStateType state;

	Vector2 dragStartPos;
	Vector2 dragMousePos;

	OnClick onClick;
	OnDrag onDrag;
	OnRelease onRelease;


	MouseClickState(){ state = Idle; lastClickTime = 0;}

	//? why use move
	void bindOnClick(OnClick cb)
	{
		onClick = std::move(cb);
	}
	void bindOnDrag(OnDrag cb)
	{
		onDrag = std::move(cb);
	}
	void bindOnRelease(OnRelease cb)
	{
		onRelease = std::move(cb);
	}

	void handleEvent(MouseEventType type,Vector2 pos)
	{
		MouseStateType pre = state;
		switch (type)
		{
		case Click:
			lastClickTime = GetTime();
			onClick(pre,pos);
			break;
		case Down:
			onDrag(pre,pos);
			break;
		case Release:
			onRelease(pre,pos);
			break;
		default:
			break;
		}
	}
};