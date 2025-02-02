#include <chrono>

#include "http_messages.hh"
#include "socket.hh"
#include "server.hh"

// If you use the recommended route handling scheme, you will probably want a default handler

void handle_stats(const HttpRequest& request, const Socket_t& sock) {
  HttpResponse response;
  response.http_version = request.http_version;
  response.status_code = 200;
  std::stringstream buf;
  auto now = std::chrono::steady_clock::now();
  auto uptime = std::chrono::duration_cast<std::chrono::seconds>(now - startup_time).count();
  buf << "<!DOCTYPE html>\
  <html>\
    <body>\
      <ul>\
        <li>Name: Zhixing Zhang</li>\
        <li>Uptime: " << uptime << "\
        <li>Total requests served: " << total_requests << "</li>\
        <li>Max request time(ms): " << max_servetime << "</li>\
        <li>Min request time(ms): " << min_servetime << "</li>\
      </ul>\
    </body>\
  </html>";
  response.message_body = buf.str();
  sock->write(response.to_string());
}

