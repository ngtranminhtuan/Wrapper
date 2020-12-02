#include "pch.h"
#include "Worker.h"
#include "Command.h"
#include "ThreadPool.h"
 
void Worker::ThreadFun()
{
while (m_Pool.Running())
{
ICommand * job = nullptr;
 
if (!m_Pool.GetQueue().empty()) {
job = m_Pool.GetQueue().front();
m_Pool.GetQueue().pop();
}
 
if (job)
job->Execute();
}
}