#include "hb_manually.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

#include "hbconf.h"

#define HACONFIG_FILE_PATH  "/etc/ha.d/ha.cf"

int main(int argc, char *argv[]) {
    int sfd;
    char buf[BUFSIZ];
    char ip_addr[BUFSIZ];

    if (argc != 4)
        exit(1);

    char *ip = argv[1];
    char *operation = argv[2];

    if (operation[strlen(operation)] == '\n') {
        operation[strlen(operation)] = '\0';
    }

    HBConfig hb_config;
    int tcpport, udpport, commonport;
    char value[20] = {0};
    // 如果有设置tcpport 就使用tcpport， 没有则使用udpport
    if (hb_config.GetValue("tcpport", value) == RET_SUCCESS) {
        tcpport = atoi(value);
        commonport = tcpport;
    } else {
        if (hb_config.GetValue("udpport", value) == RET_SUCCESS)
            udpport = atoi(value);
        commonport = udpport;
    }



    if (strcmp(ip, "localhost") == 0) {
        strcpy(ip_addr, "127.0.0.1");
    } else {
        char ucast[BUFSIZ];
        if (RET_SUCCESS == hb_config.OpenFile(HACONFIG_FILE_PATH, "r")) {
            char value[BUFSIZ] = {0};
            if (hb_config.GetValue("ucast", value) == RET_SUCCESS)
                strcpy(ucast, value);
        }
        std::string saddr;
        saddr.assign(ucast);
        int offset = saddr.find(" ");
        if (offset != std::string::npos) {
            strcpy(ip_addr, saddr.substr(offset + 1).c_str());
        }
        hb_config.CloseFile();

        printf("remoteip = %s\n", ip_addr);
    }

    hb_config.CloseFile();

    printf("ip = %s, port = %d, operation = %s\n", ip_addr, commonport, operation);

    sfd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr.s_addr);
    serv_addr.sin_port = htons(commonport);

    if (strstr(operation, "takeover"))
        operation = "takeover";
    else
        operation = "standby";

    int ret = sendto(sfd, operation, strlen(operation), 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    if (ret == -1) {
        perror("sendto error");
    } else {
        printf("sendto %d bytes\n", ret);
    }

    return 0;
}
