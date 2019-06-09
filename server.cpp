#include <iostream>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdio>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "utils.h"

using std::cout;
using std::stoi;
using std::cerr;
using std::string;
using std::vector;

void get_args(char *argv[], string &host, int &port) {
    try {
        port = stoi(argv[2]);
    } catch (...) {
        cerr << "Invalid port";
        exit(EXIT_FAILURE);
    }
    host = argv[1];
}

void transform_msg(string &msg) {
    if (msg.empty())
        return;
    msg = "Z" + msg.substr(1, msg.size() - 1);
}

const int CONNECTIONS_TOTAL = 64;
const int BUFFER_SIZE = 1024;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: ./os-net-descriptor-passing-server [socket]";
        exit(EXIT_FAILURE);
    }

    auto socket_name = argv[1];
    unlink(socket_name);

    //creating socket file descriptor
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    utils::check(fd, "in socket");

    //initializing server
    struct sockaddr_un server{}, client{};

    server.sun_family = AF_UNIX;
    strncpy(server.sun_path, socket_name, sizeof(server.sun_path) - 1);

    socklen_t s_len = sizeof(server);
    char buffer[BUFFER_SIZE];
    char size;

    string msg = "aaaaaaa";

    utils::check(bind(fd, (sockaddr *) (&server), s_len), "in bind");
    utils::check(listen(fd, SOMAXCONN), "in listen");

    //running...
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

    while (true) {
        struct sockaddr_in client_addr{};
        socklen_t address_size = sizeof(client_addr);

        int ad = accept(fd, (struct sockaddr *) &client_addr, &address_size);
        if (ad == -1) {
            perror("bad accept descriptor");
            continue;
        }
        int pipe1[2], pipe2[2];
        utils::check(pipe(pipe1), "in pipe1");
        utils::send_fd(ad, pipe1[0]);

        utils::check(pipe(pipe2), "in pipe2");
        utils::send_fd(ad, pipe2[1]);

        int ret = read(ad, buffer, BUFFER_SIZE);
        utils::check(ret, "error while reading");

        size = msg.size();
        utils::send_msg(&size, 1, pipe1[1]);
        utils::send_msg((&msg[0], size, pipe1[1]);
        utils::receive_msg(&size, 1, pipe2[0]);
        msg.resize(size);
        utils::receive_msg(&msg[0], size, pipe2[0]);
        utils::print_msg(msg);
        utils::check(shutdown(ad, SHUT_RDWR), "shutdown");
        utils::check(close(ad), "close");
        utils::check(close(pipe1[0]), "p_in");
        utils::check(close(pipe1[1]), "p_in");
        utils::check(close(pipe2[0]), "p_out");
        utils::check(close(pipe2[1]), "p_out");
    }
}

#pragma clang diagnostic pop
}