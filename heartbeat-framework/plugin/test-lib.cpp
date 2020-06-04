#include <unistd.h>
#include <string.h>

#include "test-lib.h"
#include "hb-plugin.h"


int test_add(int a, int b)
{
    return a + b;
}

int plug_init(void *data)
{
    write(STDOUT_FILENO, "hello plug init", strlen("hello plug init"));
    return 0;
}

int plug_stop(void)
{
    write(STDOUT_FILENO, "hello plug stop", strlen("hello plug stop"));
    return 0;
}

int plug_run(void *data)
{
    write(STDOUT_FILENO, "hello plug run", strlen("hello plug run"));
    return 0;
};