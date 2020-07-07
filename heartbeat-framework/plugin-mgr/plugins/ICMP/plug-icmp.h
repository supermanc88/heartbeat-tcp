#ifndef HEARTBEAT_TCP_PLUG_ICMP_H
#define HEARTBEAT_TCP_PLUG_ICMP_H

#ifdef __cplusplus
extern "C"{
#endif

    int plug_init(void * data);

    int plug_stop(void);

    int plug_run(void * data);

    int my_system(const char *cmd_string);


#define CMD_STRING_LEN 256
#define MY_TMP_FILENAME "/tmp/icmp.tmp"

#ifdef __cplusplus
};
#endif

#endif //HEARTBEAT_TCP_PLUG_ICMP_H
