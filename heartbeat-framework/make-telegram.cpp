/**
 * 序列化和反序列化数据
 */

#include "make-telegram.h"
#include "telegram.pb.h"
#include "heartbeat-config.h"


int make_telegram(void *data, std::string &telegram_data, size_t *t_data_size) {
    TRANS_DATA *pdata = (TRANS_DATA *) data;

    TransData telegram_transdata;

    switch (pdata->type) {
        case TRANS_TYPE_ACTION: {
            telegram_transdata.set_type(ACTION);
            telegram_transdata.set_size(pdata->size);
            telegram_transdata.mutable_trans_action_data()->set_type(
                    static_cast<ActionType>(pdata->trans_action_data.type));
            telegram_transdata.mutable_trans_action_data()->set_result(pdata->trans_action_data.result);
            telegram_transdata.mutable_manual_switch()->set_toggle(pdata->manual_switch.toggle);
            telegram_transdata.mutable_manual_switch()->set_take_or_release(pdata->manual_switch.take_or_release);
        }
            break;
        case TRANS_TYPE_REPLY_ACTION: {
            telegram_transdata.set_type(REPLY_ACTION);
            telegram_transdata.set_size(pdata->size);
            telegram_transdata.mutable_trans_action_data()->set_type(
                    static_cast<ActionType>(pdata->trans_action_data.type));
            telegram_transdata.mutable_trans_action_data()->set_result(pdata->trans_action_data.result);
            telegram_transdata.mutable_manual_switch()->set_toggle(pdata->manual_switch.toggle);
            telegram_transdata.mutable_manual_switch()->set_take_or_release(pdata->manual_switch.take_or_release);
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
            telegram_transdata.mutable_manual_switch()->set_toggle(pdata->manual_switch.toggle);
            telegram_transdata.mutable_manual_switch()->set_take_or_release(pdata->manual_switch.take_or_release);
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
            telegram_transdata.mutable_manual_switch()->set_toggle(pdata->manual_switch.toggle);
            telegram_transdata.mutable_manual_switch()->set_take_or_release(pdata->manual_switch.take_or_release);
        }
            break;
        case TRANS_TYPE_GET_DATA: {
            telegram_transdata.set_type(GET_DATA);
            telegram_transdata.set_size(pdata->size);
            telegram_transdata.mutable_data_collection()->set_size(3);
            telegram_transdata.mutable_data_collection()->set_data("get");
            telegram_transdata.mutable_manual_switch()->set_toggle(pdata->manual_switch.toggle);
            telegram_transdata.mutable_manual_switch()->set_take_or_release(pdata->manual_switch.take_or_release);
        }
            break;
        case TRANS_TYPE_REPLY_DATA: {
            telegram_transdata.set_type(REPLY_DATA);
            telegram_transdata.set_size(pdata->size);
//            telegram_transdata.mutable_data_collection()->assign(pdata->data,
//                                                                 pdata->size - (size_t) (&((TRANS_DATA *) 0)->data));
            telegram_transdata.mutable_data_collection()->set_size(5);
            telegram_transdata.mutable_data_collection()->set_data("reply");
            telegram_transdata.mutable_manual_switch()->set_toggle(pdata->manual_switch.toggle);
            telegram_transdata.mutable_manual_switch()->set_take_or_release(pdata->manual_switch.take_or_release);
        }
            break;
        default: {
            telegram_transdata.set_type(HEARTBEAT);
            telegram_transdata.set_size(pdata->size);
            telegram_transdata.mutable_manual_switch()->set_toggle(pdata->manual_switch.toggle);
            telegram_transdata.mutable_manual_switch()->set_take_or_release(pdata->manual_switch.take_or_release);
        }
            break;
    }
    telegram_transdata.SerializeToString(&telegram_data);
    *t_data_size = telegram_data.length();
    return 0;
}

int parse_telegram(std::string telegram_data, size_t t_data_size, void **data) {
    TransData transdata;
    transdata.ParseFromString(telegram_data);

    TRANS_DATA *pdata = (TRANS_DATA *) malloc(sizeof(TRANS_DATA));
    bzero(pdata, sizeof(TRANS_DATA));

    switch (transdata.type()) {
        case ACTION: {
            pdata->type = TRANS_TYPE_ACTION;
            pdata->size = transdata.size();
            pdata->trans_action_data.type = static_cast<ACTION_TYPE>(transdata.mutable_trans_action_data()->type());
            pdata->trans_action_data.result = transdata.mutable_trans_action_data()->result();
            pdata->manual_switch.toggle = transdata.mutable_manual_switch()->toggle();
            pdata->manual_switch.take_or_release = transdata.mutable_manual_switch()->take_or_release();
        }
            break;
        case REPLY_ACTION: {
            pdata->type = TRANS_TYPE_REPLY_ACTION;
            pdata->size = transdata.size();
            pdata->trans_action_data.type = static_cast<ACTION_TYPE>(transdata.mutable_trans_action_data()->type());
            pdata->trans_action_data.result = transdata.mutable_trans_action_data()->result();
            pdata->manual_switch.toggle = transdata.mutable_manual_switch()->toggle();
            pdata->manual_switch.take_or_release = transdata.mutable_manual_switch()->take_or_release();
        }
            break;
        case GET_SERVER_STATUS: {
            pdata->type = TRANS_TYPE_GET_SERVER_STATUS;
            pdata->size = transdata.size();
            pdata->server_status_datas.server_status = transdata.mutable_server_status_datas()->server_status();
            pdata->server_status_datas.have_virtual_ip = transdata.mutable_server_status_datas()->have_virtual_ip();
            pdata->manual_switch.toggle = transdata.mutable_manual_switch()->toggle();
            pdata->manual_switch.take_or_release = transdata.mutable_manual_switch()->take_or_release();
        }
            break;
        case REPLY_SERVER_STATUS: {
            pdata->type = TRANS_TYPE_REPLY_SERVER_STATUS;
            pdata->size = transdata.size();
            pdata->server_status_datas.server_status = transdata.mutable_server_status_datas()->server_status();
            pdata->server_status_datas.have_virtual_ip = transdata.mutable_server_status_datas()->have_virtual_ip();
            pdata->manual_switch.toggle = transdata.mutable_manual_switch()->toggle();
            pdata->manual_switch.take_or_release = transdata.mutable_manual_switch()->take_or_release();
        }
            break;
        case GET_DATA: {
            pdata->type = TRANS_TYPE_GET_DATA;
            pdata->size = transdata.size();
            pdata->manual_switch.toggle = transdata.mutable_manual_switch()->toggle();
            pdata->manual_switch.take_or_release = transdata.mutable_manual_switch()->take_or_release();
        }
            break;
        case REPLY_DATA: {
            pdata->type = TRANS_TYPE_REPLY_DATA;
            pdata->size = transdata.size();
            pdata->manual_switch.toggle = transdata.mutable_manual_switch()->toggle();
            pdata->manual_switch.take_or_release = transdata.mutable_manual_switch()->take_or_release();
        }
            break;
        default: {
            pdata->type = TRANS_TYPE_HEARTBEAT;
            pdata->size = transdata.size();
            pdata->manual_switch.toggle = transdata.mutable_manual_switch()->toggle();
            pdata->manual_switch.take_or_release = transdata.mutable_manual_switch()->take_or_release();
        }
            break;
    }

    *data = pdata;

    return 0;
}
