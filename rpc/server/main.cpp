
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>

#include "buffer.h"
#include "server.h"

using boost::asio::ip::tcp;

enum { max_length = 1024 };

void print(uint8_t *buf,int len)
{
	for(int i=0; i < len; i++) {
		printf("%02x ",buf[i]);
	}
	printf("\n");
}


void process(uint8_t *buf,int length,uint8_t *resp,int *resp_len)
{
/*
	Buffer args(buf,length);
	Buffer res(resp,max_length);
	invoke(args,res);
	*resp_len = res.length();
*/
	print(buf,length);
	memcpy(resp,buf,length);
	*resp_len = length;
}


class session : public std::enable_shared_from_this<session>
{
public:
  session(tcp::socket socket) : socket_(std::move(socket)), length_(0),response_length_(0)
  {
  }

  void start()
  {
    do_read();
  }

private:
  void do_read()
  {
    auto self(shared_from_this());

    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
          if (!ec)
          {
            length_ = data_[0] + (data_[1] << 8);
            process(data_+2,length_,response_+2,&response_length_);
            response_[0] = response_length_ & 0xff;
            response_[1] = (response_length_ >> 8) & 0xff;
            do_write();
          }
        });
  }

  void do_write()
  {
    auto self(shared_from_this());

    boost::asio::async_write(socket_, boost::asio::buffer(response_, response_length_+2),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            do_read();
          }
        });
  }

  tcp::socket socket_;

  uint8_t data_[max_length];
  int length_;
  uint8_t response_[max_length];
  int response_length_;
};



class server
{
public:
  server(boost::asio::io_service& io_service, short port)
		: acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),  socket_(io_service)
  {
    do_accept();
  }

private:
  void do_accept()
  {
    acceptor_.async_accept(socket_,
        [this](boost::system::error_code ec)
        {
          if (!ec)
          {
            std::make_shared<session>(std::move(socket_))->start();
          }

          do_accept();
        });
  }

  tcp::acceptor acceptor_;
  tcp::socket socket_;
};



int main(int argc, char* argv[])
{
  try {
    boost::asio::io_service io_service;
    server s(io_service, 8000);
    io_service.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

