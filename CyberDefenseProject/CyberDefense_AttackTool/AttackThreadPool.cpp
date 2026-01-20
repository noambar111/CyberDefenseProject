#include "AttackThreadPool.h"

AttackThreadPool::AttackThreadPool(size_t threadCount)
{
	for (size_t i = 0; i < threadCount; i++)
	{
		m_workers.emplace_back(std::thread(&AttackThreadPool::workerLoop, this));
	}
}

void AttackThreadPool::enqueue(std::function<void()> task)
{
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		this->m_tasks.push(std::move(task));
	}
	m_cv.notify_one();
}
