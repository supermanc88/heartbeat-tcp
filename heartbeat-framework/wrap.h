//
// Created by CHM on 5/27/2020.
//

#ifndef CLIENT_WRAP_H
#define CLIENT_WRAP_H


#include <unistd.h>

void perr_exit(const char *s);
int Accept(int socket, struct sockaddr *address, socklen_t *address_len);
int Bind(int socket, struct sockaddr *address, socklen_t address_len);
int Connect(int socket, struct sockaddr *address, socklen_t address_len);
int Listen(int socket, int backlog);
int Socket(int family, int type, int protocol);
ssize_t Read(int fd, void *ptr, size_t nbytes);
ssize_t Write(int fd, void *ptr, size_t nbytes);

#endif //CLIENT_WRAP_H
