#include "logic/ThreadPool/ThreadPool.h"
#include <Windows.h>

void func() 
{
    printf("task is doing,thread id:%d\n", GetCurrentThreadId());
    ::Sleep(6000);    //模拟工作时长
    printf("task finished,thread id:%d\n", GetCurrentThreadId());
}

int add(int a, int b)
{
    return a + b;
}

class TestPool
{
public:
    void func1(int a, int b)
    {
        printf("task is doing,thread id:%d\n", GetCurrentThreadId());
        int tmpNum = a + b;
        ::Sleep(6000);    //模拟工作时长
        printf("task finished,thread id:%d,result:%d\n", GetCurrentThreadId(), tmpNum);
    }
};

int main()
{
    ThreadPool threadPool(10);
    threadPool.startPool();
    threadPool.addTask(func);
    auto result=threadPool.addTask(add, 1, 4);
    printf("add result:%d\n", result.get());
    TestPool testPool;
    threadPool.addTask(std::bind(&TestPool::func1, &testPool, std::placeholders::_1, std::placeholders::_2), 3, 4);
    return 0;
}