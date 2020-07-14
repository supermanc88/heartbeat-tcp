#ifndef HEARTBEAT_TCP_HARES_H
#define HEARTBEAT_TCP_HARES_H

#include <stdio.h>

class HBRes {
public:
    HBRes();

    ~HBRes();

public:
/**
 * 从 haresources中读取vip
 * @param vip
 */
    void get_virtual_ip(char *vip);

/**
 * 从 haresources中读取网卡名
 * @param eth
 */
    void get_ethernet_name(char *eth, int *eth_num);

/**
 * 获取主节点名
 * @param node_name
 */
    void get_primary_node(char *node_name);

    void open_file(char *filename);

    void close_file();

private:

    char vip[BUFSIZ];
    char eth[BUFSIZ];
    char node_name[BUFSIZ];
    int eth_num;
    FILE *fp;
};

#endif //HEARTBEAT_TCP_HARES_H
