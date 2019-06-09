#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include "utils.h"

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


void utils::send_fd(int socket, int file_descriptor) {
    auto fd_len = sizeof(file_descriptor);

    char buffer[CMSG_SPACE(fd_len)];
    memset(buffer, 0, sizeof(buffer));

    struct iovec iovec_{};
    iovec_.iov_base = (void *) "123";
    iovec_.iov_len = 3;

    msghdr msg{};
    msg.msg_iov = &iovec_;
    msg.msg_control = buffer;
    msg.msg_controllen = sizeof(buffer);
    msg.msg_iovlen = 1;

    auto cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_len = fd_len;
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;

    *((int *) CMSG_DATA(cmsg)) = file_descriptor;
    msg.msg_controllen = CMSG_LEN(fd_len);

    check(sendmsg(socket, &msg, 0), "in send_msg");
}

int utils::receive_fd(int socket_fd) {
    char buffer[DEFAULT_BUFFER_SIZE];
    char control_buf[DEFAULT_BUFFER_SIZE];

    struct iovec iovec_{};
    iovec_.iov_base = (void *) buffer;
    iovec_.iov_len = sizeof(buffer);

    msghdr msg{};
    msg.msg_iov = &iovec_;
    msg.msg_iovlen = 1;
    msg.msg_control = control_buf;
    msg.msg_controllen = sizeof(control_buf);

    check(recvmsg(socket_fd, &msg, 0), "in receive_msg");
    cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);

    return *((int *) CMSG_DATA(cmsg));
}

void utils::print_msg(const string& msg) {
    std::cout << "Received message:\n";
    std::cout << msg << std::endl;
}

void utils::close_all(int *pipe1, int *pipe2, const int *ad) {
    check(shutdown(*ad, SHUT_RDWR), "shutdown");
    check(close(*ad), "close");
    check(close(pipe1[0]), "p_in");
    check(close(pipe1[1]), "p_in");
    check(close(pipe2[0]), "p_out");
    check(close(pipe2[1]), "p_out");
}
