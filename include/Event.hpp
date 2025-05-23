#pragma once

#include <mutex>

#include "raylib.h"
#include "util.hpp"
#include "functional"

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

	Vector2 dragStartPos; // 触发点击状态的时候鼠标位置
	Vector2 dragMousePos; // 实际的鼠标位置

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

class Timer{
public:
	Timer(){}
	~Timer(){}
	size_t _size = 10;

	std::chrono::time_point<std::chrono::high_resolution_clock> M_start_point_;
	std::vector<std::pair<uint64_t,uint64_t>> recorder;

	std::mutex mtx;
	std::chrono::time_point<std::chrono::high_resolution_clock> record_start_;
	
	void start(){
		M_start_point_ = std::chrono::high_resolution_clock::now();
	}

	uint64_t get_elapsed_ms () const
	{
		auto now = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(now - M_start_point_).count();
	}
	
	std::string get_formatted_runtime() {
		auto ms = get_elapsed_ms();
		int seconds = (ms / 1000) % 60;
		int minutes = (ms / 1000) / 60;
		char buffer[16];
		snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);
		return std::string(buffer);
	}

	void start_record() {
		record_start_ = std::chrono::high_resolution_clock::now();
	}

	void end_record() {
		auto end = std::chrono::high_resolution_clock::now();
		uint64_t start_ms = std::chrono::duration_cast<std::chrono::milliseconds>(record_start_ - M_start_point_).count();
		uint64_t end_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - M_start_point_).count();

		std::lock_guard<std::mutex> lock(mtx);
		recorder.push_back({start_ms, end_ms});
		if (recorder.size() > _size) recorder.erase(recorder.begin()); // 保留最近 5 次
	}
	
	uint64_t record() {
		auto end = std::chrono::high_resolution_clock::now();
		uint64_t start_ms = std::chrono::duration_cast<std::chrono::milliseconds>(record_start_ - M_start_point_).count();
		uint64_t end_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - M_start_point_).count();

		return end_ms - start_ms;
	}


	std::vector<std::string> get_recent_think_times() {
		std::vector<std::string> results;

		std::lock_guard<std::mutex> lck (mtx);
		for (const auto& [start, end] : recorder) {
			results.push_back("+ " + std::to_string(end - start) + " ms");
		}
		return results;
	}

};