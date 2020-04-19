#include "http_messages.hh"
#include "socket.hh"
#include <fstream>

// You may find implementing this function and using it in server.cc helpful

void handle_htdocs(const HttpRequest& request, const Socket_t& sock) {
  HttpResponse response;
  response.http_version = request.http_version;
  // TODO: Task 1.3
  // Get the request URI, verify the file exists and serve it
  std::cout << "http-root-dir/htdocs" + request.request_uri << std::endl;

  std::ifstream input("http-root-dir/htdocs" + request.request_uri, std::ios_base::in | std::ios_base::binary );

  if (!input) {
    response.status_code = 404;
    sock->write(response.to_string());
    return;
  }

  input.seekg(0, std::ios::end);
  size_t size = input.tellg();
  if (size > 0) {
    std::cout << "file size:  a " << size << std::endl;
    char * buf = new char[size];
    input.seekg(0, std::ios::beg);
    
    input.read(buf, size);
    response.status_code = 200;
    response.headers["Content-Length"] = std::to_string(size);

    sock->write(response.to_string());
    sock->write(buf, size);
    delete[] buf;
  } else {
    response.headers["Content-Length"] = "0";

    sock->write(response.to_string());
  }
  
}
