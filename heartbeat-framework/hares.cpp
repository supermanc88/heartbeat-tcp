#include <strings.h>
#include <string.h>
#include <string>

#include "hares.h"

HBRes::HBRes()
{
    fp = NULL;
    bzero(vip, BUFSIZ);
    bzero(eth, BUFSIZ);
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

        std::string svip;
        std::string seth_and_num;

        int pos = sline.find(" ");

        svip = sline.substr(0, pos);
        seth_and_num = sline.substr(pos+1, sline.length() - pos - 1);

        std::string seth, seth_num;
        pos = seth_and_num.find(":");
        seth = seth_and_num.substr(0, pos);
        seth_num = seth_and_num.substr(pos+1, sline.length() - pos - 1);
        strcpy(eth, seth.c_str());
        strcpy(vip, svip.c_str());
        eth_num = atoi(seth_num.c_str());
        break;
    }
}

void HBRes::close_file()
{
    if(fp != NULL) {
        fclose(fp);
        fp = NULL;
    }
}
