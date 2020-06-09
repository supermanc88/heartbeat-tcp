#include <stdio.h>
#include <unistd.h>

#include "resource-mgr.h"
#include "heartbeat-config.h"


int main(void)
{
    resource_manager(NULL, 0);
    printf("resource manager\n");
    return 0;
}

int resource_manager(void *data, size_t size)
{
    TRANS_DATA * p_trans_data = (TRANS_DATA *) data;

    TRANS_TYPE trans_type = p_trans_data->type;

    ACTION_TYPE action_type;
    bool server_status;
    char * content;

    size_t content_size;

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
            server_status = p_trans_data->server_status;
        }
            break;
        case TRANS_TYPE_GET_DATA: {
            // 备机收到，向TRANS_DATA中填充数据
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
