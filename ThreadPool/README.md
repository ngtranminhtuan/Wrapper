I think you are familiar with thread pool.
Thread pool will be useful when you have lot of tasks to be done parallelly by dedicated threads.

Advantage will be you can save thread creation overhead each time new task has to be executed and each task can be executed parallelly
I had got opportunity to create thread pool for both in Linux (using pthread) and windows(win32 thread).Now threading is there in C++ (C++ 11 onwards); here I will share a thread pool class using modern C++ .This thread pool class uses a blocking queue which I posted early:blocking queue