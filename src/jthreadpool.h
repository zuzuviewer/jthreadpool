#ifndef JTHREADPOOL_H
#define JTHREADPOOL_H

#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <atomic>
#include <list>
#include <future>
#include "jthreadsafequeue.h"

class function_wrapper
{
	struct impl_base {
		virtual void call() = 0;
		virtual ~impl_base() {}
	};

	std::unique_ptr<impl_base> impl;
	template<typename F>
	struct impl_type : impl_base
	{
		F f;
		impl_type(F&& f_) : f(std::move(f_)) {}
		void call() { f(); }
	};
public:
	template<typename F>
	function_wrapper(F&& f) :
		impl(new impl_type<F>(std::move(f)))
	{}

	void operator()() { impl->call(); }

	function_wrapper() = default;

	function_wrapper(function_wrapper&& other) :
		impl(std::move(other.impl))
	{}

	function_wrapper& operator=(function_wrapper&& other)
	{
		impl = std::move(other.impl);
		return *this;
	}

	function_wrapper(const function_wrapper&) = delete;
	//function_wrapper(function_wrapper&) = delete;
	function_wrapper& operator=(const function_wrapper&) = delete;
};

/**
 * @brief The JThreadPool class 线程池
 */
class JThreadPool
{
public:
    JThreadPool();
    JThreadPool(const int32_t maxThreadCount,const int32_t currentThreadCount);
    ~JThreadPool();

public:
    static void sleep(const int32_t millSecond);
    static std::thread::id currentThreadId();

public:
	//************************************
	// Method:    submit 提交任务到线程池，并且可以获取任务的返回值
	// FullName:  JThreadPool::submit
	// Access:    public 
	// Returns:   
	// Qualifier: >::type> submit(FunctionType f)
	//************************************
	template<typename FunctionType>
	std::future<typename std::result_of<FunctionType()>::type>
		submit(FunctionType f)
	{
		typedef typename std::result_of<FunctionType()>::type
			result_type; 

		std::packaged_task<result_type()> task(std::move(f));
		std::future<result_type> res(task.get_future()); 
		taskQueue_.push(std::move(task)); 
		return res; 
	}

    bool start();

    void stop();

	bool isRunning() const;
protected:
    void threadRun();

private:
	int32_t maxThreadCount_ = std::thread::hardware_concurrency();
    int32_t currentThreadCount_ = 0;//当前线程数量
    std::atomic_bool isExit_;//线程是否退出
    std::list<std::thread> threadGroup_;//线程管理
	JThreadSafeQueue<function_wrapper> taskQueue_;//任务队列
};

#endif // JTHREADPOOL_H
