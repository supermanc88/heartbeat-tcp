#include "hb_manually.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <string>
#include <net/if.h>
#include <string>

#include "hbconf.h"

#define HACONFIG_FILE_PATH  "/etc/ha.d/ha.cf"

int main(int argc, char *argv[]) {
    int sfd;
    char buf[BUFSIZ] = {0};
    char peer_addr[BUFSIZ] = {0};
    char router_ethernet_name[BUFSIZ] = "eth0";
    int ret = 0;
    bool is_ipv4 = true;

    printf("................\n");

    if (argc != 3)
        exit(1);

    char *ip = argv[1];
    char *operation = argv[2];

    if (operation[strlen(operation)] == '\n') {
        operation[strlen(operation)] = '\0';
    }


    HBConfig hb_config;
    int tcpport, udpport, commonport;
    char value[BUFSIZ] = {0};
    char ucast[BUFSIZ] = {0};
    if (RET_SUCCESS == hb_config.OpenFile(HACONFIG_FILE_PATH, "r")) {
        if (hb_config.GetValue("ucast", value) == RET_SUCCESS)
            strcpy(ucast, value);
        // 如果有设置tcpport 就使用tcpport， 没有则使用udpport
        if (hb_config.GetValue("tcpport", value) == RET_SUCCESS) {
            tcpport = atoi(value);
            commonport = tcpport;
        } else {
            if (hb_config.GetValue("udpport", value) == RET_SUCCESS)
                udpport = atoi(value);
            commonport = udpport;
        }
    }

    hb_config.CloseFile();

    std::string str_ucast;
    str_ucast.assign(ucast);
    int offset = str_ucast.find(" ");
    if (offset != std::string::npos) {
        strcpy(router_ethernet_name, str_ucast.substr(0, offset).c_str());
        strcpy(peer_addr, str_ucast.substr(offset + 1).c_str());
    }

    std::string str_port;
    str_port = std::to_string(commonport);

    struct addrinfo hints, *res, *ressave;

    if (gethostbyname2(peer_addr, AF_INET6) != NULL) {
        // ipv6
        is_ipv4 = false;

    } else if (gethostbyname2(peer_addr, AF_INET) != NULL) {
        // ipv4
        is_ipv4 = true;
    } else {
        printf("ip format error\n");
        return -1;
    }

    if (strcmp(ip, "localhost") == 0) {
        if (is_ipv4) {
            bzero(peer_addr, BUFSIZ);
            strcpy(peer_addr, "127.0.0.1");
        } else {
            bzero(peer_addr, BUFSIZ);
            strcpy(peer_addr, "::1");
        }
    } else{
        printf("remoteip = %s\n", peer_addr);
    }


    printf("ip = %s, port = %d, operation = %s\n", peer_addr, commonport, operation);

    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_IP;
    res = NULL;

    ret = getaddrinfo(peer_addr, str_port.c_str(), &hints, &res);
    if (ret != 0) {
        return -1;
    }

    ressave = res;

    sfd = socket(res->ai_family, SOCK_DGRAM, res->ai_protocol);


    if (strcmp(peer_addr, "127.0.0.1") != 0 &&
        strcmp(peer_addr, "::1") != 0) {

        struct ifreq interface;
        strncpy(interface.ifr_ifrn.ifrn_name, router_ethernet_name, strlen(router_ethernet_name));

        if(setsockopt(sfd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interface, sizeof(interface)) == -1) {
            perror("setsockopt error");
            return  -1;
        }
    }



//    struct sockaddr_in serv_addr;
//
//    serv_addr.sin_family = AF_INET;
//    inet_pton(AF_INET, peer_addr, &serv_addr.sin_addr.s_addr);
//    serv_addr.sin_port = htons(commonport);

    if (strstr(operation, "takeover"))
        operation = "takeover";
    else
        operation = "standby";

    ret = sendto(sfd, operation, strlen(operation), 0, res->ai_addr, res->ai_addrlen);

    close(sfd);

    if (ret == -1) {
        perror("sendto error");
    } else {
        printf("sendto %d bytes\n", ret);
    }

    freeaddrinfo(ressave);

    return 0;
}
