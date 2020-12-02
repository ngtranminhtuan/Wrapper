#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
 
#include "Thread.h"
#include "Semaphore.h"
#include <iostream>
using namespace IPC;
using namespace std;
/* prototype for thread routine */
 
 
 
/* semaphores are declared global so they can be accessed
in main() and in thread routine,
here, the semaphore is used as a mutex */
 
ISemaphore* pSemaphore;
int n_GlobalCounter; /* shared variable */
 
class SemaphoreTestThread : public CThread
{
 
public:
    SemaphoreTestThread(int nID) :m_nID(nID){}
 
    virtual void Run()
    {
        printf("\nInside run");
        pSemaphore->Wait();
        /* START CRITICAL REGION */
        printf("Thread %d: Now in critical region...\n", m_nID);
        printf("Thread %d: Counter Value: %d\n", m_nID, n_GlobalCounter);
        printf("Thread %d: Incrementing Counter...\n", m_nID);
        n_GlobalCounter++;
        printf("Thread %d: New Counter Value: %d\n", m_nID, n_GlobalCounter);
        printf("Thread %d: Exiting critical region...\n", m_nID);
        /* END CRITICAL REGION */
        pSemaphore->Post();
    }
    int m_nID;
};
 
int main()
{
    printf("\nStarting Test App run");
 
    try
    {
         
        //Binary semaphore
        pSemaphore = new CAnoymousSemaphore(1, IPC::INSIDE_PROCESS);
     
        SemaphoreTestThread thread1(1);
        SemaphoreTestThread thread2(2);
        thread1.Start();
        thread2.Start();
        thread1.Join();
        thread2.Join();
        /* destroy semaphore */
        pSemaphore->Remove();
        delete pSemaphore;
    
    }   
    catch (CSemaphoreException& ex)
    {
        cout << "\nException occured:" << ex.what();
    }
 
    return 0;
    /* exit */
//  exit(0);
} /* main() */