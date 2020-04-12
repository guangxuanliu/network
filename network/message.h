#pragma once
#include <iostream>
#include <string>

//报告类
struct Report
{
	unsigned int type;
	std::string message;
};


//态势类
struct Posture
{
	unsigned int type;
	int height;							
	float location;
	std::string message;
};