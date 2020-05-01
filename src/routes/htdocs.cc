#include "http_messages.hh"
#include "socket.hh"
#include "misc.hh"
#include <fstream>
#include <experimental/filesystem>
// You may find implementing this function and using it in server.cc helpful


void serve_file(const std::experimental::filesystem::path& path, const Socket_t& sock) {
  HttpResponse response;
  std::ifstream input(path, std::ios_base::in | std::ios_base::binary );

  if (!input) {
    std::cout << "Going with filename " << path.filename() << std::endl;
    if (path.filename().compare("index.html") != 0) {
      std::experimental::filesystem::path newpath = path / std::experimental::filesystem::path("index.html");
      serve_file(newpath, sock);
      return;
    }
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
    response.headers["Content-Type"] = get_content_type(std::experimental::filesystem::absolute(path));
    sock->write(response.to_string());
    sock->write(buf, size);
    delete[] buf;
  } else {
    response.status_code = 404;

    sock->write(response.to_string());
  }
}

void serve_dir(const std::experimental::filesystem::path& path, const Socket_t& sock) {
  HttpResponse response;
  response.status_code = 200;
  response.message_body = "Serve Dir Test";
  sock->write(response.to_string());
}

void handle_htdocs(const HttpRequest& request, const Socket_t& sock) {
  auto querystr_pos = request.request_uri.find('?');
  std::string uri = request.request_uri;
  if (querystr_pos != std::string::npos) {
    uri = uri.substr(0, querystr_pos);
  }
  uri = "http-root-dir/htdocs" + uri;
  std::experimental::filesystem::path path(uri);

  if (uri.back() == '/') {
    serve_dir(path, sock);
  } else {
    serve_file(path, sock);
  }
}
