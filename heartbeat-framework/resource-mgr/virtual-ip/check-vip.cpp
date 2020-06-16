#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "check-vip.h"


int my_system(const char *cmd_string)
{
    char my_cmd_str[CMD_STRING_LEN];
    char * tmp_file = MY_TMP_FILENAME;

    sprintf(my_cmd_str, "%s > %s", cmd_string, tmp_file);

    return system(my_cmd_str);
}



/**
 * 判断本机是否存在vip
 * @param argc
 * @param argv arg[1] 传入vip
 * @return
 * 1 表示存在vip
 * 0 不存在vip
 */
int main(int argc, char * argv[])
{
    char str_line[256] = {0};
    std::string sline;

    // 命令行运行 ip addr，在内容中查找virtual_ip
    my_system("ip addr");

    FILE * fp = fopen(MY_TMP_FILENAME, "r");

    if(fp == NULL) {
        return 0;
    }

    while(fgets(str_line, 256, fp)) {
        sline.assign(str_line);

        if(sline.find(argv[1]) != std::string::npos) {
            printf("has vip\n");
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    printf("do not has vip\n");
    return 0;
}