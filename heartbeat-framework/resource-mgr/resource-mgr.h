#ifndef HEARTBEAT_TCP_RESOURCE_MGR_H
#define HEARTBEAT_TCP_RESOURCE_MGR_H

#include "../heartbeat-config.h"

#define POLICY_LINK_PATH "/etc/ha.d/policy/policy.dat"
#define POLICY_NOLINK_PATH "/etc/ha.d/policy/policy2.dat"
#define POLICY_NOLINK_PRIMARY_PATH "/etc/ha.d/policy/policy_primary.dat"
#define POLICY_NOLINK_BACKUP_PATH "/etc/ha.d/policy/policy_backup.dat"

#define LINK_ACT_DO_NOTHING 0
#define LINK_ACT_BACKUP_NODE_TAKEOVER 1
#define LINK_ACT_PRIMARY_NODE_TAKEOVER 2

#define NOLINK_ACT_DO_NOTING 0
#define NOLINK_ACT_TAKEOVER 1
#define NOLINK_ACT_RELEASE 2

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


/**
 * 根据收到的数据，结合本机的服务状态，开始对资源进行操作,并填充下个要发送的数据包
 * @param recved_data
 * @param next_data
 * @return
 */
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
int policy_link_manager(bool primary_server_status, bool primary_have_virtual_ip, bool primary_auto_fail_back,
                        bool backup_server_status, bool backup_have_virtual_ip);



/**
 * 这是在主备机不能正常通信时采取的策略
 * @param server_status
 * @param have_virtual_ip
 * @param node_type 主机0 or 备机1
 * @return 动作类型
 */
int policy_nolink_manager(bool server_status, bool have_virtual_ip, int node_type);


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
int policy_link_init();


/**
 * 初始化策略表，不连通状态的策略
 * @return
 */
int policy_no_link_init();

/**
 * 初始化主机在不连通状态下的策略
 * @return
 */
int policy_no_link_primary_init();


/**
 * 初始化备机在不连通状态下的策略
 * 此函数已不再使用，未连通状态下的策略由policy_primary.dat提供
 * @return
 */
int policy_no_link_backup_init();


/**
 * 接管资源
 * @return
 */
int take_over_resources(const char *virtual_ip_with_mask, const char *ethernet_name, int eth_num);

/**
 * 释放资源
 * @return
 */
int release_resources(const char *virtual_ip_with_mask, const char *ethernet_name);


/**
 * 封装一层system函数，重定向写文件，不在屏幕打印
 * @param cmd_string
 * @param tmp_file
 * @return
 */
int my_system(const char *cmd_string, const char * tmp_file);

#endif //HEARTBEAT_TCP_RESOURCE_MGR_H
