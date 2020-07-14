#ifndef PLUGIN_TEST_HB_PLUGIN_H
#define PLUGIN_TEST_HB_PLUGIN_H

#include <unistd.h>
#include <stdio.h>


typedef int (*PLUGINIT)(void *data);

typedef int (*PLUGSTOP)(void);

typedef int (*PLUGRUN)(void *data);



// 下面的这个结构体需要在插件初始化函数调用的时候被填充

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
    char plugpath[256];
    char plugname[256];

    bool is_loaded;

    void *plug_instance;

    PLUGINIT plug_init;       // 插件初始化函数

    PLUGSTOP plug_stop;       // 插件停止函数

    PLUGRUN plug_run;        // 插件运行函数

    PLUG_INITSTRUCT initStruct;
};


#endif //PLUGIN_TEST_HB_PLUGIN_H
