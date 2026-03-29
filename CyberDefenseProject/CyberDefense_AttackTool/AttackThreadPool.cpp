#include "AttackThreadPool.h"

AttackThreadPool::AttackThreadPool(size_t threadCount) : m_stop(false)
{
	for (size_t i = 0; i < threadCount; i++)
	{
		m_workers.emplace_back(std::thread(&AttackThreadPool::workerLoop, this));
	}
}

AttackThreadPool::~AttackThreadPool()
{
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		this->m_stop = true;
	}

	this->m_cv.notify_all();

	for (auto& t : this->m_workers)
	{
		if (t.joinable())
		{
			t.join();
		}
	}
}

void AttackThreadPool::enqueue(std::function<void()> task)
{
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		if (m_stop) return;
		this->m_tasks.push(std::move(task));
	}
	m_cv.notify_one();
}

void AttackThreadPool::workerLoop()
{
	std::function<void()> task;
	while(true)
	{
		
		{
			std::unique_lock<std::mutex> ul(m_mutex);

			m_cv.wait(ul, [&]() {
				return m_tasks.size() > 0 || m_stop;
				});

			if (m_stop && m_tasks.size() == 0) return;

			task = std::move(this->m_tasks.front());
			this->m_tasks.pop();
		}
		task();
	}
}
