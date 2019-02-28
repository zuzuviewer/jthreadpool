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

bool JThreadPool::addTask(const std::function<void()>&& task)
{
    taskQueueMutex_.lock();
    taskQueue_.push(std::move(task));
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
            JThreadPool::sleep(10);
            continue;
        }
        /**左边变量写成引用或者右值引用都会有问题，最后一次调用都提示std::function没有可用的函数调用.
         * 不知道什么原因（初步判断是因为RAII导致function对象里面指向的函数的指针被置空）**/
        //const auto&& task = std::move(taskQueue_.back());
       // const auto task = std::move(taskQueue_.back());
        const auto task = taskQueue_.back();
        taskQueue_.pop();
        taskQueueMutex_.unlock();
        try {
            task();
        } catch (std::exception &ex) {
            std::cout << "ThreadPool执行任务捕获到异常 :" << ex.what()<<std::endl;
        }
        JThreadPool::sleep(10);
    }
}

