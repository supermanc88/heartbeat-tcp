#include "MyLog.h"

//	ȥ���ַ�����β���ɼ��ַ�
char * trim(char *str)
{
	int index=0,i=0;

	//ȥ�����׿ո� 
	while(str[index]==' ') index++;
	for(i=0; i<strlen(str)-index; i++) str[i] = str[i+index]; 
	str[i]='\0';

	//ȥ����β�ո� 
	index= strlen(str);
	//while(index>0 && (str[index-1]==' '))  index--;
	while(index>0 && ( 0x20 >= (unsigned char)str[index-1]))  
		index --;
	str[index]='\0';

	return str;
}
