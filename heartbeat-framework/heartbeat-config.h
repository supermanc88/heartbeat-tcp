//
// Created by CHM on 5/21/2020.
//

#ifndef INFOSEC_HEARTBEAT_HEARTBEAT_CONFIG_H
#define INFOSEC_HEARTBEAT_HEARTBEAT_CONFIG_H

#define HEARTBEAT_VERSION 0.1

/**
 * 默认数值
 */
#define KEEYALIVE           2
#define DEADTIME            30
#define WARNTIME            10
#define INITDEAD            120
#define SERVERPORT          8888


#define HACONFIG_FILE_PATH  "/etc/ha.d/ha.cf"
#define HARESOURCES_FILE_PATH "/etc/ha.d/haresources"

/**
 * 传输数据的类型
 * 主机向备机发起action,备机收到后处理并reply，
 * 主机向备机发起获取数据请求getdata,备机收到后replydata
 *
 * TRANS_TYPE_ACTION TRANS_TYPE_REPLY时，TRANS_DATA选用trans_action_data
 * TRANS_TYPE_GET_SERVER_STATUS TRANS_TYPE_REPLY_SERVER_STATUS时，TRANS_DATA选用server_status
 * TRANS_TYPE_GET_DATA TRANS_TYPE_REPLY_DATA时，TRANS_DATA选用data
 * TRANS_TYPE_NONE时，TRANS_DATA中union数据均不使用
 */
enum TRANS_TYPE {
    TRANS_TYPE_HEARTBEAT,
    /********通用发包选择，仅检测是否活跃**********/
    TRANS_TYPE_ACTION,
    TRANS_TYPE_GET_SERVER_STATUS,
    TRANS_TYPE_GET_DATA,
    /************上3为主机发包选择**************/
    TRANS_TYPE_REPLY_ACTION,
    TRANS_TYPE_REPLY_SERVER_STATUS,
    TRANS_TYPE_REPLY_DATA,
    /************上3为备机发包选择**************/
};

enum ACTION_TYPE {
    ACTION_TYPE_GET_RES,            // 主机要求备机接管资源
    ACTION_TYPE_GOT_RES,            // 备机向主机回复已接管资源
    ACTION_TYPE_FREE_RES,           // 主机要求备机释放资源
    ACTION_TYPE_FREED_RES,          // 备机向主机回复已释放资源
};

/**
 * 用来保存发起动作的类型和结果
 * type 操作类型
 * result 操作结果 如接管资源成功或失败
 */
struct TRANS_ACTION_DATA {
    ACTION_TYPE type;
    bool result;
};



struct SERVER_STATUS_DATAS {
    bool server_status;
    bool have_virtual_ip;
};

struct DATA_COLLECTION {
    size_t size;
    unsigned char data[1];
};

/**
 * TRANS_DATA 数据结构体
 * type 传输数据类型
 * size 传输数据总大小
 * trans_action_data 用来保存发起动作的类型和结果
 * server_status 本机服务状态（所有插件运行and结果）
 * data_ollection 传输数据正文
 *
 * 如果type为TRANS_TYPE_NONE时，直接使用data可拿到后面的数据，其它情况使用data时需要
 * 加上TRANS_ACTION_DATA结构体大小，或直接使用extra_data
 */
struct TRANS_DATA {
    TRANS_TYPE type;
    size_t size;
    union {
        struct TRANS_ACTION_DATA trans_action_data;
        struct SERVER_STATUS_DATAS server_status_datas;
//        bool server_status;
        struct DATA_COLLECTION data_ollection;
//        char data[1];
    };
//    char extra_data[1];
};



#endif //INFOSEC_HEARTBEAT_HEARTBEAT_CONFIG_H
