#include "ThreadPool.h"

ThreadPool::ThreadPool(int num)
{
	m_worker.reserve(num);
	for (int i = 0; i < num; i++)
		m_worker.emplace_back(&ThreadPool::workerLoop, this);	
}

ThreadPool::~ThreadPool()
{
	m_stop.store(true);
	m_cv.notify_all();

	for (auto& t : m_worker)
	{
		if (t.joinable())
		{
			t.join();
		}
	}
}

void ThreadPool::enqueue(std::function<void()> job)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_stop.load()) return;

	m_tasks.push(std::move(job));
	m_cv.notify_one();
}

void ThreadPool::workerLoop()
{
	while (true)
	{
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_cv.wait(lock, [this]() {
				return m_stop.load() || !m_tasks.empty();
				});

			if (m_stop.load() && m_tasks.empty())
			{
				return;
			}
			task = std::move(m_tasks.front());
			m_tasks.pop();
		}
		task();
	}
}

