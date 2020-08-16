/**
 * @file min-heap.h
 * @author yuwangliang (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-08-16
 * 
 * @copyright Copyright (c) 2020 yuwangliang
 * 
 */

#ifndef __wotsen_MIN_HEAP_H__
#define __wotsen_MIN_HEAP_H__

#include <vector>
#include <utility>
#include <stddef.h>
#include <exception>
#include <functional>
#include <iostream>
#include <algorithm>

namespace wotsen
{

	template <class K>
	class MinHeap
	{
		public:
			MinHeap() : heap_(nullptr)
			{
				heap_ = new std::vector<K>;
			}

			~MinHeap() {
				delete heap_;
			}

			bool insert(K&& key)
			{
				heap_->push_back(std::forward<K>(key));
				sift_up(heap_->size() - 1);
				return true;
			}

			bool insert(const K& key)
			{
				heap_->push_back(key);
				sift_up(heap_->size() - 1);
				return true;
			}

			K extract(void)
			{
				if (empty())
				{
					throw std::logic_error("heap is empty");
				}

				auto r = (*heap_)[0];

				if (size() == 1)
				{
					heap_->pop_back();
					return r;
				}

				// 最后一个值移到第一个位置
				auto last = (*heap_)[size() - 1];

				heap_->erase(std::remove_if(heap_->begin(), heap_->end(),
											[&](auto& item) -> bool {
												return last == item;
											}),
							heap_->end());

				(*heap_)[0] = last;

				sift_down(0);

				return r;
			}

			size_t size(void) const
			{
				return heap_->size();
			}

			bool empty(void) const
			{
				return heap_->empty();
			}

			K min(void) const
			{
				if (empty())
				{
					throw std::logic_error("heap is empty");
				}
				return (*heap_)[0];
			}

			void show(std::function<void(const K&)> s =nullptr)
			{
				for (auto &item : *heap_)
				{
					if (s)
					{
						s(item);
					}
					else
					{
						std::cout << item << std::endl;
					}
					
				}
			}

		private:
			void sift_up(const size_t& idx)
			{
				auto parent = parent_idx(idx);
				auto _idx = idx;

				while (_idx && ((*heap_)[parent] > (*heap_)[_idx]))
				{
					/* code */
					swap(parent, _idx);
					_idx = parent;
					parent = parent_idx(_idx);
				}
			}

			void sift_down(const size_t& idx)
			{
				auto _idx = idx;
				auto l = left_idx(idx);
				auto r = right_idx(idx);
				auto cnt = size();

				if (l < cnt && (*heap_)[_idx] > (*heap_)[l])
				{
					_idx = l;
				}

				if (r < cnt && (*heap_)[_idx] > (*heap_)[r])
				{
					_idx = r;
				}

				if (_idx != idx)
				{
					swap(_idx, idx);
					sift_down(_idx);
				}
			}

			void swap(const size_t& a, const size_t& b)
			{
				auto tmp = (*heap_)[a];

				(*heap_)[a] = (*heap_)[b];
				(*heap_)[b] = tmp;
			}

			size_t left_idx(const size_t& root) const
			{
				// root * 2 + 1;
				return (root << 1) + 1;
			}

			size_t right_idx(const size_t& root) const
			{
				// root * 2 + 2;
				return (root << 1) + 2;
			}

			size_t parent_idx(const size_t& root) const
			{
				if (0 == root)
				{
					return 0;
				}

				// return (root - 1) / 2;
				return (root - 1) >> 1;
			}
		private:
			std::vector<K> *heap_;
	};
	
} // namespace wotsen

#endif // !__wotsen_MIN_HEAP_H__