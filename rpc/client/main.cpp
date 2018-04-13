/*
g++ main.cpp -lboost_system -lboost_thread -lpthread -o main
*/

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>

void print(uint8_t *buf,int len)
{
	for(int i=0; i < len; i++) {
		printf("%02x ",buf[i]);
	}
	printf("\n");
}

void send_something(std::string host, int port)
{
    boost::asio::io_service ios;
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(host), port);
    boost::asio::ip::tcp::socket socket(ios);
    socket.connect(endpoint);

    for(int j=0; j < 5; j++) {
    	boost::array<uint8_t, 128> buf;
    	int len = 10;
    	for(int i=0; i < len; i++) {
    		buf[i+2] = i+j;
    	}
    	buf[0] = len & 0xff;
    	buf[1] = (len >> 8) & 0xff;

    	boost::system::error_code error;
        socket.write_some(boost::asio::buffer(buf,len+2), error);

        int len1 = socket.read_some(boost::asio::buffer(buf,128), error);
        print(buf.elems,len1);
     }

    usleep(1000*1000);

    socket.close();
}

int main()
{
    send_something("127.0.0.1", 8000);
    return 0;
}
