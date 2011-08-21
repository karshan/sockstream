#include "sockstream.h"

#include <sys/socket.h>

int main(int argc, char *argv[])
{
    sockstream sock;
    struct sockaddr_storage addr;
    socklen_t addrlen = sizeof(addr);

    if (argc != 2) {
        std::cout << "usage " << argv[0] << " <port>\n";
        return 1;
    }

    sock.bind(argv[1]);
    sockstream conn(sock.accept((struct sockaddr*)&addr, &addrlen));
    conn << "Hi there!\n";

    conn.close();
    sock.close();
    return 0;
}
