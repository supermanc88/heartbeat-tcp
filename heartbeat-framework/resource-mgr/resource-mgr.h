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
int trans_data_generator(void *recved_data, void **next_send_data);

/**
 * 主机构造数据，内容为请求备机的动作
 * @param data  传入数据结构的指针
 * @param type  请求动作的类型
 * @return
 */
int trans_data_set_action(void * data, ACTION_TYPE type);

/**
 * 主机构造数据，请求备机的状态
 * @param data 传入数据结构的指针
 * @return
 */
int trans_data_set_get_server_status(void * data);

/**
 * 主机构造数据，内容为请求备机收集的数据
 * @param data 传入数据结构的指针
 * @return
 */
int trans_data_set_get_data(void * data);



#endif //HEARTBEAT_TCP_RESOURCE_MGR_H
