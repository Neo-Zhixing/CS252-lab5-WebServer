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
  int fdpipe[2];
  pipe(fdpipe);

  int writefd = fdpipe[1];
  int readfd = fdpipe[0];

  int ret = fork();
  if (ret == 0) {
    close(readfd);
    // Is child
    dup2(writefd, 1); // Redirect stdout to the pipe
    setenv("REQUEST_METHOD", request.method.c_str(), 1);
    setenv("QUERY_STRING", original_querystring.c_str(), 1);
    std::string program_name = "http-root-dir/" + request.request_uri;
    //program_name.erase(0, 9);
    size_t index = program_name.find('?');
    if (index != std::string::npos)
      program_name.erase(index);

    char *argv[1];
    argv[0] = const_cast<char *>(program_name.c_str());
    std::cout << "About to run " << program_name << std::endl;
    execvp(argv[0], argv);
    _exit(1);
  } else {
    // Is parent
    close(writefd);
    char buf[128];
    while(true){
      int len = read(readfd, buf, 128);
      if (len <= 0) {
        break;
      }
      std::cout << "Read " << buf << std::endl;
      sock->write(buf, len);
    }
    waitpid(ret, NULL, 0);
  }
  
  sock->write(response.to_string());
}
