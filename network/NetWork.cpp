#include "NetWork.h"

client::client(asio::io_context& ioc)
	:io_context_(ioc),socket_(ioc)
{
}

void client::connect(std::string ip, unsigned short port)
{
	asio::ip::tcp::resolver r(io_context_);
	auto endpoints = r.resolve(ip, std::to_string(port));

	asio::async_connect(socket_, endpoints,
		//std::bind(connectionHandler_,std::placeholders::_1)
		[this](const std::error_code err,asio::ip::tcp::endpoint end) {
		if (!err)
		{
			do_read_header();
			connectionHandler_(err);
		}
	}
	);
}

void client::write(std::vector<char> buffer)
{

	sendBuffer = buffer;
	char sz[4] = { 0 };
	int length = sendBuffer.size();
	memcpy(sz, &length, sizeof(length));
	sendBuffer.insert(sendBuffer.begin(), sz, sz + sizeof(sz));

	asio::async_write(socket_, asio::buffer(sendBuffer),
		std::bind(writeHandler_, std::placeholders::_1, std::placeholders::_2));
}

void client::read()
{
	//∂¡»°Õ∑
	do_read_header();
}

client::~client()
{
}

void client::do_read_header()
{
	asio::async_read(socket_,
		asio::buffer(header, 4),
		[this](std::error_code ec, std::size_t /*length*/)
	{
		if (!ec)
		{
			int length = 0;
			memcpy(&length, header, sizeof(length));
			std::cout << "length:" << length << std::endl;
			do_read_body(length);
		}
		else
		{
			socket_.close();
		}
	});
}

void client::do_read_body(int length)
{
	receiveBuffer.resize(length);
	asio::async_read(socket_,
		asio::buffer(receiveBuffer,length),
		/*std::bind(readHandler_, receiveBuffer,std::placeholders::_1,std::placeholders::_2)*/
		[this](std::error_code ec, std::size_t length)
	{
		if (!ec)
		{
			std::cout << "received body length: " << length << std::endl;
			readHandler_(receiveBuffer,ec);
			do_read_header();
		}
		else
		{
			socket_.close();
		}
	}
	);
}


