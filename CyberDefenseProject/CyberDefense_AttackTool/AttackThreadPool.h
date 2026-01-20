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
	void waitIdle();

private:
	void workerLoop();

	std::vector<std::thread> m_workers;
	std::queue<std::function<void()>> m_tasks;

	std::mutex m_mutex;
	std::condition_variable m_cv;

	std::atomic<bool> m_stop{ false };
	std::mutex m_idleMutex;
	std::condition_variable m_idleCv;
	size_t m_inFlight = 0;
};
