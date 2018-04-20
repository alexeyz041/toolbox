// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
// https://www.boost.org/doc/libs/1_63_0/doc/html/boost_asio/example/cpp03/ssl/client.cpp
//

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

enum { max_length = 1024 };

class client
{
public:
  client(boost::asio::io_service& io_service, boost::asio::ssl::context& context, boost::asio::ip::tcp::resolver::iterator endpoint_iterator,
      const char *host, int port)
    : socket_(io_service, context), host_(host), port_(port)
  {
    socket_.set_verify_callback(boost::bind(&client::verify_certificate, this, _1, _2));
    boost::asio::async_connect(socket_.lowest_layer(), endpoint_iterator,  boost::bind(&client::handle_connect, this,  boost::asio::placeholders::error));
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
    std::cout << "Verifying " << subject_name << " preverified: " << preverified << "\n";

    const EVP_MD *fp_type = EVP_sha1();
    unsigned int fp_size = 0;
    unsigned char fp[EVP_MAX_MD_SIZE];
    X509_digest(cert, fp_type, fp, &fp_size);
    for(int i=0; i < fp_size; i++) {
	std::cout << boost::format("%02x:") % (int)fp[i];
    }
    std::cout << "\n";

    return preverified;
  }

  void handle_connect(const boost::system::error_code& error)
  {
    if (!error)
    {
      socket_.async_handshake(boost::asio::ssl::stream_base::client,  boost::bind(&client::handle_handshake, this, boost::asio::placeholders::error));
    }
    else
    {
      std::cout << "Connect failed: " << error.message() << "\n";
    }
  }

  void handle_handshake(const boost::system::error_code& error)
  {
    if (!error)
    {
	std::stringstream ss;
	ss << "GET / HTTP/1.1\r\n";
	ss << "Host: " << host_ << ":" << port_ << "\r\n";
	ss << "\r\n";
	std::string request_ = ss.str();

      boost::asio::async_write(socket_,  boost::asio::buffer(request_.c_str(), request_.length()),
          boost::bind(&client::handle_write, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      std::cout << "Handshake failed: " << error.message() << "\n";
    }
  }

  void handle_write(const boost::system::error_code& error, size_t bytes_transferred)
  {
    if (!error)
    {
      socket_.async_read_some(boost::asio::buffer(reply_, max_length),
          boost::bind(&client::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      std::cout << "Write failed: " << error.message() << "\n";
    }
  }

  void handle_read(const boost::system::error_code& error, size_t bytes_transferred)
  {
    if (!error)
    {
      std::string reply(reply_,bytes_transferred);
      std::cout << "Reply: " << reply << "\n";
    }
    else
    {
      std::cout << "Read failed: " << error.message() << "\n";
    }
  }

private:
  boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
  std::string request_;
  char reply_[max_length];
  std::string host_;
  int port_;
};


int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: client <host> <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::resolver::query query(argv[1], argv[2]);
    boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
    boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);

    ctx.set_options(boost::asio::ssl::context::default_workarounds);
    ctx.set_verify_mode(boost::asio::ssl::verify_peer);
    ctx.use_certificate_chain_file("client.pem");
    ctx.use_private_key_file("client.key", boost::asio::ssl::context::pem);
    ctx.load_verify_file("ca.pem");

    client c(io_service, ctx, iterator, argv[1], atoi(argv[2]));
    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

