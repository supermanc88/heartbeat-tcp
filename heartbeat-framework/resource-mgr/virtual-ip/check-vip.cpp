#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "check-vip.h"
#include "../../common/custom-functions.h"


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
    char read_line_buf[256] = {0};
    std::string str_line;

    // 命令行运行 ip addr，在内容中查找virtual_ip
    system_to_file("ip addr", MY_TMP_FILENAME);

    FILE * fp = fopen(MY_TMP_FILENAME, "r");

    if(fp == NULL) {
        return 0;
    }

    while(fgets(read_line_buf, 256, fp)) {
        str_line.assign(read_line_buf);

        if(str_line.find(argv[1]) != std::string::npos) {
//            printf("has vip\n");
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
//    printf("do not has vip\n");
    return 0;
}