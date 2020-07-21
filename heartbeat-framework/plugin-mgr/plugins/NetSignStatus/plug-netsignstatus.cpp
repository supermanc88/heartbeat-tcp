#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>

#include "plug-netsignstatus.h"
#include "plugin-mgr/hb-plugin.h"
#include "log/hb-log.h"
#include "MonitorSrv.h"


int plug_init(void *data)
{

    PLUG_DATA *pdata = (PLUG_DATA *) data;

    pdata->initStruct.sdkVersion = 1;
    pdata->initStruct.pluginVersion = 1;
    pdata->initStruct.pluginHandle = 0;

    memcpy(pdata->initStruct.pluginName, "netsignstatus", strlen("netsignstatus"));


    hb_log(PLUG_LOG_FILE_PATH, PLUG_LOG_FILE_PREFIX, INFO_SOURCE_HEARTBEAT, INFO_LEVEL_INFO, "netsignstatus plug init\n");

    return 0;
}


int plug_stop(void)
{
    return 0;
}

int plug_run(void *data)
{
    monitor_srv_init();

    int ret = GetSrvStatus("127.0.0.1", NULL);
    return (ret == 0 ? 1 : 0);
}

int main()
{
    monitor_srv_init();

    int ret = GetSrvStatus("127.0.0.1", NULL);
    return (ret == 0 ? 1 : 0);
}
