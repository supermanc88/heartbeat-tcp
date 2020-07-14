#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "loadconfig.h"

using namespace std;

int connect_timeout = 1500;
int connect_retry = 3;
int ping_timeout = 1500;
int ping_retry = 3;

int debug_mode = 0;
#define IF_DEBUG if(0 != debug_mode)


//	从conf.ini中读取配置参数
int loadconfig() {
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
        }
    }
    fclose(fp);
    return 0;
}

//	去除字符串首尾不可见字符
char *trim_str(char *str) {
    int index = 0, i = 0;

    //去除串首空格
    while (str[index] == ' ') index++;
    for (i = 0; i < strlen(str) - index; i++) str[i] = str[i + index];
    str[i] = '\0';

    //去除串尾空格
    index = strlen(str);
    //while(index>0 && (str[index-1]==' '))  index--;
    while (index > 0 && (0x20 >= (unsigned char) str[index - 1]))
        index--;
    str[index] = '\0';

    return str;
}
