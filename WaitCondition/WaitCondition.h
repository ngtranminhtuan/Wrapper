#ifndef  CWaitCondition_H
#define  CWaitCondition_H
extern "C" {
#include <pthread.h>
#include<climits>
#include<errno.h>
}
 
using namespace std;
class CMutex;
 
/**
*  Wrapper class for conditional wait
*/
class CWaitCondition
{
public:
    /**
     * Enum to represent Wait Result
     */
    enum WaitConditionResult
    {
       SIGNALLED = 0,
       TIMED_OUT = ETIMEDOUT,
       INVALID   = 255
    };
    /**
     *   Default Constructor
     */
    CWaitCondition();
 
    /**
     *   virtual destructor
     */
    virtual ~CWaitCondition();
 
    /**
     *   Releases the lockedMutex and waits on the wait condition.
     *   The lockedMutex must be initially locked by the calling thread.
     *   If lockedMutex is not in a locked state, the behavior is undefined.
     *   The lockedMutex will be unlocked, and the calling thread will block
     *   until either of these conditions is met
     *   1):Another thread signals it using Signal().
     *      This function will return SIGNALLED in this case.
     *   2):Time 'timeToWait' seconds has elapsed. If time is ULONG_MAX (the default),
     *     then the wait will never timeout (the event must be signalled).
     *     This function will return TIMED_OUT, if the wait timed out.
     *   The lockedMutex will be returned to the same locked state.
     *   This function is provided to allow the atomic transition from
     *   the locked state to the wait state.
     *   @return WaitConditionResult SIGNALLED , TIMED_OUT or INVALID
     *   @param lockedMutex lockedMutex
     *   @param timeToWait Time to wait in seconds.
     */
    int Wait(CMutex&  lockedMutex, unsigned long timeToWait = ULONG_MAX);
 
    /**
     *   Signals the wait condition
     */
    int Signal();
 
private:
 
    /**
     *   Internal pthraed cond variable
     */
    pthread_cond_t m_Cond;
    bool m_bPredicate;
};
#endif