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
    //���캯����Ĭ�Ͽ���10���߳�
    ThreadPool(int num = 10);
    ~ThreadPool();
    //�������̳߳�
    void stopPool();
    //�ر��̳߳�
    void startPool();
    //�߳�ִ������ĺ���
    void doTask();


    //�������������������
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
    std::condition_variable m_conDequeNotEmpty; //������зǿ�֪ͨ
    std::deque<Task> m_deqTask;     //�������
    std::mutex m_mutex;     //��
    std::vector<std::thread> m_vecThread;   //�̴߳洢��vec��
    int m_ithreadNum;   //�߳�����
    std::atomic<bool> m_bIsRunning{ false };    //�Ƿ���������
};
