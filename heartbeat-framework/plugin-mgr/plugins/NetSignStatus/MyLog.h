#ifndef _MY_LOG_H_
#define _MY_LOG_H_

#include <time.h>  
#include <cstdio>  
#include <cstdlib>  
#include <cstring>   
#include <cstdarg> 

#define FMT_TMP_LEN 256

char * trim(char *str);

void log_info(char * format, ...);
void log_warn(char * format, ...);
void log_error(char * format, ...);
void log_notice(char * format, ...);


#define LOGINFO(A, args...) log_info( (char *)A, ##args)
#define LOGWARN(A, args...) log_warn( (char *)A, ##args)
#define LOGERROR(A, args...)  log_error((char *)A, ##args)
#define LOGNOTICE(A, args...)  log_notice((char *)A, ##args)

#endif

