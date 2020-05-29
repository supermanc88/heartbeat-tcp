//
// Created by CHM on 5/27/2020.
//

#include "wrap.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>

void perr_exit(const char *s)
{
    perror(s);
    exit(1);
}


int Accept(int socket, struct sockaddr *address,
           socklen_t *address_len)
{
    int n = 0;
    again:
    if( (n = accept(socket, address, address_len)) < 0 ) {
        if( (errno == ECONNABORTED) || (errno == EINTR) )
            goto again;
        else
            perr_exit("accept error");
    }
    return n;
}


int Bind(int socket, struct sockaddr *address, socklen_t address_len)
{
    int n;
    if( (n = bind(socket, address, address_len)) < 0)
        perr_exit("bind error");
    return n;
}


int Connect(int socket, struct sockaddr *address, socklen_t address_len)
{
    int n;
    if( (n = connect(socket, address, address_len)) <0 )
        perr_exit("connect error");
    return n;
}


int Listen(int socket, int backlog)
{
    int n;
    if( (n = listen(socket, backlog)) < 0 )
        perr_exit("listen error");
    return n;
}

int Socket(int family, int type, int protocol)
{
    int n;
    if( (n = socket(family, type, protocol)) < 0 )
        perr_exit("socket error");
    return n;
}


ssize_t Read(int fd, void *ptr, size_t nbytes)
{
    ssize_t n;
again:
    if( (n = read(fd, ptr, nbytes)) == -1) {
        if(errno == EINTR)
            goto again;
        else
            return -1;
    }
    return n;
}


ssize_t Write(int fd, void *ptr, size_t nbytes)
{
    ssize_t n;
    again:
    if( (n = write(fd, ptr, nbytes)) < -1 ) {
        if(errno = EINTR)
            goto again;
        else
            return -1;
    }
    return n;
}