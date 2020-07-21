#include "hbconf.h"

#include <string.h>

HBConfig::HBConfig() {
    keymap.clear();
    fp = NULL;
}

HBConfig::~HBConfig() {
    keymap.clear();
    if (fp != NULL) {
        fclose(fp);
        fp = NULL;
    }
}

HB_RET HBConfig::OpenFile(const char *file_path, const char *type) {
    HB_RET ret = RET_OPENFILE_ERROR;

    fp = fopen(file_path, type);

    if (fp == NULL)
        return ret;


    ret = ParseFile();


    return ret;
}

HB_RET HBConfig::CloseFile() {
    if (fp != NULL) {
        fclose(fp);
        fp = NULL;
    }
    return RET_SUCCESS;
}

HB_RET HBConfig::ParseFile() {
    HB_RET ret = RET_SUCCESS;
    char read_line_buf[256] = {0};
    std::string str_line;
    int tail_pos = -1;
    int space_pos = -1;

    while (fgets(read_line_buf, 256, fp)) {
        // 如果是#开头的，说明是注释的，不需要解析 或者是换行
        if (read_line_buf[0] == '#' || read_line_buf[0] == '\n' || read_line_buf[0] == '\r')
            continue;

        str_line.assign(read_line_buf);

        tail_pos = str_line.find("\n");
        if (std::string::npos != tail_pos)
            str_line.erase(tail_pos, 1);

        tail_pos = str_line.find("\r");
        if (std::string::npos != tail_pos)
            str_line.erase(tail_pos, 1);

        tail_pos = str_line.find("#");
        if (std::string::npos != tail_pos)
            str_line.erase(tail_pos);

        if (space_pos = str_line.find(" "), std::string::npos != space_pos) {
            std::string str_key = str_line.substr(0, space_pos);
            std::string str_value = str_line.substr(space_pos + 1, str_line.length() - space_pos - 1);

            ClearHeadTailSpace(str_key);
            ClearHeadTailSpace(str_value);

            if (str_key.compare("node") == 0) {
                if (keymap.count("node") > 0) {
                    keymap["node-backup"] = str_value;
                } else {
                    keymap["node"] = str_value;
                }
            } else {
                keymap[str_key] = str_value;
            }

        }
        if (space_pos = str_line.find("\t"), std::string::npos != space_pos) {
            std::string str_key = str_line.substr(0, space_pos);
            std::string str_value = str_line.substr(space_pos + 1, str_line.length() - space_pos - 1);

            ClearHeadTailSpace(str_key);
            ClearHeadTailSpace(str_value);

            if (str_key.compare("node") == 0) {
                if (keymap.count("node") > 0) {
                    keymap["node-backup"] = str_value;
                } else {
                    keymap["node"] = str_value;
                }
            } else {
                keymap[str_key] = str_value;
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
    if (keymap.count(key) > 0) {
        std::string tmp = keymap[key];
        strcpy(value, tmp.c_str());
        return RET_SUCCESS;
    }
    return RET_GETVALUE_ERROR;
}
