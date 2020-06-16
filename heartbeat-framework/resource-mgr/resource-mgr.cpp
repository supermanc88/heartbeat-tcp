#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <map>
#include <string>
#include <vector>

#include "resource-mgr.h"
#include "../plugin-mgr/plugin-manager.h"

std::map<std::string, int> policy_map;
std::map<std::string, std::map<std::string, int>> policy_nolink_map;


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

    TRANS_DATA * p_next_data;

    size_t size, content_size;


    p_trans_data = (TRANS_DATA *) recved_data;

    size = p_trans_data->size;

    trans_type = p_trans_data->type;


    switch (trans_type) {
        case TRANS_TYPE_ACTION: {

            printf("reciving data type: TRANS_TYPE_ACTION\n");

            /*
              * 只有备机会收到此类信息
              */
            action_type = p_trans_data->trans_action_data.type;
            if (action_type == GET_RES) {

            } else if (action_type == FREE_RES) {

            } else {

            }
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
                // nothing!
            } else if (action_type == FREED_RES) {
                // 开始接管资源
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
            backup_server_status = p_trans_data->server_status_datas.server_status;


            // 这里根据决策开始操作,先把结构体填充成无操作
            p_next_data = (TRANS_DATA *)malloc(sizeof(TRANS_DATA));
            trans_data_set_none(p_next_data);

            resource_manager(recved_data, next_send_data);
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

            policy = policy_manager(local_server_status_datas.server_status, local_server_status_datas.have_virtual_ip, true,
                           backup_server_status, backup_have_virtual_ip);

            if( policy == 0 ) {
                trans_data_set_none(next_data);
            } else if (policy == 1) {
                trans_data_set_action(next_data, GET_RES);
            } else if (policy == 2) {
                trans_data_set_action(next_data, FREE_RES);
            }

        }
            break;
        default: {

        }
            break;
    }


    return 0;
}

int policy_manager(bool primary_server_status, bool primary_have_virtual_ip, bool primary_auto_fail_back,
                   bool backup_server_status, bool backup_have_virtual_ip)
{
    // 这里根据８＊８的策略表来写

    return 0;
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

    fp = fopen(POLICY_PATH, "r");

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

            policy_map[skey] = atoi(svalue.c_str());
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
    FILE * fp;
    char str_line[256] = {0};
    std::string sline;
    int tail_pos = -1;
    int space_pos = -1;

    fp = fopen(POLICY_NOLINK_PATH, "r");

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

            // 开始拆分主机和备机 p:1|b:1

            if(space_pos = svalue.find("|"), std::string::npos != space_pos) {
                std::map<std::string, int> tmp_map;
                int m_pos = - 1;
                std::string buf1 = svalue.substr(0, space_pos);
                std::string buf2 = svalue.substr(space_pos+1, svalue.length()-space_pos-1);

                if(m_pos = buf1.find(":"), std::string::npos != m_pos) {
                    std::string p = buf1.substr(0, m_pos);
                    std::string p_value = buf1.substr(m_pos+1, buf1.length()-m_pos-1);

                    p.erase(0, p.find_first_not_of(" "));
                    p.erase(p.find_last_not_of(" ") + 1);

                    p_value.erase(0, p_value.find_first_not_of(" "));
                    p_value.erase(p_value.find_last_not_of(" ") + 1);

                    tmp_map[p] = atoi(p_value.c_str());
                }

                if(m_pos = buf2.find(":"), std::string::npos != m_pos) {
                    std::string b = buf2.substr(0, m_pos);
                    std::string b_value = buf2.substr(m_pos+1, buf2.length()-m_pos-1);

                    b.erase(0, b.find_first_not_of(" "));
                    b.erase(b.find_last_not_of(" ") + 1);

                    b_value.erase(0, b_value.find_first_not_of(" "));
                    b_value.erase(b_value.find_last_not_of(" ") + 1);

                    tmp_map[b] = atoi(b_value.c_str());
                }

                policy_nolink_map[skey] = tmp_map;
            }

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

