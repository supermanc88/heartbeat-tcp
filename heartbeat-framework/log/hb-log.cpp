#include "hb-log.h"


void hb_log(const char *format, ...)
{

}


/**********************************************************
    function: dLog
    description: 写日志信息
    Input: dIfomSource: 信息来源
		   dIfomLevel: 信息级别
           dFmt: 信息格式
    Output:
    Return: D_SUCCEED: 成功, 失败退出
    others:
***********************************************************/
int dLog( char* dIfomSource, char* dIfomLevel, char* dFmt, ... )
{
    int ret = 0;
    va_list vaList;
    char tmpBuf[ MAX_RECORD_LEN + 1 ];
    char tmpTime[ 14 + 1 ];
    FILE* fp;

    /* 0: print in terminal
     * 1: print in log file */
    int fTorF = ( dLogPath[0] == '\0' || dLogPrefix[0] == '\0' ) ? 0 : 1; //判断是打印终端还是文件
    memset( tmpTime, 0, sizeof(tmpTime) );
    getCurrTime( tmpTime );

    if ( fTorF )
    {
        sprintf( tmpBuf, "%s/%s%8.8s.runlog", dLogPath, dLogPrefix, tmpTime );
        fp = fopen( tmpBuf, "a" );
        if ( NULL == fp )
        {
            fprintf( stderr, "[%4.4s%2.2s%2.2s%2.2s%2.2s%2.2s][%s][%s][%s]\n",
                     tmpTime,
                     tmpTime + 4,
                     tmpTime + 6,
                     tmpTime + 8,
                     tmpTime + 10,
                     tmpTime + 12,
                     INFO_SOURCE_SYS, INFO_LEVEL_EXIT, "不能打开日志文件" );
            fclose( fp );
            fp = NULL;
            exit(1);
        }
    }
    sprintf( tmpBuf, "[%4.4s%2.2s%2.2s%2.2s%2.2s%2.2s][%s][%s]",
             tmpTime,
             tmpTime + 4,
             tmpTime + 6,
             tmpTime + 8,
             tmpTime + 10,
             tmpTime + 12,
             dIfomSource, dIfomLevel );
    if ( fTorF )
    {
        fprintf( fp, "%s", tmpBuf );
    }
    else
    {
        fprintf( stdout, "%s", tmpBuf );
    }
    va_start( vaList, dFmt );
    if ( fTorF )
    {
        vfprintf( fp, dFmt, vaList );
    }
    else
    {
        vsprintf( tmpBuf, dFmt, vaList );
        printf( "%s", tmpBuf );
    }
    va_end( vaList );
    if ( 0 == strcmp( dIfomLevel, INFO_LEVEL_EXIT ) )
    {
        sprintf( tmpBuf, "[%4.4s%2.2s%2.2s%2.2s%2.2s%2.2s][%s][%s][%s]\n",
                 tmpTime,
                 tmpTime + 4,
                 tmpTime + 6,
                 tmpTime + 8,
                 tmpTime + 10,
                 tmpTime + 12,
                 INFO_SOURCE_SYS, INFO_LEVEL_EXIT, "发生异常情况，程序退出！\n" );
        if( fTorF )
        {
            fprintf( fp, "%s", tmpBuf );
        }
        else
        {
            printf( "%s", tmpBuf );
        }
        fclose( fp );
        fp = NULL;
        exit(1);
    }
    if ( NULL != fp )
    {
        fclose(fp);
        fp = NULL;
    }
    ret = D_SUCCEED;
    return ret;
}

/**********************************************************
    function: getCurrTime
    description: 获取当前系统时间
    Input: outTimeStr: 保存时间的字符串
    Output:
    Return: D_SUCCEED: 成功
    others:
***********************************************************/
int getCurrTime( char* outTimeStr )
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



int main(void)
{
    strcpy(dLogPath, "/var/log");
    strcpy(dLogPrefix, "hb-test");


    dLog(INFO_SOURCE_APP, INFO_LEVEL_GEN, "hhhhhhhhhhhhhhhhhhhhhh\n");
    dLog(INFO_SOURCE_APP, INFO_LEVEL_WARN, "aaaaaaaaaaaaaaaaaaaaaaaaa\n");

    return 0;
}