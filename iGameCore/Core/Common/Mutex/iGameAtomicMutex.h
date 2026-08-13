


#ifndef iGameAtomicMutex_h
#define iGameAtomicMutex_h

#include "iGameObject.h"
#include <atomic>   

IGAME_NAMESPACE_BEGIN
class iGameAtomicMutex  {
public:
    
    iGameAtomicMutex();
    ~iGameAtomicMutex() = default;
    iGameAtomicMutex(const iGameAtomicMutex& other);
    iGameAtomicMutex& operator=(const iGameAtomicMutex& other);

    void lock();
    void unlock();

private:
    std::atomic_bool m_Locked;
};
IGAME_NAMESPACE_END
#endif 
