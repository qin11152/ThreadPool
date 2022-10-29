#pragma once
#include <deque>
#include <memory>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <vector>
#include <atomic>
#include <thread>
#include <future>

using Task = std::function<void()>;

class ThreadPool
{
public:
    //构造函数，默认开启10个线程
    ThreadPool(int num = 10);
    ~ThreadPool();
    //开启此线程池
    void stopPool();
    //关闭线程池
    void startPool();
    //线程执行任务的函数
    void doTask();


    //向任务队列中增加任务
    template<typename F, typename...Args>
    auto addTask(F&& f, Args&&... args)
    {
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
        std::function<void()> wrapperFunc = [=]() {
            (*task_ptr)();
        };
            
        m_deqTask.push_back(wrapperFunc);
        m_conDequeNotEmpty.notify_one();
        return task_ptr->get_future();
    }

private:
    std::condition_variable m_conDequeNotEmpty; //任务队列非空通知
    std::deque<Task> m_deqTask;     //任务队列
    std::mutex m_mutex;     //锁
    std::vector<std::thread> m_vecThread;   //线程存储在vec中
    int m_ithreadNum;   //线程数量
    std::atomic<bool> m_bIsRunning{ false };    //是否正在运行
};
