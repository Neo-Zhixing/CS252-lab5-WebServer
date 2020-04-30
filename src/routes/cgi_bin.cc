#include "http_messages.hh"
#include "socket.hh"
#include <iostream>
#include <map>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <sys/wait.h>

void handle_cgi_bin(const HttpRequest& request, const Socket_t& sock) {
  std::string original_querystring;
  if (request.method.compare("GET") == 0) {
    original_querystring = request.query;
  } else {
    original_querystring = request.message_body;
  }

  int ret = fork();
  if (ret == 0) {
    std::string program_name = "http-root-dir" + request.request_uri;
    std::cout << "About to run " << program_name << std::endl;

    // Remove everything after ?
    size_t index = program_name.find('?');
    if (index != std::string::npos)
      program_name.erase(index);


    // Is child
    setenv("REQUEST_METHOD", request.method.c_str(), 1);
    setenv("QUERY_STRING", original_querystring.c_str(), 1);
    int socketfd = sock->get_socket();
    dup2(socketfd, 1); // Redirect stdout to the pipe
    close(socketfd);
    std::cout << "HTTP/1.1 200 OK" << std::endl;
  
    char *argv[1];
    argv[0] = "dfasdf";
    
    execvp(argv[0], argv);
    std::cout << "Warning: something's wrong." << strerror(errno) << std::endl;
    _exit(1);
  } else {
    waitpid(ret, NULL, 0);
    std::cout << "Parent thread finished" << std::endl;
  }
}
