#ifndef PLUGIN_TEST_HB_LOG_H
#define PLUGIN_TEST_HB_LOG_H


#define LOG_FILE_PATH "/var/log/hb-log"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>

#define MAX_RECORD_LEN	2048	/* 记录最大长度 */

#define D_SUCCEED		0		/* 自定义正确状态值 */
#define D_FAIL			1		/* 自定义错误状态值 */

#define INFO_LEVEL_INFO	"LOG"	/* 普通信息 */
#define INFO_LEVEL_WARN	"WARN"	/* 告警信息 */
#define INFO_LEVEL_ERROR	"ERROR"	/* 严重警告 */
#define INFO_LEVEL_NOTICE "NOTICE"

#define INFO_SOURCE_APP	"app"	/* 进程自身产生的信息 */
#define INFO_SOURCE_DB	"db"	/* 数据库产生的信息 */
#define INFO_SOURCE_SYS	"sys"	/* 除进程自身、数据库以外的信息, 如因进程调度等其他原因产生的信息 */
#define INFO_SOURCE_HBLOG   "HBLOG"
#define INFO_SOURCE_ICMP    "ICMP"
#define INFO_SOURCE_NETSIGN "NETSIGN"


/**
 * 写日志信息
 * @param info_source 信息来源
 * @param info_level 信息级别
 * @param fmt 信息格式
 * @param ...
 * @return D_SUCCEED: 成功, 失败退出
 */
int hb_log(char* info_source, char* info_level, char* fmt, ... );

/**
 * 获取当前系统时间
 * @param outTimeStr 保存时间的字符串
 * @return D_SUCCEED: 成功
 */
int get_current_time(char* outTimeStr );	/* 得到当前时间 */

/**
 * 当日志路径不存在时，创建日志目录
 * @param dir_path 日志存在路径
 * @return mkdir ret value
 */
int makedir(const char * dir_path);


int mmy_system(const char * cmd);

int get_n_days_ago_time(int num, char *outtime);

#endif //PLUGIN_TEST_HB_LOG_H
