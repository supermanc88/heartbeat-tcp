#include "Ping.h"
#include "MyLog.h"

#define PACKET_SIZE     4096
#define ERROR           1
#define SUCCESS         0

extern int debug_mode;
#define IF_DEBUG if(0 != debug_mode)

//	效验算法
unsigned short cal_chksum(unsigned short *Addr, int len)
{
	int nleft=len;
	int sum=0;
	unsigned short *w=Addr;
	unsigned short answer=0;

	while(nleft > 1)
	{
		sum += *w++;
		nleft -= 2;
	}

	if( nleft == 1)
	{       
		*(unsigned char *)(&answer) = *(unsigned char *)w;
		sum += answer;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;

	return answer;
}

//	Ping函数
int icmpping( char *ips, int timeout)
{

	int sockfd;

	struct sockaddr_in Addr;
	struct sockaddr_in From;
	struct timeval Timeo;
	struct timeval *pTval = NULL;
	struct ip *pIph = NULL;
	struct icmp *pIcmp = NULL;
	
	char sendpacket[PACKET_SIZE] = {0x00};
	char recvpacket[PACKET_SIZE] = {0x00};

	int n;
	pid_t pid;
	int maxfds = 0;
	fd_set readfds;

	// 设定Ip信息
	bzero(&Addr, sizeof(Addr));
	Addr.sin_family = AF_INET;
	Addr.sin_addr.s_addr = inet_addr(ips);  


	// 取得socket
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) 
	{
		return ERROR;
	}

	// 设定TimeOut时间

	//Timeo.tv_sec = timeout / 1000;
	//Timeo.tv_usec = timeout % 1000;	//错误计算
	Timeo.tv_sec = timeout / 1000;
	Timeo.tv_usec = (timeout % 1000 ) * 1000;
	
	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &Timeo, sizeof(Timeo)) == -1)
	{
		close(sockfd);
		return ERROR;
	}

	// 设定Ping包
	memset(sendpacket, 0, sizeof(sendpacket));

	// 取得PID，作为Ping的Sequence ID
	pid=getpid();
	int i,packsize;
	pIcmp=(struct icmp*)sendpacket;
	pIcmp->icmp_type=ICMP_ECHO;
	pIcmp->icmp_code=0;
	pIcmp->icmp_cksum=0;
	pIcmp->icmp_seq=0;
	pIcmp->icmp_id=pid;
	packsize=8+56;
	pTval= (struct timeval *)pIcmp->icmp_data;
	gettimeofday(pTval,NULL);
	pIcmp->icmp_cksum=cal_chksum((unsigned short *)pIcmp,packsize);

	// 发包
	n = sendto(sockfd, (char *)&sendpacket, packsize, 0, (struct sockaddr *)&Addr, sizeof(Addr));
	if (n < 1)
	{
		close(sockfd);
		return ERROR;
	}

	// 接受
	// 由于可能接受到其他Ping的应答消息，所以这里要用循环
	while(1)
	{
		// 设定TimeOut时间，这次才是真正起作用的
		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);
		maxfds = sockfd + 1;
		n = select(maxfds, &readfds, NULL, NULL, &Timeo);
		if (n <= 0)
		{
			//printf("ip:%s, Time out error\n",ips);
			close(sockfd);
			return ERROR;
		}

		// 接受
		memset(recvpacket, 0, sizeof(recvpacket));
		int fromlen = sizeof(From);
		n = recvfrom(sockfd, recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)&From, (socklen_t*)&fromlen);
		if (n < 1) {
			//printf("ip:%s, recvfrom data error\n",ips);
			close(sockfd);
			return ERROR;
		}

		// 判断是否是自己Ping的回复
		char *from_ip = (char *)inet_ntoa(From.sin_addr);

		if (strcmp(from_ip,ips) != 0)
		{
			//printf("ip:%s, Ip wang\n",ips);
			close(sockfd);
			return ERROR;
		}

		pIph = (struct ip *)recvpacket;

		pIcmp=(struct icmp *)(recvpacket + (pIph->ip_hl<<2));

		if (pIcmp->icmp_type == ICMP_ECHOREPLY && pIcmp->icmp_id == pid)
		{
			// 正常就退出循环
			break;
		}
		else
		{
			// 否则继续等
			continue;
		}
	}

	// 关闭socket
	close(sockfd);

	//printf("ip:%s, Success\n",ips);
	return SUCCESS;
}


int RetyPing( char *ips, int timeout, int failretrytimes)
{
	int retrycount = 0;


	for(;retrycount <= failretrytimes; retrycount++)
	{
		if(0 == icmpping( ips, timeout))
			break;
	}

	return ((failretrytimes >= retrycount)? 0 : -1);
}



int Ping(char *ips, int timeout, int failretrytimes)
{
	int ret = 0;

	char *ip = strtok(ips, " ");
	
	while(ip != NULL)
	{
		ret = RetyPing(ip, timeout, failretrytimes);

		LOGINFO("PING target: %s Ret = %d\n", ip, ret);

		ip = strtok(NULL," ");

		IF_DEBUG 
		{
			continue;
		}
		if(SUCCESS == ret)
		{
			return SUCCESS;
		}

	}
	return ERROR;

}
