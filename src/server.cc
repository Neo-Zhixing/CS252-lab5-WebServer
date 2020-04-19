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
  HttpRequest request = get_request(sock, request);
  // TODO: implement parsing HTTP requests
  // recommendation:
  // void parse_request(const Socket_t& sock, HttpRequest* const request);
  std::cout << "test" <<std::endl;
  request.print();

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

HttpRequest Server::get_request(const Socket_t& sock) {
    auto line = sock.readline();
    while ((auto pos = line.find(' ')) != std::string::npos) {
        auto token = line.substr(0, pos);
        std::cout << token << std::endl;
        line.erase(0, pos + delimiter.length());
    }
}
