/**
 * @file thread-pool.h
 * @author yuwangliang (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-08-16
 * 
 * @copyright Copyright (c) 2020 yuwangliang
 * 
 */
#ifndef __wotsen_THREAD_POLL_H__
#define __wotsen_THREAD_POLL_H__

#include <list>
#include <thread>
#include <functional>
#include <future>
#include <memory>
#include <atomic>
#include "sync-queue.h"

namespace wotsen
{

	// 可调用对象返回类型
	template <typename F, typename... Args>
	using callable_ret_type = typename std::result_of<F(Args...)>::type;

	// 可调用对象返回值
	template <typename F, typename... Args>
	using future_callback_type = std::future<callable_ret_type<F, Args...>>;

	static const int MaxTaskCount = 100;
	class ThreadPool
	{
	public:
		using Task = std::function<void()>;
		ThreadPool(int numThreads = std::thread::hardware_concurrency()) : m_queue(MaxTaskCount)
		{
			start(numThreads);
		}

		~ThreadPool(void)
		{
			//如果没有停止时则主动停止线程池
			stop();
		}

		void stop()
		{
			std::call_once(m_flag, [this] { stop_thread_group(); }); //保证多线程情况下只调用一次StopThreadGroup
		}

		template <typename F, typename... Args>
		std::future<callable_ret_type<F, Args...>>
		add_task(F&& f, Args &&... args)
		{
			// 可调用对象封装为void(void)
			auto task = std::make_shared<std::packaged_task<callable_ret_type<F, Args...>()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...));

			// 获取未来值对象
			std::future<callable_ret_type<F, Args...>> ret = task->get_future();

			m_queue.put([task]() { (*task)(); });

			return ret;
		}

	private:
		void start(int numThreads)
		{
			m_running = true;
			//创建线程组
			for (int i = 0; i < numThreads; ++i)
			{
				m_threadgroup.push_back(std::make_shared<std::thread>(&ThreadPool::run_in_thread, this));
			}
		}

		void run_in_thread()
		{
			while (m_running)
			{
				//取任务分别执行
				std::list<Task> list;
				m_queue.take(list);

				for (auto &task : list)
				{
					if (!m_running)
						return;

					task();
				}
			}
		}

		void stop_thread_group()
		{
			m_queue.stop();	   //让同步队列中的线程停止
			m_running = false; //置为false，让内部线程跳出循环并退出

			for (auto thread : m_threadgroup) //等待线程结束
			{
				if (thread)
					thread->join();
			}
			m_threadgroup.clear();
		}

		std::list<std::shared_ptr<std::thread>> m_threadgroup; //处理任务的线程组
		SyncQueue<Task> m_queue;							   //同步队列
		std::atomic_bool m_running;							   //是否停止的标志
		std::once_flag m_flag;
	};

} // namespace wotsen

#endif // !__wotsen_THREAD_POLL_H__