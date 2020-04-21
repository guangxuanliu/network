#pragma once
#include <iostream>
#include <string>

//the header of session class
typedef struct MESSAGE_HEADER_
{
	int type;
	int length;
}MESSAGE_HEADER;

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