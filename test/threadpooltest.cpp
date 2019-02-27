#include <iostream>
#include "jthreadpool.h"

void test(){
    std::cout<<"current thread id is "<<JThreadPool::currentThreadId()<<std::endl;
}

int main(int argc, char *argv[])
{
    JThreadPool threadPool(4,2);

    for(int32_t i = 0;i < 1000;++i){
        threadPool.addTask([&](){
            std::cout<<"current thread id is "<<JThreadPool::currentThreadId()<<std::endl;
        });
    }
    const bool ok = threadPool.start();
    if(!ok){
        std::cout<<"start thread pool failed"<<std::endl;
        return -1;
    }
    JThreadPool::sleep(10000);
    threadPool.stop();
    return 0;
}
