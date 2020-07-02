#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include <string>

#include "hb-log.h"


char log_path[512] = LOG_FILE_PATH;			/* 日志文件存放路径 */
char log_prefix[128] = "hblog";		/* 日志文件前缀 */

int hb_log(char* info_source, char* info_level, char* fmt, ... )
{

    int ret = 0;
    va_list vaList;
    char tmpBuf[ MAX_RECORD_LEN + 1 ];
    char tmpTime[ 14 + 1 ];
    FILE* fp;

    /* 0: print in terminal
     * 1: print in log file */
    int fTorF = (log_path[0] == '\0' || log_prefix[0] == '\0' ) ? 0 : 1; //判断是打印终端还是文件
    memset( tmpTime, 0, sizeof(tmpTime) );
    get_current_time(tmpTime);

    if ( fTorF )
    {

        int ret = makedir(log_path);

        if(!ret)
            printf("create dir success\n");
        else {
//            perror("mkdir error");
            if( errno != EEXIST)
                exit(1);
        }


        sprintf(tmpBuf, "%s/%s%8.8s.runlog", log_path, log_prefix, tmpTime );
        fp = fopen( tmpBuf, "a" );
        if ( NULL == fp )
        {
            fprintf(stderr, "[%4.4s%2.2s%2.2s%2.2s%2.2s%2.2s][%s][%s][%s]\n",
                    tmpTime,
                     tmpTime + 4,
                     tmpTime + 6,
                     tmpTime + 8,
                     tmpTime + 10,
                     tmpTime + 12,
                    INFO_SOURCE_HBLOG, INFO_LEVEL_ERROR, "can not open file" );

            perror("fopen error");
            fclose( fp );
            fp = NULL;
            exit(1);
        }
    }
    sprintf(tmpBuf, "[%4.4s-%2.2s-%2.2s %2.2s:%2.2s:%2.2s][%s][%s]",
            tmpTime,
             tmpTime + 4,
             tmpTime + 6,
             tmpTime + 8,
             tmpTime + 10,
             tmpTime + 12,
            info_source, info_level );
    if ( fTorF )
    {
        fprintf( fp, "%s\n", tmpBuf );
    }
    else
    {
        fprintf( stdout, "%s\n", tmpBuf );
    }
    va_start(vaList, fmt );
    if ( fTorF )
    {
        vfprintf(fp, fmt, vaList );
    }
    else
    {
        vsprintf(tmpBuf, fmt, vaList );
        printf( "%s", tmpBuf );
    }
    va_end( vaList );
    if ( 0 == strcmp(info_level, INFO_LEVEL_ERROR ) )
    {
        sprintf(tmpBuf, "[%4.4s-%2.2s-%2.2s %2.2s:%2.2s:%2.2s][%s][%s][%s]",
                tmpTime,
                 tmpTime + 4,
                 tmpTime + 6,
                 tmpTime + 8,
                 tmpTime + 10,
                 tmpTime + 12,
                info_source, INFO_LEVEL_ERROR, "exit with error" );
        if( fTorF )
        {
            fprintf( fp, "%s\n", tmpBuf );
        }
        else
        {
            printf( "%s\n", tmpBuf );
        }
        fclose( fp );
        fp = NULL;
//        exit(1);
    }
    if ( NULL != fp )
    {
        fclose(fp);
        fp = NULL;
    }
    ret = D_SUCCEED;
    return ret;
}


int get_current_time(char* outTimeStr )
{
    int ret = 0;
    time_t tTime;
    struct tm *tmTime;
    struct timeval mTime;
    time( &tTime );
    tmTime = localtime( &tTime );
    gettimeofday( &mTime, NULL );
    sprintf( outTimeStr, "%04d%02d%02d%02d%02d%02d",
             tmTime->tm_year + 1900, tmTime->tm_mon + 1,
             tmTime->tm_mday, tmTime->tm_hour,
             tmTime->tm_min, tmTime->tm_sec );
    ret = D_SUCCEED;
    return ret;
}

int makedir(const char *dir_path)
{
    int ret;

    std::string tmp_string;

    tmp_string.assign(dir_path);

    if(dir_path[strlen(dir_path)-1] == '/')
        tmp_string.erase(tmp_string.length()-1);

    ret = mkdir(dir_path, 0666);

    if( !ret ) {
        printf("create dir success\n");
    } else {
        if (errno == EEXIST) {
            printf("dir already exist\n");
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


//int main(void)
//{
//    strcpy(log_path, "/var/log/heartbeat");
//    strcpy(log_prefix, "hb-test");
//
//
////    hb_log(INFO_SOURCE_APP, INFO_LEVEL_GEN, "hhhhhhhhhhhhhhhhhhhhhh\n");
////    hb_log(INFO_SOURCE_APP, INFO_LEVEL_WARN, "aaaaaaaaaaaaaaaaaaaaaaaaa\n");
//
////    makedir("/home/superman/111/222/333/4444/");
//
//    return 0;
//}