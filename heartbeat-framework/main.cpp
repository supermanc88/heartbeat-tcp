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


#define SERVER_IP "127.0.0.1"

int keepalive = KEEYALIVE;
int deadtime = DEADTIME;
int warntime = WARNTIME;
int initdead = INITDEAD;
int server_port = SERVERPORT;


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

    struct sockaddr_in serv_addr;
    unsigned int i_addr;

    char buf[BUFSIZ];

    int n, i, ret;

    int try_time_sum = 0;

    reconnect:
    cfd = Socket(AF_INET, SOCK_STREAM, 0);

    inet_pton(AF_INET, SERVER_IP, &i_addr);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5555);
    serv_addr.sin_addr.s_addr = i_addr;


    ret = connect(cfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if(ret == -1){
        perror("connect error");

        // 每隔2秒尝试重连一次，当超过deadtime时，就应该接管资源
        if(try_time_sum >= deadtime) {
            // 测试 直接退出 多次尝试均不能连通，退出
            printf("Cannot connect after multiple attempts, exit!\n");

            // 不能连通就直接接管资源
            take_over_resources();

            return 0;
        }
        sleep(keepalive);
        try_time_sum += keepalive;
        goto reconnect;
    }

    // 向服务端发包
    TRANS_DATA trans_data;
    TRANS_DATA * send_data;
    // 一般情况下只发普通的包

    // 开始构造none包
    send_data = &trans_data;
    trans_data_set_none(send_data);
    while(1){
//        fgets(buf, BUFSIZ, stdin);

        n = Write(cfd, send_data, send_data->size);

        n = Read(cfd, buf, n);
        if(n == 0){
            printf("server colse connect\n");
            close(cfd);
            goto reconnect;
            break;
        }
        TRANS_DATA * next_send_data;
        // 开始处理从服务器返回的包
        trans_data_generator(buf, reinterpret_cast<void **>(&next_send_data));

        send_data = next_send_data;

        // 休眠 keepalive的时间再发
        sleep(keepalive);
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
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr.s_addr);

    // 设置端口重用
    setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, &serv_addr, sizeof(serv_addr));

    ret = Bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    ret = Listen(lfd, 128);

    socklen_t addr_len = sizeof(serv_addr);

    char buf[BUFSIZ];
    memset(buf, 0, BUFSIZ);

    while (1){


        cfd = Accept(lfd, (struct sockaddr *)&client_addr, &addr_len);

        inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, buf, BUFSIZ);
        // 打印连入的客户端信息
        printf("client addr: %s\n", buf);
        printf("client port: %d\n", ntohs(client_addr.sin_port));

        // select系统调用的的用途是：在一段指定的时间内，监听用户感兴趣的文件描述符上可读、可写和异常等事件。
        // 使用select监控client发来的内容，在一定的时间内

        while (1){

            fd_set set;
            FD_ZERO(&set);
            FD_SET(cfd, &set);

            tv.tv_sec = 20;
            ret = select(cfd+1, &set, NULL, NULL, &tv);

            if(ret == -1){
                close(cfd);
                break;
            }else if(ret == 0){
                printf("time out\n");
                close(cfd);
                break;
            } else{
                n = Read(cfd, buf, sizeof(buf));

                if(n == 0){
                    printf("client close connect!\n");
                    close(cfd);
                    break;
                }

//                printf("client input: %s\n", buf);

//                for (i=0; i<n; i++){
//                    buf[i] = toupper(buf[i]);
//                }

                // 服务端开始处理收到的包
                TRANS_DATA * next_send_data;

                trans_data_generator(buf, reinterpret_cast<void **>(&next_send_data));

                n = Write(cfd, next_send_data, next_send_data->size);
            }

        }
    }

    close(lfd);
    close(cfd);

    return 0;
}

int main(int argc, char * argv[])
{
    char opt;
    char mode[20];
    bool b_mode_set = false;


    // 初始化所有的策略文件
    policy_link_init();
    policy_no_link_init();

    bzero(mode, 20);

    while( (opt = getopt(argc, argv, "m:")) != -1 ) {
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
    if(RET_SUCCESS == hb.OpenFile("/etc/ha.d/ha.cf", "r") ) {
        char value[20] = {0};
        if(hb.GetValue("keepalive", value) == RET_SUCCESS)
            keepalive = atoi(value);
        if(hb.GetValue("deadtime", value) == RET_SUCCESS)
            deadtime = atoi(value);
        if(hb.GetValue("warntime", value) == RET_SUCCESS)
            warntime = atoi(value);
        if(hb.GetValue("initdeat", value) == RET_SUCCESS)
            initdead = atoi(value);
    }

    printf("---------------------------------------\n");
    printf("deadtime = %d, keepalice = %d\n", deadtime, keepalive);
    printf("---------------------------------------\n");

    if(b_mode_set) {
// 只有两种启动方式，client server
        if(strcmp(mode, "client") == 0)
            start_by_client_mode();
        else if(strcmp(mode, "server") == 0)
            start_by_server_mode();
        else {
            usage();
            exit(1);
        }
    } else          // 如果没有设置-m参数，则默认以client形式启动
        start_by_client_mode();


    return 0;
}