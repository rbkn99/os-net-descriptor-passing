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
        cout << "Usage: ./os-net-descriptor-passing-client [socket]";
        exit(EXIT_FAILURE);
    }

    auto socket_name = argv[1];

    //creating socket file descriptor
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    utils::check(fd, "in socket");

    //initializing server
    struct sockaddr_un server{};

    server.sun_family = AF_UNIX;
    strncpy(server.sun_path, socket_name, sizeof(server.sun_path) - 1);

    socklen_t s_len = sizeof(server);
    char buffer[BUFFER_SIZE];
    char size;
    string data;

    struct sockaddr_in client_addr{};
    socklen_t adress_size = sizeof(client_addr);

    int pipe1, pipe2;
    pipe1 = utils::receive_fd(fd);
    pipe2 = utils::receive_fd(fd);

    utils::receive_msg(&size, 1, pipe1);
    data.resize(size);
    utils::receive_msg(&data[0], size, pipe1);
    utils::print_msg(data);

    data = "bbbbbbbb";
    size = data.size();
    utils::send_msg(&size, 1, pipe2);
    utils::send_msg(&data[0], size, pipe2);

    utils::check(shutdown(fd, SHUT_RDWR), "in shutdown");
    utils::check(close(fd), "in close");
    utils::check(close(pipe1), "in pipe1");
    utils::check(close(pipe2), "in pipe2");
}