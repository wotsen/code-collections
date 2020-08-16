#include "min-heap.h"

using namespace wotsen;

int main(void)
{
	MinHeap<int> min_heap;

	min_heap.insert(2);
	min_heap.insert(3);
	min_heap.insert(4);
	min_heap.insert(5);
	min_heap.insert(1);

	min_heap.show();

	std::cout << "min : " << min_heap.extract() << std::endl;

	min_heap.show();

	return 0;
}