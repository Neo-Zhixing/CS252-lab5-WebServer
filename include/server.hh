#ifndef  INCLUDE_SERVER_HH_
#define INCLUDE_SERVER_HH_

#include "socket.hh"
#include "http_messages.hh"

class Server {
 private:
    SocketAcceptor const& _acceptor;

 public:
    std::string realm = "cs252";

    explicit Server(SocketAcceptor const& acceptor);
    void run_linear() const;
    void run_fork() const;
    void run_thread_pool(const int num_threads) const;
    void run_thread() const;


    void handle(const Socket_t sock) const;

    bool authenticate(const HttpRequest& req, const Socket_t& sock) const;

    void get_request(const Socket_t& sock, HttpRequest& req) const;
};

#endif  // INCLUDE_SERVER_HH_
