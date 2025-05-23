#include <iostream>
#include <string>
// #include <windows.h>
#include <bitset>
#include <cstdint>
#include "raylib.h"
using namespace std;

template <typename T>
void change(T && right_value) {
    right_value = 8;
}

// 编译不通过,当使用泛型的时候,T会推导传递参数类型，此时change(a)
// a 被推导成 int&,即对 int& 的右值引用
// void change(int&& right_value) {
//     right_value = 8;
// }


// 函数形参类型	传入实参类型	推导后的实际函数参数类型
// T&			左引用			T&
// T&			右引用			T&
// T&&			左引用			T&
// T&&			右引用			T&&

bool f(bool a, bool b,bool c) {
	return !(a ^ b ^ c);
}


bool isRedTurn = false;
inline bool isPieceThisTurn(int pt){
std::cout << "pt: " <<(pt & (1 << 5)) << std::endl;
  return ((pt & (1 << 5)) >> 5) ^ isRedTurn; 
}

inline int PopCnt16(uint16_t w) {
  int n;
  n = ((w >> 1) & 0x5555) + (w & 0x5555); // 步骤1：每2位统计1的个数
  cout << "w>>1 & 0x5555 " << bitset<16>((w >> 1) & 0x5555)<< endl;
  cout << "w & 0x5555 " << bitset<16>(w & 0x5555) << endl;
  cout << "n1: " << bitset<16>(n) << endl;
  n = ((n >> 2) & 0x3333) + (n & 0x3333);  // 步骤2：每4位统计1的个数
  cout << "n>>2 & 0x3333 " << bitset<16>((n >> 2) & 0x3333)<< endl;
  cout << "n & 0x3333 " << bitset<16>(n & 0x3333) << endl;
  cout << "n2: " << bitset<16>(n) << endl;
  n = ((n >> 4) & 0x0f0f) + (n & 0x0f0f);   // 步骤3：每8位统计1的个数
  cout << "n>>4 & 0x0f0f " << bitset<16>((n >> 4) & 0x0f0f)<< endl;
  cout << "n & 0x0f0f " << bitset<16>(n & 0x0f0f) << endl;
  cout << "n3: " << bitset<16>(n) << endl;
  
  cout << "n >> 8 " << bitset<16>(n >> 8) << endl;
  cout << "n & 0x00ff " << bitset<16>(n & 0x00ff) << endl;
  return (n >> 8) + (n & 0x00ff);           // 步骤4：合并高低8位
}

int main() {

	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	SetTargetFPS(60);

	InitWindow(800,600, "Hello Raylib");

	while (!WindowShouldClose())
	{
    BeginDrawing();
    DrawText("Hello World", 10, 10, 20, BLACK);
		EndDrawing();
	}

	CloseWindow();
  
	return 0;
}