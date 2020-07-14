#ifndef _MONITORSRV_H_
#define _MONITORSRV_H_

#include "hb-log.h"

#define MONSRVCONF        "/opt/infosec/NetSignServer52/NetSignServer/haplugin/conf.ini"

#define HA_RESOURCES    "/etc/ha.d/haresources"
#define HA_CONFIG        "/etc/ha.d/ha.cf"
#define HA_HOST            "/etc/hosts"

#define LOGINFO(A, args...) hb_log(INFO_SOURCE_NETSIGN, INFO_LEVEL_INFO, (char *)A, ##args)
#define LOGWARN(A, args...) hb_log(INFO_SOURCE_NETSIGN, INFO_LEVEL_WARN, (char *)A, ##args)
#define LOGERROR(A, args...)  hb_log(INFO_SOURCE_NETSIGN, INFO_LEVEL_ERROR, (char *)A, ##args)

int loadconfig();

char *trim_str(char *str);

#endif