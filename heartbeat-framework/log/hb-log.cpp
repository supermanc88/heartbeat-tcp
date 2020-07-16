#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include <string>

#include "hb-log.h"

#include "../common/custom-functions.h"

#define DAYS_AGO_TIME_FILE  "/tmp/getndaysagotime.tmp"

int max_keep_day = 30;

int hb_log(char *log_path, char *log_prefix, char *info_source, char *info_level, char *fmt, ...) {

    int ret = 0;
    va_list vaList;
    char tmpBuf[BUFSIZ] = {0};
    char tmpTime[14 + 1] = {0};
    FILE *fp;

    memset(tmpTime, 0, sizeof(tmpTime));
    get_current_time(tmpTime);

    ret = makedir(log_path);

    if (!ret) {
        //        printf("create dir success\n");
    } else {
//            perror("mkdir error");
        if (errno != EEXIST)
            exit(1);
    }

    // 找到 max_keep_day 时的日志文件，并删除
    char days_ago_time[BUFSIZ] = {0};
    if (get_n_days_ago_time(max_keep_day, days_ago_time) == 0) {
        remove("/tmp/getndaysagotime.tmp");

        sprintf(tmpBuf, "%s/%s%8.8s.runlog", log_path, log_prefix, days_ago_time);

        fp = fopen(tmpBuf, "r");

        if (fp != NULL) {
            remove(tmpBuf);
        }
    }

    bzero(tmpBuf, BUFSIZ);

    sprintf(tmpBuf, "%s/%s%8.8s.runlog", log_path, log_prefix, tmpTime);
    fp = fopen(tmpBuf, "a");
    if (NULL == fp) {
        fprintf(stderr, "%4.4s%2.2s%2.2s%2.2s%2.2s%2.2s [%s][%s] %s\n",
                tmpTime,
                tmpTime + 4,
                tmpTime + 6,
                tmpTime + 8,
                tmpTime + 10,
                tmpTime + 12,
                INFO_SOURCE_HBLOG, INFO_LEVEL_ERROR, "can not open file");

        perror("fopen error");
        fclose(fp);
        fp = NULL;
        exit(1);
    }
    sprintf(tmpBuf, "%4.4s-%2.2s-%2.2s %2.2s:%2.2s:%2.2s [%s][%s] ",
            tmpTime,
            tmpTime + 4,
            tmpTime + 6,
            tmpTime + 8,
            tmpTime + 10,
            tmpTime + 12,
            info_source, info_level);

    fprintf(fp, "%s", tmpBuf);

    va_start(vaList, fmt);

    vfprintf(fp, fmt, vaList);

    va_end(vaList);

    if (NULL != fp) {
        fclose(fp);
        fp = NULL;
    }
    ret = D_SUCCEED;
    return ret;
}


int get_current_time(char *outTimeStr) {
    int ret = 0;
    time_t tTime;
    struct tm *tmTime;
    struct timeval mTime;
    time(&tTime);
    tmTime = localtime(&tTime);
    gettimeofday(&mTime, NULL);
    sprintf(outTimeStr, "%04d%02d%02d%02d%02d%02d",
            tmTime->tm_year + 1900, tmTime->tm_mon + 1,
            tmTime->tm_mday, tmTime->tm_hour,
            tmTime->tm_min, tmTime->tm_sec);
    ret = D_SUCCEED;
    return ret;
}

int makedir(const char *dir_path) {
    int ret;

    std::string tmp_string;

    tmp_string.assign(dir_path);

    if (dir_path[strlen(dir_path) - 1] == '/')
        tmp_string.erase(tmp_string.length() - 1);

    ret = mkdir(dir_path, 0666);

    if (!ret) {
//        printf("create dir success\n");
    } else {
        if (errno == EEXIST) {
//            printf("dir already exist\n");
            return 0;
        } else {
//            perror("mkdir error");

            std::string up_dir = tmp_string.substr(0, tmp_string.rfind('/'));

            makedir(up_dir.c_str());

            ret = mkdir(dir_path, 0666);
        }
    }

    return ret;
}

int get_n_days_ago_time(int num, char *outtime) {

    char cmdstr[BUFSIZ] = {0};
    sprintf(cmdstr, "date -d \"-%d day\" +%%Y%%m%%d", num);

    system_to_file(cmdstr, DAYS_AGO_TIME_FILE);

    FILE *fp = fopen(DAYS_AGO_TIME_FILE, "r");

    if (fp == NULL) {
        return -1;
    }

    fgets(outtime, BUFSIZ, fp);

    fclose(fp);

    return 0;
}
