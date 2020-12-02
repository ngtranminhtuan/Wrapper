#pragma once
#include
#include
#include
class ThreadPool;
class Worker
{
public:
Worker(ThreadPool& pool) :m_Pool(pool), m_bBoolStatus(false), m_bRunning(true)
{
m_Thread = new std::thread(&Worker::ThreadFun, this);
}
 
void Join()
{
m_Thread->join();
}
 
private:
void ThreadFun();
 
bool m_bBoolStatus;
std::thread* m_Thread;
volatile bool m_bRunning;
ThreadPool& m_Pool;
};