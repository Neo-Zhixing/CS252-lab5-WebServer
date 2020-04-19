#include "http_messages.hh"

// You may find implementing this function and using it in server.cc helpful

void handle_htdocs(const HttpRequest& request, const Socket_t& sock) {
  HttpResponse response;
  response.http_version = request.http_version;
  // TODO: Task 1.3
  // Get the request URI, verify the file exists and serve it
  std::cout << "http-root-dir/htdocs" + request.request_uri << std::endl;

  ifstream input("http-root-dir/htdocs" + request.request_uri, ios_base::in | ios_base::binary );

  input.seekg(0, std::ios::end);
  size_t size = input.tellg();
  char * buf = new char[size];
  input.seekg(0, std::ios::beg);
  
  input >> buf;

  sock->send(response.to_string());
  sock->send(buf, size);
  delete[] buf;
}
