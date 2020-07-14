#include <unistd.h>
#include <string.h>

#include "test-lib.h"
#include "hb-plugin.h"


int test_add(int a, int b) {
    return a + b;
}

int plug_init(void *data) {

    PLUG_DATA *pdata = (PLUG_DATA *) data;

    pdata->initStruct.sdkVersion = 1;
    pdata->initStruct.pluginVersion = 1;
    pdata->initStruct.pluginHandle = NULL;

    memcpy(pdata->initStruct.pluginName, "test-lib", strlen("test-lib"));

    write(STDOUT_FILENO, "hello plug init", strlen("hello plug init"));
    return 0;
}

int plug_stop(void) {
    write(STDOUT_FILENO, "hello plug stop", strlen("hello plug stop"));
    return 0;
}

int plug_run(void *data) {

    write(STDOUT_FILENO, "hello plug run", strlen("hello plug run"));
    return 0;
};