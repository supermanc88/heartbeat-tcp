#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>


#include "wrap.h"
#include "heartbeat-config.h"
#include "hbconf.h"
#include "resource-mgr/resource-mgr.h"
#include "plugin-mgr/plugin-manager.h"
#include "make-telegram.h"


#define SERVER_IP "192.168.231.132"
#define VIRTUAL_IP "192.168.231.155"

// 每发送10次none包，便向服务端询问一次服务状态
#define GET_STATUS_TIME_INTERVAL    30


int keepalive = KEEYALIVE;
int deadtime = DEADTIME;
int warntime = WARNTIME;
int initdead = INITDEAD;
int server_port = SERVERPORT;
bool auto_failback = true;


//资源接管状态,资源接管后置为true，释放后置为false
extern bool client_resources_takeover_status;
extern bool server_resources_takeover_status;

void usage(void)
{
    printf("heart help\n"
           "heartbeat [-m mode]\n"
           "if no argument, default started by client mode\n"
           "-m mode : start mode client or server\n");
}

int start_by_client_mode(void)
{
    int cfd;
    int none_package_send_times = 0;

    struct sockaddr_in serv_addr;
    unsigned int i_addr;

    char buf[BUFSIZ];
    TRANS_DATA *next_send_data;

    int n, i, ret;

    int try_time_sum = 0;
    struct timeval tv;
    tv.tv_sec = deadtime;

    reconnect:
    cfd = Socket(AF_INET, SOCK_STREAM, 0);

    inet_pton(AF_INET, SERVER_IP, &i_addr);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5555);
    serv_addr.sin_addr.s_addr = i_addr;


    ret = connect(cfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    if (ret == -1) {
        perror("connect server error");

        // 每隔2秒尝试重连一次，当超过deadtime时，就应该接管资源
        if (try_time_sum >= deadtime) {
            // 测试 直接退出 多次尝试均不能连通，所以根据“nolink“策略进行操作资源
            printf("Can not connect through multiple attempts, so operate resources according to the \"nolink\" strategy!\n");

            SERVER_STATUS_DATAS datas = {0};
            get_local_server_status_datas(&datas);

            int act = policy_nolink_manager(datas.server_status, datas.have_virtual_ip, 0);
            if (act == NOLINK_ACT_DO_NOTING) {
                // do nothing
            } else if (act == NOLINK_ACT_TAKEOVER) {
                if (!client_resources_takeover_status) {
                    take_over_resources("192.168.231.155", "ens33");
                    client_resources_takeover_status = true;
                    printf("client take over resource\n");
                } else {
                    printf("client take over resource already\n");
                }

            } else {
                // release
                if (!client_resources_takeover_status) {
                    printf("client release resource already\n");
                } else {
                    release_resources("192.168.231.155", "ens33");
                    client_resources_takeover_status = false;
                    printf("client release resource\n");
                }

            }

            try_time_sum = 0;

            goto reconnect;
        }
        sleep(keepalive);
        try_time_sum += keepalive;
        goto reconnect;
    }

    // 向服务端发包
    TRANS_DATA *send_data;
    // 第一次发包时，先询问一次服务端的状态
    send_data = (TRANS_DATA *) malloc(sizeof(TRANS_DATA));
//    trans_data_set_none(send_data);
    trans_data_set_get_server_status(send_data);
    while (1) {
        unsigned char * serialized_data;
        size_t serialized_data_size;

        make_telegram(send_data, reinterpret_cast<void **>(&serialized_data), &serialized_data_size);

        n = Write(cfd, reinterpret_cast<void *>(serialized_data_size), serialized_data_size);
        // 释放内存
        if (send_data) {
            free(send_data);
            send_data = NULL;
        }

        fd_set set;
        FD_ZERO(&set);
        FD_SET(cfd, &set);

        ret = select(cfd + 1, &set, NULL, NULL, &tv);

        if (ret == -1) {
            printf("select cfd error\n");
            break;
        } else if (ret == 0) {
            printf("select cfd time out\n");
            SERVER_STATUS_DATAS datas = {0};
            get_local_server_status_datas(&datas);

            int act = policy_nolink_manager(datas.server_status, datas.have_virtual_ip, 0);
            if (act == NOLINK_ACT_DO_NOTING) {
                // do nothing
            } else if (act == NOLINK_ACT_TAKEOVER) {
                if (!client_resources_takeover_status) {
                    take_over_resources("192.168.231.155", "ens33");
                    client_resources_takeover_status = true;
                    printf("client take over resource\n");
                } else {
                    printf("client take over resource already\n");
                }

            } else {
                // release
                if (!client_resources_takeover_status) {
                    printf("client release resource already\n");
                } else {
                    release_resources("192.168.231.155", "ens33");
                    client_resources_takeover_status = false;
                    printf("client release resource\n");
                }

            }
            next_send_data = (TRANS_DATA *) malloc(sizeof(TRANS_DATA));
            trans_data_set_none(next_send_data);
            send_data = next_send_data;
            continue;
        } else {
            n = Read(cfd, buf, n);
            if (n == 0) {
                // 服务端关闭了连接，重新创建socket尝试连接服务端,并接管资源
                printf("server colse connect\n");
                close(cfd);
                SERVER_STATUS_DATAS datas = {0};
                get_local_server_status_datas(&datas);

                int act = policy_nolink_manager(datas.server_status, datas.have_virtual_ip, 0);
                if (act == NOLINK_ACT_DO_NOTING) {
                    // do nothing
                } else if (act == NOLINK_ACT_TAKEOVER) {
                    if (!client_resources_takeover_status) {
                        take_over_resources("192.168.231.155", "ens33");
                        client_resources_takeover_status = true;
                        printf("client take over resource\n");
                    } else {
                        printf("client take over resource already\n");
                    }

                } else {
                    // release
                    if (!client_resources_takeover_status) {
                        printf("client release resource already\n");
                    } else {
                        release_resources("192.168.231.155", "ens33");
                        client_resources_takeover_status = false;
                        printf("client release resource\n");
                    }

                }
                goto reconnect;
            }

            // 开始处理从服务器返回的包
            unsigned char * parsed_buf;
            parse_telegram(buf, n, reinterpret_cast<void **>(&parsed_buf));

            trans_data_generator(parsed_buf, reinterpret_cast<void **>(&next_send_data));

            if (next_send_data->type == TRANS_TYPE_HEARTBEAT) {
                none_package_send_times++;
            }

            if (none_package_send_times >= GET_STATUS_TIME_INTERVAL) {
                trans_data_set_get_server_status(next_send_data);
                none_package_send_times = 0;
            }

            send_data = next_send_data;

            // 休眠 keepalive的时间再发
            sleep(keepalive);
        }

    }

    close(cfd);

    return 0;
}

int start_by_server_mode(void)
{
    int lfd, cfd;
    int n, i, ret;
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
    struct timeval tv;


    lfd = Socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5555);
    unsigned int addr;
//    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr.s_addr);

    serv_addr.sin_addr.s_addr = INADDR_ANY;
    // 设置端口重用
    setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, &serv_addr, sizeof(serv_addr));

    ret = Bind(lfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    ret = Listen(lfd, 128);

    socklen_t addr_len = sizeof(serv_addr);

    char buf[BUFSIZ];
    memset(buf, 0, BUFSIZ);

    while (1) {

        fd_set set;
        FD_ZERO(&set);
        FD_SET(lfd, &set);

        tv.tv_sec = deadtime;
        ret = select(lfd + 1, &set, NULL, NULL, &tv);

        if (ret == -1) {
            printf("select lfd error\n");
            break;
        } else if (ret == 0) {
            // 如果在deadtime时间内，客户端未和服务端建立连接，服务端会认为客户端死亡，开始接管资源
            printf("select lfd time out\n");

            SERVER_STATUS_DATAS datas = {0};
            get_local_server_status_datas(&datas);

            int act = policy_nolink_manager(datas.server_status, datas.have_virtual_ip, 0);
            if (act == NOLINK_ACT_DO_NOTING) {
                // do nothing
            } else if (act == NOLINK_ACT_TAKEOVER) {
                if (!server_resources_takeover_status) {
                    take_over_resources("192.168.231.155", "ens33");
                    server_resources_takeover_status = true;
                    printf("server take over resource\n");
                } else {
                    printf("server take over resource already\n");
                }

            } else {
                // release
                if (!server_resources_takeover_status) {
                    printf("server release resource already\n");
                } else {
                    release_resources("192.168.231.155", "ens33");
                    server_resources_takeover_status = false;
                    printf("server release resource\n");
                }
            }
            continue;
        } else {
            // 在deadtime时间内建立连接
            cfd = Accept(lfd, (struct sockaddr *) &client_addr, &addr_len);
            inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, buf, BUFSIZ);

            // 打印连入的客户端信息
            printf("client addr: %s\n", buf);
            printf("client port: %d\n", ntohs(client_addr.sin_port));

            while (1) {

                // 在正常通信过程中，如果在deadtime时间内未收到客户端发来的消息，便认为客户端死亡，接管资源
                FD_ZERO(&set);
                FD_SET(cfd, &set);

                tv.tv_sec = deadtime;
                ret = select(cfd + 1, &set, NULL, NULL, &tv);

                if (ret == -1) {
                    close(cfd);
                    break;
                } else if (ret == 0) {
                    printf("time out\n");
                    SERVER_STATUS_DATAS datas = {0};
                    get_local_server_status_datas(&datas);

                    int act = policy_nolink_manager(datas.server_status, datas.have_virtual_ip, 0);
                    if (act == NOLINK_ACT_DO_NOTING) {
                        // do nothing
                    } else if (act == NOLINK_ACT_TAKEOVER) {
                        if (!server_resources_takeover_status) {
                            take_over_resources("192.168.231.155", "ens33");
                            server_resources_takeover_status = true;
                            printf("server take over resource\n");
                        } else {
                            printf("server take over resource already\n");
                        }

                    } else {
                        // release
                        if (!server_resources_takeover_status) {
                            printf("server release resource already\n");
                        } else {
                            release_resources("192.168.231.155", "ens33");
                            server_resources_takeover_status = false;
                            printf("server release resource\n");
                        }
                    }

                    close(cfd);
                    break;
                } else {
                    n = Read(cfd, buf, sizeof(buf));
                    if (n == 0) {
                        // 如果客户端关闭的连接，也接管资源
                        printf("client close connect!\n");
                        SERVER_STATUS_DATAS datas = {0};
                        get_local_server_status_datas(&datas);

                        int act = policy_nolink_manager(datas.server_status, datas.have_virtual_ip, 0);
                        if (act == NOLINK_ACT_DO_NOTING) {
                            // do nothing
                        } else if (act == NOLINK_ACT_TAKEOVER) {
                            if (!server_resources_takeover_status) {
                                take_over_resources("192.168.231.155", "ens33");
                                server_resources_takeover_status = true;
                                printf("server take over resource\n");
                            } else {
                                printf("server take over resource already\n");
                            }

                        } else {
                            // release
                            if (!server_resources_takeover_status) {
                                printf("server release resource already\n");
                            } else {
                                release_resources("192.168.231.155", "ens33");
                                server_resources_takeover_status = false;
                                printf("server release resource\n");
                            }
                        }
                        close(cfd);
                        break;
                    }
                    // 服务端开始处理收到的包
                    TRANS_DATA *next_send_data;

                    unsigned char * parsed_buf;
                    parse_telegram(buf, n, reinterpret_cast<void **>(&parsed_buf));

                    trans_data_generator(parsed_buf, reinterpret_cast<void **>(&next_send_data));

                    unsigned char * serialized_data;
                    size_t serialized_data_size;

                    make_telegram(next_send_data, reinterpret_cast<void **>(&serialized_data), &serialized_data_size);

                    n = Write(cfd, serialized_data, serialized_data_size);

                    printf("server send %d bytes datas to client\n", n);
                }

            }
        }

    }

    close(lfd);
    close(cfd);

    return 0;
}

int main(int argc, char *argv[])
{
    char opt;
    char mode[20];
    bool b_mode_set = false;


    // 初始化所有的策略文件
    policy_link_init();
    policy_no_link_init();


    // 初始化插件管理器
    plugin_manager_init();
    load_all_plugin();

    bzero(mode, 20);

    while ((opt = getopt(argc, argv, "m:")) != -1) {
        switch (opt) {
            case 'm':
                b_mode_set = true;
                memcpy(mode, optarg, strlen(optarg));
                break;
            case '?':
            default:
                usage();
                exit(1);
                break;
        }
    }

    // 读取配置,不成功就使用默认配置
    HBConfig hb;
    if (RET_SUCCESS == hb.OpenFile("/etc/ha.d/ha.cf", "r")) {
        char value[20] = {0};
        if (hb.GetValue("keepalive", value) == RET_SUCCESS)
            keepalive = atoi(value);
        if (hb.GetValue("deadtime", value) == RET_SUCCESS)
            deadtime = atoi(value);
        if (hb.GetValue("warntime", value) == RET_SUCCESS)
            warntime = atoi(value);
        if (hb.GetValue("initdeat", value) == RET_SUCCESS)
            initdead = atoi(value);
        if (hb.GetValue("auto_failback", value) == RET_SUCCESS) {
            if (strcmp(value, "on") == 0)
                auto_failback = true;
            else
                auto_failback = false;
        }
    }
    hb.CloseFile();

    printf("---------------------------------------\n");
    printf("deadtime = %d, keepalive = %d\n", deadtime, keepalive);
    printf("---------------------------------------\n");

    // 读取补充配置
    HBConfig hb_extra;
    if (RET_SUCCESS == hb_extra.OpenFile("./ha-extra.cf", "r")) {
        char value[20] = {0};
        if (hb_extra.GetValue("mode", value) == RET_SUCCESS)
            strcpy(mode, value);
    }
    hb_extra.CloseFile();

    printf("start mode %s\n", mode);
    printf("---------------------------------------\n");

    if (b_mode_set) {
// 只有两种启动方式，client server
        if (strcmp(mode, "client") == 0)
            start_by_client_mode();
        else if (strcmp(mode, "server") == 0)
            start_by_server_mode();
        else {
            usage();
            exit(1);
        }
    } else          // 如果没有设置-m参数，则默认以client形式启动
        start_by_client_mode();


    return 0;
}