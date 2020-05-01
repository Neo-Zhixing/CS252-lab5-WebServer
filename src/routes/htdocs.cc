#include "http_messages.hh"
#include "socket.hh"
#include "misc.hh"
#include <fstream>
#include <experimental/filesystem>
// You may find implementing this function and using it in server.cc helpful

namespace fs = std::experimental::filesystem;

void serve_file(const fs::path& path, const Socket_t& sock) {
  if (fs::is_directory(path)) {
    fs::path newpath = path / fs::path("index.html");
    serve_file(newpath, sock);
    return;
  }
  HttpResponse response;
  std::ifstream input(path, std::ios_base::in | std::ios_base::binary );

  if (!input) {
    response.status_code = 404;
    response.message_body = "Can't get at all";
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
    response.headers["Content-Type"] = get_content_type(std::experimental::filesystem::absolute(path));
    sock->write(response.to_string());
    sock->write(buf, size);
    delete[] buf;
  } else {
    response.status_code = 404;

    sock->write(response.to_string());
  }
}

void serve_dir(const fs::path& path, const Socket_t& sock) {
  HttpResponse response;
  response.status_code = 200;
  response.message_body = "Serve Dir Test";
  sock->write(response.to_string());

  for (auto const & elem : fs::directory_iterator(path)) {
    sock->write(elem.to_string());
  }
}

void handle_htdocs(const HttpRequest& request, const Socket_t& sock) {
  auto querystr_pos = request.request_uri.find('?');
  std::string uri = request.request_uri;
  if (querystr_pos != std::string::npos) {
    uri = uri.substr(0, querystr_pos);
  }
  uri = "http-root-dir/htdocs" + uri;
  fs::path path(uri);

  if (uri.back() == '/') {
    serve_dir(path, sock);
  } else {
    serve_file(path, sock);
  }
}
