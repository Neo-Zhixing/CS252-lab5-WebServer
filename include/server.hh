#ifndef  INCLUDE_SERVER_HH_
#define INCLUDE_SERVER_HH_

#include <chrono>
#include <string>

#include "socket.hh"
#include "http_messages.hh"


extern unsigned int total_requests;
extern unsigned int min_servetime;
extern unsigned int max_servetime;
extern std::chrono::time_point<std::chrono::steady_clock> startup_time;

class Server {
 private:
    SocketAcceptor const& _acceptor;

 public:
    std::string realm = "cs252";

    explicit Server(SocketAcceptor const& acceptor);
    void run_linear() const;
    void run_fork() const;
    void run_thread_pool(const int num_threads) const;
    void run_thread_pool_worker() const;
    void run_thread() const;


    void handle(const Socket_t sock) const;

    bool authenticate(const HttpRequest& req, const Socket_t& sock) const;

    void get_request(const Socket_t& sock, const HttpRequest& req) const;
};

#endif  // INCLUDE_SERVER_HH_
