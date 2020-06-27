
#include "MyLog.h"

char fmt_print[] = "%-24s  %-8s  %s \n";

char fmt_str_tmp[FMT_TMP_LEN] = {0x00};

//	去除字符串首尾不可见字符
char * trim(char *str)
{
	int index=0,i=0;

	//去除串首空格 
	while(str[index]==' ') index++;
	for(i=0; i<strlen(str)-index; i++) str[i] = str[i+index]; 
	str[i]='\0';

	//去除串尾空格 
	index= strlen(str);
	//while(index>0 && (str[index-1]==' '))  index--;
	while(index>0 && ( 0x20 >= (unsigned char)str[index-1]))  
		index --;
	str[index]='\0';

	return str;
}

void log_info(char * format, ...)
{
	va_list ap;
	char str_time[64] = {0x00};

	time_t lt =time(NULL);  
	strcpy(str_time, ctime(&lt));

	memset(fmt_str_tmp, 0x00, FMT_TMP_LEN);

	va_start(ap, format);//将ap指向fmt后的第一个参数
	vsprintf(fmt_str_tmp, format, ap);
	va_end(ap);//将ap置为NULL

	printf(fmt_print, trim(str_time), "[INFO]", fmt_str_tmp);
}

void log_warn(char * format, ...)
{
	va_list ap;
	char str_time[64] = {0x00};

	time_t lt =time(NULL);  
	strcpy(str_time, ctime(&lt));

	memset(fmt_str_tmp, 0x00, FMT_TMP_LEN);

	va_start(ap, format);//将ap指向fmt后的第一个参数
	vsprintf(fmt_str_tmp, format, ap);
	va_end(ap);//将ap置为NULL

	printf(fmt_print, trim(str_time), "[WARN]", fmt_str_tmp);
}

void log_error(char * format, ...)
{
	va_list ap;
	char str_time[64] = {0x00};

	time_t lt =time(NULL);  
	strcpy(str_time, ctime(&lt));

	memset(fmt_str_tmp, 0x00, FMT_TMP_LEN);

	va_start(ap, format);//将ap指向fmt后的第一个参数
	vsprintf(fmt_str_tmp, format, ap);
	va_end(ap);//将ap置为NULL

	printf(fmt_print, trim(str_time), "[ERROR]", fmt_str_tmp);
}

void log_notice(char * format, ...)
{
	va_list ap;
	char str_time[64] = {0x00};

	time_t lt =time(NULL);  
	strcpy(str_time, ctime(&lt));

	memset(fmt_str_tmp, 0x00, FMT_TMP_LEN);

	va_start(ap, format);//将ap指向fmt后的第一个参数
	vsprintf(fmt_str_tmp, format, ap);
	va_end(ap);//将ap置为NULL

	printf(fmt_print, trim(str_time), "[NOTICE]", fmt_str_tmp);
}


