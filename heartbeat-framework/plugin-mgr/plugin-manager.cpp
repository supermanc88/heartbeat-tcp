#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <string>
#include <dlfcn.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <map>

#include "plugin-manager.h"
#include "hb-plugin.h"


std::vector<std::string> plugins_path;       // 用来存储插件的所有路径

// 觉得不太需要用map 先使用vector
std::map<std::string, PLUG_DATA> plugin_map;

std::vector<PLUG_DATA> plug_data_vector;

PLUG_RET load_all_plugin()
{
    int i;
//    list_plug_directory(PLUGINS_DIRECTORY);

    printf("plugins_path.size = %d\n", plugins_path.size());

    for ( i = 0; i < plugins_path.size(); i++) {
        load_plugin((char *)plugins_path[i].c_str());
    }

    return PLUG_RET_SUCCESS;
}

PLUG_RET load_plugin(char *plugin_path)
{
    void *dl = NULL;

    dl  = dlopen(plugin_path, RTLD_NOW);

    if (!dl) {
        printf("load plugin error\n");
        return PLUG_RET_ERROR;
    }

    printf("load plugin: %s\n", plugin_path);

    struct PLUG_DATA data;

    bzero(&data, sizeof(struct PLUG_DATA));

    memcpy(data.plugpath, plugin_path, strlen(plugin_path));
    memcpy(data.plugname, plugin_path, strlen(plugin_path));


    PLUGINIT plug_init = (PLUGINIT)dlsym(dl, "plug_init");
    PLUGSTOP plug_stop = (PLUGSTOP)dlsym(dl, "plug_stop");
    PLUGRUN  plug_run = (PLUGRUN)dlsym(dl, "plug_run");

    data.plug_init = plug_init;
    data.plug_stop = plug_stop;
    data.plug_run = plug_run;


    // 填充结构体中需要初始化的内容
    plug_init(&data);

    plug_data_vector.push_back(data);

    return PLUG_RET_SUCCESS;
}

PLUG_RET plugin_manager_init()
{

    int i;
    plugins_path.clear();
    plug_data_vector.clear();

    // 先根据插件的路径 把 LD_LIBRARY_PATH 的环境变量设置好

    // 在这里设置环境变量经测试是不生效的，应该在的父进程设置环境变量，此进程就会用父进程的环境变量
    // 所以在加载so的时候，需要使用全路径，不能只有一个文件名


    // First traverse your own plugin directory, load all plugins

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

        // Find the .so file in the file list
        // and judge based on whether the last three characters are .so
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

int run_all_plugin() {

    int result = 1;
    int i = 0;

//    std::map<std::string , PLUG_DATA> ::iterator iter;
//
//    iter = plugin_map.begin();
//
//    while (iter != plugin_map.end()) {
//        result & iter->second.plug_run(NULL);
//    }

    for( i= 0; i < plug_data_vector.size(); i++) {
        int tmp = plug_data_vector[i].plug_run(NULL);
        printf("current runing plug:%s, and result = %d\n", plug_data_vector[i].plugname, tmp);

//        result &= plug_data_vector[i].plug_run(NULL);
        result &= tmp;
    }

    printf("result = %d\n", result);

    return result;
}


//int main(void)
//{
//    plugin_manager_init();
//
//    load_all_plugin();
//
//    run_all_plugin();
//
//    return 0;
//}