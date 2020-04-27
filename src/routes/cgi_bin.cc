#include "http_messages.hh"
#include "socket.hh"
#include <iostream>
#include <map>
#include <boost/algorithm/string.hpp> 

void handle_cgi_bin(const HttpRequest& request, const Socket_t& sock) {
  HttpResponse response;
  response.http_version = request.http_version;
  response.status_code = 200;
  std::cout << request.query << std::endl;

  sock->write(response.to_string());
}
