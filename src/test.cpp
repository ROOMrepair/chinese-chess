#include <iostream>
#include <string>
#include <windows.h>

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

int main() {
	// std::cout << f(0,0,0) << std::endl;
	// std::cout << f(0,0,1) << std::endl;
	// std::cout << f(0,1,0) << std::endl;
	// std::cout << f(0,1,1) << std::endl;

	// std::cout << f(1,0,0) << std::endl;
	// std::cout << f(1,0,1) << std::endl;
	// std::cout << f(1,1,0) << std::endl;
	// std::cout << f(1,1,1) << std::endl;
	// std::cout << isPieceThisTurn(16) << std::endl;
	// std::cout << isPieceThisTurn(31) << std::endl;
	// std::cout << isPieceThisTurn(32) << std::endl;
	// std::cout << isPieceThisTurn(47) << std::endl;
	std::cout << (6 && 7) << std::endl;

	std::cout << (-1 && 7) << std::endl;
}