#include "hares.h"

#include <strings.h>
#include <string.h>
#include <string>


HBRes::HBRes() {
    fp = NULL;
    bzero(vip, BUFSIZ);
    bzero(eth, BUFSIZ);
    bzero(node_name, BUFSIZ);
    bzero(script_name, BUFSIZ);
    eth_num = 0;
}

HBRes::~HBRes() {
    if (fp != NULL) {
        fclose(fp);
        fp = NULL;
    }
}

void HBRes::get_virtual_ip(char *vip) {
    if (this->vip[0])
        strcpy(vip, this->vip);
}

void HBRes::get_ethernet_name(char *eth, int *eth_num) {
    if (this->eth[0])
        strcpy(eth, this->eth);
    *eth_num = this->eth_num;
}

void HBRes::open_file(char *filename) {
    fp = fopen(filename, "r");

    if (fp == NULL)
        return;

    char readlnbuf[BUFSIZ];

    while (fgets(readlnbuf, BUFSIZ, fp)) {
        if (readlnbuf[0] == '#' || readlnbuf[0] == '\n' || readlnbuf[0] == '\r')
            continue;

        std::string str_line;
        str_line.assign(readlnbuf);

        // 清除前后的空格
        str_line.erase(0, str_line.find_first_not_of(" "));
        str_line.erase(str_line.find_last_not_of(" ") + 1);


#pragma region get_script_name
        std::string str_script_name = str_line.substr(str_line.rfind(" ") + 1);
        strcpy(script_name, str_script_name.c_str());
#pragma endregion

        // 读出来的类似 netsign1 IPaddr::192.168.0.153/24/eth0 SendArp::192.168.0.153/eth0 netsignscript.dat
        // 现在要把 netsign1 和 192.168.0.153/24/eth0 拿出来

        // 解析出node
        std::string str_node;
        str_node = str_line.substr(0, str_line.find(" "));
        // 解析出 netsign1
        strcpy(node_name, str_node.c_str());

        // 解析IPaddr
        // str_laststring 为 IPaddr::192.168.0.153/24/eth0 SendArp::192.168.0.153/eth0 netsignscript.dat
        std::string str_laststring = str_line.substr(str_line.find(" ") + 1);

        // str_partofipaddr 为 IPaddr::192.168.0.153/24/eth0
        std::string str_partofipaddr = str_laststring.substr(0, str_laststring.find(" "));

        // str_ipaddr 为 192.168.0.153/24/eth0
        std::string str_ipaddr = str_partofipaddr.substr(str_partofipaddr.find("IPaddr::") + strlen("IPaddr::"));


#pragma region parse_IPaddr
        std::string svip;
        std::string seth_and_num;

        int pos = str_ipaddr.find_last_of("/");

        svip = str_ipaddr.substr(0, pos);
        seth_and_num = str_ipaddr.substr(pos + 1, str_ipaddr.length() - pos - 1);
        strcpy(vip, svip.c_str());
        std::string str_eth, str_eth_num;
        pos = seth_and_num.find(":");
        if (pos != std::string::npos) {
            str_eth = seth_and_num.substr(0, pos);
            str_eth_num = seth_and_num.substr(pos + 1, str_ipaddr.length() - pos - 1);
            strcpy(eth, str_eth.c_str());
            eth_num = atoi(str_eth_num.c_str());
        } else {
            // 没有找到冒号分隔，则整个就是网卡名
            strcpy(eth, seth_and_num.c_str());

            // 自己通过ifconfig找一个可以用的eth_num
            char my_cmd_str[256];
            sprintf(my_cmd_str, "%s %s > %s", "ifconfig | grep", eth, "/tmp/ifconfig.tmp");
            system(my_cmd_str);

            FILE *fp = fopen("/tmp/ifconfig.tmp", "r");

            if (fp == NULL) {
                eth_num = 0;
                break;
            }
            eth_num = -1;
            char read_line_buf[256];
            while (fgets(read_line_buf, 256, fp)) {
                str_line.clear();
                str_line.assign(read_line_buf);
                std::string se = str_line.substr(0, str_line.find(" "));

                if (se.find(":") == std::string::npos)
                    continue;
                str_eth_num = se.substr(se.find(":") + 1);
                if (atoi(str_eth_num.c_str()) > eth_num)
                    eth_num = atoi(str_eth_num.c_str());
            }
            eth_num++;
            fclose(fp);
        }
#pragma endregion parse_IPaddr
        break;

    }
}

void HBRes::close_file() {
    if (fp != NULL) {
        fclose(fp);
        fp = NULL;
    }
}

void HBRes::get_primary_node(char *node_name) {
    if (node_name) {
        strcpy(node_name, this->node_name);
    }
}


void HBRes::get_script_name(char *script_name)
{
    if (script_name) {
        strcpy(script_name, this->script_name);
    }
}
