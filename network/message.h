#pragma once
#include <iostream>
#include <string>

//report class
struct Report
{
	unsigned int type;
	std::string message;
};


//posture class
struct Posture
{
	unsigned int type;
	int height;							
	float location;
	std::string message;
};