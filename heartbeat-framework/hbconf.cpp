#include <iostream>
#include <string.h>
#include "hbconf.h"

HBConfig::HBConfig()
{
    keymap.clear();
    fp = NULL;
}

HBConfig::~HBConfig()
{
    keymap.clear();
    if(fp != NULL) {
        fclose(fp);
        fp = NULL;
    }
}

HB_RET HBConfig::OpenFile(const char *file_path, const char *type)
{
    HB_RET ret = RET_OPENFILE_ERROR;

    fp = fopen(file_path, type);

    if( fp == NULL )
        return ret;


    ret = ParseFile();


    return ret;
}

HB_RET HBConfig::CloseFile()
{
    if( fp != NULL) {
        fclose(fp);
        fp = NULL;
    }
    return RET_SUCCESS;
}

HB_RET HBConfig::ParseFile() {
    HB_RET ret = RET_SUCCESS;
    char str_line[256] = {0};
    std::string sline;
    int tail_pos = -1;
    int space_pos = -1;

    while(fgets(str_line, 256, fp)) {
        // 如果是#开头的，说明是注释的，不需要解析 或者是换行
        if(str_line[0] == '#' || str_line[0] == '\n' || str_line[0] == '\r')
            continue;

        sline.assign(str_line);

        tail_pos = sline.find("\n");
        if(std::string::npos != tail_pos )
            sline.erase(tail_pos, 1);

        tail_pos = sline.find("\r");
        if(std::string::npos != tail_pos )
            sline.erase(tail_pos, 1);

        tail_pos = sline.find("#");
        if(std::string::npos != tail_pos)
            sline.erase(tail_pos);

        if(space_pos = sline.find(" "), std::string::npos != space_pos) {
            std::string skey = sline.substr(0, space_pos);
            std::string svalue = sline.substr(space_pos+1, sline.length()-space_pos-1);

            ClearHeadTailSpace(skey);
            ClearHeadTailSpace(svalue);

            if(skey.compare("node") == 0) {
                if (keymap.count("node") > 0) {
                    keymap["node-backup"] = svalue;
                } else {
                    keymap["node"] = svalue;
                }
            } else {
                keymap[skey] = svalue;
            }

        }
        if(space_pos = sline.find("\t"), std::string::npos != space_pos) {
            std::string skey = sline.substr(0, space_pos);
            std::string svalue = sline.substr(space_pos+1, sline.length()-space_pos-1);

            ClearHeadTailSpace(skey);
            ClearHeadTailSpace(svalue);

            if(skey.compare("node") == 0) {
                if (keymap.count("node") > 0) {
                    keymap["node-backup"] = svalue;
                } else {
                    keymap["node"] = svalue;
                }
            } else {
                keymap[skey] = svalue;
            }
        }

    }


    return ret;
}

std::string &HBConfig::ClearHeadTailSpace(std::string &str) {
    if (str.empty()) {
        return str;
    }
    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
    return str;
}

HB_RET HBConfig::GetValue(const char *key, char *value) {
    bzero(value, BUFSIZ);
    if(keymap.count(key) > 0 ) {
        std::string tmp = keymap[key];
        strcpy(value, tmp.c_str());
        return RET_SUCCESS;
    }
    return RET_GETVALUE_ERROR;
}


//int main(void)
//{
//    HBConfig hb;
//    char value[256] = {0};
//    hb.OpenFile("/home/superman/ha.cf", "r");
//
//    if(hb.GetValue("udpport", value) == RET_SUCCESS)
//        printf("%s\n", value);
//
//    if(hb.GetValue("auto_failback", value) == RET_SUCCESS)
//        printf("%s\n", value);
//
//    return 0;
//}