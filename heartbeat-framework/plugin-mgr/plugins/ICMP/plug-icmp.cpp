#include "plug-icmp.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <string>
#include <netdb.h>


#include "plugin-mgr/hb-plugin.h"

#include "heartbeat-config.h"
#include "hbconf.h"
#include "common/custom-functions.h"
#include "loadconfig.h"
#include "log/hb-log.h"

char ping_target[BUFSIZ] = "192.168.1.1";
char router_ethernet_name[BUFSIZ] = "eth0";
extern int ping_timeout;
extern int ping_retry;

int plug_init(void *data) {

    char ucast[BUFSIZ] = {0};
    PLUG_DATA *pdata = (PLUG_DATA *) data;

    pdata->initStruct.sdkVersion = 1;
    pdata->initStruct.pluginVersion = 1;
    pdata->initStruct.pluginHandle = 0;

    memcpy(pdata->initStruct.pluginName, "icmp", strlen("icmp"));


    hb_log(PLUG_LOG_FILE_PATH, PLUG_LOG_FILE_PREFIX, INFO_SOURCE_HEARTBEAT, INFO_LEVEL_INFO, "icmp plug init\n");


    HBConfig hb;
    if (RET_SUCCESS == hb.OpenFile(HACONFIG_FILE_PATH, "r")) {
        char value[BUFSIZ] = {0};
        if (hb.GetValue("ping", value) == RET_SUCCESS)
            strcpy(ping_target, value);
        if (hb.GetValue("ucast", value) == RET_SUCCESS)
            strcpy(ucast, value);
    }
    hb.CloseFile();


    std::string str_ucast;
    str_ucast.assign(ucast);
    int offset = str_ucast.find(" ");
    if (offset != std::string::npos) {
        strcpy(router_ethernet_name, str_ucast.substr(0, offset).c_str());
    }


    loadconfig();

    hb_log(PLUG_LOG_FILE_PATH, PLUG_LOG_FILE_PREFIX, INFO_SOURCE_HEARTBEAT, INFO_LEVEL_INFO, "icmp plug ping_timeout = %d, ping_retry = %d\n", ping_timeout,
           ping_retry);

    ping_timeout = ping_timeout / 1000;

    return 0;
}


int plug_stop(void) {
    return 0;
}

int plug_run(void *data) {
    int ret;

    char cmdstr[BUFSIZ] = {0};

    if (gethostbyname2(ping_target, AF_INET6) != NULL) {
        // ipv6

        if (ping_timeout > 0) {
            sprintf(cmdstr, "ping6 -c %d -W %d %s%%%s", ping_retry, ping_timeout, ping_target, router_ethernet_name);
        } else {
            sprintf(cmdstr, "ping6 -c %d %s%%%s", ping_retry, ping_target, router_ethernet_name);
        }

    } else if (gethostbyname2(ping_target, AF_INET) != NULL) {
        // ipv4
        if (ping_timeout > 0) {
            sprintf(cmdstr, "ping -c %d -W %d %s", ping_retry, ping_timeout, ping_target);
        } else {
            sprintf(cmdstr, "ping -c %d %s", ping_retry, ping_target);
        }
    }


    ret = system_to_file(cmdstr, MY_TMP_FILENAME);

    if (ret == -1)
        return 0;
    else {
        if (WIFEXITED(ret)) {
            if (WEXITSTATUS(ret) == 0) {
                LOGINFO("system ping success\n");
                return 1;
            } else {
                LOGINFO("system ping error\n");
                return 0;
            }
        } else
            LOGINFO("system error code = %d\n", WEXITSTATUS(ret));
    }

    return 0;
}


int main(void) {
    plug_run(NULL);
}
