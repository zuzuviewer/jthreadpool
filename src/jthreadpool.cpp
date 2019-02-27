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

JThreadPool::JThreadPool(const unsigned int maxThreadCount, const int32_t currentThreadCount):
    maxThreadCount_(maxThreadCount),
    currentThreadCount_(currentThreadCount),
    isExit_(false)
{

}

JThreadPool::~JThreadPool()
{
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

int JThreadPool::currentThreadId()
{
#ifdef WIN32
    return GetCurrentThreadId();
#else
    return gettid();
#endif
}

bool JThreadPool::addTask(const std::function<void()> task)
{
    taskQueueMutex_.lock();
    taskQueue_.push(task);
    taskQueueMutex_.unlock();
    return true;
}

bool JThreadPool::start()
{
    if(currentThreadCount_ <= 0){
        return false;
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

void JThreadPool::threadRun()
{
    for(;;){
        if(isExit_){
            break;
        }
        taskQueueMutex_.lock();
        if(taskQueue_.empty()){
            taskQueueMutex_.unlock();
            JThreadPool::sleep(100);
            continue;
        }
        const auto&& task = std::move(taskQueue_.back());
        taskQueue_.pop();
        taskQueueMutex_.unlock();
        try {
            task();
        } catch (std::exception &ex) {
            std::cout << "ThreadPool执行任务捕获到异常 :" << ex.what()<<std::endl;
        }
        JThreadPool::sleep(100);
    }
}
