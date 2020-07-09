#include <strings.h>
#include <string.h>
#include <string>

#include "hares.h"

HBRes::HBRes()
{
    fp = NULL;
    bzero(vip, BUFSIZ);
    bzero(eth, BUFSIZ);
    bzero(node_name, BUFSIZ);
    eth_num = 0;
}

HBRes::~HBRes()
{
    if(fp != NULL) {
        fclose(fp);
        fp = NULL;
    }
}

void HBRes::get_virtual_ip(char *vip)
{
    if (this->vip[0])
        strcpy(vip, this->vip);
}

void HBRes::get_ethernet_name(char *eth, int *eth_num)
{
    if (this->eth[0])
        strcpy(eth, this->eth);
    *eth_num = this->eth_num;
}

void HBRes::open_file(char *filename)
{
    fp = fopen(filename, "r");

    if (fp == NULL)
        return;

    char readlnbuf[BUFSIZ];

    while( fgets(readlnbuf, BUFSIZ, fp) ) {
        if(readlnbuf[0] == '#' || readlnbuf[0] == '\n' || readlnbuf[0] == '\r')
            continue;

        std::string sline;
        sline.assign(readlnbuf);

        // 读出来的类似 netsign1 IPaddr::192.168.0.153/24/eth0 SendArp::192.168.0.153/eth0 netsignscript.dat
        // 现在要把 netsign1 和 192.168.0.153/24/eth0 拿出来

        // 解析出node
        std::string snode;
        snode = sline.substr(0, sline.find(" "));
        strcpy(node_name, snode.c_str());

        // 解析IPaddr
        std::string slaststring = sline.substr(sline.find(" ")+1);
        std::string spartofipaddr = slaststring.substr(0, slaststring.find(" "));

        std::string sipaddr = spartofipaddr.substr(spartofipaddr.find_last_of(":")+1);


#pragma region parse_IPaddr
        std::string svip;
        std::string seth_and_num;

        int pos = sipaddr.find_last_of("/");

        svip = sipaddr.substr(0, pos);
        seth_and_num = sipaddr.substr(pos+1, sipaddr.length() - pos - 1);

        std::string seth, seth_num;
        pos = seth_and_num.find(":");
        if (pos != std::string::npos) {
            seth = seth_and_num.substr(0, pos);
            seth_num = seth_and_num.substr(pos+1, sipaddr.length() - pos - 1);
            strcpy(eth, seth.c_str());
            strcpy(vip, svip.c_str());
            eth_num = atoi(seth_num.c_str());
        } else {
            // 没有找到冒号分隔，则整个就是网卡名
            strcpy(eth, seth_and_num.c_str());

            // 自己通过ifconfig找一个可以用的eth_num
            char my_cmd_str[256];
            sprintf(my_cmd_str, "%s %s > %s", "ifconfig | grep", eth, "/tmp/ifconfig.tmp");
            system(my_cmd_str);

            FILE * fp = fopen("/tmp/ifconfig.tmp", "r");

            if(fp == NULL) {
                eth_num = 0;
                break;
            }
            char str_line[256];
            while(fgets(str_line, 256, fp)) {
                sline.clear();
                sline.assign(str_line);
                std::string se = sline.substr(0, sline.find(" "));

                if (se.find(":") == std::string::npos)
                    continue;
                seth_num = se.substr(se.find(":")+1);
                if( atoi(seth_num.c_str()) > eth_num )
                    eth_num = atoi(seth_num.c_str());
            }
            eth_num++;
            fclose(fp);
        }
        break;
#pragma endregion parse_IPaddr
    }
}

void HBRes::close_file()
{
    if(fp != NULL) {
        fclose(fp);
        fp = NULL;
    }
}

void HBRes::get_primary_node(char *node_name)
{
    if (node_name[0]) {
        strcpy(node_name, this->node_name);
    }
}
