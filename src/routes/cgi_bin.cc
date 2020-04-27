#include "http_messages.hh"
#include "socket.hh"
#include <iostream>
#include <map>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <sys/wait.h>

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
    std::string program_name = request.request_uri;
    //program_name.erase(0, 9);
    size_t index = program_name.find('?');
    if (index != std::string::npos)
      program_name.erase(index);

    char *argv[1];
    argv[0] = const_cast<char *>(program_name.c_str());
    std::cout << "About to run " << program_name << std::endl;
    execvp(argv[0], argv);
  } else {
    waitpid(ret, NULL, 0);
  }
  
  sock->write(response.to_string());
}
