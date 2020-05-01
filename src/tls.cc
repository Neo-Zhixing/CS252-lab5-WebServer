/**
 * This file contains your implementation of a TLS socket and socket acceptor. The TLS socket uses
 * the OpenSSL library to handle all socket communication, so you need to configure OpenSSL and use the
 * OpenSSL functions to read/write to the socket. src/tcp.cc is provided for your reference on 
 * Sockets and SocketAdaptors and examples/simple_tls_server.c is provided for your reference on OpenSSL.
 */

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <iostream>
#include <sstream>
#include <cstring>
#include <memory>

#include "tls.hh"
#include "errors.hh"



int create_socket(int port)
{
    int s;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
	perror("Unable to create socket");
	exit(EXIT_FAILURE);
    }

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
	perror("Unable to bind");
	exit(EXIT_FAILURE);
    }

    if (listen(s, 1) < 0) {
	perror("Unable to listen");
	exit(EXIT_FAILURE);
    }

    return s;
}

void init_openssl()
{ 
    SSL_load_error_strings();	
    OpenSSL_add_ssl_algorithms();
}

SSL_CTX *create_context()
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = SSLv23_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
	perror("Unable to create SSL context");
	ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }

    return ctx;
}

void configure_context(SSL_CTX *ctx)
{
    SSL_CTX_set_ecdh_auto(ctx, 1);

    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }
}









TLSSocket::TLSSocket(int port_no, struct sockaddr_in addr, SSL* ssl) :
  _socket(port_no), _addr(addr), _ssl(ssl) {
    char inet_pres[INET_ADDRSTRLEN];
    // sin_family will be AF_INET
    if (inet_ntop(addr.sin_family, &(addr.sin_addr), inet_pres, INET_ADDRSTRLEN)) {
        std::cout << "Received a TLS connection from " << inet_pres << std::endl;
    }
}
TLSSocket::~TLSSocket() noexcept {
    std::cout << "Closing TCP socket fd " << _socket;
    char inet_pres[INET_ADDRSTRLEN];
    // sin_family will be AF_INET
    if (inet_ntop(_addr.sin_family, &(_addr.sin_addr), inet_pres, INET_ADDRSTRLEN)) {
        std::cout << " from " << inet_pres;
    }
    std::cout << std::endl;
    close(_socket);
    SSL_free(_ssl);
}

char TLSSocket::getc() {
    char c;
    ssize_t read = this->read(&c, 1);
    if (read < 0) {
        std::cout << "Bad things happening " << (SSL_get_error(_ssl, read) == SSL_ERROR_ZERO_RETURN) << std::endl;
        ERR_print_errors_fp(stdout);
        throw ConnectionError("Unable to read a character: ");
    } else if (read > 1) {
        throw ConnectionError("Read more than one byte when expecting to only read one.");
    } else if (read == 0) {
        c = EOF;
    }
    std::cout << "Read character " << c << std::endl;
    return c;
}

ssize_t TLSSocket::read(char *buf, size_t buf_len) {
    return SSL_read(_ssl, buf, buf_len);
}

std::string TLSSocket::readline() {
    std::string str;
    char c;
    while ((c = getc()) != '\n' && c != EOF) {
        str.append(1, c);
    }
    if (c == '\n') {
        str.append(1, '\n');
    }
    return str;
}

void TLSSocket::write(std::string const &str) {
    SSL_write(_ssl, str.c_str(), str.length());
}

void TLSSocket::write(char const *const buf, const size_t buf_len) {
    if (buf == NULL)
        return;
    SSL_write(_ssl, buf, buf_len);
}

int TLSSocket::get_socket() {
    return -1;
}

TLSSocketAcceptor::TLSSocketAcceptor(const int portno) {
    init_openssl();
    _ssl_ctx = create_context();

    configure_context(_ssl_ctx);

    _master_socket = create_socket(portno);
}

Socket_t TLSSocketAcceptor::accept_connection() const {
    struct sockaddr_in addr;
    uint len = sizeof(addr);

    int client = accept(_master_socket, (struct sockaddr*)&addr, &len);
    if (client < 0) {
        std::cout << "Unable to accept" << std::endl;
        return NULL;
    }

    SSL* ssl = SSL_new(_ssl_ctx);
    SSL_set_fd(ssl, client);

    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
    }


    return std::make_unique<TLSSocket>(client, addr, ssl);
}

TLSSocketAcceptor::~TLSSocketAcceptor() noexcept {
    std::cout << "Closing socket " << _master_socket << std::endl;
    close(_master_socket);
    SSL_CTX_free(_ssl_ctx);
    EVP_cleanup();
}
