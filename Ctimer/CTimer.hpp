/**
 *   C++ Wrapper class for POSIX timer API Impl
 **/
#include "CTimer.h"
#include "TimerException.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
 
 
template <class T>
const unsigned long CTimer<T>::MILLISECONDS_TO_NANOSECONDS_FACTOR = 1000000;
template <class T>
void CTimer<T>::TimerThreadFunc(union sigval arg)
{
    CTimer<T>* pThis = static_cast<CTimer<T>*>(arg.sival_ptr);
    pThis->GetHandler()->OnTimeOut();
}
 
template <class T>
CTimer<T>::CTimer(T * pHandler):m_Handler(pHandler),
                                m_TimerType(TimerType::MULTI_SHOT),
                                m_nInterval(100),
                                m_TimerID(NULL)
{
 
}
template <class T>
T*  CTimer<T>::GetHandler()    { return m_Handler; }
 
template <class T>
TimerType  CTimer<T>::GetTimerType()
{
    return m_TimerType;
}
template <class T>
void  CTimer<T>::SetTimerType(TimerType type)
{
    m_TimerType = type;
}
template <class T>
void  CTimer<T>::SetInterval(long msec)
{
    m_nInterval = msec;
}
template <class T>
int  CTimer<T>::GetInterval()
{
    return m_nInterval;
}
 
template <class T>
void CTimer<T>::Start(long msec)
{
    StopTimer();
    StartTimer(msec);
    m_Handler->OnStart();
}
 
template <class T>
void CTimer<T>::Start()
{
    StopTimer();
    StartTimer(m_nInterval);
    m_Handler->OnStart();
}
 
template <class T>
void CTimer<T>::StopTimer() {
    if(m_TimerID != NULL)
    {
        try {
            Stop();
        } catch(CTimerException& ex)
        {   //Catching exception if any during stoping of alraedy started timer
            cout<<"Exception in stopping the timer."<<ex.what();
        }
    }
}
 
 
template <class T>
void CTimer<T>::Stop()
{
    int status = timer_delete(m_TimerID);
    if (status == -1){
        HandleTimerDeleteError();
        return;
    }
    m_TimerID = NULL;
    m_Handler->OnStop();
}
 
template <class T>
void CTimer<T>::HandleTimerCreateError() const {
    switch(errno)
    {
        case EAGAIN:
            throw CTimerException("Temporary error during kernel allocation of timer structures");
            break;
        case EINVAL:
            throw CTimerException("Invalid parameter passed");
            break;
        case ENOMEM:
            throw CTimerException("Could not allocate memory.");
            break;
        default:
            throw CTimerException("Unknown exception occurred in timer creation");
            break;
    }
}
 
template <class T>
void CTimer<T>::HandleTimerSetTimeError() const {
    switch(errno)
    {
        case EFAULT:
            throw CTimerException("Invalid internal data passed");
            break;
        case EINVAL:
            throw CTimerException("Invalid timer ");
            break;
        default:
            throw CTimerException("Unknown exception occurred in setting the timer value");
            break;
    }
}
 
template <class T>
void CTimer<T>::HandleTimerDeleteError() const {
    switch(errno)
    {
        case EINVAL:
            throw CTimerException("Invalid timer");
            break;
        default:
            throw CTimerException("Unknown exception occurred in stopping the timer");
            break;
    }
}
 
template <class T>
void CTimer<T>::StartTimer(long msec)
{
 
    int status;
    struct itimerspec ts;
    struct sigevent se;
 
    /*
     * Set the sigevent structure to cause the signal to be
     * delivered by creating a new thread.
     */
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr =  this;
    se.sigev_notify_function = TimerThreadFunc;
    se.sigev_notify_attributes = NULL;
 
    if(m_TimerType == TimerType::MULTI_SHOT)
    {
        ts.it_value.tv_sec     = 0;
        ts.it_value.tv_nsec    = 1;
        ts.it_interval.tv_sec  = msec*MILLISECONDS_TO_NANOSECONDS_FACTOR / 1000000000;
        ts.it_interval.tv_nsec = (msec *MILLISECONDS_TO_NANOSECONDS_FACTOR) % 1000000000;
    } else{
        ts.it_value.tv_sec     = msec*MILLISECONDS_TO_NANOSECONDS_FACTOR / 1000000000;
        ts.it_value.tv_nsec    = (msec *MILLISECONDS_TO_NANOSECONDS_FACTOR) % 1000000000;
        ts.it_interval.tv_sec  = 0;
        ts.it_interval.tv_nsec = 0;
    }
 
 
    status = timer_create(CLOCK_REALTIME, &se, &m_TimerID);
    if (status == -1){
        HandleTimerCreateError();
        return;
    }
 
    status = timer_settime(m_TimerID, 0, &ts, 0);
    if (status == -1){
        HandleTimerSetTimeError();
        return;
    }
 
}