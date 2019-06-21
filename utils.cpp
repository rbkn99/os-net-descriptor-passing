#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include "utils.h"

const string utils::SOCKET = "/tmp/os-net-descriptor-passing-socket.socket";

void utils::check(int var_to_check, const char *msg, bool soft_check) {
    if (var_to_check == -1) {
        std::perror(msg);
        if (!soft_check) exit(EXIT_FAILURE);
    }
}

void utils::send_msg(char *buf_ptr, int size, int receiver) {
    int i = 0;
    while (i < size) {
        int result = write(receiver, buf_ptr + i, size - i);
        check(result, "in write");
        i += result;
    }
}

void utils::receive_msg(char *buf_ptr, int size, int sender) {
    int i = 0;
    while (i < size) {
        int result = read(sender, buf_ptr + i, size - i);
        check(result, "in receive");
        i += result;
    }
}

void utils::print_msg(const string& msg) {
    std::cout << "Received message:\n";
    std::cout << msg << std::endl;
}


void utils::set_msg(msghdr *msg, iovec* iv, char *buffer, char* data) {
    iv->iov_len = sizeof(data);
    iv->iov_base = &data;

    msg->msg_iov = iv;
    msg->msg_iovlen = 1;
    msg->msg_control = buffer;
    msg->msg_controllen = sizeof (buffer);
}

void utils::set_cmsg(cmsghdr *cmsg, msghdr *msg) {
    cmsg = CMSG_FIRSTHDR(msg);
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));
}

