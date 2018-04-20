//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// updated to wark as very simple HTTPS server
//
// https://www.boost.org/doc/libs/1_63_0/doc/html/boost_asio/examples.html
// https://www.boost.org/doc/libs/1_39_0/doc/html/boost_asio/example/ssl/server.cpp
//
// certificate generation
// https://stackoverflow.com/questions/6452756/exception-running-boost-asio-ssl-example
//

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/asio/ssl.hpp>
#include <sstream>

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;
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


class session
{
public:
    session(boost::asio::io_service& io_service, boost::asio::ssl::context& context)
	: socket_(io_service, context)
    {
    }

  ssl_socket::lowest_layer_type& socket()
  {
    return socket_.lowest_layer();
  }

  void start()
  {
    socket_.async_handshake(boost::asio::ssl::stream_base::server, boost::bind(&session::handle_handshake, this, boost::asio::placeholders::error));
  }

  void handle_handshake(const boost::system::error_code& error)
  {
    if (!error)
    {
      socket_.async_read_some(boost::asio::buffer(data_, max_length),
          boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      delete this;
    }
  }

  void handle_read(const boost::system::error_code& error, size_t bytes_transferred)
  {
    if (!error)
    {
        std::cout << data_;
	std::string line(data_);
    	std::vector<std::string> strs;
    	boost::split(strs,line,boost::is_any_of(" "));
    	std::string doc = document(strs[0],strs[1]);

	std::stringstream ss;
        ss << "HTTP/1.0 200 OK\r\n";
	ss << "Content-Type: text/html\r\n";
	ss << "Content-Length: " << doc.length() << "\r\n";
	ss << "\r\n";
	ss << doc;
	response = ss.str();
	std::cout << response;

      boost::asio::async_write(socket_, boost::asio::buffer(response.c_str(), response.length()), boost::bind(&session::handle_write, this, boost::asio::placeholders::error));
    }
    else
    {
      delete this;
    }
  }

  void handle_write(const boost::system::error_code& error)
  {
    if (!error)
    {

      socket_.async_read_some(boost::asio::buffer(data_, max_length),
	    boost::bind(&session::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      delete this;
    }
  }

private:
  ssl_socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  std::string response;
};



class server
{
public:
  server(boost::asio::io_service& io_service, short port)
     : io_service_(io_service),
	acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
        context_(io_service, boost::asio::ssl::context::sslv23)
  {
    context_.set_options(boost::asio::ssl::context::default_workarounds
        | boost::asio::ssl::context::no_sslv2
//        | boost::asio::ssl::context::single_dh_use
    );
//    context_.set_password_callback(boost::bind(&server::get_password, this));
    context_.use_certificate_chain_file("server.crt");
    context_.use_private_key_file("server.key", boost::asio::ssl::context::pem);
//    context_.use_tmp_dh_file("dh512.pem");

    session* new_session = new session(io_service_, context_);
    acceptor_.async_accept(new_session->socket(), boost::bind(&server::handle_accept, this, new_session, boost::asio::placeholders::error));
  }

//  std::string get_password() const
//  {
//    return "test";
//  }

  void handle_accept(session* new_session, const boost::system::error_code& error)
  {
    if (!error) {
      new_session->start();
      new_session = new session(io_service_, context_);
      acceptor_.async_accept(new_session->socket(), boost::bind(&server::handle_accept, this, new_session, boost::asio::placeholders::error));
    } else {
      delete new_session;
    }
  }

private:
  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
  boost::asio::ssl::context context_;
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

