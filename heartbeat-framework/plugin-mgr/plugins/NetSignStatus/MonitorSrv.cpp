#include "MonitorSrv.h"


using namespace std;
using namespace slim;


int connect_timeout = 1500;
int connect_retry = 3;
int ping_timeout = 1500;
int ping_retry = 3;

//0为不检查，1为检查
int if_chk_detect_ret = 0;

int detect_strLen = 0;
char detect_string[FILEBUFFSIZE] = {0x00};

int base64_detect_strLen = 0;
char base64_detect_string[FILEBUFFSIZE] = {0x00};
int b64_det_str_no_trash_len = 0;
char b64_det_str_no_trash[FILEBUFFSIZE] = {0x00};

int debug_mode = 0;
#define IF_DEBUG if(0 != debug_mode)

//0 为不开启检查主机服务，1为开启检查主机服务， 
//默认设置为不启用检查是为了防止主机服务绑定在虚IP上。

//如果mon_mode为0，则(监控程序仅在HA主机运行，主机监控本身服务，不正常则切换，正常则往主机回切)
//如果mon_mode为1，则(如果虚IP不在本机则检测有虚IP的机器服务状态，如果在本机则什么都不做)
//如果mon_mode为2，则(本机有虚IP->本机服务不正常->对端服务正常->切换过去)
int mon_mode = 0;

static int lock_fd = -1;

//	检测服务连接及响应
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

//	获取服务状态，正常返回0，不正常返回非0
int GetSrvStatus(char *RIP, char *VIP)
{
    int Ret = 0;
    int ConnectCount = 0;

    //解析NetSign配置文件并检测每个服务是否正常将结果存入Srv_Status数组
    NodeIterator NodeIter;
    XmlDocument readingDoc;

    //载入xml文件
    bool ld = readingDoc.loadFromFile(SRV_CONF_PATH);
    if (false == ld) {
        LOGERROR("%s\n", "LOADXML_ERROR");
        return LOADXML_ERROR;
    }

    //获取根节点
    XmlNode *readingNode = readingDoc.findChild("service-config");
    if (NULL == readingNode) {
        LOGERROR("%s\n", "GET_ROOTNODE_ERROR");
        return GET_ROOTNODE;
    }
    XmlNode *serviceNode = NULL;

    for (serviceNode = readingNode->findFirstChild("service", NodeIter);
         NULL != serviceNode;
         serviceNode = readingNode->findNextChild("service", NodeIter)) {

        //直接获取check-service
        XmlNode *ifchksrvNode = serviceNode->findChild("checkService");

        if (NULL == ifchksrvNode || 0 == strcmp(ifchksrvNode->getString(), "true")) {

            //获取service名称
            XmlAttribute *attr = serviceNode->findAttribute("name");
            if (attr == NULL) {
                LOGERROR("%s\n", "GET_SRVNAME_ERROR");
                return GET_SRVNAME;
            }

            //直接获取service port
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


//	从conf.ini中读取配置参数
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


        //这里单独对detectstring处理是因为detectstring是Base64字符串，
        //当中可能含有=号，用strtok不方便处理。
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
    //载入配置文件
    if (0 != LoadMonSrvConf()) {
        LOGERROR("%s\n", "LoadMonSrvIni err");
        goto EXIT;
    }

    EXIT:

    LOGINFO("%s\n", "MonSrv init");

    return 0;
}



