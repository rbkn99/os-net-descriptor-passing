#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <sys/epoll.h>
#include "utils.h"

using std::cin;
using std::cout;
using std::stoi;
using std::cerr;
using std::string;
using std::vector;

const int CONNECTIONS_TOTAL = 64;

void get_args(char *argv[], string &host, int &port) {
    try {
        port = stoi(argv[2]);
    } catch (...) {
        cerr << "Invalid port\n";
        exit(EXIT_FAILURE);
    }
    host = argv[1];
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "Usage: ./os-net-client [host] [port]";
        exit(EXIT_FAILURE);
    }

    int port;
    string host;
    int ed = epoll_create1(0);
    get_args(argv, host, port);

    //creating socket file descriptor
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    utils::check(fd, "in socket");

    struct sockaddr_in server{};

    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    socklen_t s_len = sizeof(sockaddr_in);

    utils::check(inet_pton(AF_INET, host.data(), &server.sin_addr), "in inet_pton");
    utils::check(connect(fd, (sockaddr *) (&server), sizeof(server)), "in connect");

    struct epoll_event epoll_event{}, events[CONNECTIONS_TOTAL];

    //1
    epoll_event.events = EPOLLIN;
    epoll_event.data.fd = fd;
    utils::check(epoll_ctl(ed, EPOLL_CTL_ADD, fd, &epoll_event), "epoll_ctl");

    //2
    epoll_event.events = EPOLLIN;
    epoll_event.data.fd = 0;
    utils::check(epoll_ctl(ed, EPOLL_CTL_ADD, 0, &epoll_event), "epoll_ctl");

    int descriptors = epoll_wait(ed, events, CONNECTIONS_TOTAL, -1);
    utils::check(descriptors, "in epoll_wait");

    string msg;
    char msg_size = msg.size();

    for (int i = 0; i < descriptors; i++) {
        if (events[i].data.fd == 0) {
            cout << "Enter message:\n";
            cin >> msg;
            if (msg.size() >= 127) {
                cerr << "Size of the message must be less than 127\n";
                exit(EXIT_FAILURE);
            }
            msg_size = msg.size();

            utils::send_fd(&msg_size, 1, fd);
            utils::send_fd(&msg[0], msg_size, fd);

            utils::receive_fd(&msg[0], msg_size, fd);
            utils::print_msg(msg);
        }
        else if (events[i].data.fd == fd) {
            utils::receive_fd(&msg[0], msg_size, fd);
            utils::print_msg(msg);
        }
    }

    utils::check(shutdown(fd, SHUT_RDWR), "in descriptor shutdown");
    utils::check(close(fd), "in descriptor close");
    return 0;
}