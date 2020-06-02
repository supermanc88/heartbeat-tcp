#ifndef HEARTBEAT_FRAMEWORK_HBCONF_H
#define HEARTBEAT_FRAMEWORK_HBCONF_H

#include <string>
#include <map>
#include <stdio.h>

/**
 * 用来读取heartbeat-ha 原来的config
 */

enum HB_RET{
    RET_SUCCESS,
    RET_OPENFILE_ERROR,
    RET_PARSE_ERROR,
    RET_GETVALUE_ERROR,
};

class HBConfig
{
public:
    HBConfig();

    virtual ~HBConfig();

public:
    HB_RET OpenFile(const char * file_path, const char * type);
    HB_RET CloseFile();
    HB_RET GetValue(const char * key, char * value);

private:
    HB_RET ParseFile();
    std::string & ClearHeadTailSpace (std::string & str);

    // 用来存放从ha.cf中读取的键值对
    std::map<std::string, std::string> keymap;
    FILE * fp;

};


#endif //HEARTBEAT_FRAMEWORK_HBCONF_H
