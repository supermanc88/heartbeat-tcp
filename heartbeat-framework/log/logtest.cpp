#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "hb-log.h"

#define LOGINFO(A, args...) hb_log(PLUG_LOG_FILE_PATH, PLUG_LOG_FILE_PREFIX, INFO_SOURCE_NETSIGN, INFO_LEVEL_INFO, (char *)A, ##args)
#define LOGWARN(A, args...) hb_log(PLUG_LOG_FILE_PATH, PLUG_LOG_FILE_PREFIX, INFO_SOURCE_NETSIGN, INFO_LEVEL_WARN, (char *)A, ##args)
#define LOGERROR(A, args...)  hb_log(PLUG_LOG_FILE_PATH, PLUG_LOG_FILE_PREFIX, INFO_SOURCE_NETSIGN, INFO_LEVEL_ERROR, (char *)A, ##args)

int main()
{

    LOGINFO("%s\n", "11111111111111");
    LOGERROR("%s\n", "22222222222222");

    return 0;
}