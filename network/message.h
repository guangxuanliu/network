#pragma once
#include <iostream>
#include <string>

//������
struct Report
{
	unsigned int type;
	std::string message;
};


//̬����
struct Posture
{
	unsigned int type;
	int height;							
	float location;
	std::string message;
};