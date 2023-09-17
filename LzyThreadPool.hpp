// LzyThreadPool.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include "LzyRingbuffer.hpp"
#include <thread>
#include <vector>
#include <functional>

namespace Lzy::ThreadPool {
	using Callable = std::function<void()>;
	constexpr auto ring_buffer_size = 1024;
	struct ThreadPool {
	private:
		const unsigned int& number_of_threads = std::thread::hardware_concurrency();
		std::vector<Lzy::Ringbuffer::Array<Callable>> ring_buffers;
		std::vector<std::thread> workers;
		std::atomic<uint64_t> round_robin_id{ 0 };

		ThreadPool() :ring_buffers(number_of_threads, ring_buffer_size) {
			workers.reserve(number_of_threads);
			for (int i = 0; i < number_of_threads; i++) {
				workers.emplace_back([i, this]() { worker_fn(i); });
			}
			std::ranges::for_each(workers, [](std::thread& thread) { thread.detach(); });
		}

		void worker_fn(int i) {
			thread_local int id = i;
			//std::cout << std::format("i am thread {}!\n", id);
			while (true)
			{
				if (!ring_buffers[id].empty()) {
					ring_buffers[id].pop()();
				}
				else {
					ring_buffers[id].wait();
				}
			}
		}

	public:
		static ThreadPool& get_instance() {
			static ThreadPool thread_pool;
			return thread_pool;
		}

		void add_task(Callable&& callable) {

			auto id = round_robin_id++;
			while (ring_buffers[id % number_of_threads].unused_size() < 1) {
				id++;
			}
			ring_buffers[id % number_of_threads].push(std::move(callable));
			ring_buffers[id % number_of_threads].notify_one();

		};
	};
}
namespace Lzy {
	void async(std::invocable auto&& callable) {
		ThreadPool::ThreadPool::get_instance().add_task(std::move(callable));
	}
}