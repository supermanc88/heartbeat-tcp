#include "MonitorSrv.h"


using namespace std;
using namespace slim;


int connect_timeout = 1500;
int connect_retry = 3;
int ping_timeout = 1500;
int ping_retry = 3;

//0Ϊ����飬1Ϊ���
int if_chk_detect_ret = 0;			

int detect_strLen = 0;
char detect_string[FILEBUFFSIZE] = {0x00};

int base64_detect_strLen = 0;
char base64_detect_string[FILEBUFFSIZE] = {0x00};
int b64_det_str_no_trash_len = 0;
char b64_det_str_no_trash[FILEBUFFSIZE] = {0x00};

int debug_mode = 0;
#define IF_DEBUG if(0 != debug_mode)

//0 Ϊ�����������������1Ϊ��������������� 
//Ĭ������Ϊ�����ü����Ϊ�˷�ֹ�������������IP�ϡ�

//���mon_modeΪ0����(��س������HA�������У�������ر�����񣬲��������л�������������������)
//���mon_modeΪ1����(�����IP���ڱ�����������IP�Ļ�������״̬������ڱ�����ʲô������)
//���mon_modeΪ2����(��������IP->������������->�Զ˷�������->�л���ȥ)
int mon_mode = 0;

static int lock_fd = -1;

static void Cleanup(void)
{
    if (lock_fd != -1) {
        close(lock_fd);
        lock_fd = -1;
    }
}

//	�ж��Ƿ��Ѿ���һ��ʵ��������
int IsRunning(const char *process_name)
{
	char lock_file[128] = {0x00};
    snprintf(lock_file, sizeof(lock_file), "/var/tmp/%s.lock", process_name);

    lock_fd = open(lock_file, O_CREAT|O_RDWR, 0644);
    if (-1 == lock_fd) {
        fprintf(stderr, "Fail to open lock file(%s). Error: %s\n",
            lock_file, strerror(errno));
        return -1;
    }

    if (0 == flock(lock_fd, LOCK_EX | LOCK_NB)) {
        atexit(Cleanup);
        return -1;
    }

    close(lock_fd);
    lock_fd = -1;
    return 0;

}


//	������������ȡIP��ַ
int GetIpByHost(char *hostname, char *master_ip)
{
	FILE * fp = NULL;
	char * name_bgn = NULL;
	char buff[BUFFSIZE] = {0x00};


	if((fp = fopen(HA_HOST, "r")) == NULL)
	{
		LOGERROR("%s","Open hosts Err");
		return -1;
	}


	while(!feof(fp))
	{
		memset(buff, 0x00, BUFFSIZE);
		fgets(buff, BUFFSIZE, fp);
		
		name_bgn = strstr(buff, hostname);
		if(NULL != name_bgn)
		{
			memcpy(master_ip, buff, name_bgn - buff);

			trim(master_ip);

			break;
		}

	}

	if(NULL == name_bgn || 0 == strlen(master_ip))
	{
		LOGERROR("%s","GetIpByHost Err");
		fclose(fp);
		return -1;
	}
	fclose(fp);
	return 0;

}

//	��ȡ����������
int GetHostName(char *host_name)
{
	FILE * fp = NULL;
	char * echo = NULL;

	char name[BUFFSIZE] = {0x00};     
	char command[BUFFSIZE] = "hostname";

	fp = popen(command, "r");

	echo = fgets(name, BUFFSIZE, fp);

	pclose(fp);

	trim(name);

	strcpy(host_name, name);

	return 0;
}

//	��ȡ����ʵIP
int GetHostRealIP(char *real_ip)
{
	FILE * fp = NULL;
	char * echo = NULL;

	char host_ip[BUFFSIZE] = {0x00};     
	char command[BUFFSIZE] = "hostname -i";

	fp = popen(command, "r");

	echo = fgets(host_ip, BUFFSIZE, fp);

	pclose(fp);

	trim(host_ip);

	LOGINFO("This node's real ip: %s", host_ip);

	strcpy(real_ip, host_ip);

	return 0;
}

//	��ȡ�Զ�������
int GetPeerHostName(char *perr_name)
{
	int nHostCount = 0;
	FILE * fp = NULL;
	char Buff[BUFFSIZE] = {0x00};	
	char HostNameBuff[2][BUFFSIZE] = {0x00};	

	char * NodeBgn =NULL;
	char * NodeEnd =NULL;

	memset(&HostNameBuff[0], 0x00, BUFFSIZE);
	memset(&HostNameBuff[1], 0x00, BUFFSIZE);

	char ThisHostName[BUFFSIZE] = {0x00};

	GetHostName(ThisHostName);
	LOGINFO("This node's host_name: %s", ThisHostName);

	if((fp = fopen(HA_CONFIG, "r")) == NULL)
	{
		LOGERROR("%s","Open ha.cf Err");
		return OPEN_CONFIG;
	}

	while(!feof(fp))
	{
		memset(Buff, 0x00, BUFFSIZE); 
		fgets(Buff, BUFFSIZE, fp);

		if('#' == Buff[0]) continue;

		//��"node "�еĿո�ȥ��
		if( NULL == strstr(Buff, "#node") &&
			(NodeBgn = strstr(Buff, "node ")) != NULL)
		{
			NodeEnd = strstr(Buff, "node ") + strlen("node ");

			memcpy(HostNameBuff[nHostCount], NodeEnd, strlen(NodeEnd));
			nHostCount++;

			if (nHostCount > 1)
				break;
		}
	}
	fclose(fp);

	if (0 == nHostCount)
	{
		LOGERROR("%s","GetNode err");
		return GETPARAMETER;
	}

	trim(HostNameBuff[0]);
	trim(HostNameBuff[1]);


	if (0 == strcmp(HostNameBuff[0], ThisHostName))
	{
		strcpy(perr_name, HostNameBuff[1]);
	}
	else if (0 == strcmp(HostNameBuff[1], ThisHostName))
	{
		strcpy(perr_name, HostNameBuff[0]);
	}
	else
	{
		LOGERROR("%s","GetPeerName err");
		return GET_PEERNAME_ERR;
	}
	return 0;

}

//	��ȡ�Զ�ʵIP
int GetPeerRealIP(char *perr_ip)
{
	int iRet = 0;
	char peer_hostname[BUFFSIZE] = {0x00};
	
	iRet = GetPeerHostName(peer_hostname);
	if (0 != iRet)
		return iRet;

	LOGINFO("Peer HostName: %s", peer_hostname);

	iRet = GetIpByHost(peer_hostname, perr_ip);
	if (0 != iRet)
		return iRet;

	LOGINFO("Peer Ip: %s", perr_ip);

	return 0;
}

//	��ha.cf�л�ȡ����IP
int GetPingTargets(char * PingTargets)
{
	FILE * fp = NULL;
	char Buff[BUFFSIZE] = {0x00};	
	char IpBuff[BUFFSIZE] = {0x00};	

	char * PingBgn =NULL;
	char * PingEnd =NULL;

	if((fp = fopen(HA_CONFIG, "r")) == NULL)
	{
		LOGERROR("%s","Open ha.cf Err");
		return OPEN_CONFIG;
	}

	while(!feof(fp))
	{
		memset(Buff, 0x00, BUFFSIZE); 
		fgets(Buff, BUFFSIZE, fp);

		if('#' == Buff[0]) continue;

		//��ping��Ŀո�ȥ��
		if( NULL == strstr(Buff, "#ping") &&
			(PingBgn = strstr(Buff, "ping ")) != NULL)
		{
			PingEnd = strstr(Buff, "ping ") + strlen("ping ");

			memcpy(IpBuff, PingEnd, strlen(PingEnd));

			break;
		}
	}

	//ViewMem("PingTargets", IpBuff, BUFFSIZE);

	if(NULL == PingBgn || NULL == PingBgn)
	{
		LOGERROR("%s","GetGwIP err");
		fclose(fp);
		return GETPARAMETER;
	}

	trim(IpBuff);

	//ViewMem("PingTargets", IpBuff, BUFFSIZE);
	strcpy(PingTargets, IpBuff);
	fclose(fp);
	return 0;
}

//	��haresources�ļ�������ȡ��IP��HA��������
int GetVipAndMasterName(char * VIP, char *MasterName)
{
	FILE * fp = NULL;

	char *pVIPBgn = NULL;
	char *pVIPEnd = NULL;

	char *pIPaddrStr = NULL;
	char Buff[BUFFSIZE] = {0x00};



	if((fp = fopen(HA_RESOURCES, "r")) == NULL)
	{
		LOGERROR("%s","Open Haresources Err");
		return -1;
	}


	while(!feof(fp))
	{
		memset(VIP, 0x00, BUFFSIZE);
		memset(Buff, 0x00, BUFFSIZE);
		fgets(Buff, BUFFSIZE, fp);

		if('#' == Buff[0]) continue;

		if((pIPaddrStr = strstr(Buff, " IPaddr::")) != NULL)
		{
			pVIPBgn = pIPaddrStr + strlen(" IPaddr::");

			pVIPEnd = strstr(Buff, "/");
			if(NULL == pVIPEnd)
			{
				LOGERROR("%s","Get Visual Ip Err");
				fclose(fp);
				return -1;
			}

			memcpy(VIP, pVIPBgn, pVIPEnd - pVIPBgn);

			LOGINFO("Visual ip: %s", VIP);

			memcpy(MasterName, Buff, pIPaddrStr - Buff);
			trim(MasterName);

			LOGINFO("HA MasterName: %s", MasterName);

			break;
		}

	}

	if(NULL == pIPaddrStr || NULL == pVIPBgn || NULL == pVIPEnd)
	{
		LOGERROR("%s","Get Visual Ip Err");
		fclose(fp);
		return -1;
	}

	fclose(fp);
	return 0;

}


//	�ж��Ƿ��Ѿ��������IP����÷���0��û�л�÷��ط�0
int HasVip(char * VIP)
{

	FILE * fp = NULL;
	char * pEcho = NULL;

	char Buff[BUFFSIZE] = {0x00};     
	char Command[BUFFSIZE] = {0x00};


	sprintf(Command, "/sbin/ip -o -f inet addr show | grep \"%s/\"", VIP);
	//sprintf(Command, "/sbin/ifconfig | grep \"%s/\"", VIP);


	fp = popen(Command, "r");

	pEcho = fgets(Buff, BUFFSIZE, fp);

	pclose(fp);

	return (NULL == pEcho) ? -1: 0;

}

//	�жϱ�����HA��������HA����
int IsMaster(char *HaMasterName)
{
	FILE * fp = NULL;
	char * pEcho = NULL;

	char HostName[BUFFSIZE] = {0x00};     
	char Command[BUFFSIZE] = "hostname";

	fp = popen(Command, "r");

	pEcho = fgets(HostName, BUFFSIZE, fp);

	pclose(fp);
	trim(HostName);


	LOGINFO("This node's hostname: %s", HostName);
	if(0 == strcmp(HaMasterName, HostName))
	{
		return 0;
	}
	else
	{
		return -1;
	}

}


//	���������Ӽ���Ӧ
int DetectService(char * IpAddr, int Port, int TimeOut)
{
	int ret = 0;
	int sockfd = 0;

	ret = DetectConnect(IpAddr, Port, TimeOut, &sockfd);
	if(0 != ret)
	{
		LOGWARN("%s", "DetectConnect failed...");
		return -1;
	}
	else
		LOGINFO("%s", "DetectConnect success");

	
	ret = DetectResponse(sockfd, detect_string, detect_strLen);
	if(0 != ret)
	{
		LOGWARN("%s", "DetectResponse failed...");
		SocketClose(sockfd);
		return -1;
	}
	else
		LOGINFO("%s", "DetectResponse success");


	SocketClose(sockfd);

	return 0;
}

//	��ȡ����״̬����������0�����������ط�0
int GetSrvStatus(char * RIP, char * VIP)
{
	int Ret = 0;
	int ConnectCount = 0;

	//����NetSign�����ļ������ÿ�������Ƿ��������������Srv_Status����
	NodeIterator NodeIter;
	XmlDocument readingDoc;

	//����xml�ļ�
	bool ld = readingDoc.loadFromFile(SRV_CONF_PATH);
	if ( false == ld ) 
	{
		LOGERROR("%s","LOADXML_ERROR");
		return LOADXML_ERROR;
	}

	//��ȡ���ڵ�
	XmlNode* readingNode = readingDoc.findChild("service-config");
	if ( NULL == readingNode)
	{
		LOGERROR("%s","GET_ROOTNODE_ERROR");
		return GET_ROOTNODE;
	}
	XmlNode* serviceNode = NULL;

	for(serviceNode = readingNode->findFirstChild("service", NodeIter);
		NULL != serviceNode;
		serviceNode = readingNode->findNextChild( "service", NodeIter))
	{

		//ֱ�ӻ�ȡcheck-service
		XmlNode* ifchksrvNode = serviceNode->findChild("checkService");
/*		if ( NULL == ifchksrvNode) 
		{
			LOGERROR("%s","GET_CHKSRVNODE_ERROR");	
			return GET_CHKSRVNODE;
		}
		*/
		if( NULL == ifchksrvNode || 0 == strcmp(ifchksrvNode->getString(), "true") )
		{

			//��ȡservice����
			XmlAttribute *attr = serviceNode->findAttribute("name");
			if ( attr == NULL ) 
			{
				LOGERROR("%s","GET_SRVNAME_ERROR");	
				return GET_SRVNAME;
			}

			//ֱ�ӻ�ȡservice port
			XmlNode* portNode = serviceNode->findChild("port");
			if ( portNode == NULL )
			{
				LOGERROR("%s","GET_PORTNODE_ERROR");	
				return GET_PORTNODE;
			}


			//ֱ�ӻ�ȡservice ip
			//XmlNode* ipNode = serviceNode->findChild("ip");

			//�����ǰ�ڵ�û��ռ����IP�����ȴ������ļ��н���ʵIP�����û��IP����
			//����IP�����к���*����ʹ��127.0.0.1
			//if ( 0 == strcmp("127.0.0.1", VIP))
			//{
			//	if(NULL != ipNode && NULL != strstr(ipNode->getString(), "*") )
			//	{
			//		memset(RemoteIP, 0x00, BUFFSIZE);
			//		strcpy(RemoteIP, ipNode->getString());			
			//	}
			//}
			//else	//���Ѿ�ռ����IP����ʹ����IP
			//{
			//	memset(RemoteIP, 0x00, BUFFSIZE);
			//	strcpy(RemoteIP, VIP);			
			//
			//}

			for(ConnectCount = 0; ConnectCount <= connect_retry; ConnectCount ++)
			{
				LOGINFO("Detect Service [%s]...", attr->getString());

				//̽�����״̬
				//Ret = Detect((char *)ipNode->getString(), atoi(portNode->getString()), connect_timeout);
				
				//̽�����״̬
				//Ret = DetectService(VIP, atoi(portNode->getString()), connect_timeout);
				//
				//if(0 == Ret) 
				//{
				//	break;
				//}
				//else if(0 != Ret) 
				//{
					Ret = DetectService(RIP, atoi(portNode->getString()), connect_timeout);
					if(0 == Ret) 
					{
						break;
					}
				//}

			}

			if(0 != Ret)
			{
				LOGERROR("Service [%s] status: invalid", attr->getString());
				
				return GOTO_STANDBY;
			}
			else
			{
				LOGINFO("Service [%s] status: normal", attr->getString());
		
			}
		}
	}
	
	return 0;

}


//	��conf.ini�ж�ȡ���ò���
int LoadMonSrvConf()
{
	char Buff[FILEBUFFSIZE] = {0x00};
	
	char ConfBuff[FILEBUFFSIZE] = {0x00};

	FILE * fp = fopen(MONSRVCONF, "r");
	if(NULL == fp)
	{
		LOGERROR("%s", "Open monsrvconf err");
		return -1;
	}
	char *pName  = NULL;
	char *pValue = NULL;

	while(!feof(fp))
	{
		memset(Buff, 0x00, FILEBUFFSIZE);
		fgets(Buff, FILEBUFFSIZE, fp);


		//���ﵥ����detectstring��������Ϊdetectstring��Base64�ַ�����
		//���п��ܺ���=�ţ���strtok�����㴦��
		if( 0 == memcmp(Buff, "detectstring=", strlen("detectstring=")))
		{
			strcpy(base64_detect_string,  strstr(Buff, "=") + 1);

			trim(base64_detect_string);

			ClearB64Trash((unsigned char *)base64_detect_string, strlen(base64_detect_string), 
				(unsigned char *)b64_det_str_no_trash, &b64_det_str_no_trash_len);
			
			IF_DEBUG printf("Base64DetectString = %s\n", b64_det_str_no_trash);

			detect_strLen = Base64Decode(b64_det_str_no_trash, b64_det_str_no_trash_len, detect_string);

			IF_DEBUG ViewMem("detect_string", (unsigned char *)detect_string, detect_strLen);
		}

		pName = strtok( Buff, "=");
		if(NULL == pName ) continue;

		pValue = strtok(NULL, "=");
		if(NULL == pValue) continue;

		if( 0 == strcmp(pName, "connecttimeout"))
		{
			connect_timeout = atoi(trim(pValue));
		}
		else if( 0 == strcmp(pName, "connectretrytimes"))
		{
			connect_retry = atoi(trim(pValue));
		}
		if( 0 == strcmp(pName, "pingtimeout"))
		{
			ping_timeout = atoi(trim(pValue));
		}
		else if( 0 == strcmp(pName, "pingretrytimes"))
		{
			ping_retry = atoi(trim(pValue));
		}
		else if( 0 == strcmp(pName, "checkdetectret"))
		{
			if(0 == strcmp("YES", trim(pValue)))
			{
			//	LOGINFO("CheckDetectRet:%s","YES");
				if_chk_detect_ret = 1;
			}
			else if(0 == strcmp("NO", trim(pValue)))
			{
			//	LOGINFO("CheckDetectRet:%s","NO");
				if_chk_detect_ret = 0;
			}
			else
			{
				LOGERROR("%s", "MonsrvConf Parameter Err");
				fclose(fp);
				return -1;
			}
		}
	/*	else if( 0 == strcmp(pName, "monswitch"))
		{
			if(0 == strcmp("OFF", trim(pValue)))
			{
				mon_mode = 0;
			}
			else if(0 == strcmp("ON", trim(pValue)))
			{
				mon_mode = 1;
			}
			else
			{
				LOGERROR("%s", "MonsrvConf Parameter Err");
				fclose(fp);
				return -1;
			}
		}*/
		else if( 0 == strcmp(pName, "mon_mode"))
		{
			mon_mode = atoi(trim(pValue));

			LOGINFO("mon_mode = %d", mon_mode);

			if (mon_mode > 2 || mon_mode < 0)
			{
				LOGERROR("%s", "MonsrvConf Parameter Err");
				fclose(fp);
				return -1;
			}
		}
	}
	fclose(fp);
	return 0;
}

int monitor_srv_init()
{
	int Ret = 0;
	
	char VIP[BUFFSIZE] = {0x00};
	char PIP[BUFFSIZE] = {0x00};
	char HIP[BUFFSIZE] = {0x00};
	char PingTargets[BUFFSIZE] = {0x00};
	char MasterName[BUFFSIZE] = {0x00};

	//���������ļ�
	if(0 != LoadMonSrvConf())
	{
		LOGERROR("%s","LoadMonSrvIni err");
		goto EXIT;
	}


	//��ȡ��IP��HA����������
	Ret = GetVipAndMasterName(VIP, MasterName);
	if(0 != Ret) 	
	{
		LOGERROR("%s","GetVipAndMasterName err");
		goto EXIT;
	}

#pragma region comment
//	//���mon_modeΪ0����(��س������HA�������У�������ر�����񣬲��������л�������������������)
//	//���mon_modeΪ1����(�����IP���ڱ�����������IP�Ļ�������״̬������ڱ�����ʲô������)
//	//���mon_modeΪ2����(��������IP->������������->�Զ˷�������->�л���ȥ)
//
//	if(0 == mon_mode)
//	{
//		LOGNOTICE("%s","[Master own monitoring mode]");
//
//		//Patch6ģʽ�������Ǳ�������ֱ���˳�
//		if(0 != IsMaster(MasterName))
//		{
//			LOGINFO("%s","Host is [slave]");
//			goto EXIT;
//		}
//		else
//		{
//			LOGINFO("%s","Host is [master]");
//
//			Ret = HasVip(VIP);
//			if(0 != Ret)
//			{
//				LOGINFO("%s","This node doesn't have visual ip");
//
//				Ret = GetHostRealIP(HIP);
//				if(0 != Ret)
//				{
//					LOGINFO("%s","GetHostRealIP err");
//					goto EXIT;
//				}
//
//				//��ȡ����״̬,���ʱ��û��ռ����IP�������ﴫ��ʵIP
//				LOGINFO("%s","Test this other node's service");
//				Ret = GetSrvStatus(HIP, VIP);
//				if(0 != Ret)
//				{
//					LOGINFO("%s","Master server abnormal");
//					goto EXIT;
//				}
//				LOGINFO("%s", "Master server normal");
//
//				Ret = GetPingTargets(PingTargets);
//				if(0 != Ret)
//				{
//					LOGINFO("%s", "GetPingTargets err");
//					goto EXIT;
//				}
//
//				LOGINFO("GetPingTargets: %s", PingTargets);
//				LOGINFO("PING TimeOut: %d", ping_timeout);
//				if(0 == Ping(PingTargets, ping_timeout, ping_retry))
//				{
//					LOGINFO("%s","Ping success");
//					LOGNOTICE("%s","Goto takeover");
//					system(HA_TAKEOVER);
//				}
//				else
//					LOGERROR("%s","Ping failed");
//
//			}
//			else
//			{
//				LOGINFO("%s","This node has visual ip");
//
//				Ret = GetHostRealIP(HIP);
//				if(0 != Ret)
//				{
//					LOGINFO("%s","GetHostRealIP err");
//					goto EXIT;
//				}
//
//				LOGINFO("%s","Test this other node's service");
//
//				Ret = GetSrvStatus(HIP, VIP);
//				if(0 != Ret)
//				{
//					//ִ���л�
//					LOGINFO("%s","Master server abnormal");
//					LOGNOTICE("%s","Goto standby");
//					system(HA_STANDBY);
//				}
//				else
//				{
//					LOGINFO("%s", "Master server normal");
//				}
//			}
//		}
//	}
//	else if(1 == mon_mode)
//	{
//		//Patch8ģʽ
//		LOGNOTICE("%s","[Nodes mutual monitoring mode]");
//
//		Ret = HasVip(VIP);
//		if(0 != Ret)
//		{
//			if(0 == IsMaster(MasterName))
//			{
//				//����Լ�������  ����Լ�  ���Ծ�takeover  �����ԾͲ���
//				LOGINFO("%s","Host is [master]");
//
//				//��ȡ����IP��ַ
//				Ret = GetHostRealIP(HIP);
//				if(0 != Ret)
//				{
//					LOGINFO("%s","GetHostRealIP err");
//					goto EXIT;
//				}
//				LOGINFO("HostIp: %s", HIP);
//
//				LOGINFO("%s","Test this other node's service");
//
//				//��ȡ��������״̬
//				Ret = GetSrvStatus(HIP, VIP);
//				if(0 != Ret)
//				{
//					LOGINFO("%s","Master server abnormal");
//					goto EXIT;
//				}
//				LOGINFO("%s", "Master server normal");
//
//				Ret = GetPingTargets(PingTargets);
//				if(0 != Ret)
//				{
//					LOGINFO("%s", "GetPingTargets err");
//					goto EXIT;
//				}
//				LOGINFO("GetPingTargets: %s", PingTargets);
//				LOGINFO("PING TimeOut: %d", ping_timeout);
//				if(0 != Ping(PingTargets, ping_timeout, ping_retry))
//				{
//					LOGERROR("%s","Ping failed");
//					goto EXIT;
//				}
//				LOGINFO("%s","Ping success");
//
//				LOGNOTICE("%s","Goto takeover");
//				system(HA_TAKEOVER);
//			}
//			else
//			{
//				//����Լ��Ǳ�����ȥ���Զ˷���  �����Ծ�takeover  ���ԾͲ���
//				LOGINFO("%s","Host is [slave]");
//
//				LOGINFO("%s","Test the other node's service");
//
//				//��ȡ��������״̬
//				Ret = GetSrvStatus(VIP, VIP);
//				if(0 != Ret)
//				{
//					LOGINFO("%s","Master server abnormal");
//					LOGNOTICE("%s","Goto takeover");
//					system(HA_TAKEOVER);
//				}
//
//			}
//		}
//		else
//		{
//			LOGINFO("%s","This node has visual ip, No care");
//
//			//--------------------PING���ܲ���----------------------------
//			IF_DEBUG
//			{
//				Ret = GetPingTargets(PingTargets);
//				if(0 != Ret)
//				{
//					LOGINFO("%s", "GetPingTargets err");
//					goto EXIT;
//				}
//				LOGINFO("GetPingTargets: %s", PingTargets);
//				LOGINFO("Ping TimeOut: %d", ping_timeout);
//				if(0 != Ping(PingTargets, ping_timeout, ping_retry))
//				{
//					LOGERROR("%s","Ping failed");
//					goto EXIT;
//				}
//				LOGINFO("%s","Ping success");
//			}
//			//--------------------PING���ܲ���----------------------------
//
//		}
//	}
//	else if(2 == mon_mode)
//	{
//		//˫�����ģʽ��������
//		//����IP->������������->�Զ˷�������->�л���ȥ
//
//		LOGNOTICE("%s","[Nodes all monitoring mode]");
//
//		Ret = HasVip(VIP);
//		if(0 == Ret)
//		{
//			LOGINFO("%s","This node has visual ip");
//
//			//��ȡ�Լ�IP����ȡ�Զ�IP
//			if (0 != GetHostRealIP(HIP))
//			{
//				LOGERROR("%s", "GetHostRealIP Err");
//				goto EXIT;
//			}
//
//			if (0 != GetPeerRealIP(PIP))
//			{
//				LOGERROR("%s", "GetPeerRealIP Err");
//				goto EXIT;
//			}
//
//
//			Ret = GetPingTargets(PingTargets);
//			if(0 != Ret)
//			{
//				LOGINFO("%s", "GetPingTargets err");
//				goto EXIT;
//			}
//			LOGINFO("GetPingTargets: %s", PingTargets);
//			LOGINFO("Ping TimeOut: %d", ping_timeout);
//
//			Ret = Ping(PingTargets, ping_timeout, ping_retry);
//			if (0 != Ret)
//			{
//				LOGERROR("%s","Ping failed");
//				LOGINFO("%s", "Check Peer Node's Server");
//				Ret = GetSrvStatus(PIP, VIP);
//				if (0 == Ret)
//				{
//					LOGNOTICE("%s","Goto standby");
//					system(HA_STANDBY);
//				}
//			}
//			LOGINFO("%s","Ping success");
//
//
//			//����Լ�������������������Զ˷��񣬶Զ˷����������л�
//			LOGINFO("%s", "Check This Node's Service");
//			Ret = GetSrvStatus(HIP, VIP);
//			if (0 != Ret)
//			{
//				LOGERROR("%s","Check Service failed");
//				LOGINFO("%s", "Check Peer Node's Service");
//				Ret = GetSrvStatus(PIP, VIP);
//				if (0 == Ret)
//				{
//					LOGNOTICE("%s","Goto standby");
//					system(HA_STANDBY);
//				}
//			}
//		}
//	}
//	else
//	{
//		LOGERROR("%s", "MonsrvConf Parameter Err");
//		goto EXIT;
//	}
#pragma endregion comment
EXIT:

	LOGINFO("%s","MonSrv exit");

	printf("%s","-------------------------------------------------------------------\n");


	return 0;
}



