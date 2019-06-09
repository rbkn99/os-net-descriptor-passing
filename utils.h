#ifndef OS_NET_UTILS_H
#define OS_NET_UTILS_H

#include <iostream>
#include <vector>
#include <sys/socket.h>

using std::string;

class utils {
public:
    static void check(int var_to_check, const char* msg, bool soft_check=false);
    static void send_msg(char *buf_ptr, int size, int receiver);
    static void receive_msg(char *buf_ptr, int size, int sender);
    static int receive_fd(int socket_fd);
    static void send_fd(int socket, int file_descriptor);
    static void print_msg(const string& msg);

    static const int DEFAULT_BUFFER_SIZE = 128;
};


#endif //OS_NET_UTILS_H