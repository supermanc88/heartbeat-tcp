#include "custom-functions.h"

#include <stdlib.h>
#include <stdio.h>

int system_to_file(const char *cmd_string, const char *file_path) {
    char my_cmd_str[BUFSIZ] = {0};
    sprintf(my_cmd_str, "%s > %s 2>&1", cmd_string, file_path);
    return system(my_cmd_str);
}