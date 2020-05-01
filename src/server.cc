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
#include <thread>
#include <unistd.h>
#include <chrono>
#include <limits>

#include "server.hh"
#include "http_messages.hh"
#include "errors.hh"
#include "misc.hh"
#include "routes.hh"

auto startup_time = std::chrono::steady_clock::now();
unsigned int total_requests = 0;
unsigned int min_servetime = std::numeric_limits<unsigned int>::max();
unsigned int max_servetime = 0;


Server::Server(SocketAcceptor const& acceptor) : _acceptor(acceptor) { }

void Server::run_linear() const {
  while (1) {
    Socket_t sock = _acceptor.accept_connection();
    handle(std::move(sock));
  }
}

void Server::run_fork() const {
  while (1) {
    Socket_t sock = _acceptor.accept_connection();
    int ret = fork();
    if (ret == 0) {
      // Child process
      handle(std::move(sock));
      exit(0);
    }
  }
}

void Server::run_thread() const {
  while (1) {
    Socket_t sock = _acceptor.accept_connection();
    std::thread thread_obj = std::thread(&Server::handle, this, std::move(sock));
    thread_obj.detach();
  }
}

void Server::run_thread_pool_worker() const {
  while(true) {
    Socket_t sock = _acceptor.accept_connection();
    handle(std::move(sock));
  }
}

void Server::run_thread_pool(const int num_threads) const {
  for (int i = 0; i < num_threads; i++) {
    std::thread thread_obj = std::thread(&Server::run_thread_pool_worker, this);
    thread_obj.detach();
  }
  run_thread_pool_worker();
}

// example route map. you could loop through these routes and find the first route which
// matches the prefix and call the corresponding handler. You are free to implement
// the different routes however you please

std::vector<Route_t> route_map = {
  std::make_pair("/cgi-bin", handle_cgi_bin),
  std::make_pair("/stats", handle_stats),
  std::make_pair("/", handle_htdocs),
  std::make_pair("", handle_default)
};



void Server::handle(const Socket_t sock) const {
  auto start_time = std::chrono::steady_clock::now();
  total_requests += 1;
  HttpRequest request;
  get_request(sock, request);
  if (request.request_uri.length() == 0) {
    return;
  }
  // TODO: implement parsing HTTP requests
  // recommendation:
  // void parse_request(const Socket_t& sock, HttpRequest* const request);
  request.print();


  if (!authenticate(request, sock)) {
    return;
  }

  for (auto pair : route_map) {
    if (request.request_uri.compare(0, pair.first.length(), pair.first) == 0) {
      pair.second(request, sock);
      break;
    }
  }
  auto end_time = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
  if (duration < min_servetime) {
    min_servetime = duration;
  }
  if (duration > max_servetime) {
    max_servetime = duration;
  }
}

bool Server::authenticate(const HttpRequest& req, const Socket_t& sock) const {
  auto auth = req.headers.find("Authorization");
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
    return;
    // throw std::invalid_argument("Unexpected end of line on the first line");
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
    req.headers[line.substr(0, pos)] = line.substr(pos+2, line.size() - pos - 4);
  }
}
