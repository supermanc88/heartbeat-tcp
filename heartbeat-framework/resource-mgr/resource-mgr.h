#ifndef HEARTBEAT_TCP_RESOURCE_MGR_H
#define HEARTBEAT_TCP_RESOURCE_MGR_H

#include "heartbeat-config.h"

#define POLICY_PATH "/tmp/heartbeat/heartbeat-framework/resource-mgr/policy.dat"
#define POLICY_NOLINK_PATH "/tmp/heartbeat/heartbeat-framework/resource-mgr/policy2.dat"

/**
 * 通过收到的包的内容，来生成下次要发送的包的内容
 * 这个函数只有备机去调用,主机就是三个状态讯问加无数据
 * @param recved_data
 * @param next_send_data
 * @return
 */
int trans_data_generator(void *recved_data, void **next_send_data);

/**
 * 主机构造数据，内容为请求备机的动作,由主机调用
 * @param data  传入数据结构的指针
 * @param type  请求动作的类型
 * @return
 */
int trans_data_set_action(void * data, ACTION_TYPE type);

/**
 * 主机构造数据，请求备机的状态,由主机调用
 * @param data 传入数据结构的指针
 * @return
 */
int trans_data_set_get_server_status(void * data);

/**
 * 主机构造数据，内容为请求备机收集的数据,由主机调用
 * @param data 传入数据结构的指针
 * @return
 */
int trans_data_set_get_data(void * data);


/**
 * 填充空动作
 * @param data
 * @return
 */
int trans_data_set_none(void * data);



int resource_manager(void * recved_data, void * next_data);

/**
 *
 * @param primary_server_status
 * @param primary_have_virtual_ip
 * @param primary_auto_fail_back
 * @param backup_server_status
 * @param backup_have_virtual_ip
 * @return 0 什么也不做
 * 1 主机让备机拿资源
 * 2 主机让备机释放资源
 */
int policy_manager(bool primary_server_status, bool primary_have_virtual_ip, bool primary_auto_fail_back,
        bool backup_server_status, bool backup_have_virtual_ip);


/**
 * 获取本机的服务状态和是否有虚ip
 * @param data
 * @return
 */
int get_local_server_status_datas(SERVER_STATUS_DATAS * data);

/**
 * 初始化策略表，连通状态的策略
 * @return
 */
int resource_manager_init();


/**
 * 初始化策略表，不连通状态的策略
 * @return
 */
int policy_no_link_init();


#endif //HEARTBEAT_TCP_RESOURCE_MGR_H
