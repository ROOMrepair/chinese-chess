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


int main() {
	int a = 5;
	change(a);	
    return 0;
}