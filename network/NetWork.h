#pragma once
#include <asio-1.12.2/asio.hpp>
#include "message.h"
#include <vector>
#include <memory>

class client 
{
public:
	typedef std::function<void(const std::error_code& err)> ConnectionHandler;
	typedef std::function<void(const std::error_code& err, size_t length)> WriteHandler;
	typedef std::function<void(std::vector<char> receiveBuffer,const std::error_code& err)> ReadHandler;

public:
	client(asio::io_context& ioc);

	void connect(std::string ip,unsigned short port);

	void write(std::vector<char> buffer);

	void read();

	void registerConnectionHandler(const ConnectionHandler& handler)
	{
		connectionHandler_ = handler;
	}

	void registerWriteHandler(const WriteHandler& handler)
	{
		writeHandler_ = handler;
	}

	void registerReadHandler(const ReadHandler& handler)
	{
		readHandler_ = handler;
	}

	~client();
	
private:
	void do_read_header();
	void do_read_body(int length);

private:
	asio::io_context& io_context_;
	asio::ip::tcp::socket socket_;

	ConnectionHandler connectionHandler_;
	WriteHandler writeHandler_;
	ReadHandler readHandler_;

	char header[4];
	std::vector<char> sendBuffer;
	std::vector<char> receiveBuffer;
};


