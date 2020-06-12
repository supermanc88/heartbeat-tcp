#include <stdio.h>
#include <stdlib.h>

#include "check-vip.h"


int my_system(const char *cmd_string)
{
    char my_cmd_str[CMD_STRING_LEN];
    char * tmp_file = MY_TMP_FILENAME;

    sprintf(my_cmd_str, "%s > %s", cmd_string, tmp_file);

    return system(my_cmd_str);
}



int main(void)
{
    my_system("ping -c 5 192.168.231.33");
    return 0;
}