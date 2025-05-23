#pragma once

#include "raylib.h"
#include <string>
#include <vector>

class Counter {
private:
    Rectangle bounds;
    int value;
    int minValue;
    int maxValue;
    int step;
    
    Rectangle decreaseBtn;
    Rectangle increaseBtn;
    bool decreaseHover = false;
    bool increaseHover = false;
    
    int fontSize = 20;
    Color bgColor = LIGHTGRAY;
    Color btnColor = GRAY;
    Color btnHoverColor = DARKGRAY;
    Color textColor = BLACK;
    Color borderColor = DARKGRAY;

    std::function<void(int)> onValueChanged;

public:
    Counter(float x, float y, float width, float height, 
            int initialValue = 0, int minVal = 0, int maxVal = 100, int stepSize = 1)
        : bounds{x, y, width, height}, value(initialValue), 
          minValue(minVal), maxValue(maxVal), step(stepSize) {
        // 设置按钮区域
        float btnWidth = height; // 使按钮为正方形
        float btnHeight = height;

        decreaseBtn = {x, y, btnWidth, btnHeight};
        increaseBtn = {x + width - btnWidth, y, btnWidth, btnHeight};
    }

    void Update(Vector2 mousePos) {
        // 检查按钮悬停状态
        decreaseHover = CheckCollisionPointRec(mousePos, decreaseBtn);
        increaseHover = CheckCollisionPointRec(mousePos, increaseBtn);
        
        // 处理按钮点击
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (decreaseHover) {
                value = (value - step < minValue) ? minValue : value - step;
                onValueChanged(value);
            } else if (increaseHover) {
                value = (value + step > maxValue) ? maxValue : value + step;
                onValueChanged(value);
            }
        }
    }

    void Draw() {
        // 绘制背景
        DrawRectangleRec(bounds, bgColor);
        DrawRectangleLinesEx(bounds, 2, borderColor);
        
        // 绘制减少按钮
        DrawRectangleRec(decreaseBtn, decreaseHover ? btnHoverColor : btnColor);
        DrawText("-", decreaseBtn.x + decreaseBtn.width/2 - 5, 
                decreaseBtn.y + decreaseBtn.height/2 - 10, fontSize, textColor);
        
        // 绘制增加按钮
        DrawRectangleRec(increaseBtn, increaseHover ? btnHoverColor : btnColor);
        DrawText("+", increaseBtn.x + increaseBtn.width/2 - 5, 
                increaseBtn.y + increaseBtn.height/2 - 10, fontSize, textColor);
        
        // 绘制当前值
        std::string valText = std::to_string(value);
        float textWidth = MeasureText(valText.c_str(), fontSize);
        DrawText(valText.c_str(), 
                bounds.x + bounds.width/2 - textWidth/2,
                bounds.y + bounds.height/2 - fontSize/2,
                fontSize, textColor);
    }

    // 获取当前值
    int GetValue() const { return value; }
    
    // 设置当前值(会自动限制在min-max范围内)
    void SetValue(int newValue) {
        value = (newValue < minValue) ? minValue : 
               ((newValue > maxValue) ? maxValue : newValue);
    }
    
    // 设置范围
    void SetRange(int minVal, int maxVal) {
        minValue = minVal;
        maxValue = maxVal;
        SetValue(value); // 确保当前值在新范围内
    }
    
    // 设置步长
    void SetStep(int stepSize) { step = stepSize; }

    void BindValue(std::function<void(int)> callback) {
        onValueChanged = callback;
    }
};

class CounterManager {
private:
    std::vector<Counter> counters;
    CounterManager() {} // 私有构造函数
    
public:
    // 删除拷贝构造函数和赋值运算符
    CounterManager(const CounterManager&) = delete;
    CounterManager& operator=(const CounterManager&) = delete;
    
    // 获取单例实例
    static CounterManager& getInstance() {
        static CounterManager instance;
        return instance;
    }
    
	void Add(Counter &&c){
		counters.push_back(std::move(c));
	}
    
    void Update(Vector2 mousePos) {
        for (auto& counter : counters) {
            counter.Update(mousePos);
        }
    }
    
    void DrawAll() {
        for (auto& counter : counters) {
            counter.Draw();
        }
    }
    
    size_t GetCounterCount() const {
        return counters.size();
    }
    
    // 清除所有计数器
    void Clear() {
        counters.clear();
    }
    
    ~CounterManager() {
		std::cout << "CounterManager destroyed" << std::endl;
        Clear();
    }
};