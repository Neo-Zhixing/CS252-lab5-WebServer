#include "http_messages.hh"
#include "socket.hh"
#include <iostream>
#include <map>
#include <vector>
#include <boost/algorithm/string.hpp> 

void handle_cgi_bin(const HttpRequest& request, const Socket_t& sock) {
  HttpResponse response;
  response.http_version = request.http_version;
  response.status_code = 200;

  std::string original_querystring;
  if (request.method.compare("GET") == 0) {
    original_querystring = request.query;
  } else {
    original_querystring = request.message_body;
  }

  int ret = fork();
  if (ret == 0) {
    // Is child
    setenv("REQUEST_METHOD", request.method.c_str(), 1);
    setenv("QUERY_STRING", original_querystring.c_str(), 1);
    std::cout << "About to run " << request.request_uri << std::endl;
  } else {
    // Is parent
  }
  
  sock->write(response.to_string());
}
