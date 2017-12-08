#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

int main(int argc, char *argv[])
{
	try
	{
		if (argc != 2)
		{
			std::cerr << "Usage: udp_client <host>" << std::endl;
			return 1;
		}

		boost::asio::io_service io_service;
		
		// resolve endpoint	
		udp::resolver resolver(io_service);
		udp::resolver::query query(udp::v4(), argv[1], "daytime"); // TODO: basic query daytime (change...)
		udp::endpoint receiver_endpoint = *resolver.resolve(query);

		// open a socket
		udp::socket socket(io_service);
		socket.open(udp::v4());

		// initialize send/receive buffers
		boost::array<char, 1> send_buf = {{0}}; // TODO: expand size in actual application?
		boost::array<char, 128> recv_buf;
		udp::endpoint sender_endpoint; // defaulted to localhost???
		// connect send/receive buffers, perform transmission
		socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);
		size_t len = socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);
	
		std::cout.write(recv_buf.data(), len);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
