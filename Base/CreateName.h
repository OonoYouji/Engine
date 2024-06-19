#pragma once

#include <iostream>
#include <typeinfo>
#include <string>


template <class T>
std::string CreateName(const T* t) {
	std::string name = typeid(*t).name();
	name = name.substr(std::string("class ").length());
	return name;
}