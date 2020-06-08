#include <stdio.h>
#include <unistd.h>


#include "plug-netsignstatus.h"
#include "../../hb-plugin.h"
#include "hb-log.h"


int plug_init(void * data)
{

    PLUG_DATA * pdata = (PLUG_DATA *) data;

    pdata->initStruct.sdkVersion = 1;
    pdata->initStruct.pluginVersion = 1;
    pdata->initStruct.pluginHandle = 0;

    memcpy(pdata->initStruct.pluginName, "netsignstatus", strlen("netsignstatus"));


    hb_log(INFO_SOURCE_APP, INFO_LEVEL_GEN, "netsignstatus plug init\n");

    return 0;
}


int plug_stop(void)
{
    return 0;
}

int plug_run(void * data)
{
    return 0;
}