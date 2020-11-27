#include "custom-functions.h"
//#include "log/hb-log.h"

#include <stdlib.h>
#include <stdio.h>

int system_to_file(const char *cmd_string, const char *file_path) {
    char my_cmd_str[BUFSIZ] = {0};
    sprintf(my_cmd_str, "%s > %s 2>&1", cmd_string, file_path);

//    hb_log(MAIN_LOG_FILE_PATH, MAIN_LOG_FILE_PREFIX, INFO_SOURCE_HEARTBEAT, INFO_LEVEL_INFO, "cmd_str = %s\n", my_cmd_str);

    return system(my_cmd_str);
}