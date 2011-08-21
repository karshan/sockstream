#include "sockstream.h"

#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

void sockstreambuf::close()
{
    ::close(sockfd);
    sockfd = -1;
}

void sockstreambuf::open(const char *hname, const char *port)
{
    int ret;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((ret = getaddrinfo(hname, port, &hints, &res)))
        throw sockerr(std::string("getaddrinfo(): ") + gai_strerror(ret));

    if((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
        throw sockerr(std::string("socket(): ") + strerror(errno));

    if ((ret = connect(sockfd, res->ai_addr, res->ai_addrlen)) < 0) {
        close();
        throw sockerr(std::string("connect(): ") + strerror(errno));
    }
}

void sockstreambuf::bind(const char *port)
{
    int ret;
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((ret = getaddrinfo(NULL, port, &hints, &res)))
        throw sockerr(std::string("getaddrinfo(): ") + gai_strerror(ret));

    if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
        throw sockerr(std::string("socket(): ") + strerror(errno));

    if ((ret = ::bind(sockfd, res->ai_addr, res->ai_addrlen)) < 0) {
        close();
        throw sockerr(std::string("bind(): ") + strerror(errno));
    }

    if ((ret = ::listen(sockfd, 32)) < 0) {
        close();
        throw sockerr(std::string("listen(): ") + strerror(errno));
    }
}

sockstream sockstreambuf::accept(struct sockaddr *addr, socklen_t *addrlen)
{
    int ret = ::accept(sockfd, addr, addrlen);
    if (ret < 0)
        throw sockerr(std::string("accept(): ") + strerror(errno));
    return sockstream(ret);
}

int poll(sockstream socks[], int nsocks, int timeout)
{
    struct pollfd *fds = new struct pollfd[nsocks];
    for (int i = 0; i < nsocks; i++) {
        fds[i].fd = socks[i]->sockfd;
        fds[i].events = POLLIN;
        socks[i]->m_revents = 0;
    }
    int ret = poll(fds, nsocks, timeout);
    if (ret < 0)
        throw sockerr(std::string("poll(): ") + strerror(errno));
    for (int i = 0; i < nsocks; i++)
        socks[i]->m_revents = fds[i].revents;
    delete fds;
    return ret;
}
