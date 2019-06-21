#ifndef OS_NET_UTILS_H
#define OS_NET_UTILS_H

#include <iostream>
#include <vector>
#include <sys/socket.h>

using std::string;

class utils {
public:

    static const string SOCKET;
    static const int DEFAULT_BUFFER_SIZE = 128;

    static void check(int var_to_check, const char* msg, bool soft_check=false);
    static void send_msg(char *buf_ptr, int size, int receiver);
    static void receive_msg(char *buf_ptr, int size, int sender);
    static void print_msg(const string& msg);
    static void set_msg(msghdr* msg, iovec* iv, char* buffer, char* data);
    static void set_cmsg(cmsghdr *cmsg, msghdr *msg);
};


#endif //OS_NET_UTILS_H
