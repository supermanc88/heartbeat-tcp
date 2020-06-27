#include "MyFunc.h"

//	写文件
int WtFile(char * filename, unsigned char * pbData,int nDataLen)
{
	FILE *stream = NULL;
	if( ( stream = fopen( filename, "w+" ) ) == NULL )
		return -1;
	fwrite(pbData, 1, nDataLen, stream);
	fclose( stream );
	
	return 0;
}

//将内存中数据以十六进制方式打印在字符串中
void SprtHex(char * Deststr,unsigned char * str,int len)
{
	int i=0;
	for(i=0;i<len;i++)
	{
		sprintf(Deststr+i*5,"0x%02x ",str[i]);
	}
	printf("\n");
}

//	打印十六进制数组
void ViewMem(const char * str, unsigned char * p, int len)
{
	int i=0;
	printf("\n------Name:%s--Len:%d----\n",str,len);
	for(;i<len;i++)
	{
		if(0 == i%8)
			printf("\n");
		printf("0x%02x ",*(p+i));
	}
	printf("\n");
}

//	去除字符串首尾不可见字符
void Trim(char *str)
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

}




int IsLegalChar(unsigned char i)
{
	if( i >= 'A' && i <= 'Z' ){//upper case
		return 1;
	}
	if( i >= 'a' && i <= 'z' ){//lower case
		return 1;
	}
	if( i >= '/' && i <= '9' ){//number or /
		return 1;
	}
	if( i == '+' ){
		return 1;
	}
	if( i == '=' )
		return 1 ;

	return 0;
}

//	清除Base64所含垃圾数据
int ClearB64Trash(unsigned char* btSrc, int iSrcLen, unsigned char* buf, int* iBufLen)
{
	int i = 0;
	int j = 0;
	int len = iSrcLen;

	while( i < len )
	{
		if( IsLegalChar(btSrc[i]) )
		{
			buf[j++] = btSrc[i++];
		}else
		{
			i++;
		}
	}

	*iBufLen = j;

	return 0;
}

//	Base64解码
int Base64Decode(char *src, int src_len, char *dst)  
{  

	int i = 0, j = 0;

	unsigned char base64_decode_map[256] = {  

		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 

		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,

		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 62,  255, 255, 255, 63,

		52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  255, 255, 255, 255, 255, 255,

		255, 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,

		15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  255, 255, 255, 255, 255,

		255, 26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,

		41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  255, 255, 255, 255, 255

	};  



	for (; i < src_len - 4; i += 4) {  

		dst[j++] = base64_decode_map[src[i]] << 2 |  

			base64_decode_map[src[i + 1]] >> 4;  

		dst[j++] = base64_decode_map[src[i + 1]] << 4 |  

			base64_decode_map[src[i + 2]] >> 2;  

		dst[j++] = base64_decode_map[src[i + 2]] << 6 |  

			base64_decode_map[src[i + 3]];  

	} 

	dst[j++] = base64_decode_map[src[i]] << 2 |  

		base64_decode_map[src[i + 1]] >> 4;  


	if(src[i + 2] != '='){

		dst[j++] = base64_decode_map[src[i + 1]] << 4 |  

			base64_decode_map[src[i + 2]] >> 2;  

	}

	if(src[i + 3] != '='){

		dst[j++] = base64_decode_map[src[i + 2]] << 6 |  

			base64_decode_map[src[i + 3]];  

	}

	return j;  

}  

