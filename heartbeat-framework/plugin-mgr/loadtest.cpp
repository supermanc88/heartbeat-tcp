#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "hb-plugin.h"

int main() {
    void *dl;
    PLUG_DATA data = {0};
    dl = dlopen("/opt/infosec-heartbeat/plugins/libplug_icmp.so", RTLD_NOW);

    if (!dl) {
        fprintf(stderr, "%s\n", dlerror());
    }

    PLUGRUN plug_run = (PLUGRUN) dlsym(dl, "plug_run");
    PLUGINIT plug_init = (PLUGINIT) dlsym(dl, "plug_init");
    plug_init(&data);
    plug_run(NULL);


    void *dl1;
    dl1 = dlopen("/opt/infosec-heartbeat/plugins/libplug_netsignstatus.so", RTLD_NOW);

    PLUGRUN plug_run1 = (PLUGRUN) dlsym(dl1, "plug_run");

    if (!dl1) {
        fprintf(stderr, "%s\n", dlerror());
    } else {
        printf("11111\n");
    }
    plug_run1(NULL);

    return 0;
}