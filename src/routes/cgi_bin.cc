#include "http_messages.hh"
#include "socket.hh"
#include <iostream>
// You could implement your logic for handling /cgi-bin requests here

void handle_cgi_bin(const HttpRequest& request, const Socket_t& sock) {
  HttpResponse response;
  response.http_version = request.http_version;
  response.status_code = 200;
  // TODO: Task 2.2

  std::cout << request.query << std::endl;

  sock->write(response.to_string());
}
