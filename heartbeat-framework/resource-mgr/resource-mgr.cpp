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
    bool server_status;
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
//            server_status = p_trans_data->server_status;
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
        case TRANS_TYPE_REPLY: {
            /*
             * 只有主机会收到此类信息
             */
            action_type = p_trans_data->trans_action_data.type;
            if (action_type == GOT_RES) {

            } else if (action_type == FREED_RES) {

            } else {

            }
        }
            break;
        case TRANS_TYPE_REPLY_SERVER_STATUS: {
            server_status = p_trans_data->server_status;
        }
            break;
        case TRANS_TYPE_REPLY_DATA: {
            content = p_trans_data->data;
            content_size = size - (size_t)(((TRANS_DATA *)0)->data);
        }
            break;
        case TRANS_TYPE_NONE: {

        }
            break;
        default:
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

