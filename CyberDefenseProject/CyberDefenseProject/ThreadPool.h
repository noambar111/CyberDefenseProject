#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>

class ThreadPool {
public:
	explicit ThreadPool(int num);
	~ThreadPool();
	void enqueue(std::function<void()> job);
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;
	size_t getQueueSize()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_tasks.size();
	}
private:
	void workerLoop();
	std::vector<std::thread> m_worker;
	std::queue<std::function<void()>> m_tasks;

	std::mutex m_mutex;
	std::condition_variable m_cv;
	std::atomic<bool> m_stop{ false };

};