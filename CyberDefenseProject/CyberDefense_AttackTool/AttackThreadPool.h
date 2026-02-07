#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class AttackThreadPool
{
public:
	explicit AttackThreadPool(size_t threadCount);
	~AttackThreadPool();
	void enqueue(std::function<void()> task);

private:
	bool m_stop;

	void workerLoop();

	std::vector<std::thread> m_workers;
	std::queue<std::function<void()>> m_tasks;

	std::mutex m_mutex;
	std::condition_variable m_cv;
 
};
