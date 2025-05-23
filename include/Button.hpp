#pragma once

#include <functional>
#include <vector>
#include <iostream>
#include <string>
#include "raylib.h"

enum ButtonType {
	NONE = 0,
	TEXT_RECT,
	CIRCLE
};

struct Button
{
	using OnClick = std::function<void()>;

	Vector2 position;
	std::string msg;
	ButtonType btype = ButtonType::NONE;

	OnClick onClick;
	bool isDown = false;

	int fontSize = 16;
	float spacing = 0;
	float paddingLR = 10.0f;
	float paddingTB = 5.0f;

	Vector2 textSize;
	float w, h; // 宽高，默认矩形按钮
	float r;
	Color c;

	void SetPadding(float lr = -1.0f, float tb = -1.0f){
		if(lr >= 0)paddingLR = lr;					
		if(tb >= 0)paddingTB = tb;							
		Caculate();
	}

	void SetFontSize(int fs = 0){
		if(fs > 0)fontSize = fs;
		Caculate();
	}

	void SetSpacing(float sp = -1.0f){
		if(sp >= 0)spacing = sp;
		Caculate();
	}

	void Caculate(){
		auto &asset = Asset::getInstance();
		auto &font = asset.fonts[0].font;
		const char *text = msg.c_str();

		textSize = MeasureTextEx(font, text, fontSize, spacing);
		w = textSize.x + 2 * paddingLR;
		h = textSize.y + 2 * paddingTB;
	}

	Button(const Vector2 &pos, const std::string &txt = "") : position(pos),msg(txt)
	{
		btype = ButtonType::TEXT_RECT;
		Caculate();
	}
	
	// circle type
	Button(const Vector2 &pos,float radius,Color color):position(pos),r(radius),c(color){
		btype = ButtonType::CIRCLE;
	}

	void bindOnClick(OnClick cb)
	{
		onClick = std::move(cb);
	}

	void Draw() const
	{
		if(btype == ButtonType::TEXT_RECT){
			auto &asset = Asset::getInstance();
			auto &font = asset.fonts[0].font;

			if (isDown)
			{
				DrawRectangleRounded({position.x, position.y, w, h}, 8, 4, BLACK);
				DrawTextEx(font, msg.c_str(), {position.x + (w - textSize.x) * 0.5f, position.y + (h - textSize.y) * 0.5f}, fontSize, spacing, WHITE);
			}
			else
			{
				DrawRectangleRounded({position.x, position.y, w, h}, 8, 4, WHITE);
				DrawTextEx(font, msg.c_str(), {position.x + (w - textSize.x) * 0.5f, position.y + (h - textSize.y) * 0.5f}, fontSize, spacing, BLACK);
			}
		}else if(btype == ButtonType::CIRCLE){
			if(isDown){
				DrawCircleV(position,r,c);
				DrawRing(position, r, r + 5, 0, 360, 36, BLUE);
			}else{
				DrawCircleV(position,r,c);				
			}
		}
	}

	bool CheckClick(Vector2 pos)
	{
		if(btype == ButtonType::TEXT_RECT){
			return CheckCollisionPointRec(pos, {position.x, position.y, w, h});
		}else if(btype == ButtonType::CIRCLE){
			return CheckCollisionPointCircle(pos,position,r);
		}
		return false;
	}
};

class ButtonManager
{
private:
	ButtonManager() {}

	std::unordered_map<int, std::vector<size_t>> buttonGroups; // 组ID到按钮索引的映射
    std::unordered_map<size_t, int> buttonToGroup; // 按钮索引到组ID的映射
    // size_t selectedButtonInGroup = -1; // 当前选中的按钮索引

public:
	static ButtonManager &getInstance()
	{
		static ButtonManager instance;
		return instance;
	}

	ButtonManager(const ButtonManager &) = delete;
	ButtonManager &operator=(const ButtonManager &) = delete;
	std::vector<Button> buttonList;
	size_t lastClickIndex = 0;

	// 添加互斥按钮
	void AddToMutexGroup(Button&& button, int groupId,bool setDefault = false) {
       	size_t index = buttonList.size();
       	Add(std::move(button));

		// 如果是第一个则激活
	 	if (setDefault) {
			buttonList.back().isDown = true;
			if(buttonList.back().onClick){
				buttonList.back().onClick();
			}
       		buttonGroups[groupId].push_back(index);
       		buttonToGroup[index] = groupId;
		}else{
       		buttonGroups[groupId].push_back(index);
       		buttonToGroup[index] = groupId;
		}
   	}

	bool Update(Vector2 pos)
	{
		bool isHitButton = false;
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
			for (size_t i = 0; i < buttonList.size();++i){
				if (buttonList[i].CheckClick(pos)){
					isHitButton = true;
					auto it = buttonToGroup.find(i);
					if(it != buttonToGroup.end()){
						int groupId = it->second;	
						for(auto idx:buttonGroups[groupId]){
							buttonList[idx].isDown = false;	
						}
					}
					buttonList[i].isDown = true;
					lastClickIndex = i;
					if (buttonList[i].onClick)
					{
						buttonList[i].onClick();
					}
				}
			}
			if(!isHitButton){
				lastClickIndex = buttonList.size();
			}
		}

		if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
			for (size_t i = 0; i < buttonList.size();++i){
				if(i == lastClickIndex){
					// 如果不是互斥形的，松开
					auto it = buttonToGroup.find(i);
					if(it == buttonToGroup.end()){
						buttonList[i].isDown = false; 
					}
				}
			}
		}

		return isHitButton;
	}

	void Add(Button &&button)
	{
		buttonList.push_back(std::move(button));
	}

	void Draw()
	{
		for (size_t i = 0; i < buttonList.size(); ++i)
		{
			const auto &button = buttonList[i];
			button.Draw();
		}
	}
	
	void Clear(){
		buttonGroups.clear();
        buttonToGroup.clear();
        buttonList.clear();
        lastClickIndex = 0;
	}

	~ButtonManager()
	{
		std::cout << "ButtonManager destroyed" << std::endl;
		buttonList.clear();
	}
};