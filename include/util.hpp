#pragma once

#include <string>
#include <iostream>
#include <algorithm>
#include <cstdarg> 
#include <cassert>

#ifndef DEBUG_
	#define DEBUG_(...) DebugPrint(__VA_ARGS__)
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

template <typename T>
void debugPrint(const T& t){
	std::cout << t;
}

template <typename T,typename... Args>
void debugPrint(const T& t,const Args&... rest){
	std::cout << t << " ";
	debugPrint(rest...);
}

template <typename... Args>
inline void DebugPrint(const Args& ...args){
	std::cout << "\033[32m";
	debugPrint(args...);
	std::cout << "\033[0m" << std::endl;
}