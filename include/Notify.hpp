#pragma once

#include "raylib.h"
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

struct Notify
{
    std::string text;
    Vector2 position;
    Color color;
    float duration = 3;
    int fontSize = 20;
    float timer = 0;
    float alpha = 0; // 透明度（用于淡入淡出）
    bool isActive = true;
    float fadingTime = 1.0f;
     
    void Draw(int id) const{
        if(!isActive) return;
        float spacing = 2.0f;
        auto textSize = MeasureTextEx(GetFontDefault(),text.c_str(),fontSize,spacing);

        //      top 20
        // l-25 content 25-r
        //      bottom 20
        float x = SCREEN_WIDTH - textSize.x - 50 - 10; 
        float y = id * (textSize.y + 40 + 10);
        float w = textSize.x + 50;
        float h = textSize.y + 40;

        DrawRectangle(x,y,w,h,{242,213,171,(unsigned char)(255 * alpha)});
	    DrawRectangleRoundedLinesEx({x+5,y+5,w-5,h-5}, 0.05f, 8, 8, {143,129,112,(unsigned char)(255 * alpha)});
        DrawTextEx(GetFontDefault(),text.c_str(),{x + 25,y + 20},fontSize,spacing,BLACK);
    }

    void Update(float deltaTime){
        if(!isActive) return;
        timer += deltaTime;
        
        if (timer < 0.3f)
            alpha = timer / 0.3f;
        else if (timer > duration - 0.3f)
            alpha = 1.0f - (timer - (duration - 0.3f)) / 0.3f;
        else
            alpha = 1.0f;

        if (timer >= duration)
            isActive = false;
    }
};

class NotifyManager
{
private:
    std::vector<Notify> notifications;
    int size;
    NotifyManager() : size(10) {};

public:
    NotifyManager(const NotifyManager &) = delete;
    NotifyManager &operator=(const NotifyManager &) = delete;

    static NotifyManager &getInstance()
    {
        static NotifyManager instance;
        return instance;
    }

    int getSize() const { return size; };

    void Add(Notify notify)
    {
        if (notifications.size() >= size) return;
        notifications.push_back(std::move(notify));
    }

    void Update(float deltaTime)
    {
        for (auto &notif :notifications)
        {
            notif.Update(deltaTime);
        }

        // 移除已失效的通知
        notifications.erase(
            std::remove_if(
                notifications.begin(),
                notifications.end(),
                [](const Notify&n)
                { return !n.isActive; }),
            notifications.end());
    }

    void Draw()
    {
        for (int i = 0; i < notifications.size();++i)
        {
            const auto &notif = notifications[i];
            notif.Draw(i);
        }
    }
};