/**
 *   C++ Wrapper class for POSIX timer API.
 */
 
#ifndef MYAPPLICATION_CTIMER_H
#define MYAPPLICATION_CTIMER_H
 
#include <time.h>
 
/**
 *   Timer type.
 */
typedef enum
{
    SINGLE_SHOT, //timer which Signals only one time
    MULTI_SHOT //Periodic timer
}TimerType;
 
template <class T>
class CTimer {
public:
  /**
   *   Construct a CTimer with a explanatory message.
   *   @param pHandler class to which timer needs to notify the during time out
   *
   **/
    CTimer(T * pHandler);
    TimerType GetTimerType();
    void SetTimerType(TimerType type);
    /*
     * Sets the interval for timer
     */
    void SetInterval(long msec);
    int GetInterval();
    /*
     * gets the internal thread ID
     */
    timer_t GetID() {  return m_TimerID; };
    /*
     * Starts the timer with interval passed.
     * @param msec Timer interval
     */
    void Start(long msec);
    /*
     * Starts the timer with interval set.
     *
     */
    void Start();
    /*
     * Stops the timer
     */
    void Stop();
T*  GetHandler();
private:
    static void TimerThreadFunc(union sigval arg);
    void HandleTimerCreateError()  const;
    void HandleTimerSetTimeError() const;
    void HandleTimerDeleteError()  const;
    void StartTimer(long msec);
    void StopTimer();
private:
    /*
     * FACTOR to convert milliseconds to nano seconds
     */
    static const unsigned long  MILLISECONDS_TO_NANOSECONDS_FACTOR;
    TimerType m_TimerType;
    T * m_Handler;
    long m_nInterval;
    /*
     * Internal Timer ID
     */
    timer_t m_TimerID;
};
 
#include "CTimer.hpp"
 
#endif //MYAPPLICATION_CTIMER_H