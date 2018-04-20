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
// Test with open ssl
// openssl s_client -connect 127.0.0.1:8443 -cert client.pem -key client.key -CAfile ca.pem
//
// Test with wget
// wget https://127.0.0.1:8443 --certificate client.pem --private-key client.key --ca-cert ca.pem --no-check-certificate

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
	std::string line(data_,bytes_transferred);
        std::cout << line;
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
//        | boost::asio::ssl::context::no_sslv2
//        | boost::asio::ssl::context::single_dh_use
    );
//    context_.set_password_callback(boost::bind(&server::get_password, this));
//    context_.use_tmp_dh_file("dh512.pem");
    context_.use_certificate_chain_file("server.pem");
    context_.use_private_key_file("server.key", boost::asio::ssl::context::pem);
    context_.load_verify_file("ca.pem");
    context_.set_verify_mode(boost::asio::ssl::verify_peer | boost::asio::ssl::verify_fail_if_no_peer_cert);
    context_.set_verify_callback(boost::bind(&server::verify_certificate, this, _1, _2));

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

  bool verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx)
  {
    // The verify callback can be used to check whether the certificate that is
    // being presented is valid for the peer. For example, RFC 2818 describes
    // the steps involved in doing this for HTTPS. Consult the OpenSSL
    // documentation for more details. Note that the callback is called once
    // for each certificate in the certificate chain, starting from the root
    // certificate authority.

    // In this example we will simply print the certificate's subject name.
    char subject_name[256];
    X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
    X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
    std::cout << "Verifying " << subject_name << "\n";

    return preverified;
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

