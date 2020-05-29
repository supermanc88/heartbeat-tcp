//
// Created by CHM on 5/21/2020.
//

#include "heartbeat-config.h"

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "wrap.h"

using namespace std;

#define SERVER_IP "127.0.0.1"

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

    reconnect:
    cfd = Socket(AF_INET, SOCK_STREAM, 0);

    inet_pton(AF_INET, SERVER_IP, &i_addr);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5555);
    serv_addr.sin_addr.s_addr = i_addr;

    ret = Connect(cfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if(ret){
        sleep(2);
        close(cfd);
        goto reconnect;
    }

    while(1){
        fgets(buf, BUFSIZ, stdin);
        n = Write(cfd, buf, strlen(buf));

        n = Read(cfd, buf, n);
        if(n == 0){
            printf("server colse connect\n");
            close(cfd);
            goto reconnect;
            break;
        }

        for(i = 0; i < n; i++)
            printf("%c", buf[i]);
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

    ret = Bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    ret = Listen(lfd, 128);

    socklen_t addr_len = sizeof(sockaddr_in);

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
            FD_SET(0, &set);
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

                for (i=0; i<n; i++){
                    buf[i] = toupper(buf[i]);
                }

                n = Write(cfd, buf, n);
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