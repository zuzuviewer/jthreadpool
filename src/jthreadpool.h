#ifndef JTHREADPOOL_H
#define JTHREADPOOL_H

#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <atomic>
#include <list>

/**
 * @brief The JThreadPool class 线程池
 */
class JThreadPool
{
public:
    JThreadPool();
    JThreadPool(const unsigned int maxThreadCount,const int32_t currentThreadCount);
    ~JThreadPool();

public:
    static void sleep(const int32_t millSecond);
    static int currentThreadId();

public:
    bool addTask(const std::function<void()> task);

    bool start();

    void stop();

protected:
    void threadRun();

private:
    unsigned int maxThreadCount_ = std::thread::hardware_concurrency();
    int32_t currentThreadCount_ = 0;//当前线程数量
    std::queue<std::function<void()>> taskQueue_;//执行任务任务队列
    std::mutex taskQueueMutex_;//任务队列锁
    std::atomic_bool isExit_;//线程是否退出
    std::list<std::thread> threadGroup_;//线程管理
};

#endif // JTHREADPOOL_H
