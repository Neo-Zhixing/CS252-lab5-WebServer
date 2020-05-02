#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>

#include <dlfcn.h>
#include <stdlib.h>
#include <fcntl.h>

#include <boost/algorithm/string.hpp>

#include <experimental/filesystem>

#include <iostream>
#include <map>
#include <vector>


#include "http_messages.hh"
#include "socket.hh"

namespace fs = std::experimental::filesystem;

void handle_cgi_bin_fork(
  const std::string& program_name,
  const std::string& original_querystring,
  const Socket_t& sock,
  const HttpRequest& request) {
  int socketfd = sock->get_socket();
  int readfd, writefd;
  if (socketfd == -1) {
    int pipefd[2];
    pipe(pipefd);
    readfd = pipefd[0];
    writefd = pipefd[1];
  }

  int ret = fork();
  if (ret == 0) {
    std::cout << "About to run " << program_name << std::endl;

    // Is child
    setenv("REQUEST_METHOD", request.method.c_str(), 1);
    setenv("QUERY_STRING", original_querystring.c_str(), 1);
    if (socketfd == -1) {
      // Redirect stdout to the pipe
      close(readfd);
      dup2(writefd, 1);
      close(writefd);
    } else {
      dup2(socketfd, 1);  // Redirect stdout to the socket
      close(socketfd);
    }

    std::cout << "HTTP/1.1 200 OK" << std::endl;

    char *argv[2];
    argv[0] = const_cast<char *>(program_name.c_str());
    argv[1] = NULL;
    execvp(argv[0], argv);
    std::cout << "Warning: something's wrong." << strerror(errno) << std::endl;
    _exit(1);
  } else {
    if (socketfd == -1) {
      close(writefd);
      char buf[512];
      int ret;
      while ((ret = read(readfd, buf, 512)) > 0) {
        sock->write(buf, ret);
      }
      std::cout << "read pipe ended";
      close(readfd);
    }
    waitpid(ret, NULL, 0);
    std::cout << "Parent thread finished" << std::endl;
  }
}


std::map<std::string, void*> dlmap;
void handle_loadable(
  const std::string& program_name,
  const std::string& original_querystring,
  const Socket_t& sock,
  const HttpRequest& request) {
  int socketfd = sock->get_socket();
  bool buffered = false;

  if (socketfd == -1) {
    socketfd = shm_open("shm-name", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    buffered = true;
  }

  dprintf(socketfd, "HTTP/1.1 200 OK");
  fs::path path(program_name);
  std::string absolute_path = fs::absolute(path);
  void *dlo;

  auto i = dlmap.find(program_name);
  if (i == dlmap.end()) {
    std::cout << "Loading lib " << absolute_path << std::endl;
    // Lib does not exist
    dlo = dlopen(absolute_path.c_str(), RTLD_LAZY);
    if (!dlo) {
      std::cout << "Can't load " << absolute_path << " with error " << dlerror() << std::endl;
    }

    dlmap[program_name] = dlo;
  } else {
    std::cout << "Reusing lib " << program_name << std::endl;
    dlo = i->second;
  }

  void (*dls)(int, const char *);
  *reinterpret_cast<void **>(&dls) = dlsym(dlo, "httprun");
  char *error;
  if ((error = dlerror()) != NULL)  {
    std::cout << "Can't find httprun. " << absolute_path << "   " << error << std::endl;
    return;
  }

  (*dls)(socketfd, original_querystring.c_str());


  if (buffered) {
    lseek(socketfd, 0, SEEK_SET);
    char buf[512];
    int ret;
    while ((ret = read(socketfd, buf, 512)) > 0) {
      sock->write(buf, ret);
    }
    close(socketfd);
  }
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
  if (0 == program_name.compare(
    program_name.length() - ending.length(), ending.length(), ending)) {
    // Load shared lib
    handle_loadable(program_name, original_querystring, sock, request);
  } else {
    // Run fork
    handle_cgi_bin_fork(program_name, original_querystring, sock, request);
  }
}
