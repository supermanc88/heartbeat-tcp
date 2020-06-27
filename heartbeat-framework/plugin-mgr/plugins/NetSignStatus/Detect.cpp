#include "Detect.h"

int TIMEOUT = 30;
//int CONN_TIMES = 10;

extern int if_chk_detect_ret;
extern int debug_mode;
#define IF_DEBUG if(0 != debug_mode)


int  clearMem(unsigned char * buffer)
{
	memset( buffer , 0 ,sizeof( buffer ) );
	return 0;
}

void makeMessage(unsigned char * pmsg,long msglen, unsigned char identifier, const char *type, unsigned char *pOutbuf, long *outMesLen)
{	
	char  tempbuf[20] = { 0 };

	memcpy( pOutbuf , (unsigned char *)&identifier, TAGNAMELEN );
	memcpy( pOutbuf + TAGCLASSLEN  , (unsigned char *)&type , TAGCLASSLEN);
	sprintf(tempbuf,"%012ld",msglen);
	memcpy( pOutbuf + TAGNAMELEN + TAGCLASSLEN , tempbuf , COMMLENGTHLEN );
	memcpy( pOutbuf + TAGNAMELEN + TAGCLASSLEN + COMMLENGTHLEN , pmsg , msglen );
	*outMesLen = TAGNAMELEN + TAGCLASSLEN + COMMLENGTHLEN + msglen;

}
void makeMessageHead(unsigned char * pmsg,long msglen, unsigned char identifier, const char *type, unsigned char *pOutbuf, long *outMesLen)
{	
	char  tempbuf[20] = { 0 };
	memcpy( pOutbuf , (unsigned char *)&identifier, TAGNAMELEN );
	memcpy( pOutbuf + TAGCLASSLEN  , (unsigned char *)type , TAGCLASSLEN);


	//printf("type:  %c", *type);
	//ViewMem("type", (unsigned char *)pOutbuf, 2);

	sprintf(tempbuf,"%012ld",msglen);
	memcpy( pOutbuf + TAGNAMELEN + TAGCLASSLEN , tempbuf , COMMLENGTHLEN );
	/*memcpy( pOutbuf + TAGNAMELEN + TAGCLASSLEN + COMMLENGTHLEN , pmsg , msglen );*/
	*outMesLen = TAGNAMELEN + TAGCLASSLEN + COMMLENGTHLEN ;
}
void makeMsg(unsigned char * msg, int iMsgLen, unsigned char identifier, const char *type, char * truck, int *from)
{
	long iRet = 0;
	char  tmpBuffer[MAXCERTBUFFSIZE] = { 0 };
	makeMessage( msg , iMsgLen , identifier , type , (unsigned char *)tmpBuffer , &iRet);
	memcpy(truck + *from, tmpBuffer , iRet);
	*from += iRet ;
}
void makeMsgHead(int iMsgLen, unsigned char identifier, const char *type, char * truck, int *from)
{
	long iRet = 0;
	char  tmpBuffer[20] = { 0 };
	makeMessageHead( NULL, iMsgLen , identifier , type , (unsigned char *)tmpBuffer , &iRet);
	memcpy(truck + *from, tmpBuffer , iRet);
	*from += iRet ;
}

int socket_send( int sockfd, char *argV, int len )
{
	char *buf;
	int sendLen, sendBytes;

	buf = argV;
	sendLen = len;

	while( sendLen > 0 )
	{
		sendBytes = send( sockfd, buf, sendLen, 0 );
		if( sendBytes <= 0 )
		{

			return -1;
		}
		buf += sendBytes;
		sendLen -= sendBytes;
	}
	return 0;
}
int readn(int iFd, char *lpszBuf, int iNumOfBytes, int iTimeout )

{

	int		nleft, nread ;

	fd_set	fdset;

	time_t	t0, t;

	struct	timeval tv;

	int		i;



	if( iNumOfBytes < 0 )

	{

		return -1;

	}



	time( & t0 );

	for(  nleft = iNumOfBytes;  nleft > 0;  nleft -= nread, lpszBuf += nread )

	{

		while( iTimeout >= 0 )

		{

			time( & t );

			if( t - t0 > iTimeout )

			{
				return -2;		/* time out */

			}



			tv.tv_sec = 1;

			tv.tv_usec = 0;



			FD_ZERO( & fdset );

			FD_SET( iFd, & fdset );



			i = select( iFd+1, & fdset, 0, 0, & tv );

			/* check if the service entered StopPending mode */

			if( i==0 )

			{

				continue;

			}

			if( i<0 )

			{

				/*fprintf(stdout,"Select failed in readn\n" );*/


				return -1;		/* error */

			}

			break;

		}



		nread = recv( iFd , lpszBuf , nleft, 0 ) ;

		if( nread < 0 )

		{

			if( errno == EINTR )

			{/* any signal*/ 

				/*fprintf(stdout,"recv interrupted in readn\n" );*/

				nread = 0;

				continue;

			}



			/*fprintf(stdout,"recv failed in readn\n" );	*/

			return( -1 );	/* error, return < 0 */

		} else {

			if( nread == 0 )	/* socket close gracefully*/

			{

				break;				/* EOF */

			}

		}

	}



	return iNumOfBytes - nleft;		/* return >= 0 */

}
int writen(int iFd, char *lpszBuf, int iNumOfBytes, int iTimeout )

{

	int		nleft, nwritten ;

	time_t	t0, t;

	struct	timeval tv;

	fd_set	fdset;

	int		i;



	time( &t0 );



	for(  nleft = iNumOfBytes;  nleft > 0;  nleft -= nwritten, lpszBuf += nwritten )

	{

		while( iTimeout >= 0 )

		{

			time( &t );

			if( t - t0 > iTimeout )

			{

				return -2;		/* time out */

			}

			tv.tv_sec = 1;

			tv.tv_usec = 0;



			FD_ZERO( &fdset );

			FD_SET( iFd, &fdset );

			i = select( iFd+1, NULL, &fdset,  NULL, &tv );

			if( i==0 )

			{

				continue;

			}

			if( i<0 )

			{

				/*fprintf(stdout,"Select failed in writen" );*/

				return -1;		/* error */

			}

			break;

		}

		nwritten = send( iFd, lpszBuf, nleft, 0 );

		if( nwritten==0 )

		{

			break;  /* error */

		}

		if( nwritten<0 )

		{

			if( errno == EINTR )

			{/* any signal*/ 

				/*fprintf(stdout,"send interrupted in writen\n" );*/

				nwritten = 0;

				continue;

			}

			/*fprintf(stdout,"send failed in writen\n" );*/

			return( -1 );	/* error, return < 0 */

		}

	}



	return iNumOfBytes - nleft;

}

int receiveTotalLen(int sockFd, int *ifRecv)
{

	char  tempbuf[BufBlockSize] = { 0 };

	int iRet = 0;

	clearMem((unsigned char * )tempbuf);

	iRet = readn( sockFd,  tempbuf,  COMMLENGTHLEN + 1,  TIMEOUT );/*读回拖拉机头部*/

	if(iRet > 0)
	{
		*ifRecv = 1;
	}
	if(iRet <= 0 ) return iRet;

	iRet = atoi(tempbuf + 1);/*返回报文的总长度*/

	return iRet;

}
int receiveContent(int sockFd,  int iSegmentLen, unsigned char * tmpBuffer, int * iHaveReceiveLen)
{

	int iCurrentReceiveLen = 0;

	clearMem(tmpBuffer);

	iCurrentReceiveLen = readn( sockFd, (char *)tmpBuffer,  iSegmentLen,  TIMEOUT );/*读回蚂蚁头部*/

	*iHaveReceiveLen += iCurrentReceiveLen;

	return iCurrentReceiveLen;

}
int receiveTagAndLen(int sockFd,unsigned char * TAG, int * iSegmentLen, int * iHaveReceiveLen)
{

	int iCurrentReceiveLen = 0;

	char  tmpBuffer[BufBlockSize] = { 0 };

	char  tempbuf[BufBlockSize] = { 0 };

	iCurrentReceiveLen = receiveContent(sockFd, TAGNAMELEN + TAGCLASSLEN + COMMLENGTHLEN, (unsigned char *)tmpBuffer, iHaveReceiveLen);

	if( iCurrentReceiveLen <= 0 ) return Err_RecvDataFromServer;



	clearMem((unsigned char *)tempbuf);			

	memcpy( tempbuf , tmpBuffer  , TAGNAMELEN);/*从tmpBuffer中获取标识*/

	*TAG = tempbuf[0];



	memcpy( tempbuf , tmpBuffer + TAGNAMELEN + TAGCLASSLEN , COMMLENGTHLEN);/*从tmpBuffer中获取长度*/

	*iSegmentLen = atoi( tempbuf );

	return iCurrentReceiveLen;

}
int handleResult(unsigned char TAG, char * tmpBuffer, int iReadLen, int handleType, int * iResult, result_param *result, const char * info)
{

	int iRet = 0;

	char  tempbuf[MAXCERTBUFFSIZE] = { 0 };

	char  *str = NULL;

	int   i = 0;

	if(SIGN == handleType)
	{
		switch ( TAG ) 
		{
		case TAG_RESULT:
			*iResult = atoi(tmpBuffer);
			break;

			/*判断result,如果失败，返回,如果result失败，做失败标记，返回*/
		case TAG_ERROR_MESSAGE:
			//fprintf(stdout,"[%s],recverrmsg:[%s]\n",info,tmpBuffer);
			break;

		default	:
			//fprintf(stdout,"[%s],unknown type:[%s]\n",info,tmpBuffer);
			break;
		}
	}
	else
	{
		//fprintf(stdout,"unknown type:[%s]\n",tmpBuffer);

		iRet = -1;
	}



	return iRet;

}

int ParseReceive(int sockFd, char *debugInfo, char * retValue, void *lenOrMem, int type)
{

	int iReturnValue = -1;

	int iNoChkRetValue = -1;

	int iRet = -1;

	int iRecTotalLen = 0;

	int iRecCRSegLen = 0;

	int iRecSofarLen = 0;

	int ifRecv = 0;

	unsigned char TAG = 0;

	char  tmpBuffer[MAXCERTBUFFSIZE] = { 0 };

	Transmit *trans = NULL;

	int * len = NULL;

	iRecTotalLen = receiveTotalLen(sockFd, &ifRecv);

	if(1 == ifRecv && 0 == if_chk_detect_ret)
	{
		return 1;
	}

	if(iRecTotalLen <= 0 )
	{
		return Err_RecvDataFromServer;
	}
	//printf("iRecTotalLen = %d\n", iRecTotalLen );

	while( iRecSofarLen < iRecTotalLen)
	{
		//接收tag标记及剩下的内容部分
		iRet = receiveTagAndLen(sockFd, &TAG, &iRecCRSegLen, &iRecSofarLen);
		if(iRet < 0)
		{
			return Err_RecvDataFromServer;
		}

		//printf("iRet = %d\n", iRet );

		//printf("TAG: %02x\n", TAG);;


		switch(TAG)

		{

		case TAG_CRYPTO_TEXT:

		case TAG_PLAIN_TEXT:

		case TAG_BC_SYMBOLOGY:


			//printf("iRecCRSegLen == %d\n", iRecCRSegLen);
			iRet = readn( sockFd, (char *)retValue,  iRecCRSegLen,  TIMEOUT );

			if(iRet < 0)
			{
			//	printf("readn error\n");
				return iRet;
			}


			iRecSofarLen += iRet;

			if(RETURN_CRYPTO == type)
			{

				len = (int *)lenOrMem;

				*len = iRet;

			}else{

				trans->num = iRet;

			}			
			//printf("break\n");
			break;

		default:

			memset(tmpBuffer, 0, MAXCERTBUFFSIZE);

			iRet = receiveContent(sockFd, iRecCRSegLen, (unsigned char *)tmpBuffer, &iRecSofarLen);
			//ViewMem("receiveContent", (unsigned char *)tmpBuffer, 50);
			if(iRet < 0)
			{
				return iRet;
			}



			switch(type)
			{

			case RETURN_CRYPTO:	

			//	printf("RETURN_CRYPTO\n");
				handleResult(TAG, tmpBuffer, iRet,  SIGN, &iReturnValue, NULL, debugInfo);

				break;

			case RETURN_CERT:

			case RETURN_CERT_PLAIN:

				handleResult(TAG, tmpBuffer, iRet,  SIGN_CERT, &iReturnValue, trans->sCert,debugInfo);

				break;

			case RETURN_CERTS_PLAIN:

				handleResult(TAG, tmpBuffer, iRet,  SIGN_CERT, &iReturnValue, trans->sCert,debugInfo);

				handleResult(TAG, tmpBuffer, iRet,  ENCRYPT_CERT, &iReturnValue, trans->eCert,debugInfo);

				break;

			}//switch type

		}//switch tag

	}//while end


	return iReturnValue;
}

//	检测连接响应
int DetectResponse(int iSockfd, char* pDetectStr, int iDetectStrlen)
{

	int	iRet = 0;
	int iRetMsgLen = 0;
	char RetMsg[MAXCERTBUFFSIZE] = {0x00};

	IF_DEBUG ViewMem("DetectString", (unsigned char *)pDetectStr, iDetectStrlen);

	if(( pDetectStr == NULL ) ||( iDetectStrlen <= 0 ))
	{
		return -1;
	}

	socket_send(iSockfd, pDetectStr, iDetectStrlen);
	iRet = ParseReceive(iSockfd, NULL, RetMsg, &iRetMsgLen, RETURN_CRYPTO);

	IF_DEBUG printf("iRet = %d\n", iRet);

	if( iRet != 1 ) return iRet;


	return 0;
}

//	Socket试探服务
int DetectConnect(char * IpAddr, int Port, int TimeOut, int * Socket)
{
	int ret = 0;
	int sockfd, numbytes;

	int flags;
	unsigned long ul = 1;

	fd_set fds,rfds;
	struct timeval timeout ;

	struct hostent *he;
	struct sockaddr_in serv_addr;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(Port);
	serv_addr.sin_addr.s_addr = inet_addr(IpAddr);


	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		return SOCKET_CREATE;
	}

	bzero(&(serv_addr.sin_zero), 8);

	//设置为非阻塞模式	
	flags = fcntl(sockfd,F_GETFL,0);
	ret = fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);	

	connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr));


	FD_ZERO(&fds);
	FD_ZERO(&rfds);

	FD_SET(sockfd, &fds);
	FD_SET(sockfd, &rfds);	

	timeout.tv_sec = TimeOut/1000;	 //连接超时时间，超过此时间尚未连接成功则返回
	timeout.tv_usec = (TimeOut%1000)*1000 ;


	//select会等待timeout里设定的时间后再返回
	ret = select(sockfd+1,&rfds, &fds, NULL, &timeout);
	if( ret == -1|| ret==0)	 //0－－超时,-1－－出错
	{
		close(sockfd);
		return SOCKET_CONNECT;
	}

	// A)当连接建立成功时，套接口描述符变成 可写；
	// B)当连接建立出错时，套接口描述符变成 既可读又可写；
	if (FD_ISSET(sockfd,&fds) && FD_ISSET(sockfd,&rfds))
	{
		close(sockfd);
		return SOCKET_CONNECT;
	}


	//改为阻塞模式
	flags = fcntl(sockfd, F_GETFL, 0);
	ret = fcntl(sockfd, F_SETFL,flags&~O_NONBLOCK);

	if( ret == -1 )
	{
		close(sockfd);
		return SOCKET_CONNECT;
	}

	*Socket = sockfd;	

	return CONNECT_SUCCESS;

}

//	关闭Socket连接
void SocketClose(int sockfd)
{
	close(sockfd);
}



