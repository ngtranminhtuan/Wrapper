I think you are familiar with semaphores. Shortly saying Semaphore allows thread or process to synchronize their actions. Here I will explain about new POSIX API for semaphore. POSIX semaphore can be of two types
1) Named semaphore
A named semaphore is identified by a name of the form “/somename”. It can be used across process and inside process(thread)
2) Unnamed semaphore(Anonymous semaphore)
It doesn’t have a name instead an ID which is shared between multiple threads(thread shared semaphore) or processes(process shared semaphore)

Here I have shared object oriented C++ wrapper for POSIX semaphore API.
Following class diagram shows how you can use it.