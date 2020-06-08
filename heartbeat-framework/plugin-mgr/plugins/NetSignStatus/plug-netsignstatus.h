//
// Created by CHM on 2020/6/8.
//

#ifndef HEARTBEAT_TCP_PLUG_NETSIGNSTATUS_H
#define HEARTBEAT_TCP_PLUG_NETSIGNSTATUS_H

#ifdef __cplusplus
extern "C"{
#endif

    int plug_init(void * data);

    int plug_stop(void );

    int plug_run(void * data);

#ifdef __cplusplus
};
#endif

#endif //HEARTBEAT_TCP_PLUG_NETSIGNSTATUS_H
