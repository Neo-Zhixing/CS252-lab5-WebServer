#include "http_messages.hh"

// If you use the recommended route handling scheme, you will probably want a default handler

void handle_default(const HttpRequest& request, const Socket_t& sock) {
  HttpResponse response;
  response.http_version = request.http_version;
  response.status_code = 404;
  sock->send(response.to_string());
}

