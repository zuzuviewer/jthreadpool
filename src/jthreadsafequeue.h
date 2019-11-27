#ifndef JTHREADSAFEQUEUE_H
#define JTHREADSAFEQUEUE_H
#include <queue>
#include <mutex>
#include <condition_variable>
/**
 * 线程安全的先进先出队列，并且可等待
 */
template<typename _Ty,
	typename _Container = std::queue<_Ty> >
class JThreadSafeQueue
{
public:
	typedef _Container container_type;
	typedef typename _Container::value_type value_type;
	typedef typename _Container::size_type size_type;
	typedef typename _Container::reference reference;
	typedef typename _Container::const_reference const_reference;

	static_assert(is_same_v<_Ty, value_type>, "container adaptors require consistent types");
public:
	JThreadSafeQueue() {};
	~JThreadSafeQueue() {};

	void push(value_type&& value) {
		std::lock_guard<std::mutex> lc(mu);
		q.push(value);
		cv.notify_one();
	}

	bool try_pop(value_type& value) {
		std::lock_guard<std::mutex> lc(mu);
		if (q.empty()) {
			return false;
		}
		value = q.front();
		q.pop();
		return true;
	}

	void wait_and_pop(value_type& value) {
		std::unique_lock<std::mutex> lc(mu);
		cv.wait(lc, []() {return !q.empty()});
		value = q.front();
		q.pop();
		return;
	}

	bool empty() {
		std::lock_guard<std::mutex> lc(mu);
		return q.empty();
	}

private:
	container_type q;
	std::mutex mu;
	std::condition_variable cv;
};

#endif
