#ifndef _MY_FUNC_
#define _MY_FUNC_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//	д�ļ�
int WtFile(char * filename, unsigned char * pbData,int nDataLen);

//	���ڴ���������ʮ�����Ʒ�ʽ��ӡ���ַ�����
void SprtHex(char * Deststr,unsigned char * str,int len);

//	��ӡʮ����������
void ViewMem(const char * str, unsigned char * p, int len);

//	ȥ���ַ�����β���ɼ��ַ�
void Trim(char *str);

//	���Base64����
int ClearB64Trash(unsigned char* btSrc, int iSrcLen, unsigned char* buf, int* iBufLen);

//	Base64����
int Base64Decode(char *src, int src_len, char *dst);
#endif
