#pragma once

#include "raylib.h"
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

struct Notify
{
    std::string msg;
    // Vector2 position;
    Color color;

    int fontSize = DEFAULT_FONT_SIZE;
    float spacing = 0;
    float paddingLR = 50.0f;
    float paddingTB = 20.0f;

    float duration = 4.0f;
    float timer = 0;
    float alpha = 0;
    bool isActive = true;
    float fadingTime = 1.0f;


    void Draw(int id) const
    {
        if (!isActive)
            return;

        auto &asset = Asset::getInstance();
        auto &font = asset.fonts[0].font;
        const char *text = msg.c_str();
        auto textSize = MeasureTextEx(font, text , fontSize, spacing);

        //      top 20
        // l-25 content 25-r
        //      bottom 20
        float w = textSize.x + 2 * paddingLR;
        float h = textSize.y + 2 * paddingTB;
        float x = SCREEN_WIDTH - w - 20;
        float y = id * (h + 10) + 10;

        DrawRectangleRounded({x, y, w, h}, 0.1f, 4, {255, 255, 255, (unsigned char)(255 * alpha)});
        DrawTextEx(font,msg.c_str(),{x + (w - textSize.x) * 0.5f, y + (h - textSize.y) * 0.5f}, fontSize, spacing, { 0, 0, 0, (unsigned char)(255 * alpha) });
    }

    void Update(float deltaTime)
    {
        if (!isActive)
            return;
        timer += deltaTime;

        // if (timer < fadingTime)
        //     alpha = timer / fadingTime;
        if (timer > duration - fadingTime)
            alpha = 1.0f - (timer - (duration - fadingTime)) / fadingTime;
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

    ~NotifyManager(){}

    static NotifyManager &getInstance()
    {
        static NotifyManager instance;
        return instance;
    }

    int getSize() const { return size; };

    void Add(Notify&& notify)
    {
        if (notifications.size() >= size)
            return;
        notifications.push_back(std::move(notify));
    }

    void Update(float deltaTime)
    {
        for (auto &notif : notifications)
        {
            notif.Update(deltaTime);
        }

        notifications.erase(
            std::remove_if(
                notifications.begin(),
                notifications.end(),
                [](const Notify &n)
                { return !n.isActive; }),
            notifications.end());
    }

    void Draw()
    {
        for (int i = 0; i < notifications.size(); ++i)
        {
            const auto &notif = notifications[i];
            notif.Draw(i);
        }
    }
};