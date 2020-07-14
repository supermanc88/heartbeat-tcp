#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "loadconfig.h"

using namespace std;

int connect_timeout = 1500;
int connect_retry = 3;
int ping_timeout = 1500;
int ping_retry = 3;

//0为不检查，1为检查
int if_chk_detect_ret = 0;

int detect_strLen = 0;
char detect_string[BUFSIZ] = {0x00};

int base64_detect_strLen = 0;
char base64_detect_string[BUFSIZ] = {0x00};
int b64_det_str_no_trash_len = 0;
char b64_det_str_no_trash[BUFSIZ] = {0x00};

int debug_mode = 0;
#define IF_DEBUG if(0 != debug_mode)

//0 为不开启检查主机服务，1为开启检查主机服务， 
//默认设置为不启用检查是为了防止主机服务绑定在虚IP上。

//如果mon_mode为0，则(监控程序仅在HA主机运行，主机监控本身服务，不正常则切换，正常则往主机回切)
//如果mon_mode为1，则(如果虚IP不在本机则检测有虚IP的机器服务状态，如果在本机则什么都不做)
//如果mon_mode为2，则(本机有虚IP->本机服务不正常->对端服务正常->切换过去)
int mon_mode = 0;



//	从conf.ini中读取配置参数
int loadconfig()
{
    char Buff[BUFSIZ] = {0x00};

    char ConfBuff[BUFSIZ] = {0x00};

    FILE *fp = fopen(MONSRVCONF, "r");
    if (NULL == fp) {
        LOGERROR("%s\n", "Open monsrvconf err");
        return -1;
    }
    char *pName = NULL;
    char *pValue = NULL;

    while (!feof(fp)) {
        memset(Buff, 0x00, BUFSIZ);
        fgets(Buff, BUFSIZ, fp);

        pName = strtok(Buff, "=");
        if (NULL == pName) continue;

        pValue = strtok(NULL, "=");
        if (NULL == pValue) continue;

        if (0 == strcmp(pName, "connecttimeout")) {
            connect_timeout = atoi(trim_str(pValue));
        } else if (0 == strcmp(pName, "connectretrytimes")) {
            connect_retry = atoi(trim_str(pValue));
        }
        if (0 == strcmp(pName, "pingtimeout")) {
            ping_timeout = atoi(trim_str(pValue));
        } else if (0 == strcmp(pName, "pingretrytimes")) {
            ping_retry = atoi(trim_str(pValue));
        } else if (0 == strcmp(pName, "checkdetectret")) {
            if (0 == strcmp("YES", trim_str(pValue))) {
                //	LOGINFO("CheckDetectRet:%s","YES");
                if_chk_detect_ret = 1;
            } else if (0 == strcmp("NO", trim_str(pValue))) {
                //	LOGINFO("CheckDetectRet:%s","NO");
                if_chk_detect_ret = 0;
            } else {
                LOGERROR("%s\n", "MonsrvConf Parameter Err");
                fclose(fp);
                return -1;
            }
        }
    }
    fclose(fp);
    return 0;
}

//	去除字符串首尾不可见字符
char * trim_str(char *str)
{
    int index=0,i=0;

    //去除串首空格
    while(str[index]==' ') index++;
    for(i=0; i<strlen(str)-index; i++) str[i] = str[i+index];
    str[i]='\0';

    //去除串尾空格
    index= strlen(str);
    //while(index>0 && (str[index-1]==' '))  index--;
    while(index>0 && ( 0x20 >= (unsigned char)str[index-1]))
        index --;
    str[index]='\0';

    return str;
}
