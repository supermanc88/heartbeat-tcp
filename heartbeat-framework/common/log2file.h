#ifndef HEARTBEAT_TCP_LOG2FILE_H
#define HEARTBEAT_TCP_LOG2FILE_H

#include "../log/hb-log.h"

#define P2FILE(A, args...) hb_log(INFO_SOURCE_HEARTBEAT, INFO_LEVEL_INFO, (char *)A, ##args)

void set_log_path_and_prefix(const char* path, const char* prefix);

#endif //HEARTBEAT_TCP_LOG2FILE_H
