#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <dlfcn.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>

#include "plugin-manager.h"

typedef struct {
    //provided by the debugger
    int pluginHandle;
    //provided by the pluginit function
    int sdkVersion;
    int pluginVersion;
    // 前面的这几个暂不使用
    char pluginName[256];
} PLUG_INITSTRUCT;

struct PLUG_DATA {
    char plugpath[BUFSIZ];
    char plugname[BUFSIZ];

    bool is_loaded;

    void *plug_instance;

    PLUGINIT plug_init;       // 插件初始化函数

    PLUGSTOP plug_stop;       // 插件停止函数

    PLUGRUN plug_run;        // 插件运行函数

    PLUG_INITSTRUCT initStruct;
};


std::vector<std::string> plugins_path;       // 用来存储插件的所有路径


PLUG_RET load_all_plugin()
{
    return PLUG_RET_SUCCESS;
}

PLUG_RET load_plugin(char *plugin_path)
{
    return PLUG_RET_SUCCESS;
}

PLUG_RET plugin_manager_init()
{

    int i;

    // 先根据插件的路径 把 LD_LIBRARY_PATH 的环境变量设置好

    // 在这里设置环境变量经测试是不生效的，应该在的父进程设置环境变量，此进程就会用父进程的环境变量
    // 所以在加载so的时候，需要使用全路径，不能只有一个文件名


    // 先遍历自己的插件目录，加载所有插件

    list_plug_directory(PLUGINS_DIRECTORY);

    printf("----------------------------\n");

    for (i = 0; i < plugins_path.size(); i++)
        printf("%s\n", plugins_path[i].c_str());



    return PLUG_RET_SUCCESS;
}

int list_plug_directory(char *dir_path)
{
    if (dir_path == NULL) {
        printf("dir path is NULL\n");
        return -1;
    }


    struct stat st;
    char plug_path_obs[256] = {0};

    lstat(dir_path, &st);

    if (!S_ISDIR(st.st_mode)) {
        printf("path is not a directory\n");
        return -1;
    }


    struct dirent *filename;    // return value for readdir()
    DIR *dir;                   // return value for opendir()
    dir = opendir(dir_path);

    if (dir == NULL) {
        printf("can not open dir\n");
        return -1;
    }

    /* read all the files in the dir ~ */
    while ((filename = readdir(dir)) != NULL) {
        // get rid of "." and ".."
        if (strcmp(filename->d_name, ".") == 0 ||
            strcmp(filename->d_name, "..") == 0)
            continue;
        printf("file name : %s\n", filename->d_name);

        bzero(plug_path_obs, 256);
        strcpy(plug_path_obs, PLUGINS_DIRECTORY);

        // 查找文件列表中的so文件,根据最后三个字符是不是 .so 判断
        int name_len = strlen(filename->d_name);

        if (name_len > 3)
            if (strcmp(".so", &filename->d_name[name_len - 3]) == 0) {

                strcat(plug_path_obs, filename->d_name);

                std::string s_plug_path_obs;

                s_plug_path_obs.assign(plug_path_obs);

                plugins_path.push_back(s_plug_path_obs);
            }
    }

    return 0;
}


int main(void)
{

    // 由于加载so的时候，有个LD_LIBRARY_PATH的环境变量，必须要设置.so所在文件夹路径才能自动加载

//    char *env_ld_lib_path = NULL;
//
//    char env_llp[256] = {0};
//
//    char *current_dir;
//
//    void *dl = NULL;
//
//    int ret, i;
//
//    if ((current_dir = getcwd(NULL, 0)) == NULL) {
//        perror("getcwd error");
//    } else {
//        printf("%s\n", current_dir);
//
//        ret = unsetenv("LD_LIBRARY_PATH");
//
//        // 获取 LD_LIBRARY_PATH 环境变量
//        env_ld_lib_path = getenv("LD_LIBRARY_PATH");
//        if (env_ld_lib_path) {
//            strcpy(env_llp, env_ld_lib_path);
//            strcat(env_llp, ":");
//            strcat(env_llp, current_dir);
////            strcat(env_llp, "/plugins/");
//            strcat(env_llp, "/");
//        } else {
//            strcpy(env_llp, current_dir);
////            strcat(env_llp, "/plugins/");
//            strcat(env_llp, "/");
//        }
//
////        printf("%s\n", env_llp);
//
//        // 设置环境变量
//        ret = setenv("LD_LIBRARY_PATH", env_llp, 1);
//
//        env_ld_lib_path = getenv("LD_LIBRARY_PATH");
//
//        printf("%s\n", env_ld_lib_path);
//
//        if (ret == -1) {
//            perror("setenv error");
//            exit(1);
//        }
//
//    }
//
//
//    // 动态加载so库
//    int (*add_func)(int, int);
//
//    dl = dlopen("/tmp/heartbeat/heartbeat-framework/plugin/cmake-build-debug/libtest-lib.so", RTLD_NOW);
//
//    if (dl == NULL) {
//        perror("dlopen error");
//        printf("%d\n", errno);
//        exit(1);
//    }
//
//    add_func = (int (*)(int, int)) dlsym(dl, "test_add");
//
//    int a = add_func(1, 2);
//    printf("%d\n", a);
//
//    dlclose(dl);

    plugin_manager_init();

    return 0;
}