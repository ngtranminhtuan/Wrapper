#include "WaitCondition.h"
#include "Mutex.h"
#include <time.h>
#include <stdio.h>
#include<cstdlib>
#include <unistd.h>
#include<iostream>
using namespace std;
CWaitCondition::CWaitCondition():m_Cond(PTHREAD_COND_INITIALIZER),m_bPredicate(false)
{
 
}
 
CWaitCondition::~CWaitCondition()
{
    pthread_cond_destroy(&m_Cond);
}
 
int CWaitCondition::Wait(CMutex& mutex, unsigned long timeToWait )
{
    int nRet = 0;
    if (timeToWait == ULONG_MAX)
    {
        while(!m_bPredicate)
            nRet = pthread_cond_wait(&m_Cond, &(mutex.GetLock()));
        m_bPredicate = false;
    }
    else
    {
        struct timespec time_to_wait;
        time_to_wait.tv_sec = time(NULL) + timeToWait;
        while(!m_bPredicate)
            nRet = pthread_cond_timedwait(&m_Cond, &(mutex.GetLock()), &time_to_wait) ;
        m_bPredicate = false;
    }
    return ((nRet == SIGNALLED || nRet == TIMED_OUT) ? nRet : INVALID);
}
 
int CWaitCondition::Signal()
{
    m_bPredicate = true;
    return pthread_cond_signal(&m_Cond);
}
int CWaitCondition::Broadcast()
{
  return pthread_cond_broadcast(&m_Cond)
}