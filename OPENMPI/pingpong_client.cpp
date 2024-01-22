#include "pingpong_TCP.h"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main() {
    struct sockaddr_in serv_addr;
    int sock = 0;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported" << std::endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }

    int ping_count = 0;
    while (ping_count < MAX_PINGS) {
        send(sock, "Ping", strlen("Ping"), 0);
        char buffer[1024] = {0};
        read(sock, buffer, 1024);
        std::cout << "Client: " << buffer << std::endl;
        ping_count++;
        sleep(1);
    }

    close(sock);
    return 0;
}
