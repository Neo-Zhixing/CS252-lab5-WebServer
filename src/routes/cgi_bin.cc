#include "http_messages.hh"
#include "socket.hh"
#include <iostream>
#include <map>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <sys/wait.h>
#include <dlfcn.h>
#include <stdlib.h>

void handle_cgi_bin_fork(std::string& program_name, std::string& original_querystring, int socketfd, const HttpRequest& request) {
  int ret = fork();
  if (ret == 0) {
    std::cout << "About to run " << program_name << std::endl;

    // Is child
    setenv("REQUEST_METHOD", request.method.c_str(), 1);
    setenv("QUERY_STRING", original_querystring.c_str(), 1);
    dup2(socketfd, 1); // Redirect stdout to the pipe
    close(socketfd);
    std::cout << "HTTP/1.1 200 OK" << std::endl;
  
    char *argv[2];
    argv[0] = const_cast<char *>(program_name.c_str());
    argv[1] = NULL;
    execvp(argv[0], argv);
    std::cout << "Warning: something's wrong." << strerror(errno) << std::endl;
    _exit(1);
  } else {
    waitpid(ret, NULL, 0);
    std::cout << "Parent thread finished" << std::endl;
  }
}


std::map<std::string, void*> dlmap;
void handle_loadable(std::string& program_name, std::string& original_querystring, int socketfd, const HttpRequest& request) {
  dprintf(socketfd, "HTTP/1.1 200 OK");
  size_t absolute_path_size = pathconf(".", _PC_PATH_MAX);
  char* absolute_path = (char*)malloc(absolute_path_size);
  absolute_path = getcwd(absolute_path, (size_t)absolute_path_size);
  strcat(absolute_path, "/");
  strcat(absolute_path, program_name.c_str());
  void *dlo;

  auto i = dlmap.find(program_name);
  if (i == dlmap.end()) {
    std::cout << "Loading lib " << absolute_path << std::endl;
    // Lib does not exist
    dlo = dlopen(absolute_path, RTLD_LAZY);
    if (!dlo) {
      std::cout << "Can't load " << absolute_path << " with error " << dlerror() << std::endl;
    }

    dlmap[program_name] = dlo;
  } else {
    std::cout << "Reusing lib " << program_name << std::endl;
    dlo = i->second;
  }
  
  void (*dls)(int, const char *);
  *(void **)(&dls) = dlsym(dlo, "httprun");
  char *error;
  if ((error = dlerror()) != NULL)  {
    std::cout << "Can't find httprun. " << absolute_path << "   " << error << std::endl;
    return;
  }

  
	(*dls)(socketfd, original_querystring.c_str());
}

void handle_cgi_bin(const HttpRequest& request, const Socket_t& sock) {
  std::string original_querystring;
  if (request.method.compare("GET") == 0) {
    original_querystring = request.query;
  } else {
    original_querystring = request.message_body;
  }
  std::string program_name = "http-root-dir" + request.request_uri;
  size_t index = program_name.find('?');
  if (index != std::string::npos)
    program_name.erase(index);
  
  std::string ending = ".so";
  if (0 == program_name.compare (program_name.length() - ending.length(), ending.length(), ending)) {
    // Load shared lib
    handle_loadable(program_name, original_querystring, sock->get_socket(), request);
  } else {
    // Run fork
    handle_cgi_bin_fork(program_name, original_querystring, sock->get_socket(), request);
  }


}
