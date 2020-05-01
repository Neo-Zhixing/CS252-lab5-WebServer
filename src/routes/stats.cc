#include "http_messages.hh"
#include "socket.hh"
#include "server.hh"

// If you use the recommended route handling scheme, you will probably want a default handler

void handle_stats(const HttpRequest& request, const Socket_t& sock) {
  HttpResponse response;
  response.http_version = request.http_version;
  response.status_code = 200;
  response.message_body = "\
  <!DOCTYPE html>\
  <html>\
    <body>\
      <ul>\
        <li>Name: Zhixing Zhang</li>\
        <li>Total requests served: " + std::string(total_requests)+ "</li>\
      </ul>\
    </body>\
  </html>";
  sock->write(response.to_string());
}

