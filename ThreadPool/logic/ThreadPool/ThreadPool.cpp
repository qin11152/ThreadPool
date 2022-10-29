#include "ThreadPool.h"
#include <iostream>


ThreadPool::ThreadPool(int num)
    :m_ithreadNum(num)
{
}

//析构时运行状态置为false，并通知所有等待线程，状态置为false后
//dotask中就可以退出
ThreadPool::~ThreadPool()
{
    m_bIsRunning = false;
    //通知所有线程不要再等待了
    m_conDequeNotEmpty.notify_all();
    for (auto& item : m_vecThread)
    {
        //回收线程资源
        if (item.joinable())
        {
            item.join();
        }
    }
}

void ThreadPool::stopPool()
{
    //运行状态置为false
    m_bIsRunning = false;
    //逻辑和析构函数一样
    m_conDequeNotEmpty.notify_all();
    for (auto& item : m_vecThread)
    {
        //回收线程资源
        if (item.joinable())
        {
            item.join();
        }
    }
}

//开启线程池，创建一定数量的线程，并绑定dotask函数
void ThreadPool::startPool()
{
    m_bIsRunning = true;
    for (int i = 0; i < m_ithreadNum; ++i)
    {
        std::thread t(&ThreadPool::doTask, this);
        m_vecThread.push_back(move(t));
    }
}

void ThreadPool::doTask()
{
    //循环操作
    while (1)
    {
        //要获取的任务
        Task task;
        {
            std::unique_lock<std::mutex> lck(m_mutex);
            //正在运行状态且deque大小为空
            /*if (m_bIsRunning && 0 == m_deqTask.size())
            {
                printf("thread id:%d,", GetCurrentThreadId());
                printf("wiat state\n");
                //没有任务的时候就等待
                m_conDequeNotEmpty.wait(lck);
            }*/
            //等待任务队列不空或者状态为退出
            m_conDequeNotEmpty.wait(lck, [this]() {return !m_bIsRunning || !m_deqTask.empty(); });
            //不在运行状态且任务队列空了那就退出
            if (!m_bIsRunning && m_deqTask.empty())
            {
                return;
            }
            //有任务就把顶端的拿出来
            task = move(m_deqTask.front());
            m_deqTask.pop_front();
        }
        //出了上边的作用域,lck就自动解锁了
        //执行任务
        task();
    }
}
