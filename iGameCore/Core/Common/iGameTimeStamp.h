#ifndef iGameTimeStamp_h
#define iGameTimeStamp_h

#include "iGameMacro.h"
#include "iGameType.h"

IGAME_NAMESPACE_BEGIN

class TimeStamp
{
public:
    TimeStamp() {}
    ~TimeStamp() {}
    

    void Delete()
    {
        delete this;
    }

    static const char* GetNameOfClass()
    {
        return "TimeStamp";
    }

    void Modified()
    {
        static std::atomic<unsigned int> GlobalTimeStamp(0U);
        this->m_ModifiedTime = ++GlobalTimeStamp;
    }

    unsigned int GetMTime() const
    {
        return m_ModifiedTime;
    }

    bool operator>(TimeStamp& ts) const
    {
        return (m_ModifiedTime > ts.m_ModifiedTime);
    }
    bool operator<(TimeStamp& ts) const
    {
        return (m_ModifiedTime < ts.m_ModifiedTime);
    }

    operator unsigned int() const { return m_ModifiedTime; }

private:
    unsigned int m_ModifiedTime{ 0 };
};
IGAME_NAMESPACE_END
#endif

