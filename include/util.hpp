#pragma once

#include <string>
#include <iostream>
#include <algorithm>
#include <cstdarg> 
#include <cassert>

#ifndef DEBUG_
	#define DEBUG_(...) debugPrint(__VA_ARGS__)
#endif


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

inline void debugPrint(const char* format, ...) {
    va_list args;
    va_start(args, format);
	std::cout << "\033[32m";
    vprintf(format, args); // 使用 vprintf 格式化输出
	std::cout << "\033[0m";
    va_end(args);
}


