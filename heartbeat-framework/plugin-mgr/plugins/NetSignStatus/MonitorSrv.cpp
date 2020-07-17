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

//	���������Ӽ���Ӧ
int DetectService(char *IpAddr, int Port, int TimeOut)
{
    int ret = 0;
    int sockfd = 0;

    ret = DetectConnect(IpAddr, Port, TimeOut, &sockfd);
    if (0 != ret) {
        LOGWARN("%s\n", "DetectConnect failed...");
        return -1;
    } else
        LOGINFO("%s\n", "DetectConnect success");


    ret = DetectResponse(sockfd, detect_string, detect_strLen);
    if (0 != ret) {
        LOGWARN("%s\n", "DetectResponse failed...");
        SocketClose(sockfd);
        return -1;
    } else
        LOGINFO("%s\n", "DetectResponse success");


    SocketClose(sockfd);

    return 0;
}

//	��ȡ����״̬����������0�����������ط�0
int GetSrvStatus(char *RIP, char *VIP)
{
    int Ret = 0;
    int ConnectCount = 0;

    //����NetSign�����ļ������ÿ�������Ƿ��������������Srv_Status����
    NodeIterator NodeIter;
    XmlDocument readingDoc;

    //����xml�ļ�
    bool ld = readingDoc.loadFromFile(SRV_CONF_PATH);
    if (false == ld) {
        LOGERROR("%s\n", "LOADXML_ERROR");
        return LOADXML_ERROR;
    }

    //��ȡ���ڵ�
    XmlNode *readingNode = readingDoc.findChild("service-config");
    if (NULL == readingNode) {
        LOGERROR("%s\n", "GET_ROOTNODE_ERROR");
        return GET_ROOTNODE;
    }
    XmlNode *serviceNode = NULL;

    for (serviceNode = readingNode->findFirstChild("service", NodeIter);
         NULL != serviceNode;
         serviceNode = readingNode->findNextChild("service", NodeIter)) {

        //ֱ�ӻ�ȡcheck-service
        XmlNode *ifchksrvNode = serviceNode->findChild("checkService");

        if (NULL == ifchksrvNode || 0 == strcmp(ifchksrvNode->getString(), "true")) {

            //��ȡservice����
            XmlAttribute *attr = serviceNode->findAttribute("name");
            if (attr == NULL) {
                LOGERROR("%s\n", "GET_SRVNAME_ERROR");
                return GET_SRVNAME;
            }

            //ֱ�ӻ�ȡservice port
            XmlNode *portNode = serviceNode->findChild("port");
            if (portNode == NULL) {
                LOGERROR("%s\n", "GET_PORTNODE_ERROR");
                return GET_PORTNODE;
            }

            for (ConnectCount = 0; ConnectCount <= connect_retry; ConnectCount++) {
                LOGINFO("Detect Service [%s]...\n", attr->getString());

                Ret = DetectService(RIP, atoi(portNode->getString()), connect_timeout);
                if (0 == Ret) {
                    break;
                }
            }

            if (0 != Ret) {
                LOGERROR("Service [%s] status: invalid\n", attr->getString());

                return GOTO_STANDBY;
            } else {
                LOGINFO("Service [%s] status: normal\n", attr->getString());

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

    FILE *fp = fopen(MONSRVCONF, "r");
    if (NULL == fp) {
        LOGERROR("%s\n", "Open monsrvconf err");
        return -1;
    }
    char *pName = NULL;
    char *pValue = NULL;

    while (!feof(fp)) {
        memset(Buff, 0x00, FILEBUFFSIZE);
        fgets(Buff, FILEBUFFSIZE, fp);


        //���ﵥ����detectstring��������Ϊdetectstring��Base64�ַ�����
        //���п��ܺ���=�ţ���strtok�����㴦��
        if (0 == memcmp(Buff, "detectstring=", strlen("detectstring="))) {
            strcpy(base64_detect_string, strstr(Buff, "=") + 1);

            trim(base64_detect_string);

            ClearB64Trash((unsigned char *) base64_detect_string, strlen(base64_detect_string),
                          (unsigned char *) b64_det_str_no_trash, &b64_det_str_no_trash_len);

            IF_DEBUG printf("Base64DetectString = %s\n", b64_det_str_no_trash);

            detect_strLen = Base64Decode(b64_det_str_no_trash, b64_det_str_no_trash_len, detect_string);

            IF_DEBUG ViewMem("detect_string", (unsigned char *) detect_string, detect_strLen);
        }

        pName = strtok(Buff, "=");
        if (NULL == pName) continue;

        pValue = strtok(NULL, "=");
        if (NULL == pValue) continue;

        if (0 == strcmp(pName, "connecttimeout")) {
            connect_timeout = atoi(trim(pValue));
        } else if (0 == strcmp(pName, "connectretrytimes")) {
            connect_retry = atoi(trim(pValue));
        }
        if (0 == strcmp(pName, "pingtimeout")) {
            ping_timeout = atoi(trim(pValue));
        } else if (0 == strcmp(pName, "pingretrytimes")) {
            ping_retry = atoi(trim(pValue));
        } else if (0 == strcmp(pName, "checkdetectret")) {
            if (0 == strcmp("YES", trim(pValue))) {
                //	LOGINFO("CheckDetectRet:%s","YES");
                if_chk_detect_ret = 1;
            } else if (0 == strcmp("NO", trim(pValue))) {
                //	LOGINFO("CheckDetectRet:%s","NO");
                if_chk_detect_ret = 0;
            } else {
                LOGERROR("%s\n", "MonsrvConf Parameter Err");
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
    //���������ļ�
    if (0 != LoadMonSrvConf()) {
        LOGERROR("%s\n", "LoadMonSrvIni err");
        goto EXIT;
    }

    EXIT:

    LOGINFO("%s\n", "MonSrv init");

    return 0;
}



