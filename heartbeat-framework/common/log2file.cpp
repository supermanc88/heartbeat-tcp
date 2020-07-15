#include "log2file.h"

extern char log_path[512];
extern char log_prefix[128];


void set_log_path_and_prefix(const char* path, const char* prefix)
{
    strcpy(log_path, path);
    strcpy(log_prefix, prefix);
}