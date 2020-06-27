#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <dlfcn.h>
#include <semaphore.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/file.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include "MyLog.h"
#include "ErrMsg.h"
#include "MessageIdentifiers.h"
#include "MyFunc.h"

#define MAXCERTBUFFSIZE			102400
#define BufBlockSize			1024

#define COMMLENGTHLEN			12
#define TAGNAMELEN				1
#define TAGCLASSLEN				1
#define TAG_PLAIN_TEXT			10
#define TAG_PLAIN_TEXT_TYPE		"b"

#define	RETURN_CRYPTO			1
#define RETURN_CERT				2
#define RETURN_CERT_PLAIN		3
#define RETURN_CERTS_PLAIN		4

#define SIGN					1
#define SIGN_CERT				2
#define	ENCRYPT_CERT			3


typedef struct result_param_s {/* 证书详细信息 */
	char issuer[256];		/* 颁发者DN*/
	char serialNumber[40];	/* 证书序列号*/
	char subject[256];		/* 证书主题*/
	char notBefore[20];		/* 证书有效期起始时间*/
	char notAfter[20];		/* 证书有效期的终止时间*/
	char signresult[20];	/* 签名结果 */	
	unsigned char returnCert[2048];	/* 证书Der编码*/
	int  iCertLen;			/* 证书Der编码长度*/
} result_param;


typedef struct pdf_sign_param_s{/* PDF签名参数 */
	char passwd[50];
	int x1;/*签名域左下角坐标，从0开始的整数*/ 
	int y1;/*签名域左下角坐标，从0开始的整数*/
	int x2;/*签名域右上角坐标，从0开始的整数*/
	int y2;/*签名域右上角坐标，从0开始的整数*/
	int page; /*签名域页码，从1开始的整数*/
	char stamp[100]; /*图章文件名,图章必须先上传到签名服务器上, 填null时, 自动使用签名服务器配置的默认图章*/
} pdf_sign_param;


typedef struct bar_code_param_s{ /* 条码参数  */
	double barHeight; /*条形码高度*/
	double pixWidth; /*像素宽度,单位毫米*/
	int displayQuitZone;/*是否显示空白区1是0否*/
	double quitZoneWidth; /*空白区域宽度*/
	int displayHumanReadable; /*设置是否显示人工阅读字符*/
	int imageFormat; /*返回的条码图片格式0:png,1:jpeg*/
} bar_code_param;

typedef struct result_transmit_s{
	result_param *sCert;
	result_param *eCert;
	long num;
}Transmit;

typedef struct bar_code_s{ /* 另外条码参数  */
	int errorCorrectLevel;
	float aspectRatio;
	float yHeight;
	int imgFormat;
	int encodeMode;
	int barSize;
} bar_code;


//	Socket试探服务
int DetectConnect(char * IpAddr, int Port, int TimeOut, int * Socket);

void SocketClose(int sockfd);

int DetectResponse(int iSockfd, char* pDetectStr, int iDetectStrlen);


