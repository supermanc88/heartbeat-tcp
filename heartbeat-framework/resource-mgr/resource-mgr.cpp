#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>

#include "resource-mgr.h"


int main(void)
{
    trans_data_generator(NULL, nullptr);
    printf("resource manager\n");
    return 0;
}

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
            // 此类回复没有其它数据
            p_next_data = (TRANS_DATA *)malloc(sizeof(TRANS_DATA));
            bzero(p_next_data, sizeof(TRANS_DATA));
            p_next_data->size = sizeof(TRANS_DATA);
            p_next_data->type = TRANS_TYPE_REPLY_SERVER_STATUS;

            *next_send_data = (void *)p_next_data;
        }
            break;
        case TRANS_TYPE_GET_DATA: {
            // 备机收到，向TRANS_DATA中填充数据
            // 1. 通过插件拿到数据
            // 2. 根据数据大小malloc内存并拷贝
            // 3. 填充其它数据元素
        }
            break;
        case TRANS_TYPE_REPLY_ACTION: {
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
            backup_server_status = p_trans_data->server_status_datas.server_status;


            // 这里根据决策开始操作,先把结构体填充成无操作
            p_next_data = (TRANS_DATA *)malloc(sizeof(TRANS_DATA));
            trans_data_set_none(p_next_data);

            resource_manager(recved_data, next_send_data);
            *next_send_data = (void *)p_next_data;
        }
            break;
        case TRANS_TYPE_REPLY_DATA: {
            content = p_trans_data->data;
            content_size = size - (size_t)(((TRANS_DATA *)0)->data);

            p_next_data = (TRANS_DATA *)malloc(sizeof(TRANS_DATA));
            trans_data_set_none(p_next_data);
            *next_send_data = (void *)p_next_data;
        }
            break;
        case TRANS_TYPE_NONE: {
            p_next_data = (TRANS_DATA *)malloc(sizeof(TRANS_DATA));
            trans_data_set_none(p_next_data);
            *next_send_data = (void *)p_next_data;
        }
            break;
        default: {
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

    pdata->type = TRANS_TYPE_GET_DATA;
    pdata->size = sizeof(TRANS_DATA);

    return 0;
}

int trans_data_set_get_data(void *data) {

    TRANS_DATA * pdata = (TRANS_DATA *) data;

    bzero(pdata, sizeof(TRANS_DATA));

    pdata->type = TRANS_TYPE_GET_SERVER_STATUS;
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
    // 这里运行一次本地的插件获取状态

    data->have_virtual_ip = true;
    data->server_status = true;
    return 0;
}

