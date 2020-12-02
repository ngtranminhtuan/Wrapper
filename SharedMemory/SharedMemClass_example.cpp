#include "SharedMemory.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include<cstdio>
using namespace std;
using namespace MST;
int main(int argc, char* argv[])
{
   try
   {
      SharedMemory shmMemory("/testSharedmemory1");
      shmMemory.Create(100);
      shmMemory.Attach();
      char* str = (char*)shmMemory.GetData();
 
      if(std::string(argv[1]) =="1")
      {
         for(int i=0;i<10;i++)
         {
            char sTemp[10];
            
            shmMemory.Lock();
            sprintf(sTemp,"Data:%d", rand()%100);
            strcpy(str,NULL);
            strcpy(str,sTemp);
            printf("\nWriting:%s",str);
            shmMemory.UnLock();
            sleep(4);
         }
 
      }else
      {
         for(int i=0;i<10;i++)
         {
            char sTemp[10];
            printf("\nReading:%d",i+1);
            shmMemory.Lock();
            printf("--->%s",str);
            shmMemory.UnLock();
            sleep(4);
         }
      }
 
   }
   catch (std::exception& ex)
   {
      cout<<"Exception:"<<ex.what();
   }
 
}