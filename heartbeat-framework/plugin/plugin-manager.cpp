#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

#include "plugin-manager.h"

PLUG_RET load_all_plugin() {
    return PLUG_RET_SUCCESS;
}

PLUG_RET load_plugin() {
    return PLUG_RET_SUCCESS;
}

PLUG_RET plug_init() {
    return PLUG_RET_SUCCESS;
}




int main(void)
{

    // 由于加载so的时候，有个LD_LIBRARY_PATH的环境变量，必须要设置.so所在文件夹路径才能自动加载

    char * env_ld_lib_path = NULL;

    char env_llp[256] = {0};

    char * current_dir;

    void * dl = NULL;

    int ret;

    if((current_dir = getcwd(NULL, 0)) == NULL)
    {
        perror("getcwd error");
    }
    else
    {
        printf("%s\n", current_dir);

        ret = unsetenv("LD_LIBRARY_PATH");

      // 获取 LD_LIBRARY_PATH 环境变量
        env_ld_lib_path = getenv("LD_LIBRARY_PATH");
        if(env_ld_lib_path) {
            strcpy(env_llp, env_ld_lib_path);
            strcat(env_llp, ":");
            strcat(env_llp, current_dir);
//            strcat(env_llp, "/plugins/");
            strcat(env_llp, "/");
        } else {
            strcpy(env_llp, current_dir);
//            strcat(env_llp, "/plugins/");
            strcat(env_llp, "/");
        }

        printf("%s\n", env_llp);

        // 设置环境变量
        ret = setenv("LD_LIBRARY_PATH", env_llp, 1);

        if(ret == -1) {
            perror("setenv error");
            exit(1);
        }

    }


    // 动态加载so库
    int (* add_func)(int, int);

    dl = dlopen("libtest-lib.so", RTLD_LAZY);

    if( dl == NULL ) {
        perror("dlopen error");
        return 0;
    }

    add_func = (int (*)(int, int))dlsym(dl, "test_add");

    int a = add_func(1,2);
    printf("%d\n", a);

    dlclose(dl);

    return 0;
}