#ifndef HEARTBEAT_FRAMEWORK_PLUGIN_MANAGER_H
#define HEARTBEAT_FRAMEWORK_PLUGIN_MANAGER_H

#define PLUGINS_DIRECTORY "/tmp/heartbeat/build/lib/plugins/"


enum PLUG_RET {
    PLUG_RET_SUCCESS,
    PLUG_RET_ERROR,
};

// 插件管理器初始化
PLUG_RET plugin_manager_init();

// 加载所有插件
PLUG_RET load_all_plugin();

// 按照指定文件名加载插件
PLUG_RET load_plugin(char *plugin_path);

// 遍历插件目录
int list_plug_directory(char *dir_path);


// 运行所有的插件 获取结果
int run_all_plugin();

#endif //HEARTBEAT_FRAMEWORK_PLUGIN_MANAGER_H
