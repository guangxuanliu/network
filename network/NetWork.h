#pragma once
#include <asio-1.12.2/asio.hpp>
#include "message.h"
#include <vector>
#include <deque>
#include <memory>
#include <string>

using std::cout; 
using std::cerr;
using std::endl;
using std::error_code;
using std::vector;
using std::deque;
using std::string;
using std::shared_ptr;
using std::thread;

class session
{
public:
	typedef std::function<void(vector<char> &receiveBuffer)> ReadHandler;

public:
	session(asio::io_context &ioc)
		:strand_(ioc), socket_(ioc)
	{
		receiveHeaderBuffer_.resize(sizeof(MESSAGE_HEADER));
		writeHeaderBuffer_.resize(sizeof(MESSAGE_HEADER));
	}

	~session()
	{
		if (socket_.is_open())
		{
			socket_.close();
		}
	}

	void start(asio::ip::tcp::resolver::results_type endpoints)
	{
		asio::async_connect(socket_, endpoints,
			asio::bind_executor(strand_,
				std::bind(&session::handler_connect, this,
					std::placeholders::_1))
		);
	}

	void write(const vector<char> writeMsg);

	void registerReadHandler(ReadHandler handler)
	{
		readHandler_ = handler;
	}

private:
	void handler_connect(const std::error_code &ec);

	void read_header();

	void read_body(int bodyLength);

	void write_header();

	void write_body();

	void close_socket()
	{
		socket_.close();
	}

private:
	asio::io_context::strand strand_;
	asio::ip::tcp::socket socket_;
	vector<char> receiveHeaderBuffer_;
	vector<char> receiveBodyBuffer_;

	vector<char> writeHeaderBuffer_;
	deque<vector<char>> writeBuffer_;

	ReadHandler readHandler_ = nullptr;
};

class client 
{
public:
	static client &GetInstance();
	void connectToLogin(string &ip, unsigned short port);

	void setThreadNum(unsigned int num);

	void requestServerInfo();

private:
	client();
	~client();

	client(const client &c);
	const client &operator=(const client &c);

private:
	asio::io_context ioc_;
	std::shared_ptr<session> loginSession_;
	vector<thread> threads_;

	asio::executor_work_guard<asio::io_context::executor_type> workGuard_;
};


