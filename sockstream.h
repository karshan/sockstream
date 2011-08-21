#ifndef _SOCKSTREAM_H
#define _SOCKSTREAM_H

#include <iostream>
#include <cstring>
#include <exception>

#include <unistd.h>
#include <errno.h>
#include <cstdio>

class sockerr: public std::exception
{
    std::string err;
public:
    ~sockerr() throw() {}
    sockerr(std::string s) : err(s) {}
    virtual const char * what() const throw() { return err.c_str(); }
};

class sockstream;

//TODO: buffer input and output
class sockstreambuf : public std::streambuf
{
private:
    int sockfd;
    short m_revents;
    char buf;
public:
    sockstreambuf(int s) : sockfd(s) {}
    sockstreambuf() : sockfd(-1) {}
    sockstreambuf(const sockstreambuf &rhs) : sockfd(rhs.sockfd), m_revents(rhs.m_revents) {}

    void close();
    void open(const char *hname, const char *port);
    void bind(const char *port);

    sockstream accept(struct sockaddr *addr, socklen_t *addrlen);

    int overflow(int c)
    {
        char ch = c;
        int n = write(sockfd, &ch, sizeof(ch));
        if (n == 1) {
            return c;
        } else {
            close();
            throw sockerr(std::string("write() ") + strerror(errno));
//            return EOF;
        }
    }

    int underflow()
    {
        char ch;
        int n = read(sockfd, &ch, 1);
        if (n == 1) {
            setg(&buf, &buf, &buf);
            buf = ch;
            return ch;
        } else {
            close();
            throw sockerr(std::string("read() ") + strerror(errno));
//            return EOF;
        }
    }

    bool is_open() { return sockfd >= 0; }

    int sync() { return 0; }

    friend int poll(sockstream socks[], int nsocks, int timeout);

    short revents() const { return m_revents; }
};

class sockstream : public std::iostream
{
public:
    sockstream() : std::iostream(new sockstreambuf()), std::ios(0) {}
    sockstream(int s) : std::iostream(new sockstreambuf(s)), std::ios(0) {}
    sockstream(const sockstream & rhs) : std::iostream(new sockstreambuf(*((sockstreambuf*)rhs.rdbuf()))), std::ios(0) {}
    ~sockstream() { delete rdbuf(); }

    void open(const char *hname, const char *port) { ((sockstreambuf*)rdbuf())->open(hname, port); }
    void bind(const char *port) { ((sockstreambuf*)rdbuf())->bind(port); }
    sockstream accept(struct sockaddr *addr, socklen_t *addrlen) { return ((sockstreambuf*)rdbuf())->accept(addr, addrlen); }
    void close() { ((sockstreambuf*)rdbuf())->close(); }
    bool is_open() { return ((sockstreambuf*)rdbuf())->is_open(); }
    sockstreambuf* operator->() { return (sockstreambuf*)rdbuf(); }
};

#endif /* _SOCKSTREAM_H */
