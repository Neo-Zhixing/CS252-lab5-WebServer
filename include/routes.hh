#ifndef INCLUDE_ROUTES_HH_
#define INCLUDE_ROUTES_HH_

#include <functional>
#include <string>
#include <utility>

#include "http_messages.hh"
#include "socket.hh"
// You may find handling routes by using these functions (that you need to implement) helpful

void handle_cgi_bin(const HttpRequest& request, const Socket_t& sock);
void handle_htdocs(const HttpRequest& request, const Socket_t& sock);
void handle_default(const HttpRequest& request, const Socket_t& sock);
void handle_stats(const HttpRequest& request, const Socket_t& sock);

typedef std::function<void(const HttpRequest&, const Socket_t&)> Route_Handler_t;

typedef std::pair<const std::string, const Route_Handler_t> Route_t;

#endif  // INCLUDE_ROUTES_HH_
