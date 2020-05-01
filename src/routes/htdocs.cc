#include "http_messages.hh"
#include "socket.hh"
#include "misc.hh"
#include <fstream>
#include <experimental/filesystem>
// You may find implementing this function and using it in server.cc helpful




void handle_htdocs(const HttpRequest& request, const Socket_t& sock) {
  HttpResponse response;
  response.http_version = request.http_version;
  // TODO: Task 1.3
  // Get the request URI, verify the file exists and serve it

  auto querystr_pos = request.request_uri.find('?');
  std::string uri = request.request_uri;
  if (querystr_pos != std::string::npos) {
    uri = uri.substr(0, querystr_pos);
  }
  if (uri.back() == '/') {
    uri = uri + "index.html";
  }
  uri = "http-root-dir/htdocs" + uri;

  std::ifstream input(uri, std::ios_base::in | std::ios_base::binary );

  if (!input) {
    response.status_code = 404;
    sock->write(response.to_string());
    return;
  }

  input.seekg(0, std::ios::end);
  std::streampos size = input.tellg();
  if (!input) {
    response.status_code = 404;
    sock->write(response.to_string());
    return;
  }
  if (size > 0) {
    char * buf = new char[size];
    input.seekg(0, std::ios::beg);
    
    input.read(buf, size);
    response.status_code = 200;
    response.headers["Content-Length"] = std::to_string(size);
    response.headers["Content-Type"] = get_content_type(uri);
    sock->write(response.to_string());
    sock->write(buf, size);
    delete[] buf;
  } else {
    response.status_code = 404;

    sock->write(response.to_string());
  }
  
}
