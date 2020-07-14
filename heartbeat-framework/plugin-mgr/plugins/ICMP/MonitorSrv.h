#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <dlfcn.h>
#include <semaphore.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/file.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include "MyLog.h"

#define BUFFSIZE		160
#define FILEBUFFSIZE	2048

#define MONSRVCONF		"/opt/infosec/NetSignServer52/NetSignServer/haplugin/conf.ini"

#define HA_RESOURCES	"/etc/ha.d/haresources"
#define HA_CONFIG		"/etc/ha.d/ha.cf"
#define HA_HOST			"/etc/hosts"


int LoadMonSrvConf();

