#include "NetWork.h"

void session::write(const vector<char> writeMsg)
{
	asio::post(strand_,
		[this, writeMsg]() {
		bool writeInProcess = !writeBuffer_.empty();
		writeBuffer_.push_back(std::move(writeMsg));
		if (!writeInProcess)
		{
			write_header();
		}
	});
}

void session::handler_connect(const std::error_code &ec)
{
	if (!ec)
	{
		error_code set_option_err;
		asio::ip::tcp::no_delay no_delay(true);
		socket_.set_option(no_delay, set_option_err);

		if (!set_option_err)
		{
			read_header();
		}
	}
	else
	{
		cerr << __FUNCDNAME__ << " : " << __LINE__
			<< ", code:" << ec.value()
			<< ", message:" << ec.message()
			<< endl;
	}
}

void session::read_header()
{
	asio::async_read(socket_, asio::buffer(receiveHeaderBuffer_, 
		receiveHeaderBuffer_.size()),
		[this](const error_code &ec, size_t recvLengh) {

		cout << "read header " << std::this_thread::get_id() << endl;

		if (!ec && recvLengh == receiveHeaderBuffer_.size())
		{
			MESSAGE_HEADER header;
			memcpy(&header, receiveHeaderBuffer_.data(), receiveHeaderBuffer_.size());
			read_body(header.length);
		}
		else
		{
			cerr << __FUNCDNAME__ << " : " << __LINE__
				<< ", code:" << ec.value()
				<< ", message:" << ec.message()
				<< ", length:" << recvLengh
				<< endl;

			close_socket();
		}
	});
}

void session::read_body(int bodyLength)
{
	if (bodyLength > 0)
	{

		receiveBodyBuffer_.clear();
		receiveBodyBuffer_.resize(bodyLength);

		asio::async_read(socket_, asio::buffer(receiveBodyBuffer_, bodyLength),
			[this](const error_code &ec,size_t recvLengh) {

			cout << "read body " << std::this_thread::get_id() << endl;

			if (!ec && recvLengh == receiveBodyBuffer_.size())
			{
				read_header();
				readHandler_ ? readHandler_(receiveBodyBuffer_) : "nothing";
			}
			else
			{
				cerr << __FUNCDNAME__ << " : " << __LINE__
					<< ", code:" << ec.value()
					<< ", message:" << ec.message()
					<< ", length:" << recvLengh
					<< endl;

				close_socket();
			}

		});
	}
}

void session::write_header()
{

	//header
	MESSAGE_HEADER header;
	header.type = 0x21;
	header.length = writeBuffer_.front().size();

	memcpy(writeHeaderBuffer_.data(), &header, sizeof(MESSAGE_HEADER));

	asio::async_write(socket_, asio::buffer(writeHeaderBuffer_, writeHeaderBuffer_.size()),
		[this](const error_code &ec,size_t length) {

		cout << "write header " << endl;

		if (!ec && length == sizeof(MESSAGE_HEADER))
		{
			write_body();
		}
		else
		{
			cerr << __FUNCDNAME__ << " : " << __LINE__
				<< ", code:" << ec.value()
				<< ", message:" << ec.message()
				<< ", length:" << length
				<< endl;

			close_socket();
		}
	});
}

void session::write_body()
{
	asio::async_write(socket_, asio::buffer(writeBuffer_.front().data(),
		writeBuffer_.front().size()),
		[this](const error_code &ec, size_t length) {

		cout << "write body " << endl;

		if (!ec && length == writeBuffer_.front().size())
		{
			writeBuffer_.pop_front();
			if (!writeBuffer_.empty())
			{
				write_header();
			}
		}
		else
		{
			cerr << __FUNCDNAME__ << " : " << __LINE__
				<< ", code:" << ec.value()
				<< ", message:" << ec.message()
				<< ", length:" << length
				<< endl;

			close_socket();
		}
	});
}

client &client::GetInstance()
{
	static client c;
	return c;
}

void client::connectToLogin(string &ip, unsigned short port)
{
	asio::ip::tcp::resolver r(ioc_);
	auto endpoints = r.resolve(ip, std::to_string(port));
	loginSession_->start(endpoints);
}

void client::setThreadNum(unsigned int num)
{
	if (num > thread::hardware_concurrency())
	{
		num = thread::hardware_concurrency();
	}

	while (num-- > 0)
	{
		threads_.push_back(thread([this]() {
			ioc_.run();
		}));
	}

}

void client::requestServerInfo()
{
	unsigned short port = 55555;
	string ip = "127.0.0.1";
	vector<char> buf(sizeof(port) + ip.size());
	memcpy(buf.data(), &port, sizeof(port));
	memcpy(buf.data() + sizeof(port), ip.data(), ip.size());

	loginSession_->write(buf);
}

client::client()
	:workGuard_(ioc_.get_executor())
{
	loginSession_ = std::make_shared<session>(ioc_);

	std::cout << "Sorry, we'll never reach this!\n";
}

client::~client()
{
	cout << "~client" << endl;

	for (auto &t : threads_)
	{
		if (t.joinable())
			t.join();
	}

	cout << "before stop" << endl;

	ioc_.stop();
}

