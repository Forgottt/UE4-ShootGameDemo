#include "dbms.h"
void DBMS::Del(std::string strKey)
{
   map<std::string,MemData>::iterator it =  m_mapAllData.find(strKey);

   if(it!=m_mapAllData.end())
   {
    MemData & temMemData =  it->second;
   int iCount = m_mapAllDataTime.count(temMemData.m_qwTime);

    std::multimap<uint64_t,std::string>::iterator itMultiMap =  m_mapAllDataTime.find(temMemData.m_qwTime);

    for(int i=0;i<iCount;i++)
    {
        if(itMultiMap->second == strKey)
        {
            m_mapAllDataTime.erase(itMultiMap);
            break;
        }
        itMultiMap++;
    }
    m_mapAllData.erase(it);
   }
}

void DBMS::UpdateData()
{
    time_t iCurTime = time(0);
    while(m_mapAllDataTime.size()>0)
    {
     std::multimap<uint64_t,std::string>::iterator it=   m_mapAllDataTime.begin();
     uint64_t iDelTime = it->first;

        if(iCurTime > (time_t)iDelTime)
        {
            m_mapAllData.erase(it->second);
            m_mapAllDataTime.erase(it);
        }
        else
        {
            break;//return;
        }
    }
}


bool DBMS::Find(std::string key,std::string& out)
{
    map<std::string,MemData>::iterator it = m_mapAllData.find(key);
    if(it != m_mapAllData.end())
    {
        out = it->second.m_strValue;
        return true;
    }
    else
        return false;
}

void DBMS::Insert(std::string key,std::string value,uint64_t iLastTime)
{
#ifdef _REDIS_
    redis_insert(key,value,iLastTime);
#else
   this->Del(key);

   iLastTime = time(0)+iLastTime;
    m_mapAllDataTime.insert( std::make_pair(iLastTime,key) );
    MemData memData;
    memData.m_qwTime=iLastTime;
    memData.m_strValue = value;
    m_mapAllData[key] = memData;
#endif
}
