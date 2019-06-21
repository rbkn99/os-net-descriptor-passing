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

const int BUFFER_SIZE = 128;

int main(int argc, char *argv[]) {
    cout << "Usage: ./os-net-descriptor-passing-client\n";
    struct sockaddr_un client{};
    client.sun_family = AF_UNIX;
    memcpy(client.sun_path, utils::SOCKET.c_str(), utils::SOCKET.size());

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    utils::check(fd, "in fd");
    utils::check(connect(fd, (sockaddr *) (&client), sizeof(client)), "in connect");

    char buffer[CMSG_SPACE(sizeof(int))], data[BUFFER_SIZE];

    struct msghdr msg{};
    struct iovec iv{};
    memset(buffer, 0, BUFFER_SIZE);
    //utils::set_msg(&msg, &iv, buffer, data);
    iv.iov_len = sizeof(data);
    iv.iov_base = &data;

    msg.msg_iov = &iv;
    msg.msg_iovlen = 1;
    msg.msg_control = buffer;
    msg.msg_controllen = sizeof (buffer);
    utils::check(recvmsg(fd, &msg, 0), "in recvmsg");
    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);

    cout << "Enter :q to quit, else type smth" << std::endl;
    string std_msg;
    dup2(*CMSG_DATA(cmsg), 1);
    bool flag  = true;
    while (flag) {
        getline(std::cin, std_msg);
        if (std_msg == ":q") {
            flag = false;
        }
        else {
            cout << std_msg << std::endl;
        }
    }
}