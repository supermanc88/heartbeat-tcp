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
#include "ErrMsg.h"
#include "MyLog.h"
#include "Detect.h"
#include "SlimXml.h"
#include "MyFunc.h"

#define BUFFSIZE		160
#define FILEBUFFSIZE	2048

#define MONSRVCONF		"/opt/infosec/NetSignServer52/NetSignServer/haplugin/conf.ini"
#define SRV_CONF_PATH	"/opt/infosec/NetSignServer52/NetSignServer/config/service.xml"

#define HA_RESOURCES	"/etc/ha.d/haresources"
#define HA_CONFIG		"/etc/ha.d/ha.cf"
#define HA_HOST			"/etc/hosts"


//#define HA_STANDBY		"/share/heartbeat/hb_standby"
//#define HA_TAKEOVER		"/share/heartbeat/hb_takeover"

#define HA_STANDBY		"/usr/share/heartbeat/hb_standby"
#define HA_TAKEOVER		"/usr/share/heartbeat/hb_takeover"


int GetSrvStatus(char * RIP, char * VIP);

int monitor_srv_init();

