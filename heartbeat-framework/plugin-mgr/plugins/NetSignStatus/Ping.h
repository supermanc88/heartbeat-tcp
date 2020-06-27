#ifndef PING_H
#define PING_H

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <dlfcn.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

int Ping( char *ips, int timeout, int failretrytimes);

#endif

