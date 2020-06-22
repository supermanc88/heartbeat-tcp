//
// Created by CHM on 2020/6/22.
//

#include "make-telegram.h"
#include "telegram.pb.h"
#include "heartbeat-config.h"


int make_telegram(void *data, void **telegram_data, size_t *t_data_size)
{
    TRANS_DATA *pdata = (TRANS_DATA *) data;

    TransData telegram_transdata;

    *t_data_size = telegram_transdata.ByteSizeLong();

    unsigned char *t_data_array = (unsigned char *) malloc(*t_data_size);

    switch (pdata->type) {
        case TRANS_TYPE_ACTION: {
            telegram_transdata.set_type(ACTION);
            telegram_transdata.set_size(pdata->size);
            telegram_transdata.mutable_trans_action_data()->set_type(
                    static_cast<ActionType>(pdata->trans_action_data.type));
            telegram_transdata.mutable_trans_action_data()->set_result(pdata->trans_action_data.result);
        }
            break;
        case TRANS_TYPE_REPLY_ACTION: {
            telegram_transdata.set_type(REPLY_ACTION);
            telegram_transdata.set_size(pdata->size);
            telegram_transdata.mutable_trans_action_data()->set_type(
                    static_cast<ActionType>(pdata->trans_action_data.type));
            telegram_transdata.mutable_trans_action_data()->set_result(pdata->trans_action_data.result);
        }
            break;
        case TRANS_TYPE_GET_SERVER_STATUS: {
            telegram_transdata.set_type(GET_SERVER_STATUS);
            telegram_transdata.set_size(pdata->size);
            telegram_transdata.mutable_server_status_datas()->set_server_status(
                    pdata->server_status_datas.server_status
            );
            telegram_transdata.mutable_server_status_datas()->set_have_virtual_ip(
                    pdata->server_status_datas.have_virtual_ip
            );
        }
            break;
        case TRANS_TYPE_REPLY_SERVER_STATUS: {
            telegram_transdata.set_type(REPLY_SERVER_STATUS);
            telegram_transdata.set_size(pdata->size);
            telegram_transdata.mutable_server_status_datas()->set_server_status(
                    pdata->server_status_datas.server_status
            );
            telegram_transdata.mutable_server_status_datas()->set_have_virtual_ip(
                    pdata->server_status_datas.have_virtual_ip
            );
        }
            break;
        case TRANS_TYPE_GET_DATA: {
            telegram_transdata.set_type(GET_DATA);
            telegram_transdata.set_size(pdata->size);
            telegram_transdata.mutable_data_collection()->assign(pdata->data,
                                                                 pdata->size - (size_t) (&((TRANS_DATA *) 0)->data));
        }
            break;
        case TRANS_TYPE_REPLY_DATA: {
            telegram_transdata.set_type(REPLY_DATA);
            telegram_transdata.set_size(pdata->size);
            telegram_transdata.mutable_data_collection()->assign(pdata->data,
                                                                 pdata->size - (size_t) (&((TRANS_DATA *) 0)->data));
        }
            break;
        default: {
            telegram_transdata.set_type(HEARTBEAT);
            telegram_transdata.set_size(pdata->size);
        }
            break;
    }
    telegram_transdata.SerializeToArray(t_data_array, *t_data_size);
    *telegram_data = t_data_array;

    return 0;
}

int parse_telegram(void *telegram_data, size_t t_data_size, void **data)
{
    TransData transdata;
    transdata.ParseFromArray(telegram_data, t_data_size);

    TRANS_DATA * pdata = (TRANS_DATA *)malloc(sizeof(TRANS_DATA));
    bzero(pdata, sizeof(TRANS_DATA));

    switch (transdata.type()) {
        case ACTION: {
            pdata->type = TRANS_TYPE_ACTION;
            pdata->size = transdata.size();
            pdata->trans_action_data.type = static_cast<ACTION_TYPE>(transdata.mutable_trans_action_data()->type());
            pdata->trans_action_data.result = transdata.mutable_trans_action_data()->result();
        }
            break;
        case REPLY_ACTION: {
            pdata->type = TRANS_TYPE_REPLY_ACTION;
            pdata->size = transdata.size();
            pdata->trans_action_data.type = static_cast<ACTION_TYPE>(transdata.mutable_trans_action_data()->type());
            pdata->trans_action_data.result = transdata.mutable_trans_action_data()->result();
        }
            break;
        case GET_SERVER_STATUS: {
            pdata->type = TRANS_TYPE_GET_SERVER_STATUS;
            pdata->size = transdata.size();
            pdata->server_status_datas.server_status = transdata.mutable_server_status_datas()->server_status();
            pdata->server_status_datas.have_virtual_ip = transdata.mutable_server_status_datas()->have_virtual_ip();
        }
            break;
        case REPLY_SERVER_STATUS: {
            pdata->type = TRANS_TYPE_GET_SERVER_STATUS;
            pdata->size = transdata.size();
            pdata->server_status_datas.server_status = transdata.mutable_server_status_datas()->server_status();
            pdata->server_status_datas.have_virtual_ip = transdata.mutable_server_status_datas()->have_virtual_ip();
        }
            break;
        case GET_DATA: {
            pdata->type = TRANS_TYPE_GET_DATA;
            pdata->size = transdata.size();
        }
            break;
        case REPLY_DATA: {
            pdata->type = TRANS_TYPE_GET_DATA;
            pdata->size = transdata.size();
        }
            break;
        default: {
            pdata->type = TRANS_TYPE_HEARTBEAT;
            pdata->size = transdata.size();
        }
            break;
    }

    *data = pdata;

    return 0;
}
