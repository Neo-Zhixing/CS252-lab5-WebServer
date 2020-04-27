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

  std::vector<std::string> querystrings;
  boost::split(querystrings, original_querystring, boost::is_any_of("&"));
  std::map<std::string, std::string> querymap;

  for (auto str : querystrings) {
    std::cout << str << "lalal" << std::endl;
    size_t pos = str.find('/');
    if (pos != std::string::npos)
    {
      querymap[str.substr(0, pos)] = s.substr(pos+1);
    } else {
      querymap[str] = "";
    }
  }

  std::cout << request.query << std::endl;

  sock->write(response.to_string());
}
