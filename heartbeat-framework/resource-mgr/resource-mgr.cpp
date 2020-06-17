#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <map>
#include <string>
#include <vector>

#include "resource-mgr.h"
#include "../plugin-mgr/plugin-manager.h"

std::map<std::string, int> policy_link_map;
std::map<std::string, std::map<std::string, int>> policy_nolink_map;
std::map<std::string, int> policy_nolink_primary_map;
std::map<std::string, int> policy_nolink_backup_map;


bool client_resources_takeover_status = false;
bool server_resources_takeover_status = false;

extern bool auto_failback;

//int main(void)
//{
//    policy_link_init();
//    printf("resource manager\n");
//    printf("---------------------------\n");
//
//    std::map<std::string, int>::iterator iter;
//    int i = 0;
//    for(iter = policy_map.begin(); iter != policy_map.end(); iter++){
//        printf("%s = %d\n", iter->first.c_str(), iter->second);
//        i++;
//    }
//
//    printf("total count: %d\n", i);
//
//    printf("----------------------------\n");
//
//    policy_no_link_init();
//
//    std::map< std::string, std::map<std::string, int> >::iterator iter2;
//
//    i = 0;
//    for(iter2 = policy_nolink_map.begin(); iter2 != policy_nolink_map.end(); iter2++) {
//        std::map<std::string, int>::iterator iter3;
//        for(iter3 = iter2->second.begin(); iter3 != iter2->second.end(); iter3++){
//            printf("%s, %s = %d\n", iter2->first.c_str(), iter3->first.c_str(), iter3->second);
//            i++;
//        }
//    }
//
//    printf("total count: %d\n", i);
//
//    return 0;
//}

int trans_data_generator(void *recved_data, void **next_send_data)
{
    TRANS_DATA * p_trans_data;
    TRANS_TYPE trans_type;
    ACTION_TYPE action_type;
    bool backup_server_status;
    char * content;
    int policy;

    TRANS_DATA * p_next_data;

    size_t size, content_size;


    p_trans_data = (TRANS_DATA *) recved_data;

    size = p_trans_data->size;

    trans_type = p_trans_data->type;


    switch (trans_type) {
        case TRANS_TYPE_ACTION: {

            printf("reciving data type: TRANS_TYPE_ACTION\n");

            p_next_data = (TRANS_DATA *)malloc(sizeof(TRANS_DATA));
            bzero(p_next_data, sizeof(TRANS_DATA));
            /*
              * 只有备机会收到此类信息
              */
            action_type = p_trans_data->trans_action_data.type;
            if (action_type == GET_RES) {
                printf("server recv get res,so server start take over the resources\n");
                take_over_resources("192.168.231.155", "ens33");
                server_resources_takeover_status = true;
                p_next_data->type = TRANS_TYPE_REPLY_ACTION;
                p_next_data->size = sizeof(TRANS_DATA);

                p_next_data->trans_action_data.type = GOT_RES;
                p_next_data->trans_action_data.result = 1;

            } else if (action_type == FREE_RES) {
                printf("server recv free res,so server start release the resources\n");
                release_resources("192.168.231.155", "ens33");
                server_resources_takeover_status = false;
                p_next_data->type = TRANS_TYPE_REPLY_ACTION;
                p_next_data->size = sizeof(TRANS_DATA);

                p_next_data->trans_action_data.type = FREED_RES;
                p_next_data->trans_action_data.result = 1;
            } else {

            }
            *next_send_data = (void *)p_next_data;
        }
            break;
        case TRANS_TYPE_GET_SERVER_STATUS: {
            printf("reciving data type: TRANS_TYPE_GET_SERVER_STATUS\n");

            // 此类回复没有其它数据
            p_next_data = (TRANS_DATA *)malloc(sizeof(TRANS_DATA));
            bzero(p_next_data, sizeof(TRANS_DATA));
            p_next_data->size = sizeof(TRANS_DATA);
            p_next_data->type = TRANS_TYPE_REPLY_SERVER_STATUS;

            get_local_server_status_datas(&p_next_data->server_status_datas);

            *next_send_data = (void *)p_next_data;
        }
            break;
        case TRANS_TYPE_GET_DATA: {
            printf("reciving data type: TRANS_TYPE_GET_DATA\n");
            // 备机收到，向TRANS_DATA中填充数据
            // 1. 通过插件拿到数据
            // 2. 根据数据大小malloc内存并拷贝
            // 3. 填充其它数据元素
            p_next_data = (TRANS_DATA *)malloc(sizeof(TRANS_DATA));
            bzero(p_next_data, sizeof(TRANS_DATA));
            p_next_data->size = sizeof(TRANS_DATA);
            p_next_data->type = TRANS_TYPE_REPLY_DATA;

            *next_send_data = (void *)p_next_data;
        }
            break;
        case TRANS_TYPE_REPLY_ACTION: {
            printf("reciving data type: TRANS_TYPE_REPLY_ACTION\n");
            /*
             * 只有主机会收到此类信息
             */
            action_type = p_trans_data->trans_action_data.type;
            if (action_type == GOT_RES) {
                client_resources_takeover_status = false;
                // nothing!
            } else if (action_type == FREED_RES) {
                // 开始接管资源
                printf("server reply freed resource,so client start take over the resources\n");
                take_over_resources("192.168.231.155", "ens33");
                client_resources_takeover_status = true;
            } else {
                // nothing!
            }

            p_next_data = (TRANS_DATA *)malloc(sizeof(TRANS_DATA));
            trans_data_set_none(p_next_data);
            *next_send_data = (void *)p_next_data;
        }
            break;
        case TRANS_TYPE_REPLY_SERVER_STATUS: {
            printf("reciving data type: TRANS_TYPE_REPLY_SERVER_STATUS\n");
            // 这里根据决策开始操作,先把结构体填充成无操作
            p_next_data = (TRANS_DATA *)malloc(sizeof(TRANS_DATA));
            trans_data_set_none(p_next_data);

            policy = resource_manager(recved_data, p_next_data);

            if (policy == LINK_ACT_DO_NOTHING) {

            } else if (policy == LINK_ACT_BACKUP_NODE_TAKEOVER) {
                release_resources("192.168.231.155", "ens33");
                client_resources_takeover_status = false;
            } else if (policy == LINK_ACT_PRIMARY_NODE_TAKEOVER) {
                // 需要向备机发包，让备机先释放，我再拿
            }

            *next_send_data = (void *)p_next_data;
        }
            break;
        case TRANS_TYPE_REPLY_DATA: {
            printf("reciving data type: TRANS_TYPE_REPLY_DATA\n");
            content = p_trans_data->data;
            content_size = size - (size_t)(((TRANS_DATA *)0)->data);

            p_next_data = (TRANS_DATA *)malloc(sizeof(TRANS_DATA));
            trans_data_set_none(p_next_data);
            *next_send_data = (void *)p_next_data;
        }
            break;
        case TRANS_TYPE_NONE: {
            printf("reciving data type: TRANS_TYPE_NONE\n");
            p_next_data = (TRANS_DATA *)malloc(sizeof(TRANS_DATA));
            trans_data_set_none(p_next_data);
            *next_send_data = (void *)p_next_data;
        }
            break;
        default: {
            printf("reciving data type: default\n");
            p_next_data = (TRANS_DATA *)malloc(sizeof(TRANS_DATA));
            trans_data_set_none(p_next_data);
            *next_send_data = (void *)p_next_data;
        }
            break;
    }

    return 0;
}

int trans_data_set_action(void *data, ACTION_TYPE type) {

    TRANS_DATA * pdata = (TRANS_DATA *) data;

    bzero(pdata, sizeof(TRANS_DATA));

    pdata->type = TRANS_TYPE_ACTION;
    pdata->size = sizeof(TRANS_DATA);

    pdata->trans_action_data.type = type;
    pdata->trans_action_data.result = 1;

    return 0;
}

int trans_data_set_get_server_status(void *data) {

    TRANS_DATA * pdata = (TRANS_DATA *) data;

    bzero(pdata, sizeof(TRANS_DATA));

    pdata->type = TRANS_TYPE_GET_SERVER_STATUS;
    pdata->size = sizeof(TRANS_DATA);

    return 0;
}

int trans_data_set_get_data(void *data) {

    TRANS_DATA * pdata = (TRANS_DATA *) data;

    bzero(pdata, sizeof(TRANS_DATA));

    pdata->type = TRANS_TYPE_GET_DATA;
    pdata->size = sizeof(TRANS_DATA);

    return 0;
}

int trans_data_set_none(void *data)
{
    TRANS_DATA * pdata = (TRANS_DATA *) data;

    bzero(pdata, sizeof(TRANS_DATA));

    pdata->type = TRANS_TYPE_NONE;
    pdata->size = sizeof(TRANS_DATA);
    return 0;
}

int resource_manager(void *recved_data, void *next_data)
{

    TRANS_TYPE type;
    TRANS_DATA * pdata;
    bool backup_server_status;
    bool backup_have_virtual_ip;
    char * data;
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



            policy = policy_link_manager(local_server_status_datas.server_status,
                                         local_server_status_datas.have_virtual_ip, auto_failback,
                                         backup_server_status, backup_have_virtual_ip);

            if(policy == LINK_ACT_DO_NOTHING ) {
                trans_data_set_none(next_data);
            } else if (policy == LINK_ACT_BACKUP_NODE_TAKEOVER) {
                trans_data_set_action(next_data, GET_RES);
            } else if (policy == LINK_ACT_PRIMARY_NODE_TAKEOVER) {
                trans_data_set_action(next_data, FREE_RES);
            }
        }
            break;
        default: {

        }
            break;
    }


    return policy;
}

int policy_link_manager(bool primary_server_status, bool primary_have_virtual_ip, bool primary_auto_fail_back,
                        bool backup_server_status, bool backup_have_virtual_ip)
{
    char policy_str[256] = {0};
    std::string spolicy;
    // 这里根据８＊８的策略表来写

    sprintf(policy_str, "p_ss:%d|p_vip:%d|p_afk:%d|b_ss:%d|b_vip:%d",
            primary_server_status, primary_have_virtual_ip, primary_auto_fail_back,
            backup_server_status, backup_have_virtual_ip);

    printf("construct string: %s\n", policy_str);

    spolicy.assign(policy_str);

    if(policy_link_map.count(spolicy) > 0) {

        // 返回0 1 2
        return policy_link_map[spolicy];

    } else
        return LINK_ACT_DO_NOTHING;

    return LINK_ACT_DO_NOTHING;
}

int get_local_server_status_datas(SERVER_STATUS_DATAS *data)
{
    char cmd_str[256] = {0};
    int ret;
    int check_vip_ret;
    // 这里运行一次本地的插件获取状态
    printf("get_local_server_status_datas...\n");

    char * vip = "192.168.231.155";
    // 这里通过check-vip程序来获取本机是否有虚ip
    sprintf(cmd_str, "./check-virtual-ip %s", vip);

    ret = system(cmd_str);

    if(WIFEXITED(ret)) {
        check_vip_ret = WEXITSTATUS(ret);
    }

    data->have_virtual_ip = (check_vip_ret==1) ? true: false;
    data->server_status = (bool)run_all_plugin();
    return 0;
}



int policy_link_init()
{
    FILE * fp;
    char str_line[256] = {0};
    std::string sline;
    int tail_pos = -1;
    int space_pos = -1;

    fp = fopen(POLICY_LINK_PATH, "r");

    while (fgets(str_line, 256, fp)) {
        // 如果是#开头的，说明是注释的，不需要解析 或者是换行
        if(str_line[0] == '#' || str_line[0] == '\n' || str_line[0] == '\r')
            continue;

        sline.assign(str_line);

        tail_pos = sline.find("\n");
        if(std::string::npos != tail_pos)
            sline.erase(tail_pos, 1);

        tail_pos = sline.find("\r");
        if(std::string::npos != tail_pos)
            sline.erase(tail_pos, 1);

        if(space_pos = sline.find("="), std::string::npos != space_pos) {
            std::string skey = sline.substr(0, space_pos);
            std::string svalue = sline.substr(space_pos+1, sline.length()-space_pos-1);

            skey.erase(0, skey.find_first_not_of(" "));
            skey.erase(skey.find_last_not_of(" ") + 1);

            svalue.erase(0, svalue.find_first_not_of(" "));
            svalue.erase(svalue.find_last_not_of(" ") + 1);

            policy_link_map[skey] = atoi(svalue.c_str());
        }
    }

    if(fp == NULL) {
        perror("fopen error");
        return 0;
    }



    if( fp != NULL ) {
        fclose(fp);
        fp = NULL;
    }

    return 0;
}

int policy_no_link_init()
{
//    FILE * fp;
//    char str_line[256] = {0};
//    std::string sline;
//    int tail_pos = -1;
//    int space_pos = -1;
//
//    fp = fopen(POLICY_NOLINK_PATH, "r");
//
//    while (fgets(str_line, 256, fp)) {
//        // 如果是#开头的，说明是注释的，不需要解析 或者是换行
//        if(str_line[0] == '#' || str_line[0] == '\n' || str_line[0] == '\r')
//            continue;
//
//        sline.assign(str_line);
//
//        tail_pos = sline.find("\n");
//        if(std::string::npos != tail_pos)
//            sline.erase(tail_pos, 1);
//
//        tail_pos = sline.find("\r");
//        if(std::string::npos != tail_pos)
//            sline.erase(tail_pos, 1);
//
//        if(space_pos = sline.find("="), std::string::npos != space_pos) {
//            std::string skey = sline.substr(0, space_pos);
//            std::string svalue = sline.substr(space_pos+1, sline.length()-space_pos-1);
//
//            skey.erase(0, skey.find_first_not_of(" "));
//            skey.erase(skey.find_last_not_of(" ") + 1);
//
//            svalue.erase(0, svalue.find_first_not_of(" "));
//            svalue.erase(svalue.find_last_not_of(" ") + 1);
//
//            // 开始拆分主机和备机 p:1|b:1
//
//            if(space_pos = svalue.find("|"), std::string::npos != space_pos) {
//                std::map<std::string, int> tmp_map;
//                int m_pos = - 1;
//                std::string buf1 = svalue.substr(0, space_pos);
//                std::string buf2 = svalue.substr(space_pos+1, svalue.length()-space_pos-1);
//
//                if(m_pos = buf1.find(":"), std::string::npos != m_pos) {
//                    std::string p = buf1.substr(0, m_pos);
//                    std::string p_value = buf1.substr(m_pos+1, buf1.length()-m_pos-1);
//
//                    p.erase(0, p.find_first_not_of(" "));
//                    p.erase(p.find_last_not_of(" ") + 1);
//
//                    p_value.erase(0, p_value.find_first_not_of(" "));
//                    p_value.erase(p_value.find_last_not_of(" ") + 1);
//
//                    tmp_map[p] = atoi(p_value.c_str());
//                }
//
//                if(m_pos = buf2.find(":"), std::string::npos != m_pos) {
//                    std::string b = buf2.substr(0, m_pos);
//                    std::string b_value = buf2.substr(m_pos+1, buf2.length()-m_pos-1);
//
//                    b.erase(0, b.find_first_not_of(" "));
//                    b.erase(b.find_last_not_of(" ") + 1);
//
//                    b_value.erase(0, b_value.find_first_not_of(" "));
//                    b_value.erase(b_value.find_last_not_of(" ") + 1);
//
//                    tmp_map[b] = atoi(b_value.c_str());
//                }
//
//                policy_nolink_map[skey] = tmp_map;
//            }
//
//        }
//    }
//
//    if(fp == NULL) {
//        perror("fopen error");
//        return 0;
//    }
//
//    if( fp != NULL ) {
//        fclose(fp);
//        fp = NULL;
//    }


    policy_no_link_primary_init();
    policy_no_link_backup_init();

    return 0;
}

int take_over_resources(const char *virtual_ip, const char *ethernet_name)
{
    char cmd_str[256] = {0};

    // 开始接管资源
    printf("Start taking over resources...\n");

    // 1. 绑定ip到网卡
    sprintf(cmd_str, "ip -f inet addr add %s/24 dev %s label %s:0", virtual_ip, ethernet_name, ethernet_name);

    my_system((const char *)cmd_str, "/tmp/takeover.tmp");


    bzero(cmd_str, 256);
    // 2. 发送免费arp
    sprintf(cmd_str, "./send_arp -c 5 -s %s -w 5 -I %s %s", virtual_ip, ethernet_name, virtual_ip);

    my_system((const char *)cmd_str, "/tmp/send_arp.tmp");
    return 0;
}

int release_resources(const char *virtual_ip, const char *ethernet_name)
{
    char cmd_str[256] = {0};
    // 开始释放资源
    printf("Start to release resources...\n");

    // 1. 绑定ip到网卡
    sprintf(cmd_str, "ip -f inet addr delete %s/24 dev %s", virtual_ip, ethernet_name);

    my_system((const char *)cmd_str, "/tmp/release_resources.tmp");
    return 0;
}

int my_system(const char *cmd_string, const char *tmp_file)
{
    char my_cmd_str[256];
    sprintf(my_cmd_str, "%s > %s", cmd_string, tmp_file);

    return system(my_cmd_str);
}

int policy_nolink_manager(bool server_status, bool have_virtual_ip, int node_type)
{

    char policy_str[256] = {0};
    std::string spolicy;

    // primary
    if(node_type == 0) {
        sprintf(policy_str, "p_ss:%d|p_vip:%d", server_status, have_virtual_ip);
        spolicy.assign(policy_str);
        if(policy_nolink_primary_map.count(spolicy) > 0) {
            return policy_nolink_primary_map[spolicy];
        } else
            return NOLINK_ACT_DO_NOTING;

    } else if (node_type == 1) {
        // backup
        sprintf(policy_str, "p_ss:%d|p_vip:%d", server_status, have_virtual_ip);
        spolicy.assign(policy_str);
        if(policy_nolink_primary_map.count(spolicy) > 0) {
            return policy_nolink_primary_map[spolicy];
        } else
            return NOLINK_ACT_DO_NOTING;
    } else
        return NOLINK_ACT_DO_NOTING;
}

int policy_no_link_primary_init()
{
    FILE * fp;
    char str_line[256] = {0};
    std::string sline;
    int tail_pos = -1;
    int space_pos = -1;

    fp = fopen(POLICY_NOLINK_PRIMARY_PATH, "r");

    while (fgets(str_line, 256, fp)) {
        // 如果是#开头的，说明是注释的，不需要解析 或者是换行
        if(str_line[0] == '#' || str_line[0] == '\n' || str_line[0] == '\r')
            continue;

        sline.assign(str_line);

        tail_pos = sline.find("\n");
        if(std::string::npos != tail_pos)
            sline.erase(tail_pos, 1);

        tail_pos = sline.find("\r");
        if(std::string::npos != tail_pos)
            sline.erase(tail_pos, 1);

        if(space_pos = sline.find("="), std::string::npos != space_pos) {
            std::string skey = sline.substr(0, space_pos);
            std::string svalue = sline.substr(space_pos+1, sline.length()-space_pos-1);

            skey.erase(0, skey.find_first_not_of(" "));
            skey.erase(skey.find_last_not_of(" ") + 1);

            svalue.erase(0, svalue.find_first_not_of(" "));
            svalue.erase(svalue.find_last_not_of(" ") + 1);

            policy_nolink_primary_map[skey] = atoi(svalue.c_str());
        }
    }

    if(fp == NULL) {
        perror("fopen error");
        return 0;
    }



    if( fp != NULL ) {
        fclose(fp);
        fp = NULL;
    }

    return 0;
}

int policy_no_link_backup_init()
{
    FILE * fp;
    char str_line[256] = {0};
    std::string sline;
    int tail_pos = -1;
    int space_pos = -1;

    fp = fopen(POLICY_NOLINK_BACKUP_PATH, "r");

    while (fgets(str_line, 256, fp)) {
        // 如果是#开头的，说明是注释的，不需要解析 或者是换行
        if(str_line[0] == '#' || str_line[0] == '\n' || str_line[0] == '\r')
            continue;

        sline.assign(str_line);

        tail_pos = sline.find("\n");
        if(std::string::npos != tail_pos)
            sline.erase(tail_pos, 1);

        tail_pos = sline.find("\r");
        if(std::string::npos != tail_pos)
            sline.erase(tail_pos, 1);

        if(space_pos = sline.find("="), std::string::npos != space_pos) {
            std::string skey = sline.substr(0, space_pos);
            std::string svalue = sline.substr(space_pos+1, sline.length()-space_pos-1);

            skey.erase(0, skey.find_first_not_of(" "));
            skey.erase(skey.find_last_not_of(" ") + 1);

            svalue.erase(0, svalue.find_first_not_of(" "));
            svalue.erase(svalue.find_last_not_of(" ") + 1);

            policy_nolink_backup_map[skey] = atoi(svalue.c_str());
        }
    }

    if(fp == NULL) {
        perror("fopen error");
        return 0;
    }



    if( fp != NULL ) {
        fclose(fp);
        fp = NULL;
    }

    return 0;
}

