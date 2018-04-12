#ifndef _DATAMANAGER_H_
#define _DATAMANAGER_H_

#include "define.h"
#include "ObjectLru.h"
#include "ace/Date_Time.h"

#include "tinyxml.h"
#include "tinystr.h"

#include <vector>
#include <map>
using namespace std;

//PSS�ڵ�����Ϣ
typedef struct PSSNODEINFO
{
    char           m_szClientIP[MAX_BUFF_50];   //�ͻ��˵�IP��ַ
    uint32         m_u4Cpu;
    uint32         m_u4MemorySize;
    uint32         m_u4ConnectCount;
    uint32         m_u4DataIn;
    uint32         m_u4DataOut;
    ACE_Time_Value m_tvRecvTime;

    PSSNODEINFO()
    {
        m_szClientIP[0]  = '\0';
        m_u4Cpu          = 0;
        m_u4MemorySize   = 0;
        m_u4ConnectCount = 0;
        m_u4DataIn       = 0;
        m_u4DataOut      = 0;
        m_tvRecvTime     = 0;
    }

    //�������캯��
    PSSNODEINFO(const PSSNODEINFO& ar)
    {
        sprintf_safe(this->m_szClientIP, MAX_BUFF_50, "%s", ar.m_szClientIP);
        this->m_u4Cpu        = ar.m_u4Cpu;
        this->m_u4MemorySize = ar.m_u4MemorySize;
        this->m_u4ConnectCount = ar.m_u4ConnectCount;
        this->m_u4DataIn = ar.m_u4DataIn;
        this->m_u4DataOut = ar.m_u4DataOut;
        this->m_tvRecvTime = ar.m_tvRecvTime;
    }

    PSSNODEINFO& operator = (const PSSNODEINFO& ar)
    {
        sprintf_safe(this->m_szClientIP, MAX_BUFF_50, "%s", ar.m_szClientIP);
        this->m_u4Cpu        = ar.m_u4Cpu;
        this->m_u4MemorySize = ar.m_u4MemorySize;
        this->m_u4ConnectCount = ar.m_u4ConnectCount;
        this->m_u4DataIn = ar.m_u4DataIn;
        this->m_u4DataOut = ar.m_u4DataOut;
        this->m_tvRecvTime = ar.m_tvRecvTime;
        return *this;
    }

    void Serialization(char* pData, int nSize)
    {
        sprintf_safe(pData, nSize, "%s,%d,%d,%d,%d,%d,%d,%04d,%ld,\n",
                     m_szClientIP,
                     m_u4Cpu,
                     m_u4MemorySize,
                     m_u4ConnectCount,
                     m_u4DataIn,
                     m_u4DataOut,
                     m_tvRecvTime.sec());
    }

    void UnSerialization(char* pData, int nSize)
    {
        vector<string> vecData;
        SplitData(pData, vecData);

        if (vecData.size() == 7)
        {
            sprintf_safe(m_szClientIP, MAX_BUFF_50, "%s", vecData[0].c_str());
            m_u4Cpu = atoi(vecData[1].c_str());
            m_u4MemorySize = atoi(vecData[2].c_str());
            m_u4ConnectCount = atoi(vecData[3].c_str());
            m_u4DataIn = atoi(vecData[4].c_str());
            m_u4DataOut = atoi(vecData[5].c_str());

            //���ʱ��
            m_tvRecvTime.sec((time_t)atol(vecData[6].c_str()));
        }
    }

private:
    void SplitData(char* pData, vector<string>& vecData)
    {
        vecData.clear();
        const char* d = ",";
        char* p = NULL;
        p = strtok(pData, d);

        while (p)
        {
            string strData = p;
            vecData.push_back(strData);
            p = strtok(NULL, d);
        }
    }

} PssNodeInfoSt;

class CDataManager
{
public:
    ~CDataManager();

    void Close();

    //����xml�����ļ�
    bool ParseXmlFile(const char* pXmlFile);

    //���ӽڵ�����
    void AddNodeDate(const char* pIP, uint32 u4Cpu,uint32 u4MemorySize,uint32 u4ConnectCount,uint32 u4DataIn,uint32 u4DataOut);

    //����index html�ļ�
    void make_index_html();

    //����detail html�ļ�
    void make_detail_html();

    //�������л��ļ�
    void Serialization();

    //����
    void UnSerialization();

private:
    CDataManager();

    char* GetData(TiXmlElement* pRootElement,const char* pName, const char* pAttrName);
    char* GetData(TiXmlElement* pRootElement,const char* pName, const char* pAttrName, TiXmlElement*& pNextTiXmlElement);
public:
    static CDataManager* GetInstance();

    //��ȡ��������ʱ����
    uint32 GetTimeInterval();
private:
    static CDataManager* m_pInstance;

    //���ƶ��߳���
    ACE_Recursive_Thread_Mutex  m_ThreadWriteLock;

    typedef CObjectLruList<PssNodeInfoSt, ACE_Null_Mutex> PssNodeInfoList;
    typedef map<string,string> mapIP2GroupName;
    typedef map<string, PssNodeInfoList*> mapIP2NodeData;
    typedef map<string, mapIP2NodeData> mapGroupNodeData;

    //ip��������ӳ��
    mapIP2GroupName m_mapIP2GroupName;

    //�鵽��Ӧ�Ľڵ���Ϣ��ӳ��
    mapGroupNodeData m_mapGroupNodeData;

private:
    string m_strHtmlIndexPath;      //htmlindex�ļ���·��
    string m_strHtmlIndexName;      //htmlindex�ļ�������
    string m_strHtmlDetailPath;     //htmldetail�ļ���·��
    string m_strHtmlJsonPath;       //htmlJson�ļ���·��
    string m_strSerializationFile;  //���л��洢�ļ�λ��
    uint32 m_u4HistoryMaxCount;
    uint32 m_u4TimeInterval;

    TiXmlDocument* m_pTiXmlDocument;
    TiXmlElement*  m_pRootElement;
};

#endif //_DATAMANAGER_H_