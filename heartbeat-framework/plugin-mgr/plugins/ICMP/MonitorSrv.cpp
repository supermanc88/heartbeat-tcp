#include "MonitorSrv.h"


using namespace std;

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
        else if (0 == strcmp(pName, "mon_mode")) {
            mon_mode = atoi(trim(pValue));

            LOGINFO("mon_mode = %d\n", mon_mode);

            if (mon_mode > 2 || mon_mode < 0) {
                LOGERROR("%s\n", "MonsrvConf Parameter Err");
                fclose(fp);
                return -1;
            }
        }
    }
    fclose(fp);
    return 0;
}


