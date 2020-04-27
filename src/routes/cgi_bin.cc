#include "http_messages.hh"
#include "socket.hh"
#include <iostream>
// You could implement your logic for handling /cgi-bin requests here

void handle_cgi_bin(const HttpRequest& request, const Socket_t& sock) {
  HttpResponse response;
  response.http_version = request.http_version;
  // TODO: Task 2.2

  std::cout << response.querystring << std::endl;

  sock->write(response.to_string());
}
