#pragma once

#include <functional>
#include <vector>
#include <iostream>
#include <string>
#include "raylib.h"

struct Button
{

	using OnClick = std::function<void()>;

	Vector2 position;
	std::string msg;

	Vector2 textSize;

	OnClick onClick;
	bool isDown = false;

	int fontSize = 16;
	float spacing = 0;
	float paddingLR = 20.0f;
	float paddingTB = 20.0f;
	float w, h;

	Button(const Vector2 &pos, const std::string &txt = "") : msg(txt), position(pos)
	{

		auto &asset = Asset::getInstance();
		auto &font = asset.fonts[0].font;
		const char *text = msg.c_str();

		textSize = MeasureTextEx(font, text, fontSize, spacing);
		w = textSize.x + 2 * paddingLR;
		h = textSize.y + 2 * paddingTB;
	}

	void bindOnClick(OnClick cb)
	{
		onClick = std::move(cb);
	}

	void Draw() const
	{
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
	}

	bool CheckClick(Vector2 pos)
	{
		return CheckCollisionPointRec(pos, {position.x, position.y, w, h});
	}
};

class ButtonManager
{
private:
	ButtonManager() {}

public:
	static ButtonManager &getInstance()
	{
		static ButtonManager instance;
		return instance;
	}

	ButtonManager(const ButtonManager &) = delete;
	ButtonManager &operator=(const ButtonManager &) = delete;
	std::vector<Button> buttonList;

	void Update(Vector2 pos)
	{
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
			for (auto &button : buttonList){
				if (button.CheckClick(pos)){
					button.isDown = true;
					if (button.onClick)
					{
						button.onClick();
					}
				}else{
					button.isDown = false; 
				}
			}
		}

		if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
			for (auto &button : buttonList){
				button.isDown = false; 
			}
		}
	}

	void Add(Button &&button)
	{
		buttonList.push_back(std::move(button));
	}

	void Draw()
	{
		for (int i = 0; i < buttonList.size(); ++i)
		{
			const auto &button = buttonList[i];
			button.Draw();
		}
	}

	~ButtonManager()
	{
		std::cout << "ButtonManager destroyed" << std::endl;
		buttonList.clear();
	}
};