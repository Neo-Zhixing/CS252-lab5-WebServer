#include "http_messages.hh"
#include "socket.hh"
#include <iostream>
// You could implement your logic for handling /cgi-bin requests here

void handle_cgi_bin(const HttpRequest& request, const Socket_t& sock) {
  std::cout << "Trying.....to parse cgi_bin" << std::endl;
  HttpResponse response;
  response.http_version = request.http_version;
  // TODO: Task 2.2

  std::cout << request.query << std::endl;

  sock->write(response.to_string());
}
