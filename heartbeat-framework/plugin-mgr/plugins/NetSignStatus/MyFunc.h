#ifndef _MY_FUNC_
#define _MY_FUNC_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//	写文件
int WtFile(char * filename, unsigned char * pbData,int nDataLen);

//	将内存中数据以十六进制方式打印在字符串中
void SprtHex(char * Deststr,unsigned char * str,int len);

//	打印十六进制数组
void ViewMem(const char * str, unsigned char * p, int len);

//	去除字符串首尾不可见字符
void Trim(char *str);

//	清楚Base64数据
int ClearB64Trash(unsigned char* btSrc, int iSrcLen, unsigned char* buf, int* iBufLen);

//	Base64解码
int Base64Decode(char *src, int src_len, char *dst);
#endif
