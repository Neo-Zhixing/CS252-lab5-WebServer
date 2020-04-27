#include "http_messages.hh"
#include "socket.hh"

// You could implement your logic for handling /cgi-bin requests here

void handle_cgi_bin(const HttpRequest& request, const Socket_t& sock) {
  HttpResponse response;
  response.http_version = request.http_version;
  // TODO: Task 2.2

  

  sock->write(response.to_string());
}
