#include "sockstream.h"

int main(int argc, char *argv[])
{
    sockstream s;
    char ch;
    if (argc != 3) {
        std::cout << "usage " << argv[0] << " <hostname> <port>\n";
        return 1;
    }

    s.open(argv[1], argv[2]);
    s.get(ch);
    while (s.is_open()) {
        std::cout << ch;
        s.get(ch);
    }

    s.close();
    return 0;
}
