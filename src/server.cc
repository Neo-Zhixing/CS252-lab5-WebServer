/**
 * This file contains the primary logic for your server. It is responsible for
 * handling socket communication - parsing HTTP requests and sending HTTP responses
 * to the client. 
 */

#include <functional>
#include <iostream>
#include <sstream>
#include <vector>
#include <tuple>

#include "server.hh"
#include "http_messages.hh"
#include "errors.hh"
#include "misc.hh"
#include "routes.hh"

Server::Server(SocketAcceptor const& acceptor) : _acceptor(acceptor) { }

void Server::run_linear() const {
  while (1) {
    Socket_t sock = _acceptor.accept_connection();
    handle(sock);
  }
}

void Server::run_fork() const {
  // TODO: Task 1.4
}

void Server::run_thread() const {
  // TODO: Task 1.4
}

void Server::run_thread_pool(const int num_threads) const {
  // TODO: Task 1.4
}

// example route map. you could loop through these routes and find the first route which
// matches the prefix and call the corresponding handler. You are free to implement
// the different routes however you please
/*
std::vector<Route_t> route_map = {
  std::make_pair("/cgi-bin", handle_cgi_bin),
  std::make_pair("/", handle_htdocs),
  std::make_pair("", handle_default)
};
*/

void Server::handle(const Socket_t& sock) const {
  HttpRequest request;
  get_request(sock, request);
  // TODO: implement parsing HTTP requests
  // recommendation:
  // void parse_request(const Socket_t& sock, HttpRequest* const request);
  request.print();


  if (!authenticate(request, sock)) {
    return;
  }

  HttpResponse resp;
  // TODO: Make a response for the HTTP request
  resp.http_version = "HTTP/1.1";
  resp.status_code = 200;
  resp.reason_phrase = "OK";
  resp.headers["Connection"] = "close";
  resp.message_body = "Fuck CS252!";
  std::cout << resp.to_string() << std::endl;
  sock->write(resp.to_string());
}

bool Server::authenticate(const HttpRequest& req, const Socket_t& sock) const {
  auto auth = req.headers.find("Authorization");
  if (auth != req.headers.end()) {
    std::cout << "Auth header is " << auth->second << std::endl;
  }
  if (auth == req.headers.end() || auth->second.compare("Basic emhhbjMwODg6cFd4MEtSM0w=") != 0) {
    // No Authorization header present
    HttpResponse resp;
    resp.status_code = 401;
    resp.headers["WWW-Authenticate"] = "Basic realm=\"" + realm + "\"";
    sock->write(resp.to_string());
    return false;
  }
  return true;
}

void Server::get_request(const Socket_t& sock, HttpRequest& req) const {
  auto line = sock->readline();
  std::cout << line << std::endl;
  std::size_t pos;
  int state = 0;
  while ((pos = line.find(' ')) != std::string::npos) {
      auto token = line.substr(0, pos);
      switch (state) {
        case 0: req.method = token;break;
        case 1: {
          req.request_uri = token;
          std::size_t question_mark_location = token.find('?');
          if (question_mark_location != std::string::npos){
            req.query = token.substr(question_mark_location+ 1);
          }
          break;
        }
        default: throw std::invalid_argument("Extra token on the first line"); 
      }
      line.erase(0, pos + 1);
      state++;
  }
  if (state != 2) {
    throw std::invalid_argument("Unexpected end of line on the first line");
  }
  req.http_version = line.substr(0, line.size()-2);


  // Start to parse headers
  while (true) {
    line = sock->readline();
    pos = line.find(':');
    if (pos == std::string::npos) {
      // No longer a header
      break;
    }
    req.headers[line.substr(0, pos)] = line.substr(pos+2);
  }
}
