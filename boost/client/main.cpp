
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>


std::string get(std::string host, int port)
{
    std::stringstream ss;
    ss << "GET / HTTP/1.1\r\n";
    ss << "Host: " << host << ":" << port << "\r\n";
    ss << "\r\n";
    std::string req = ss.str();

    boost::asio::io_service ios;
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(host), port);
    boost::asio::ip::tcp::socket socket(ios);
    socket.connect(endpoint);

    boost::system::error_code error;
    socket.write_some(boost::asio::buffer(req.c_str(),req.length()), error);

    char buf[4096] = {0};
    int len = socket.read_some(boost::asio::buffer(buf,sizeof(buf)), error);

    socket.close();

    std::string reply = std::string(buf,len);
    int pos = reply.find("\r\n\r\n");
    std::cout << "pos:" << pos << "\n";
    if(pos != std::string::npos) {
	std::string headers = reply.substr(0,pos);
	std::cout << "headers:\n" << headers << "\n";
	reply = reply.substr(pos);
    }
    return reply;
}



int main()
{
    std::string reply = get("127.0.0.1", 8080);
    std::cout << reply << "\n";
    return 0;
}
