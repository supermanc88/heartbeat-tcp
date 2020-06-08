#ifndef PLUGIN_TEST_HB_LOG_H
#define PLUGIN_TEST_HB_LOG_H


#define LOG_FILE_PATH "/var/log/hb-log"

void hb_log(const char * format, ...);


#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>

#define MAX_RECORD_LEN	2048	/* 记录最大长度 */

#define D_SUCCEED		0		/* 自定义正确状态值 */
#define D_FAIL			1		/* 自定义错误状态值 */

#define INFO_LEVEL_GEN	"gen"	/* 普通信息 */
#define INFO_LEVEL_WARN	"warn"	/* 告警信息 */
#define INFO_LEVEL_EXIT	"exit"	/* 严重警告 */

#define INFO_SOURCE_APP	"app"	/* 进程自身产生的信息 */
#define INFO_SOURCE_DB	"db"	/* 数据库产生的信息 */
#define INFO_SOURCE_SYS	"sys"	/* 除进程自身、数据库以外的信息, 如因进程调度等其他原因产生的信息 */

char dLogPath[512];			/* 日志文件存放路径 */
char dLogPrefix[128];		/* 日志文件前缀 */

int dLog( char* dIfomSource, char* dIfomLevel, char* dFmt, ... );
int getCurrTime( char* outTimeStr );	/* 得到当前时间 */

#endif //PLUGIN_TEST_HB_LOG_H
