#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

typedef int (*PLUGRUN)(void *data);

int main() {
    void *dl;
    dl = dlopen("/opt/infosec-heartbeat/plugins/libplug_icmp.so", RTLD_NOW);

    if (!dl) {
        fprintf(stderr, "%s\n", dlerror());
    }

    void *dl1;
    dl1 = dlopen("/opt/infosec-heartbeat/plugins/libplug_netsignstatus.so", RTLD_NOW);

    PLUGRUN plug_run = (PLUGRUN) dlsym(dl1, "plug_run");

    if (!dl1) {
        fprintf(stderr, "%s\n", dlerror());
    } else {
        printf("11111\n");
    }
    plug_run(NULL);

    return 0;
}