#include <iostream>
#include <future>
#include <vector>
#include "jthreadpool.h"

void test(){
    std::cout<<"current thread id is "<<JThreadPool::currentThreadId()<<std::endl;
}

int main(int argc, char *argv[])
{
    JThreadPool threadPool(4,2);
	std::vector<std::future<int>> res;
    for(int32_t i = 0;i < 1000;++i){
		res.push_back(threadPool.submit([=](){
			std::cout<<"current thread id is "<<JThreadPool::currentThreadId()<<std::endl;
			return i;
		}));
    }
    const bool ok = threadPool.start();
    if(!ok){
        std::cout<<"start thread pool failed"<<std::endl;
        return -1;
    }
    JThreadPool::sleep(10000);
    threadPool.stop();
	for (auto it = res.begin(); it != res.end(); ++it) {
		std::cout << it->get() << std::endl;
	}
    return 0;
}
