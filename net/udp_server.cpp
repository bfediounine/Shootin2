#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#define UDP_PORT	13

using boost::asio::ip::udp;

std::string make_daytime_string()
{
	std::time_t now = std::time(0);
	return std::ctime(&now);
}

class udp_server
{
public:
	udp_server(boost::asio::io_service &io_service)
		: socket_(io_service, udp::endpoint(udp::v4(), UDP_PORT))
	{
		start_receive();
	}

private:
	udp::socket socket_;
	udp::endpoint remote_endpoint_;
	boost::array<char, 1> recv_buffer_;

	void start_receive()
	{
		socket_.async_receive_from(
				boost::asio::buffer(recv_buffer_), remote_endpoint_,
				boost::bind(&udp_server::handle_receive, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
	}

	void handle_receive(const boost::system::error_code &error, std::size_t bytes_transferred)
	{
		if (!error || error == boost::asio::error::message_size) // ignore message size errors (recv_buffer_ defaulted to 1 byte
		{
			boost::shared_ptr<std::string> message(
					      new std::string(make_daytime_string()));

			socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
					      boost::bind(&udp_server::handle_send, this, message,
						      boost::asio::placeholders::error, 
						      boost::asio::placeholders::bytes_transferred));
		}
	}

	void handle_send(boost::shared_ptr<std::string>,
			const boost::system::error_code &error,
			std::size_t bytes_transferred)
	{
	}
};

int main()
{
	try
	{
		boost::asio::io_service io_service;
		udp_server server(io_service);
		io_service.run();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}


