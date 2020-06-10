#ifndef HEARTBEAT_TCP_RESOURCE_MGR_H
#define HEARTBEAT_TCP_RESOURCE_MGR_H

#include "heartbeat-config.h"

/**
 * 通过收到的包的内容，来生成下次要发送的包的内容
 * 这个函数只有备机去调用,主机就是三个状态讯问加无数据
 * @param recved_data
 * @param next_send_data
 * @return
 */
int data_generator(void *recved_data, void **next_send_data);



#endif //HEARTBEAT_TCP_RESOURCE_MGR_H
