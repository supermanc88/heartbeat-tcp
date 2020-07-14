#ifndef HEARTBEAT_TCP_PLUG_ICMP_H
#define HEARTBEAT_TCP_PLUG_ICMP_H

#ifdef __cplusplus
extern "C"{
#endif
#include "hb-log.h"

    int plug_init(void * data);

    int plug_stop(void);

    int plug_run(void * data);

    int LoadMonSrvConf();
#define CMD_STRING_LEN 256
#define MY_TMP_FILENAME "/tmp/icmp.tmp"

#ifdef __cplusplus
};
#endif

#endif //HEARTBEAT_TCP_PLUG_ICMP_H
