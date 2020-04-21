#include <iostream>
#include "NetWork.h"
#include "message.h"
#include <thread>
using namespace std;

int main()
{

	string ip = "127.0.0.1";
	unsigned short port = 55555;

	client::GetInstance().setThreadNum(thread::hardware_concurrency() - 1);

	client::GetInstance().connectToLogin(ip, port);

	client::GetInstance().requestServerInfo();

	while (true)
	{
		this_thread::sleep_for(std::chrono::milliseconds(1000));
		client::GetInstance().requestServerInfo();
	}

	return 0;
}