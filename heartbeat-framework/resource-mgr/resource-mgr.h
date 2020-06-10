#ifndef HEARTBEAT_TCP_RESOURCE_MGR_H
#define HEARTBEAT_TCP_RESOURCE_MGR_H

#include "heartbeat-config.h"

/**
 * 通过收到的包的内容，来生成下次要发送的包的内容
 * @param recved_data
 * @param next_send_data
 * @return
 */
int data_generator(void *recved_data, void **next_send_data);



#endif //HEARTBEAT_TCP_RESOURCE_MGR_H
