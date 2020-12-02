#include "Mutex.h"
CMutex::CMutex()
{
    m_plock = PTHREAD_MUTEX_INITIALIZER;
}
 
CMutex::~CMutex()
{
    pthread_mutex_unlock(&m_plock);
}
 
int CMutex::Lock()
{
    return pthread_mutex_lock(&m_plock); 
}
 
int CMutex::UnLock()
{
    return pthread_mutex_unlock(&m_plock); 
}