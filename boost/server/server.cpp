//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// updated to wark as very simple HTTP server
//

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>

using boost::asio::ip::tcp;

std::string document(const std::string verb,const std::string url)
{
    std::stringstream ss;
    ss << "<html>\n";
    ss << "<body>\n";
    ss << "Hello! " << verb << " " << url << "\n";
    ss << "</body>\n";
    ss << "</html>\n";
    return ss.str();
}


class session : public std::enable_shared_from_this<session>
{
public:
  session(tcp::socket socket) : socket_(std::move(socket))
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
            std::cout << data_;

	    std::string line(data_);
    	    std::vector<std::string> strs;
    	    boost::split(strs,line,boost::is_any_of(" "));
    	    std::string doc = document(strs[0],strs[1]);

	    std::stringstream ss;
            ss << "HTTP/1.0 200 OK\n";
	    ss << "Content-Type: text/html\n";
	    ss << "Content-Length: " << doc.length() << "\n";
	    ss << "\n";
	    ss << doc;
	    response = ss.str();
	    
	    std::cout << response;
	    
            do_write();
          }
        });
  }

  void do_write()
  {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(response.c_str(), response.length()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            do_read();
          }
        });
  }

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  std::string response;
};



class server
{
public:
  server(boost::asio::io_service& io_service, short port) : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),  socket_(io_service)
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
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;
    server s(io_service, std::atoi(argv[1]));
    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

