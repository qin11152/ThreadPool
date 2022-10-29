#include "ThreadPool.h"
#include <iostream>


ThreadPool::ThreadPool(int num)
    :m_ithreadNum(num)
{
}

//����ʱ����״̬��Ϊfalse����֪ͨ���еȴ��̣߳�״̬��Ϊfalse��
//dotask�оͿ����˳�
ThreadPool::~ThreadPool()
{
    m_bIsRunning = false;
    //֪ͨ�����̲߳�Ҫ�ٵȴ���
    m_conDequeNotEmpty.notify_all();
    for (auto& item : m_vecThread)
    {
        //�����߳���Դ
        if (item.joinable())
        {
            item.join();
        }
    }
}

void ThreadPool::stopPool()
{
    //����״̬��Ϊfalse
    m_bIsRunning = false;
    //�߼�����������һ��
    m_conDequeNotEmpty.notify_all();
    for (auto& item : m_vecThread)
    {
        //�����߳���Դ
        if (item.joinable())
        {
            item.join();
        }
    }
}

//�����̳߳أ�����һ���������̣߳�����dotask����
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
    //ѭ������
    while (1)
    {
        //Ҫ��ȡ������
        Task task;
        {
            std::unique_lock<std::mutex> lck(m_mutex);
            //��������״̬��deque��СΪ��
            /*if (m_bIsRunning && 0 == m_deqTask.size())
            {
                printf("thread id:%d,", GetCurrentThreadId());
                printf("wiat state\n");
                //û�������ʱ��͵ȴ�
                m_conDequeNotEmpty.wait(lck);
            }*/
            //�ȴ�������в��ջ���״̬Ϊ�˳�
            m_conDequeNotEmpty.wait(lck, [this]() {return !m_bIsRunning || !m_deqTask.empty(); });
            //��������״̬��������п����Ǿ��˳�
            if (!m_bIsRunning && m_deqTask.empty())
            {
                return;
            }
            //������ͰѶ��˵��ó���
            task = move(m_deqTask.front());
            m_deqTask.pop_front();
        }
        //�����ϱߵ�������,lck���Զ�������
        //ִ������
        task();
    }
}
