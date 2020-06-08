#ifndef HEARTBEAT_TCP_PLUG_ICMP_H
#define HEARTBEAT_TCP_PLUG_ICMP_H

#ifdef __cplusplus
extern "C"{
#endif

    int plug_init(void * data);

    int plug_stop(void);

    int plug_run(void * data);

#ifdef __cplusplus
};
#endif

#endif //HEARTBEAT_TCP_PLUG_ICMP_H
