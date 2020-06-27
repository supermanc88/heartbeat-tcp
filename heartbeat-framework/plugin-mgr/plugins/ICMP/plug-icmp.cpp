#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>

#include "plug-icmp.h"
#include "../../hb-plugin.h"
#include "hb-log.h"
#include "../NetSignStatus/plug-netsignstatus.h"


int plug_init(void * data)
{

    PLUG_DATA * pdata = (PLUG_DATA *) data;

    pdata->initStruct.sdkVersion = 1;
    pdata->initStruct.pluginVersion = 1;
    pdata->initStruct.pluginHandle = 0;

    memcpy(pdata->initStruct.pluginName, "icmp", strlen("icmp"));


    hb_log(INFO_SOURCE_APP, INFO_LEVEL_GEN, "icmp plug init\n");

    return 0;
}


int plug_stop(void)
{
    return 0;
}

int plug_run(void * data)
{
    int ret;
    ret = system("ping -c 5 192.168.231.1");

    if( ret == -1 )
        return 0;
    else {
        if (WIFEXITED(ret)) {
            if (WEXITSTATUS(ret) == 0) {
                printf("system ping success\n");
                return 1;
            }
            else {
                printf("system ping error\n");
                return 0;
            }
        } else
            printf("system error code = %d\n", WEXITSTATUS(ret));
    }

    return 0;
}



int main(void)
{
//    PLUG_DATA data = {0};
//    plug_init(&data);

    plug_run(NULL);
}