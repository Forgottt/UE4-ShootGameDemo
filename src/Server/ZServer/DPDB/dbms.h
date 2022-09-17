#ifndef DBMS_H
#define DBMS_H

#include <map>
//#include <multimap>
#include <time.h>
#include <stdint.h>
#include <string>
using namespace std;

class DBMS
{
private:
struct MemData
{
    std::string m_strValue;
    uint64_t    m_qwTime;
};
    map<std::string,MemData> m_mapAllData;
    std::multimap<uint64_t,std::string> m_mapAllDataTime;
public:
    static DBMS * GetInstance()
    {
        static DBMS *one = NULL;
        if(NULL == one)
        {
            one = new DBMS;
        }
        return one;
    }

    void Insert(std::string key,std::string value,uint64_t iLastTime);
    void UpdateData();
    bool Find(std::string key,std::string& out);
    int Size()
    {
        return m_mapAllData.size();
    }
    void Del(std::string strKey);

};
#endif
