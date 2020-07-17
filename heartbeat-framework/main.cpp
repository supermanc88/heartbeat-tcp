#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>
#include <string>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>


#include "wrap.h"
#include "heartbeat-config.h"
#include "hbconf.h"
#include "resource-mgr/resource-mgr.h"
#include "plugin-mgr/plugin-manager.h"
#include "make-telegram.h"
#include "hares.h"
#include "common/log2file.h"


#define SERVER_IP "192.168.231.133"
#define VIRTUAL_IP "192.168.231.155/24"

// 每发送10次none包，便向服务端询问一次服务状态
#define GET_STATUS_TIME_INTERVAL    10


int keepalive = KEEYALIVE;                                  // 发包间隔
int deadtime = DEADTIME;                                    // 死亡判定时间
int warntime = WARNTIME;                                    // 警告时间：上次发包到现在的时间间隔，超出此时间后，会发出延时警告
int initdead = INITDEAD;                                    // 再上次死亡后，有些系统的网络不会马上恢复，启动heartbaet后，内部自己停顿时长，以确保网络恢复
int detect_interval = 60;                                   // 服务检测间隔时长
int detect_times = detect_interval / keepalive;             // 转换成每发多少次心跳，发一次服务检测请求
int tcpport = SERVERPORT;                                   // tcp端口，以server模式启动时使用
bool auto_failback = true;                                  // 回切
char ping_target[BUFSIZ] = "192.168.231.1";
char peer_addr[BUFSIZ] = SERVER_IP;
char virtual_ip_with_mask[BUFSIZ] = VIRTUAL_IP;
char ethernet_name[BUFSIZ] = "eth0";
int eth_num = 0;
char plugins_dir[BUFSIZ] = "/opt/infosec-heartbeat/plugins/";
int udpport = 694;
char primary_node[BUFSIZ] = "netsign";                      // haresources中主机名


//资源接管状态,资源接管后置为true，释放后置为false
extern bool client_resources_takeover_status;
extern bool server_resources_takeover_status;

bool local_resource_takeover_status;

bool trouble = false;

void usage(void) {
    P2FILE("The startup method is related to whether node and hostname match\n");
}

int start_by_client_mode(void) {
    int cfd;
    int none_package_send_times = 0;

    struct sockaddr_in serv_addr;
    unsigned int i_addr;

    char buf[BUFSIZ];
    TRANS_DATA *next_send_data;

    int n, i, ret;

    // 忽略此信号，进程不终止
    signal(SIGPIPE, SIG_IGN);

    int try_time_sum = 0;
    struct timeval tv;
    bzero(&tv, sizeof(struct timeval));
    tv.tv_sec = deadtime;

    P2FILE("enter start by client\n");

#pragma region client_create_connect    //客户端创建连接
    reconnect:
    cfd = Socket(AF_INET, SOCK_STREAM, 0);

    inet_pton(AF_INET, peer_addr, &i_addr);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(tcpport);
    serv_addr.sin_addr.s_addr = i_addr;


    ret = connect(cfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
#pragma endregion client_create_connect

#pragma region client_connect_fail  // 客户端创建连接失败
    if (ret == -1) {
        P2FILE("connect server error:%s\n", strerror(errno));

        if (!trouble) {
            // 每隔2秒尝试重连一次，当超过deadtime时，就应该接管资源
            if (try_time_sum >= deadtime) {
                // 测试 直接退出 多次尝试均不能连通，所以根据“nolink“策略进行操作资源
                P2FILE("---------------------------------------------------------------------------------------------------------\n");
                P2FILE("| Can not connect through multiple attempts, so operate resources according to the \"nolink\" strategy! |\n");
                P2FILE("---------------------------------------------------------------------------------------------------------\n");

                SERVER_STATUS_DATAS datas = {0};
                get_local_server_status_datas(&datas);

                int act = policy_stand_alone_manager(datas.server_status, datas.have_virtual_ip, 0);
                if (act == NOLINK_ACT_DO_NOTING) {
                    // do nothing
                } else if (act == NOLINK_ACT_TAKEOVER) {
                    if (!client_resources_takeover_status) {
                        take_over_resources(virtual_ip_with_mask, ethernet_name, eth_num);
                        client_resources_takeover_status = true;
                        P2FILE("*****************************\n");
                        P2FILE("* client take over resource *\n");
                        P2FILE("*****************************\n");
                    } else {
                        client_resources_takeover_status = true;
                        P2FILE("*************************************\n");
                        P2FILE("* client take over resource already *\n");
                        P2FILE("*************************************\n");
                    }

                } else {
                    // release
                    if (!client_resources_takeover_status) {
                        client_resources_takeover_status = false;
                        P2FILE("***********************************\n");
                        P2FILE("* client release resource already *\n");
                        P2FILE("***********************************\n");
                    } else {
                        release_resources(virtual_ip_with_mask, ethernet_name);
                        client_resources_takeover_status = false;
                        P2FILE("***************************\n");
                        P2FILE("* client release resource *\n");
                        P2FILE("***************************\n");
                    }

                }

                try_time_sum = 0;

                goto reconnect;
            }
        } else {
            P2FILE("Generated faults manually\n");
        }

        sleep(keepalive);
        try_time_sum += keepalive;
        goto reconnect;
    }
#pragma endregion client_connect_fail

#pragma region client_connect_success   // 客户端创建连接成功

#pragma region client_construct_first_data  // 客户端构造连接成功后的第一次发包内容
    // 向服务端发包
    TRANS_DATA *send_data;
    // 第一次发包时，先询问一次服务端的状态
    send_data = (TRANS_DATA *) malloc(sizeof(TRANS_DATA));
//    trans_data_set_none(send_data);
    trans_data_set_get_server_status(send_data);
#pragma endregion client_construct_first_data
    while (1) {
        if (!trouble) {

#pragma region client_send_data        // 客户端向server发送数据
            // 1. 将要发送的数据序列化
            std::string serialized_data;
            size_t serialized_data_size;

            P2FILE("serilization data start\n");
            make_telegram(send_data, serialized_data, &serialized_data_size);
            P2FILE("serilization data complete\n");


            // 2. 发送数据 如果对端已经关闭，此时write会触发SIGPIPE 进程会自动退出
            // 所以应在程序开始时处理此信号
            // man 2 write
            // EPIPE  fd is connected to a pipe or socket whose reading end is closed.  When this happens the writing process will
            // also receive a SIGPIPE signal.  (Thus, the write return value is seen only if the program catches, blocks or
            //                                                                                                   ignores this signal.)

            n = Write(cfd, (void *) serialized_data.c_str(), serialized_data_size);
            P2FILE("Send %d bytes data to Server\n", n);

            // 释放内存
            if (send_data) {
                P2FILE("free data\n");
                free(send_data);
                send_data = NULL;
            }
            P2FILE("free data done\n");
#pragma endregion client_send_data
            fd_set set;
            FD_ZERO(&set);
            FD_SET(cfd, &set);

            ret = select(cfd + 1, &set, NULL, NULL, &tv);
            P2FILE("select wait time = %d, deadtime = %d, ret = %d\n", tv.tv_sec, deadtime, ret);

            if (ret == -1) {
                P2FILE("--------------------\n");
                P2FILE("| select cfd error |\n");
                P2FILE("--------------------\n");
                // 出错就直接重新和server建立一个连接
                close(cfd);
                goto reconnect;
            } else if (ret == 0) {
#pragma region client_read_timeout  // 客户端等待server返回信息超时
                P2FILE("-----------------------\n");
                P2FILE("| select cfd time out |\n");
                P2FILE("-----------------------\n");

                SERVER_STATUS_DATAS datas = {0};
                get_local_server_status_datas(&datas);

                int act = policy_stand_alone_manager(datas.server_status, datas.have_virtual_ip, 0);
                if (act == NOLINK_ACT_DO_NOTING) {
                    // do nothing
                } else if (act == NOLINK_ACT_TAKEOVER) {
                    if (!client_resources_takeover_status) {
                        take_over_resources(virtual_ip_with_mask, ethernet_name, eth_num);
                        client_resources_takeover_status = true;
                        P2FILE("*****************************\n");
                        P2FILE("* client take over resource *\n");
                        P2FILE("*****************************\n");
                    } else {
                        P2FILE("*************************************\n");
                        P2FILE("* client take over resource already *\n");
                        P2FILE("*************************************\n");
                    }

                } else {
                    // release
                    if (!client_resources_takeover_status) {
                        client_resources_takeover_status = false;
                        P2FILE("***********************************\n");
                        P2FILE("* client release resource already *\n");
                        P2FILE("***********************************\n");
                    } else {
                        release_resources(virtual_ip_with_mask, ethernet_name);
                        client_resources_takeover_status = false;
                        P2FILE("***************************\n");
                        P2FILE("* client release resource *\n");
                        P2FILE("***************************\n");
                    }

                }
                next_send_data = (TRANS_DATA *) malloc(sizeof(TRANS_DATA));
                trans_data_set_none(next_send_data);
                send_data = next_send_data;
                continue;
#pragma endregion client_read_timeout
            } else {
                bzero(buf, BUFSIZ);
                n = Read(cfd, buf, BUFSIZ);

                if (n == 0) {
#pragma region server_closed_connect    // 客户端发现server关闭了连接
                    // 服务端关闭了连接，重新创建socket尝试连接服务端,并接管资源
                    P2FILE("------------------------\n");
                    P2FILE("| server colse connect |\n");
                    P2FILE("------------------------\n");
                    close(cfd);
                    SERVER_STATUS_DATAS datas = {0};
                    get_local_server_status_datas(&datas);

                    int act = policy_stand_alone_manager(datas.server_status, datas.have_virtual_ip, 0);
                    if (act == NOLINK_ACT_DO_NOTING) {
                        // do nothing
                    } else if (act == NOLINK_ACT_TAKEOVER) {
                        if (!client_resources_takeover_status) {
                            take_over_resources(virtual_ip_with_mask, ethernet_name, eth_num);
                            client_resources_takeover_status = true;
                            P2FILE("*****************************\n");
                            P2FILE("* client take over resource *\n");
                            P2FILE("*****************************\n");
                        } else {
                            client_resources_takeover_status = true;
                            P2FILE("*************************************\n");
                            P2FILE("* client take over resource already *\n");
                            P2FILE("*************************************\n");
                        }

                    } else {
                        // release
                        if (!client_resources_takeover_status) {
                            client_resources_takeover_status = false;
                            P2FILE("***********************************\n");
                            P2FILE("* client release resource already *\n");
                            P2FILE("***********************************\n");
                        } else {
                            release_resources(virtual_ip_with_mask, ethernet_name);
                            client_resources_takeover_status = false;
                            P2FILE("***************************\n");
                            P2FILE("* client release resource *\n");
                            P2FILE("***************************\n");
                        }

                    }
                    goto reconnect;
#pragma endregion server_closed_connect
                } else if (n == -1) {
                    P2FILE("read error:%s\n", strerror(errno));
                    close(cfd);
                    goto reconnect;
                } else {
#pragma region client_read_success      // 客户端正常处理从server返回的数据
                    // 开始处理从服务器返回的数据

                    // 1. 反序列化数据
                    unsigned char *parsed_buf;
                    std::string sbuf;
                    sbuf.assign(buf, n);
                    parse_telegram(sbuf, n, (void **) (&parsed_buf));

                    // 2. 根据收到的数据生成下次要发送的数据
                    trans_data_generator(parsed_buf, (void **) (&next_send_data));
                    free(parsed_buf);
                    P2FILE("free parsed buf\n");

                    if (next_send_data->type == TRANS_TYPE_HEARTBEAT) {
                        none_package_send_times++;
                    }

                    if (none_package_send_times >= detect_times) {
                        trans_data_set_get_server_status(next_send_data);
                        none_package_send_times = 0;
                    }

                    send_data = next_send_data;

                    // 休眠 keepalive的时间再发
                    sleep(keepalive);

                    // 3. 转到while开始处序列化数据并发送
#pragma endregion client_read_success
                }

            }
        } else {
            P2FILE("Generated faults manually\n");
            sleep(keepalive);
        }
    }

    close(cfd);
#pragma endregion client_connect_success

    return 0;
}

int start_by_server_mode(void) {
    int lfd, cfd;
    int n, i, ret;
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
    struct timeval tv;

    P2FILE("enter start by server\n");
#pragma region server_pre_create_connect    // 设置端口复用等
    lfd = Socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(tcpport);
//    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr.s_addr);

    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // 设置端口重用
    setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, &serv_addr, sizeof(serv_addr));

    ret = Bind(lfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    if (ret == -1) {
        return -1;
    }

    ret = Listen(lfd, 128);

    if (ret == -1) {
        return -1;
    }

    socklen_t addr_len = sizeof(serv_addr);

    char buf[BUFSIZ];
    memset(buf, 0, BUFSIZ);
#pragma endregion server_pre_create_connect

    while (1) {

        if (!trouble) {
            fd_set set;
            FD_ZERO(&set);
            FD_SET(lfd, &set);
            bzero(&tv, sizeof(struct timeval));
            tv.tv_sec = deadtime;
            P2FILE("select lfd wait %d seconds, lfd = %d\n", tv.tv_sec, lfd);
            ret = select(lfd + 1, &set, NULL, NULL, &tv);

            if (ret == -1) {
                P2FILE("select lfd error\n");
                break;
            } else if (ret == 0) {
#pragma region server_create_connect_timeout
                // 如果在deadtime时间内，客户端未和服务端建立连接，服务端会认为客户端死亡，开始接管资源
                P2FILE("select lfd time out\n");

                SERVER_STATUS_DATAS datas = {0};
                get_local_server_status_datas(&datas);

                int act = policy_stand_alone_manager(datas.server_status, datas.have_virtual_ip, 1);
                if (act == NOLINK_ACT_DO_NOTING) {
                    // do nothing
                } else if (act == NOLINK_ACT_TAKEOVER) {
                    if (!server_resources_takeover_status) {
                        take_over_resources(virtual_ip_with_mask, ethernet_name, eth_num);
                        server_resources_takeover_status = true;
                        P2FILE("*****************************\n");
                        P2FILE("* server take over resource *\n");
                        P2FILE("*****************************\n");
                    } else {
                        server_resources_takeover_status = true;
                        P2FILE("*************************************\n");
                        P2FILE("* server take over resource already *\n");
                        P2FILE("*************************************\n");
                    }

                } else {
                    // release
                    if (!server_resources_takeover_status) {
                        server_resources_takeover_status = false;
                        P2FILE("***********************************\n");
                        P2FILE("* server release resource already *\n");
                        P2FILE("***********************************\n");
                    } else {
                        release_resources(virtual_ip_with_mask, ethernet_name);
                        server_resources_takeover_status = false;
                        P2FILE("***************************\n");
                        P2FILE("* server release resource *\n");
                        P2FILE("***************************\n");
                    }
                }
                continue;
#pragma endregion server_create_connect_timeout
            } else {
#pragma region server_create_connect_success
                P2FILE("a read comming!\n");
                // 在deadtime时间内建立连接
                cfd = Accept(lfd, (struct sockaddr *) &client_addr, &addr_len);
                inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, buf, BUFSIZ);

                // 打印连入的客户端信息
                P2FILE("client addr: %s\n", buf);
                P2FILE("client port: %d\n", ntohs(client_addr.sin_port));

                // 连入的客户端不是ha.cf中的对端ip，直接丢弃
                if (strcmp(buf, peer_addr) != 0) {
                    close(cfd);
                    continue;
                }

                while (1) {

                    // 在正常通信过程中，如果在deadtime时间内未收到客户端发来的消息，便认为客户端死亡，接管资源
                    FD_ZERO(&set);
                    FD_SET(cfd, &set);
                    bzero(&tv, sizeof(struct timeval));
                    tv.tv_sec = deadtime;
                    ret = select(cfd + 1, &set, NULL, NULL, &tv);

                    if (ret == -1) {
                        close(cfd);
                        break;
                    } else if (ret == 0) {
#pragma region server_recv_timeout      // server等待从client来的信息超时
                        P2FILE("time out\n");
                        SERVER_STATUS_DATAS datas = {0};
                        get_local_server_status_datas(&datas);

                        int act = policy_stand_alone_manager(datas.server_status, datas.have_virtual_ip, 1);
                        if (act == NOLINK_ACT_DO_NOTING) {
                            // do nothing
                        } else if (act == NOLINK_ACT_TAKEOVER) {
                            if (!server_resources_takeover_status) {
                                take_over_resources(virtual_ip_with_mask, ethernet_name, eth_num);
                                server_resources_takeover_status = true;
                                P2FILE("*****************************\n");
                                P2FILE("* server take over resource *\n");
                                P2FILE("*****************************\n");
                            } else {
                                server_resources_takeover_status = true;
                                P2FILE("*************************************\n");
                                P2FILE("* server take over resource already *\n");
                                P2FILE("*************************************\n");
                            }

                        } else {
                            // release
                            if (!server_resources_takeover_status) {
                                server_resources_takeover_status = false;
                                P2FILE("***********************************\n");
                                P2FILE("* server release resource already *\n");
                                P2FILE("***********************************\n");
                            } else {
                                release_resources(virtual_ip_with_mask, ethernet_name);
                                server_resources_takeover_status = false;
                                P2FILE("***************************\n");
                                P2FILE("* server release resource *\n");
                                P2FILE("***************************\n");
                            }
                        }

                        close(cfd);
                        break;
#pragma endregion server_recv_timeout
                    } else {
#pragma region server_recv      // server 正常收到从client来的数据
                        bzero(buf, BUFSIZ);
                        n = Read(cfd, buf, BUFSIZ);
                        P2FILE("-------------\n");
                        P2FILE("read num %d\n", n);
                        P2FILE("-------------\n");

                        if (n == 0) {
#pragma region client_closed_connect        // server发现client关闭了连接
                            // 如果客户端关闭的连接，也接管资源
                            P2FILE("-------------------------\n");
                            P2FILE("| client close connect! |\n");
                            P2FILE("-------------------------\n");
                            SERVER_STATUS_DATAS datas = {0};
                            get_local_server_status_datas(&datas);

                            int act = policy_stand_alone_manager(datas.server_status, datas.have_virtual_ip, 1);
                            if (act == NOLINK_ACT_DO_NOTING) {
                                // do nothing
                            } else if (act == NOLINK_ACT_TAKEOVER) {
                                if (!server_resources_takeover_status) {
                                    take_over_resources(virtual_ip_with_mask, ethernet_name, eth_num);
                                    server_resources_takeover_status = true;
                                    P2FILE("*****************************\n");
                                    P2FILE("* server take over resource *\n");
                                    P2FILE("*****************************\n");
                                } else {
                                    server_resources_takeover_status = true;
                                    P2FILE("*************************************\n");
                                    P2FILE("* server take over resource already *\n");
                                    P2FILE("*************************************\n");
                                }

                            } else {
                                // release
                                if (!server_resources_takeover_status) {
                                    server_resources_takeover_status = false;
                                    P2FILE("***********************************\n");
                                    P2FILE("* server release resource already *\n");
                                    P2FILE("***********************************\n");
                                } else {
                                    release_resources(virtual_ip_with_mask, ethernet_name);
                                    server_resources_takeover_status = false;
                                    P2FILE("***************************\n");
                                    P2FILE("* server release resource *\n");
                                    P2FILE("***************************\n");
                                }
                            }
                            close(cfd);
                            break;
#pragma endregion client_closed_connect
                        } else if (n == -1) {
                            P2FILE("read error : %s\n", strerror(errno));
                            close(cfd);
                            break;
                        }

                        // 服务端开始处理收到的数据
                        TRANS_DATA *next_send_data;
                        // 1. 反序列化数据
                        unsigned char *parsed_buf;
                        std::string sbuf;
                        sbuf.assign(buf, n);
                        parse_telegram(sbuf, n, (void **) (&parsed_buf));

                        // 2. 根据收到的数据生成下次要发送的数据
                        trans_data_generator(parsed_buf, (void **) (&next_send_data));
                        free(parsed_buf);

                        // 3. 序列化数据
                        std::string serialized_data;
                        size_t serialized_data_size;
                        make_telegram(next_send_data, serialized_data, &serialized_data_size);

                        // 4. 发送数据
                        n = Write(cfd, (void *) serialized_data.c_str(), serialized_data_size);
                        // 当n=-1的时候，就是发送出错了  不处理此错误，client直接会超时处理

                        P2FILE("----------------------------------------\n");
                        P2FILE("| server send %d bytes datas to client |\n", n);
                        P2FILE("----------------------------------------\n");
#pragma endregion server_recv
                    }

                }
#pragma endregion server_create_connect_success
            }

        } else {
            P2FILE("Generated faults manually\n");
            P2FILE("sleep %d seconds\n", keepalive);
            sleep(keepalive);
        }
    }

    close(lfd);
    close(cfd);

    return 0;
}

/*
 * 手动切换的线程
 */
void *manual_switch(void *) {
    int sfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buf[BUFSIZ];

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(udpport);

    sfd = socket(AF_INET, SOCK_DGRAM, 0);

    bind(sfd, (struct sockaddr *) &server_addr, sizeof(server_addr));

    client_len = sizeof(client_addr);

    while (1) {
        bzero(buf, BUFSIZ);
        int n = recvfrom(sfd, buf, BUFSIZ, 0, (struct sockaddr *) &client_addr, &client_len);
        if (n == -1) {
            perror("recvfrom error");
        }
        P2FILE("manual_switch thread : recvfrom buf = %s\n", buf);
        if (strcmp(buf, "standby") == 0) {
            trouble = true;
            release_resources(virtual_ip_with_mask, ethernet_name);
        } else if (strcmp(buf, "takeover") == 0) {
            trouble = true;
            take_over_resources(virtual_ip_with_mask, ethernet_name, eth_num);
        } else {
            // do nothing
        }

    }

    return NULL;
}


int main(int argc, char *argv[]) {
    char opt;
    char mode[20];
    bool b_mode_set = false;

    bzero(mode, 20);

#pragma region set_log_path
//    set_plug_log_path_and_prefix("/var/log", "ha-log");
#pragma endregion set_log_path

#pragma region write_pidfile
    // 写pid到/var/run/infosec-heartbeat.pid

    FILE *pid_fp = fopen("/var/run/infosec-heartbeat.pid", "a");

    if (pid_fp == NULL) {
        return -1;
    }
    fprintf(pid_fp, "%d", getpid());
    fclose(pid_fp);
#pragma endregion write_pidfile

#pragma region main_read_config
    // 读取配置,不成功就使用默认配置
    HBConfig hb_config;
    char p_hostname[BUFSIZ];
    char b_hostname[BUFSIZ];
    char ucast[BUFSIZ];
    if (RET_SUCCESS == hb_config.OpenFile(HACONFIG_FILE_PATH, "r")) {
        char value[BUFSIZ] = {0};
        if (hb_config.GetValue("keepalive", value) == RET_SUCCESS)
            keepalive = atoi(value);
        if (hb_config.GetValue("deadtime", value) == RET_SUCCESS)
            deadtime = atoi(value);
        if (hb_config.GetValue("warntime", value) == RET_SUCCESS)
            warntime = atoi(value);
        if (hb_config.GetValue("initdeat", value) == RET_SUCCESS)
            initdead = atoi(value);
        if (hb_config.GetValue("detect_interval", value) == RET_SUCCESS) {
            detect_interval = atoi(value);
            detect_times = detect_interval / keepalive;
        }
        if (hb_config.GetValue("auto_failback", value) == RET_SUCCESS) {
            if (strcmp(value, "on") == 0)
                auto_failback = true;
            else
                auto_failback = false;
        }
        if (hb_config.GetValue("node", value) == RET_SUCCESS)
            strcpy(p_hostname, value);
        if (hb_config.GetValue("node-backup", value) == RET_SUCCESS)
            strcpy(b_hostname, value);
        if (hb_config.GetValue("ping", value) == RET_SUCCESS)
            strcpy(ping_target, value);
        if (hb_config.GetValue("tcpport", value) == RET_SUCCESS)
            tcpport = atoi(value);
        if (hb_config.GetValue("ucast", value) == RET_SUCCESS)
            strcpy(ucast, value);
        if (hb_config.GetValue("udpport", value) == RET_SUCCESS)
            udpport = atoi(value);
    }
    hb_config.CloseFile();

    P2FILE("-------------------------------------------------------------------\n");
    P2FILE("deadtime = %d, keepalive = %d\n", deadtime, keepalive);
    P2FILE("primary hostname = %s, backup hostname = %s\n", p_hostname, b_hostname);
    P2FILE("ping_target = %s, tcpport = %d\n", ping_target, tcpport);
    P2FILE("ucast = %s\n", ucast);
    P2FILE("udpport = %d\n", udpport);
    P2FILE("-------------------------------------------------------------------\n");


    std::string saddr;
    saddr.assign(ucast);
    int offset = saddr.find(" ");
    if (offset != std::string::npos) {
        strcpy(peer_addr, saddr.substr(offset + 1).c_str());
    }

    P2FILE("peer_addr = %s\n", peer_addr);
    P2FILE("-------------------------------------------------------------------\n");


    HBRes hb_res;
    hb_res.open_file(HARESOURCES_FILE_PATH);
    hb_res.get_virtual_ip(virtual_ip_with_mask);
    hb_res.get_ethernet_name(ethernet_name, &eth_num);
    hb_res.get_primary_node(primary_node);
    hb_res.close_file();

    P2FILE("virtual_ip_with_mask = %s\n", virtual_ip_with_mask);
    P2FILE("ethernet_name = %s\n", ethernet_name);
    P2FILE("eth_num = %d\n", eth_num);
    P2FILE("primary_node = %s\n", primary_node);
    P2FILE("---------------------------------------\n");

#pragma endregion main_read_config

#pragma region start_thread

    pthread_t tid;
    int ret = pthread_create(&tid, NULL, manual_switch, NULL);
    if (ret != 0) {
        perror("pthread_create error");
        return -1;
    }

#pragma endregion start_thread

    // 启动之前检查hostname是否匹配

    // 获取hostname 用户判断主备机
    char hostname[BUFSIZ];
    gethostname(hostname, BUFSIZ);

    P2FILE("hostname = %s\n", hostname);
    P2FILE("---------------------------------------\n");

    bzero(mode, 0);
//    if (strcmp(hostname, p_hostname) == 0) {
//        strcpy(mode, "client");
//    } else if (strcmp(hostname, b_hostname) == 0){
//        strcpy(mode, "server");
//    } else {
//        printf("Configuration error, hostname does not match\n");
//        return 0;
//    }

    // 如果所有node和primary_node都不匹配，则不能成功启动
    if (strcmp(p_hostname, primary_node) != 0 &&
        strcmp(b_hostname, primary_node) != 0) {
        P2FILE("ha.cf node and haresources node not match!\n");
        return -1;
    }

    // 如果本机hostname和ha.cf中所有的节点都不匹配的话，也不能启动
    if (strcmp(p_hostname, hostname) != 0 &&
        strcmp(b_hostname, hostname) != 0) {
        P2FILE("localhost name and ha.cf node not match!\n");
        return -1;
    }

    if (strcmp(hostname, primary_node) == 0) {
        strcpy(mode, "client");
    } else
        strcpy(mode, "server");

    P2FILE("start mode = %s\n", mode);
    P2FILE("---------------------------------------\n");

#pragma region start // 启动
    // 初始化所有的策略文件
    policy_init();


    // 初始化插件管理器
    plugin_manager_init();
    load_all_plugin();

    if (strcmp(mode, "client") == 0)
        start_by_client_mode();
    else if (strcmp(mode, "server") == 0)
        start_by_server_mode();
    else {
        usage();
        exit(1);
    }
#pragma endregion start

    return 0;
}