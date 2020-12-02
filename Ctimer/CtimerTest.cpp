#include<iostream>
#include "CtimerTest.h"
#include "CTimer.h"
 
void CTimerTest::OnStart()
{
    std::cout<<"Timer started";
}
void CTimerTest::OnStop()
{
    std::cout<<"Timer Stopped";
}
void CTimerTest::OnTimeOut()
{
    std::cout<<"Timer timed out";
}
 
/**
* Sample test function using the CTimer class
**/
void CTimerTest::Test()
{
    CTimer<CTimerTest> timer(this);
    timer.SetTimerType(TimerType::MULTI_SHOT);
    timer.SetInterval(30000);
    try {
        timer.Start();
    } catch(CTimerException& ex)
    {
        std::cout<<"Timer exception:"<<ex.what();
    }
}