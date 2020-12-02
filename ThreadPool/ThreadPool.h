#pragma once
#include "BlockingQueue.h"
class ThreadPool
{
public:
ThreadPool(int nSize = std::thread::hardware_concurrency()) :m_nSize(nSize), m_bRunning(true)
{
 
}
 
~ThreadPool()
{
ShutDown();
}
 
void Enque(ICommand * cmd)
{
mtxThreadJob.lock();
m_Jobs.push(cmd);
mtxThreadJob.unlock();
m_Cv.notify_one();
}
 
void Start()
{
for (int i = 0; i < m_nSize; i++)
{
Worker *pWorker = new Worker(*this);
m_Workers.push_back(pWorker);
 
}
}
 
void ShutDown()
{
{
std::unique_lock lock(mtxThreadStatus);
m_bRunning = false;
}
m_Cv.notify_all();
// Join all threads.
for (Worker *worker : m_Workers)
{
worker->Join();
delete worker;
}
 
m_Workers.clear();
}
 
volatile bool& Running()
{
return m_bRunning;
}
 
std::condition_variable& GetCondition()
{
return m_Cv;
}
 
private:
BlockingQueue& GetQueue()
{
return m_Jobs;
}
 
std::vector m_Workers;
BlockingQueue m_Jobs;
volatile bool m_bRunning;
std::mutex mtxThreadStatus;
std::mutex mtxThreadJob;
int m_nSize;
std::condition_variable m_Cv;
friend class Worker;
};