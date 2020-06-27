#ifndef _ERR_MSG_H_
#define _ERR_MSG_H_

#define CONNECT_SUCCESS		0
#define CALL_PARAM_ERR		1
#define SOCKET_CREATE		2
#define SOCKET_CONNECT		3
#define LOADXML_ERROR		4
#define GET_ROOTNODE		5
#define GET_SRVNAME			6
#define GET_IPNODE			7
#define GET_PORTNODE		8
#define GET_CHKSRVNODE		9
#define GOTO_STANDBY		10
#define OPEN_CONFIG			11
#define GETPARAMETER		12
#define GET_PEERNAME_ERR	13


/*3.Êý¾Ý³ö´í*/

#define Err_InvalidData			8001
#define Err_VerifyFromServer	8002
#define Err_Connect2Server		8003
#define Err_SendData2Server		8004
#define Err_RecvDataFromServer	8005
#define Err_NoSpecifyedCert		8006
#define Err_CloseSocket			8007
				
#define Err_VerifyPassword		8008			
#define Err_InvalidServerInfo	8009			
#define Err_BuffSizeNotEnough	8010			
#define Err_BankIDInvalid		8011	

#define Err_GetIPFailed			-8012

#endif

