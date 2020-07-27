#include "resource-mgr.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <map>
#include <string>
#include <vector>
#include <string.h>
#include <netdb.h>

#include "plugin-mgr/plugin-manager.h"
#include "common/custom-functions.h"
#include "common/log2file.h"

std::map<std::string, int> policy_link_map;
std::map<std::string, std::map<std::string, int>> policy_nolink_map;
std::map<std::string, int> policy_nolink_primary_map;
std::map<std::string, int> policy_nolink_backup_map;


bool client_resources_takeover_status = false;
bool server_resources_takeover_status = false;

extern bool auto_failback;
extern char peer_addr[BUFSIZ];
extern char virtual_ip_with_mask[BUFSIZ];
extern char ethernet_name[BUFSIZ];
extern int eth_num;
extern int detect_interval;


bool host_vip_status = false;
bool host_server_status = false;


int trans_data_generator(void *recved_data, void **next_send_data)
{
    TRANS_DATA *p_trans_data;
    TRANS_TYPE trans_type;
    ACTION_TYPE action_type;
    bool backup_server_status;
    char *content;
    int policy;

    TRANS_DATA *p_next_data;

    size_t size, content_size;


    p_trans_data = (TRANS_DATA *) recved_data;

    size = p_trans_data->size;

    trans_type = p_trans_data->type;


    switch (trans_type) {
        case TRANS_TYPE_ACTION: {

            P2FILE("-------------------------------------\n");
            P2FILE("| recv data type: TRANS_TYPE_ACTION |\n");
            P2FILE("-------------------------------------\n");

            p_next_data = (TRANS_DATA *) malloc(sizeof(TRANS_DATA));
            bzero(p_next_data, sizeof(TRANS_DATA));
            /*
              * 只有备机会收到此类信息
              */
            action_type = p_trans_data->trans_action_data.type;
            if (action_type == ACTION_TYPE_GET_RES) {
                P2FILE("---------------------------------------------------------------\n");
                P2FILE("| server recv get res,so server start take over the resources |\n");
                P2FILE("---------------------------------------------------------------\n");

                take_over_resources(virtual_ip_with_mask, ethernet_name, eth_num);
                server_resources_takeover_status = true;
                p_next_data->type = TRANS_TYPE_REPLY_ACTION;
                p_next_data->size = sizeof(TRANS_DATA);

                p_next_data->trans_action_data.type = ACTION_TYPE_GOT_RES;
                p_next_data->trans_action_data.result = 1;

            } else if (action_type == ACTION_TYPE_FREE_RES) {
                P2FILE("--------------------------------------------------------------\n");
                P2FILE("| server recv free res,so server start release the resources |\n");
                P2FILE("--------------------------------------------------------------\n");

                release_resources(virtual_ip_with_mask, ethernet_name);
                server_resources_takeover_status = false;
                p_next_data->type = TRANS_TYPE_REPLY_ACTION;
                p_next_data->size = sizeof(TRANS_DATA);

                p_next_data->trans_action_data.type = ACTION_TYPE_FREED_RES;
                p_next_data->trans_action_data.result = 1;
            } else {

            }
            P2FILE("-----------------------------------------------------\n");
            P2FILE("| construct send data type: TRANS_TYPE_REPLY_ACTION |\n");
            P2FILE("-----------------------------------------------------\n");

            *next_send_data = (void *) p_next_data;
        }
            break;
        case TRANS_TYPE_GET_SERVER_STATUS: {
            P2FILE("------------------------------------------------\n");
            P2FILE("| recv data type: TRANS_TYPE_GET_SERVER_STATUS |\n");
            P2FILE("------------------------------------------------\n");


            // 此类回复没有其它数据
            p_next_data = (TRANS_DATA *) malloc(sizeof(TRANS_DATA));
            bzero(p_next_data, sizeof(TRANS_DATA));
            p_next_data->size = sizeof(TRANS_DATA);
            p_next_data->type = TRANS_TYPE_REPLY_SERVER_STATUS;

            get_local_server_status_datas(&p_next_data->server_status_datas);

            P2FILE("------------------------------------------------------------\n");
            P2FILE("| construct send data type: TRANS_TYPE_REPLY_SERVER_STATUS |\n");
            P2FILE("------------------------------------------------------------\n");

            *next_send_data = (void *) p_next_data;
        }
            break;
        case TRANS_TYPE_GET_DATA: {
            P2FILE("---------------------------------------\n");
            P2FILE("| recv data type: TRANS_TYPE_GET_DATA |\n");
            P2FILE("---------------------------------------\n");

            // 备机收到，向TRANS_DATA中填充数据
            // 1. 通过插件拿到数据
            // 2. 根据数据大小malloc内存并拷贝
            // 3. 填充其它数据元素
            p_next_data = (TRANS_DATA *) malloc(sizeof(TRANS_DATA));
            bzero(p_next_data, sizeof(TRANS_DATA));
            p_next_data->size = sizeof(TRANS_DATA);
            p_next_data->type = TRANS_TYPE_REPLY_DATA;
            P2FILE("---------------------------------------------------\n");
            P2FILE("| construct send data type: TRANS_TYPE_REPLY_DATA |\n");
            P2FILE("---------------------------------------------------\n");

            *next_send_data = (void *) p_next_data;
        }
            break;
        case TRANS_TYPE_REPLY_ACTION: {
            P2FILE("-------------------------------------------\n");
            P2FILE("| recv data type: TRANS_TYPE_REPLY_ACTION |\n");
            P2FILE("-------------------------------------------\n");

            /*
             * 只有主机会收到此类信息
             */
            action_type = p_trans_data->trans_action_data.type;
            if (action_type == ACTION_TYPE_GOT_RES) {
                client_resources_takeover_status = false;
                // nothing!
            } else if (action_type == ACTION_TYPE_FREED_RES) {
                // 开始接管资源
                P2FILE("-----------------------------------------------------------------------\n");
                P2FILE("| server reply freed resource,so client start take over the resources |\n");
                P2FILE("-----------------------------------------------------------------------\n");

                take_over_resources(virtual_ip_with_mask, ethernet_name, eth_num);
                client_resources_takeover_status = true;
            } else {
                // nothing!
            }
            P2FILE("--------------------------------------------------\n");
            P2FILE("| construct send data type: TRANS_TYPE_HEARTBEAT |\n");
            P2FILE("--------------------------------------------------\n");
            p_next_data = (TRANS_DATA *) malloc(sizeof(TRANS_DATA));
            trans_data_set_none(p_next_data);
            *next_send_data = (void *) p_next_data;
        }
            break;
        case TRANS_TYPE_REPLY_SERVER_STATUS: {
            P2FILE("--------------------------------------------------\n");
            P2FILE("| recv data type: TRANS_TYPE_REPLY_SERVER_STATUS |\n");
            P2FILE("--------------------------------------------------\n");
            // 这里根据决策开始操作,先把结构体填充成无操作
            p_next_data = (TRANS_DATA *) malloc(sizeof(TRANS_DATA));
            trans_data_set_none(p_next_data);

            policy = resource_manager(recved_data, p_next_data);

            if (policy == LINK_ACT_DO_NOTHING) {
                P2FILE("--------------------------------------------------\n");
                P2FILE("| construct send data type: TRANS_TYPE_HEARTBEAT |\n");
                P2FILE("--------------------------------------------------\n");
            } else if (policy == LINK_ACT_BACKUP_NODE_TAKEOVER) {
                release_resources(virtual_ip_with_mask, ethernet_name);
                client_resources_takeover_status = false;
                P2FILE("-----------------------------------------------\n");
                P2FILE("| construct send data type: TRANS_TYPE_ACTION |\n");
                P2FILE("-----------------------------------------------\n");
            } else if (policy == LINK_ACT_PRIMARY_NODE_TAKEOVER) {
                // 需要向备机发包，让备机先释放，我再拿
                P2FILE("-----------------------------------------------\n");
                P2FILE("| construct send data type: TRANS_TYPE_ACTION |\n");
                P2FILE("-----------------------------------------------\n");
            }

            *next_send_data = (void *) p_next_data;
        }
            break;
        case TRANS_TYPE_REPLY_DATA: {
            P2FILE("----------------------------------------\n");
            P2FILE("| recv data type: TRANS_TYPE_REPLY_DATA |\n");
            P2FILE("----------------------------------------\n");
            p_next_data = (TRANS_DATA *) malloc(sizeof(TRANS_DATA));
            trans_data_set_none(p_next_data);
            *next_send_data = (void *) p_next_data;
            P2FILE("--------------------------------------------------\n");
            P2FILE("| construct send data type: TRANS_TYPE_HEARTBEAT |\n");
            P2FILE("--------------------------------------------------\n");
        }
            break;
        case TRANS_TYPE_HEARTBEAT: {
            P2FILE("----------------------------------------\n");
            P2FILE("| recv data type: TRANS_TYPE_HEARTBEAT |\n");
            P2FILE("----------------------------------------\n");
            p_next_data = (TRANS_DATA *) malloc(sizeof(TRANS_DATA));
            trans_data_set_none(p_next_data);
            *next_send_data = (void *) p_next_data;
            P2FILE("--------------------------------------------------\n");
            P2FILE("| construct send data type: TRANS_TYPE_HEARTBEAT |\n");
            P2FILE("--------------------------------------------------\n");
        }
            break;
        default: {
            P2FILE("---------------------------\n");
            P2FILE("| recv data type: default |\n");
            P2FILE("---------------------------\n");

            p_next_data = (TRANS_DATA *) malloc(sizeof(TRANS_DATA));
            trans_data_set_none(p_next_data);
            *next_send_data = (void *) p_next_data;
            P2FILE("--------------------------------------------------\n");
            P2FILE("| construct send data type: TRANS_TYPE_HEARTBEAT |\n");
            P2FILE("--------------------------------------------------\n");
        }
            break;
    }

    return 0;
}

int trans_data_set_action(void *data, ACTION_TYPE type)
{

    TRANS_DATA *pdata = (TRANS_DATA *) data;

    bzero(pdata, sizeof(TRANS_DATA));

    pdata->type = TRANS_TYPE_ACTION;
    pdata->size = sizeof(TRANS_DATA);

    pdata->trans_action_data.type = type;
    pdata->trans_action_data.result = 1;

    return 0;
}

int trans_data_set_get_server_status(void *data)
{

    P2FILE("----------------------------------------------------------\n");
    P2FILE("| construct send data type: TRANS_TYPE_GET_SERVER_STATUS |\n");
    P2FILE("----------------------------------------------------------\n");

    TRANS_DATA *pdata = (TRANS_DATA *) data;

    bzero(pdata, sizeof(TRANS_DATA));

    pdata->type = TRANS_TYPE_GET_SERVER_STATUS;
    pdata->size = sizeof(TRANS_DATA);

    return 0;
}

int trans_data_set_get_data(void *data)
{

    TRANS_DATA *pdata = (TRANS_DATA *) data;

    bzero(pdata, sizeof(TRANS_DATA));

    pdata->type = TRANS_TYPE_GET_DATA;
    pdata->size = sizeof(TRANS_DATA);

    return 0;
}

int trans_data_set_none(void *data)
{
    TRANS_DATA *pdata = (TRANS_DATA *) data;

    bzero(pdata, sizeof(TRANS_DATA));

    pdata->type = TRANS_TYPE_HEARTBEAT;
    pdata->size = sizeof(TRANS_DATA);
    return 0;
}

int resource_manager(void *recved_data, void *next_data)
{

    TRANS_TYPE type;
    TRANS_DATA *pdata;
    bool backup_server_status;
    bool backup_have_virtual_ip;
    char *data;
    SERVER_STATUS_DATAS local_server_status_datas;

    int policy;

    pdata = (TRANS_DATA *) recved_data;
    type = pdata->type;


    switch (type) {
        case TRANS_TYPE_REPLY_SERVER_STATUS: {
            backup_server_status = pdata->server_status_datas.server_status;
            backup_have_virtual_ip = pdata->server_status_datas.have_virtual_ip;

            // 这里有本机的server_status
            bzero(&local_server_status_datas, sizeof(SERVER_STATUS_DATAS));

            get_local_server_status_datas(&local_server_status_datas);


            policy = policy_online_manager(local_server_status_datas.server_status,
                                           local_server_status_datas.have_virtual_ip, auto_failback,
                                           backup_server_status, backup_have_virtual_ip);

            if (policy == LINK_ACT_DO_NOTHING) {
                trans_data_set_none(next_data);
            } else if (policy == LINK_ACT_BACKUP_NODE_TAKEOVER) {
                trans_data_set_action(next_data, ACTION_TYPE_GET_RES);
            } else if (policy == LINK_ACT_PRIMARY_NODE_TAKEOVER) {
                trans_data_set_action(next_data, ACTION_TYPE_FREE_RES);
            }
        }
            break;
        default: {

        }
            break;
    }


    return policy;
}

int policy_online_manager(bool primary_server_status, bool primary_have_virtual_ip, bool primary_auto_fail_back,
                          bool backup_server_status, bool backup_have_virtual_ip)
{
    char policy_str[256] = {0};
    std::string spolicy;
    // 这里根据８＊８的策略表来写

    sprintf(policy_str, "p_ss:%d|p_vip:%d|p_afk:%d|b_ss:%d|b_vip:%d",
            primary_server_status, primary_have_virtual_ip, primary_auto_fail_back,
            backup_server_status, backup_have_virtual_ip);

    P2FILE("construct string: %s\n", policy_str);

    spolicy.assign(policy_str);

    if (policy_link_map.count(spolicy) > 0) {

        // 返回0 1 2
        return policy_link_map[spolicy];

    } else
        return LINK_ACT_DO_NOTHING;

    return LINK_ACT_DO_NOTHING;
}

int get_local_server_status_datas(SERVER_STATUS_DATAS *data)
{
    data->have_virtual_ip = host_vip_status;
    data->server_status = host_server_status;
    return 0;
}


int policy_online_init()
{
    FILE *fp;
    char read_line_buf[256] = {0};
    std::string str_line;
    int tail_pos = -1;
    int space_pos = -1;

    fp = fopen(POLICY_LINK_PATH, "r");

    if (fp == NULL)
        return 0;

    while (fgets(read_line_buf, 256, fp)) {
        // 如果是#开头的，说明是注释的，不需要解析 或者是换行
        if (read_line_buf[0] == '#' || read_line_buf[0] == '\n' || read_line_buf[0] == '\r')
            continue;

        str_line.assign(read_line_buf);

        tail_pos = str_line.find("\n");
        if (std::string::npos != tail_pos)
            str_line.erase(tail_pos, 1);

        tail_pos = str_line.find("\r");
        if (std::string::npos != tail_pos)
            str_line.erase(tail_pos, 1);

        if (space_pos = str_line.find("="), std::string::npos != space_pos) {
            std::string skey = str_line.substr(0, space_pos);
            std::string svalue = str_line.substr(space_pos + 1, str_line.length() - space_pos - 1);

            skey.erase(0, skey.find_first_not_of(" "));
            skey.erase(skey.find_last_not_of(" ") + 1);

            svalue.erase(0, svalue.find_first_not_of(" "));
            svalue.erase(svalue.find_last_not_of(" ") + 1);

            policy_link_map[skey] = atoi(svalue.c_str());
        }
    }

    if (fp == NULL) {
        perror("fopen error");
        return 0;
    }


    if (fp != NULL) {
        fclose(fp);
        fp = NULL;
    }

    return 0;
}

int policy_no_link_init()
{
    policy_stand_alone_init();
//    policy_no_link_backup_init();

    return 0;
}

int take_over_resources(const char *virtual_ip_with_mask, const char *ethernet_name, int eth_num)
{
    char cmd_str[256] = {0};

    // 这里使用的virtual_ip 是用网段表示的 应该去掉网段
    std::string str_virtual_ip, str_virtual_ip_with_mask;
    str_virtual_ip_with_mask.assign(virtual_ip_with_mask);
    str_virtual_ip = str_virtual_ip_with_mask.substr(0, str_virtual_ip_with_mask.find_last_of("/"));


    // 开始接管资源
    P2FILE("Start taking over resources...\n");

    if (gethostbyname2(str_virtual_ip.c_str(), AF_INET) != NULL) {
        // 1. 绑定ip到网卡
        sprintf(cmd_str, "ip -f inet addr add %s dev %s label %s:%d", virtual_ip_with_mask, ethernet_name, ethernet_name,
                eth_num);

        system_to_file((const char *) cmd_str, "/tmp/takeover.tmp");


        bzero(cmd_str, 256);
        // 2. 发送免费arp
        sprintf(cmd_str, "/opt/infosec-heartbeat/bin/send_arp -c 5 -s %s -w 5 -I %s %s", str_virtual_ip.c_str(), ethernet_name,
                str_virtual_ip.c_str());

        system_to_file((const char *) cmd_str, "/tmp/send_arp.tmp");
    } else if(gethostbyname2(str_virtual_ip.c_str(), AF_INET6) != NULL) {
        // 1. 绑定ip到网卡
        sprintf(cmd_str, "ip -f inet6 addr add %s dev %s label %s:%d", virtual_ip_with_mask, ethernet_name, ethernet_name,
                eth_num);

        system_to_file((const char *) cmd_str, "/tmp/takeover.tmp");
    }

    return 0;
}

int release_resources(const char *virtual_ip_with_mask, const char *ethernet_name)
{
    char cmd_str[256] = {0};
    // 开始释放资源
    P2FILE("Start to release resources...\n");

    // 这里使用的virtual_ip 是用网段表示的 应该去掉网段
    std::string str_virtual_ip, str_virtual_ip_with_mask;
    str_virtual_ip_with_mask.assign(virtual_ip_with_mask);
    str_virtual_ip = str_virtual_ip_with_mask.substr(0, str_virtual_ip_with_mask.find_last_of("/"));

    if (gethostbyname2(str_virtual_ip.c_str(), AF_INET) != NULL) {
        // 1. 删除网卡ip
        sprintf(cmd_str, "ip -f inet addr delete %s dev %s", virtual_ip_with_mask, ethernet_name);
    } else if(gethostbyname2(str_virtual_ip.c_str(), AF_INET6) != NULL) {
        // 1. 删除网卡ip
        sprintf(cmd_str, "ip -f inet6 addr delete %s dev %s", virtual_ip_with_mask, ethernet_name);
    }

    system_to_file((const char *) cmd_str, "/tmp/release_resources.tmp");
    return 0;
}

int policy_stand_alone_manager(bool server_status, bool have_virtual_ip, int node_type)
{

    char policy_str[256] = {0};
    std::string spolicy;

    // primary
    if (node_type == 0) {
        sprintf(policy_str, "p_ss:%d|p_vip:%d", server_status, have_virtual_ip);
        spolicy.assign(policy_str);
        if (policy_nolink_primary_map.count(spolicy) > 0) {
            return policy_nolink_primary_map[spolicy];
        } else
            return NOLINK_ACT_DO_NOTING;

    } else if (node_type == 1) {
        // backup
        sprintf(policy_str, "b_ss:%d|b_vip:%d", server_status, have_virtual_ip);
        spolicy.assign(policy_str);
        if (policy_nolink_primary_map.count(spolicy) > 0) {
            return policy_nolink_primary_map[spolicy];
        } else
            return NOLINK_ACT_DO_NOTING;
    } else
        return NOLINK_ACT_DO_NOTING;
}

int policy_stand_alone_init()
{
    FILE *fp;
    char read_line_str[256] = {0};
    std::string str_line;
    int tail_pos = -1;
    int space_pos = -1;

    fp = fopen(POLICY_NOLINK_PRIMARY_PATH, "r");
    if (fp == NULL)
        return 0;

    while (fgets(read_line_str, 256, fp)) {
        // 如果是#开头的，说明是注释的，不需要解析 或者是换行
        if (read_line_str[0] == '#' || read_line_str[0] == '\n' || read_line_str[0] == '\r')
            continue;

        str_line.assign(read_line_str);

        tail_pos = str_line.find("\n");
        if (std::string::npos != tail_pos)
            str_line.erase(tail_pos, 1);

        tail_pos = str_line.find("\r");
        if (std::string::npos != tail_pos)
            str_line.erase(tail_pos, 1);

        if (space_pos = str_line.find("="), std::string::npos != space_pos) {
            std::string str_key = str_line.substr(0, space_pos);
            std::string str_value = str_line.substr(space_pos + 1, str_line.length() - space_pos - 1);

            str_key.erase(0, str_key.find_first_not_of(" "));
            str_key.erase(str_key.find_last_not_of(" ") + 1);

            str_value.erase(0, str_value.find_first_not_of(" "));
            str_value.erase(str_value.find_last_not_of(" ") + 1);

            policy_nolink_primary_map[str_key] = atoi(str_value.c_str());
        }
    }

    if (fp == NULL) {
        perror("fopen error");
        return 0;
    }


    if (fp != NULL) {
        fclose(fp);
        fp = NULL;
    }

    return 0;
}

int policy_init()
{
    policy_online_init();
    policy_stand_alone_init();
    return 0;
}

void *get_local_server_status_datas_thread(void *)
{
    while (true) {

        P2FILE("get_local_server_status_datas_thread start!\n");

        char cmd_str[256] = {0};
        int ret;
        int check_vip_ret;
        char vip[BUFSIZ];
        // 这里运行一次本地的插件获取状态
        P2FILE("get_local_server_status_datas...\n");

        std::string svirtual_ip, svirtual_ip_with_mask;
        svirtual_ip_with_mask.assign(virtual_ip_with_mask);
        svirtual_ip = svirtual_ip_with_mask.substr(0, svirtual_ip_with_mask.find_last_of("/"));

        strcpy(vip, svirtual_ip.c_str());
        // 这里通过check-vip程序来获取本机是否有虚ip
        sprintf(cmd_str, "/opt/infosec-heartbeat/bin/check-virtual-ip %s", vip);

        ret = system(cmd_str);

        if (WIFEXITED(ret)) {
            check_vip_ret = WEXITSTATUS(ret);
        }
        if(check_vip_ret == 1) {
            P2FILE("has virtual ip\n");
            client_resources_takeover_status = true;
            server_resources_takeover_status = true;
        }
        else {
            P2FILE("no virtual ip\n");
            client_resources_takeover_status = false;
            server_resources_takeover_status = false;
        }

        host_vip_status = (check_vip_ret == 1) ? true : false;
        host_server_status = (bool) run_all_plugin();

        P2FILE("get_local_server_status_datas_thread complete!\n");
        sleep(detect_interval);
    }
    return NULL;
}

