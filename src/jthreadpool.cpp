#include "jthreadpool.h"

#include <iostream>
#include <exception>
#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

JThreadPool::JThreadPool():isExit_(false)
{

}

JThreadPool::JThreadPool(const int32_t maxThreadCount, const int32_t currentThreadCount):
    maxThreadCount_(maxThreadCount),
    currentThreadCount_(currentThreadCount),
    isExit_(false)
{

}

JThreadPool::~JThreadPool()
{
    if(isRunning()){
       stop();
    }
    if(!threadGroup_.empty()){
        for(auto it = threadGroup_.begin();it != threadGroup_.end();++it){
            if(it->joinable()){
                it->join();
            }
        }
    }
}

void JThreadPool::sleep(const int32_t millSecond)
{
#ifdef WIN32
    Sleep(millSecond);
#else
    usleep(millSecond * 1000);
#endif
}

std::thread::id JThreadPool::currentThreadId()
{
	return std::this_thread::get_id();
//#ifdef WIN32
//	return GetCurrentThreadId();
//#else
//	return gettid();
//#endif
}

bool JThreadPool::start()
{
    if(currentThreadCount_ <= 0){
        return false;
    }
	if (currentThreadCount_ > maxThreadCount_) {
		currentThreadCount_ = maxThreadCount_;
	}
    for(int32_t i = 0;i < currentThreadCount_;++i){
        threadGroup_.push_back(std::thread(&JThreadPool::threadRun,this));
    }
    return true;
}

void JThreadPool::stop()
{
    isExit_ = true;
}

bool JThreadPool::isRunning() const
{
    return !isExit_;
}

void JThreadPool::threadRun()
{
	while (!isExit_) {
		function_wrapper func;
		if (taskQueue_.try_pop(func)) {
			try
			{
				func();
			}
			catch (const std::exception& ex)
			{
				std::cout << "ThreadPool执行任务捕获到异常 :" << ex.what() << std::endl;
			}
		}
		std::this_thread::yield();
	}
}

