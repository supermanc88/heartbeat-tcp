#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main() {
    void *dl;
    dl = dlopen("/tmp/heartbeat-tcp/build/lib/plugins/libplug_icmp.so", RTLD_NOW);

    if (!dl) {
        fprintf(stderr, "%s\n", dlerror());
    }

    void *dl1;
    dl1 = dlopen("/tmp/heartbeat-tcp/build/lib/plugins/libplug_netsignstatus.so", RTLD_NOW);

    if (!dl1) {
        fprintf(stderr, "%s\n", dlerror());
    } else {
        printf("11111\n");
    }

    return 0;
}