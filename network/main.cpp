#include <iostream>
using namespace std;
#include "NetWork.h"
#include "message.h"
#include <thread>

client c;

void writeReport()
{
	//·¢ËÍ
	Report report;
	report.type = 3;
	report.message = "this is report class";

	vector<char> sendData(sizeof(unsigned) + report.message.length());

	memcpy(sendData.data(), &report.type, sizeof(report.type));
	memcpy(sendData.data() + sizeof(report.type), report.message.data(), report.message.size());

	c.write(vector<char>(sendData));
}

void connectHandler(const std::error_code& err)
{
	if (err)
	{
		cout << err.value() << " : " << err.message() << endl;
	}
	else
	{
		cout << "connected to server" << endl;
		c.read();
	}
}

void writeHandler(const std::error_code& err,size_t length)
{
	if (err)
	{
		cout << err.value() << " : " << err.message() << endl;
	}
}

void readHandler(std::vector<char> receiveBuffer, const std::error_code& err, size_t length)
{
	if (err)
	{
		cout << err.value() << " : " << err.message() << endl;
	}
	else
	{
		int type = 0;

		memcpy(&type, receiveBuffer.data(), sizeof(type));
		switch (type)
		{
		case 3:
			std::cout << "received: report class" << std::endl;
			break;

		case 4:
			std::cout << "received: post class" << std::endl;
			break;

		default:
			break;
		}
		writeReport();
	}
}

int main()
{

	string ip = "127.0.0.1";
	unsigned short port = 55555;

	c.registerConnectionHandler(connectHandler);
	c.registerWriteHandler(writeHandler);
	c.registerReadHandler(readHandler);

	c.connect(ip, port);

	//·¢ËÍ
	writeReport();

	return 0;
}