// LzyThreadPool.cpp: 定义应用程序的入口点。
//

#include "LzyThreadPool.h"
#include <thread>

struct thread {
	int thread_id;
};
struct thread_pool {
private:
	const unsigned int& number_of_threads = std::thread::hardware_concurrency();
	thread_pool() {
	}
public:

};

int main()
{
	std::cout << "Hello CMake." << std::endl;
	return 0;
}
