#ifndef  CMUTEX_H
#define  CMUTEX_H
extern "C" {   
#include <pthread.h>
}
 
/** 
*  Wrapper class for mutex lock and unlock 
*/
class CMutex
{
public:
    /**
     *   Default Constructor for thread
     */
    CMutex();
     
    /**
     *   virtual destructor
     */
    virtual ~CMutex();
     
    /**
     *   Locks the mutex
     */
    int Lock();
 
    /**
     *   Unlocks the mutex
     */
    int UnLock();
 
private:
 
    /**
     *   Internal pthraed mutex lock variable
     */
    pthread_mutex_t m_plock;
};
#endif