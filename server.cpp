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
const int BUFFER_SIZE = 128;


int main(int argc, char *argv[]) {
    cout << "Usage: ./os-net-descriptor-passing-server\n";
    struct sockaddr_un server{};
    server.sun_family = AF_UNIX;
    memcpy(server.sun_path, utils::SOCKET.c_str(), utils::SOCKET.size());

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    utils::check(fd, "in fd");
    unlink(utils::SOCKET.c_str());

    utils::check((bind(fd, (sockaddr *) (&server), sizeof(server))), "in bind");
    utils::check(listen(fd, SOMAXCONN), "in listen");

    char buffer[CMSG_SPACE(sizeof(int))], data[BUFFER_SIZE];
    int cd;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while(true) {
        //cout << "something connected...\n";

        cd = accept(fd, NULL, NULL);
        utils::check(cd, "in accept");

        struct msghdr msg{};
        struct iovec iv{};
        memset(buffer, 0, BUFFER_SIZE);
        iv.iov_len = sizeof(data);
        iv.iov_base = &data;

        msg.msg_iov = &iv;
        msg.msg_iovlen = 1;
        msg.msg_control = buffer;
        msg.msg_controllen = sizeof (buffer);

        struct cmsghdr* cmsg;
        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_type = SCM_RIGHTS;
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_len = CMSG_LEN(sizeof(int));

        utils::check(sendmsg(cd, &msg, 0), "in sendmsg");
    }
#pragma clang diagnostic pop
}