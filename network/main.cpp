#include <iostream>
#include "NetWork.h"
#include "message.h"
#include <thread>
using namespace std;

//write repost class data
void writeReport(client &c)
{
	Report report;
	report.type = 3;
	report.message = "this is report class";

	vector<char> sendData(sizeof(unsigned) + report.message.length());

	memcpy(sendData.data(), &report.type, sizeof(report.type));
	memcpy(sendData.data() + sizeof(report.type), report.message.data(), report.message.size());

	c.write(sendData);
}

//write posture class data
void writePosture(client &c)
{
	Posture post;
	post.type = 4;
	post.height = 20;
	post.location = 3.14;
	post.message = "this is posture class";

	vector<char> sendData(sizeof(unsigned) + sizeof(post.type) + sizeof(post.height) + post.message.length());

	memcpy(sendData.data(), &post.type, sizeof(post.type));
	memcpy(sendData.data() + sizeof(post.type), &post.height, sizeof(post.height));
	memcpy(sendData.data() + sizeof(post.type) + sizeof(post.height), post.message.data(), post.message.length());
	c.write(sendData);
}

//callback of connection
void connectHandler(const std::error_code& err)
{
	if (!err)
	{
		cout << "connected to server" << endl;
	}
	else
	{
		cout << err.value() << " : " << err.message() << endl;
	}
}

//callback of write
void writeHandler(const std::error_code& err,size_t length)
{
	if (err)
	{
		cout << err.value() << " : " << err.message() << endl;
	}
}

//callback of read
void readHandler(std::vector<char> receiveBuffer, const std::error_code& err)
{
	if (!err)
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
	}
	else
	{
		cout << err.value() << " : " << err.message() << endl;
	}
}

int main()
{
	asio::io_context ioc;
	client c(ioc);

	string ip = "127.0.0.1";
	unsigned short port = 55555;

	c.registerConnectionHandler(connectHandler);
	c.registerWriteHandler(writeHandler);
	c.registerReadHandler(readHandler);
	
	c.connect(ip, port);

	thread t([&ioc]() {
		ioc.run();
	});

	//·¢ËÍ
	int i = 0;
	while (true)
	{
		this_thread::sleep_for(std::chrono::seconds(1));
		if (++i % 2)
			writeReport(c);
		else
			writePosture(c);
	}

	t.join();
	return 0;
}