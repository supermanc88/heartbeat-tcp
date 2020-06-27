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


typedef struct result_param_s {/* ֤����ϸ��Ϣ */
	char issuer[256];		/* �䷢��DN*/
	char serialNumber[40];	/* ֤�����к�*/
	char subject[256];		/* ֤������*/
	char notBefore[20];		/* ֤����Ч����ʼʱ��*/
	char notAfter[20];		/* ֤����Ч�ڵ���ֹʱ��*/
	char signresult[20];	/* ǩ����� */	
	unsigned char returnCert[2048];	/* ֤��Der����*/
	int  iCertLen;			/* ֤��Der���볤��*/
} result_param;


typedef struct pdf_sign_param_s{/* PDFǩ������ */
	char passwd[50];
	int x1;/*ǩ�������½����꣬��0��ʼ������*/ 
	int y1;/*ǩ�������½����꣬��0��ʼ������*/
	int x2;/*ǩ�������Ͻ����꣬��0��ʼ������*/
	int y2;/*ǩ�������Ͻ����꣬��0��ʼ������*/
	int page; /*ǩ����ҳ�룬��1��ʼ������*/
	char stamp[100]; /*ͼ���ļ���,ͼ�±������ϴ���ǩ����������, ��nullʱ, �Զ�ʹ��ǩ�����������õ�Ĭ��ͼ��*/
} pdf_sign_param;


typedef struct bar_code_param_s{ /* �������  */
	double barHeight; /*������߶�*/
	double pixWidth; /*���ؿ��,��λ����*/
	int displayQuitZone;/*�Ƿ���ʾ�հ���1��0��*/
	double quitZoneWidth; /*�հ�������*/
	int displayHumanReadable; /*�����Ƿ���ʾ�˹��Ķ��ַ�*/
	int imageFormat; /*���ص�����ͼƬ��ʽ0:png,1:jpeg*/
} bar_code_param;

typedef struct result_transmit_s{
	result_param *sCert;
	result_param *eCert;
	long num;
}Transmit;

typedef struct bar_code_s{ /* �����������  */
	int errorCorrectLevel;
	float aspectRatio;
	float yHeight;
	int imgFormat;
	int encodeMode;
	int barSize;
} bar_code;


//	Socket��̽����
int DetectConnect(char * IpAddr, int Port, int TimeOut, int * Socket);

void SocketClose(int sockfd);

int DetectResponse(int iSockfd, char* pDetectStr, int iDetectStrlen);


