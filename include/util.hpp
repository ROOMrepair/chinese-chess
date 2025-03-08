#pragma once

#include <string>
#include <algorithm>

inline void ltrim(std::string &s)
{
	s.erase(s.begin(),std::find_if(s.begin(),s.end(),[](unsigned char ch){
		return !std::isspace(ch);
	}));
}

inline void rtrim(std::string &s){
	s.erase(std::find_if(s.rbegin(),s.rend(),[](unsigned char ch){
		return !std::isspace(ch);
	}).base(),s.end());
}

inline void trim(std::string &s)
{
	ltrim(s);
	rtrim(s);
}

// inline void renderScale(Texture2D &t, bool isPadding = true){

// 	int tHeight = t.height;
// 	int tWidth = t.width;

// 	Vector2 pos;
	
// 	pos = isPadding ? Vector2{20,20} : Vector2{10,10};

//    	float padding = isPadding ? BOARD_PADDING_X : 10;

//     float scaleY = (float)(SCREEN_HEIGHT - padding * 2) / tHeight;
//     float scaleX = (float)(SCREEN_WIDTH - padding * 2) / tWidth;

// 	float scale = scaleY > scaleX ? scaleX : scaleY;

// 	int dstHeight = tHeight * scale; 
// 	int dstWidth = tWidth * scale;

// 	Rectangle src = {0, 0, (float)tWidth, (float)tHeight};

//     Rectangle dest = { pos.x, pos.y, (float)tWidth*scale, (float)tHeight*scale };

    // Vector2 origin = { 0.0f, 0.0f };


// 	Color cb = {171,93,22,255};
// 	if(isPadding){
// 		DrawTextureEx(t, pos, 0, scale, cb);
// 	}else{
// 		DrawTextureEx(t, pos, 0, scale, WHITE);
// 	}


// 	if(!isPadding){
		
		// DrawRectangleRounded(src, 20.0f, 10, YELLOW);

// 		Color c = {218,185,151,255};
//     	DrawRectangleRoundedLinesEx(dest, 0.1f, 10 ,10, c);
// 	}

        // 绘制边框

	// DrawTextureRec(t,src,{20,20},WHITE);
	
// }

