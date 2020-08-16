#include "thread-pool.h"

using namespace wotsen;

int main(void)
{
	ThreadPool pool;

	auto ret = pool.add_task([](int a) -> int { 
		std::cout << "hello 1" << std::endl;
		return 30;
	}, 20);

	std::cout << "back" << ret.get() << std::endl;

	pool.stop();

	return 0;
}
