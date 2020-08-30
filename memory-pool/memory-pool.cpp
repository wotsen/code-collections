/**
 * @file memory-pool.cpp
 * @author yuwangliang (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-08-27
 * 
 * @copyright Copyright (c) 2020 yuwangliang
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <assert.h>
#include "memory-pool.h"

namespace wotsen
{

typedef struct mm_pool_s mm_pool_t;
typedef struct mm_slab_s mm_slab_t;
typedef struct mm_used_s mm_used_t;
typedef struct mm_ext_s mm_ext_t;

///< 页大小
#define PAGE_SIZE (4 * 1024)

///< 对齐
#define ROUND_UP(v, align) (((v) + (align) - 1) & ~((align) - 1))

///< bit位操作
#define SETBIT(a, n) (a[n / CHAR_BIT] |= (1 << (n % CHAR_BIT)))
#define CLRBIT(a, n) (a[n / CHAR_BIT] &= ~(1 << (n % CHAR_BIT)))
#define GETBIT(a, n) (a[n / CHAR_BIT] & (1 << (n % CHAR_BIT)))

#pragma pack(1)

/**
 * @brief 内存池
 * 
 */
struct mm_pool_s {
	void* addr;				///< 内存页起始地址
	uint64_t len;			///< 内存长度
	uint64_t pages;			///< 页数量
	uint32_t page_size;		///< 页大小，默认4k，最小4k
	uint64_t min_slab;		///< 最小slab，默认8字节
	uint64_t max_slab;		///< 最大slab，默认128k
	uint64_t free;			///< 空闲页数量
	uint16_t page_head_len;	///< 页头部长度
	void *page_head_addr;	///< 页头部记录起始地址
	uint8_t *map;			///< 页映射，每个页1位，由于是按页分配的，，所以归还时能快速定位到页所在的位地址
	mm_used_t *used;		///< 页分配链表，能在释放时快速定位到slab索引
	mm_slab_t **slabs;		///< slab数组最小slab到最大slab的顺序指针数组
	mm_ext_t *ex;			///< 额外内存量表，超出内存和大内存
	mm_pool_t *next;		///< 上个内存池
	mm_pool_t *prev;		///< 下个内存池
};

/**
 * @brief 额外内存链表，建议使用红黑树
 * 
 */
struct mm_ext_s {
	void *addr;
	mm_ext_t *next;
};

/**
 * @brief 使用链表，可用其他数据结构体代替，能否用红黑树
 * 
 */
struct mm_used_s {
	mm_slab_t *slab_ptr;	///< 内存挂载到的slab
	mm_used_t *next;
};

/**
 * @brief slab
 * @details 1k以下大小类型的slab以4k按页申请内存，申请不到则先从其他slab找空闲的页，最后从额外内存申请
 * 			1k到32k的4倍slab申请，申请不到则按页、slab申请
 * 			32k到128k的2倍slab申请，申请不到则按slab申请
 * 
 */
struct mm_slab_s {
	void *addr;				///< 起始地址
	uint64_t page_no;		///< 起始页号
	uint16_t pages;			///< 页数量
	uint64_t chunk_size;	///< 块大小，最小8字节
	uint16_t cnt;			///< chunk数量
	uint16_t free;			///< 剩余
	uint16_t index;			///< 当前分配索引
	uint8_t map[64];		///< chunk映射,最多512个
	// 只保存同类型的slab
	mm_slab_t *next;
	mm_slab_t *prev;
};

///< 分配slab
static mm_slab_t *_alloc_slab(mm_pool_t *pool, const uint16_t &page);
static mm_slab_t *alloc_slab(mm_pool_t *pool, const uint64_t &size);

///< 分配额外内存
static void *alloc_ext_mm(mm_pool_t *pool, const uint64_t &size);

///< 实际内存分配
static void *_alloc_mm(mm_pool_t *pool, const uint64_t &size);

bool create_mm_pool(const uint64_t &pool_size, mm_pool_t **pool, const uint64_t &min_chunk, const uint64_t &max_chunk)
{
	uint64_t _min = min_chunk;
	uint64_t _max = max_chunk;
	uint64_t _pool_size = pool_size;

	if (_min < MIN_CHUNK) {
		_min = MIN_CHUNK;
	}

	//< 修正最小内存块
	_min = ROUND_UP(_min, MIN_CHUNK);

	///< 非法值
	if (pool_size < max_chunk || max_chunk < _min) {
		return false;
	}

	///< 最小块与最大块的距离不能太大
	if ((max_chunk / _min) > (MAX_CHUNK / MIN_CHUNK)) {
		return false;
	}

	uint64_t page_size = PAGE_SIZE;

	// 最小块超过了一页,则使用最小块作一页分页
	if (_min > PAGE_SIZE) {
		page_size = _min;
		// page_size = ROUND_UP(_min, PAGE_SIZE);
	}

	// 最大块是最小块的倍数
	_max = ROUND_UP(_max, _min);

	// 内存池大小以最大块对齐
	if (_max > MAX_CHUNK) {
		_pool_size = ROUND_UP(_pool_size, _max);
	} else {
		_pool_size = ROUND_UP(_pool_size, MAX_CHUNK);
	}

	uint64_t slab_lv = _max / _min;
	uint64_t pages = _pool_size / page_size;
	uint64_t page_head = sizeof(mm_slab_t) + sizeof(mm_used_t);
	uint64_t page_map_size = pages / CHAR_BIT;
	page_map_size += pages % CHAR_BIT ? 1 : 0;

	uint64_t pre_mm = sizeof(mm_pool_t)
					  + page_map_size * sizeof(uint8_t)
					  + slab_lv * sizeof(mm_slab_t*)
					  + page_head * pages
					  + _pool_size;

	printf("pre mm [%zu], pool size [%zu]\n", pre_mm / 1024, _pool_size / 1024);

	void *ptr = malloc(pre_mm);

	if (!ptr) {
		return false;
	}

	memset(ptr, 0, pre_mm);

	(*pool) = (mm_pool_t*)ptr;
	(*pool)->map = (uint8_t*)((char*)ptr + sizeof(mm_pool_t));
	(*pool)->slabs = (mm_slab_t**)((char*)ptr + sizeof(mm_pool_t) + page_map_size * sizeof(uint8_t));
	(*pool)->addr = (void*)((char*)ptr + sizeof(mm_pool_t) + page_map_size * sizeof(uint8_t) + slab_lv * sizeof(mm_slab_t*) + pages * page_head);

	(*pool)->len = pool_size;
	(*pool)->pages = pages;
	(*pool)->page_size = page_size;
	(*pool)->min_slab = _min;
	(*pool)->max_slab = _max;
	(*pool)->free = pages;
	(*pool)->used = nullptr;
	(*pool)->page_head_len = page_head;
	(*pool)->page_head_addr = (void*)((char*)ptr + sizeof(mm_pool_t) + page_map_size * sizeof(uint8_t) + slab_lv * sizeof(mm_slab_t*));
	(*pool)->ex = nullptr;
	(*pool)->next = nullptr;
	(*pool)->prev = nullptr;

	return true;
}

void destroy_mm_pool(mm_pool_t **pool)
{
	if (pool && *pool) {
		::free(*pool);
		*pool = nullptr;
	}
}

static void *alloc_ext_mm(mm_pool_t *pool, const uint64_t &size)
{
	void *ptr = malloc(size + sizeof(mm_ext_t));

	if (!ptr) {
		return nullptr;
	}

	mm_ext_t *new_ex = (mm_ext_t *)ptr;

	new_ex->addr = (void*)((char*)ptr + sizeof(mm_ext_t));
	new_ex->next = pool->ex;
	pool->ex = new_ex;

	return new_ex->addr;
}

static mm_slab_t *_alloc_slab(mm_pool_t *pool, const uint16_t &page)
{
	if (page > pool->free) {
		printf("pool full.\n");
		return nullptr;
	}

	uint8_t *map = pool->map;
	uint64_t i = 0;
	uint16_t serial_pages = 0;
	uint64_t start_page = 0;

	for (i = 0; i < pool->pages; i++) {
		if (!GETBIT(map, i)) {
			start_page = i;
			serial_pages++;
			if (serial_pages == page) {
				break;
			}
		} else {
			start_page = 0;
			serial_pages = 0;
		}
	}

	if (serial_pages != page) {
		return nullptr;
	}

	printf("start page : %ld\n", start_page);

	for (i = start_page; i < pool->pages && serial_pages; i++, serial_pages--) {
		SETBIT(map, i);
	}

	pool->free -= page;

	// 起始分配页的头部
	void *page_head = (void*)((char*)(pool->page_head_addr) + start_page * pool->page_head_len);
	mm_slab_t *slab = (mm_slab_t *)((char*)page_head);
	mm_used_t *used = (mm_used_t *)((char*)page_head + sizeof(mm_slab_t));

	memset(page_head, 0, page * pool->page_head_len);

	slab->addr = (void*)((char*)pool->addr + start_page * pool->page_size);
	slab->page_no = start_page;
	slab->pages = page;

	used->slab_ptr = slab;
	used->next = pool->used;
	pool->used = used;

	return slab;
}

static inline mm_slab_t *alloc_slab(mm_pool_t *pool, const uint64_t &size)
{
	mm_slab_t *s = nullptr;

	// 半页以下分配一页
	if (size <= (pool->min_slab / 2)) {
		return _alloc_slab(pool, 1);
	} else if (size > (pool->max_slab / 2) && size <= pool->min_slab) {
		// 一页以内按2页分配
		if (pool->free >= 2) {
			s = _alloc_slab(pool, 2);
		}

		if (!s) {
			// 分配不到按1页分配
			return _alloc_slab(pool, 1);
		} else {
			return s;
		}
	} else {
		// 大于1页的对齐2倍分配
		uint16_t need_page = ROUND_UP(size, pool->page_size) / pool->page_size;

		if (pool->free >= need_page * 2) {
			s = _alloc_slab(pool, 2 * need_page);
		}

		if (!s && pool->free >= need_page) {
			_alloc_slab(pool, need_page);
		} else {
			return s;
		}
	}

	return nullptr;
}

static void *_alloc_mm(mm_pool_t *pool, const uint64_t &size)
{
	uint32_t slab_idx = size / pool->min_slab - 1;
	mm_slab_t *slab = (pool->slabs)[slab_idx];

	// 该slab为空，或者满了
	if (!slab || !slab->free)
	{
		mm_slab_t *s = alloc_slab(pool, size);

		if (!s) {
			return alloc_ext_mm(pool, size);
		}

		s->chunk_size = size;
		s->cnt = (pool->page_size * s->pages) / size / CHAR_BIT;
		printf("cnt : %d\n", s->cnt);
		assert(s->cnt <= sizeof(s->map));
		s->free = s->cnt;
		s->index = 0;

		s->next = (pool->slabs)[slab_idx];
		s->prev = nullptr;
		if ((pool->slabs)[slab_idx]) {
			(pool->slabs)[slab_idx]->prev = s;
		}
		(pool->slabs)[slab_idx] = s;

		slab = s;
	}

	// 顺序分配
	if (slab->index < slab->cnt || !GETBIT(slab->map, slab->index)) {
		SETBIT(slab->map, slab->index);
		slab->index++;
		printf("index : %d\n", slab->index);
		slab->free--;
		return (void*)((char*)slab->addr + (slab->index - 1) * slab->chunk_size);
	}

	slab->index = slab->cnt;

	// 查找
	for (uint64_t i = 0; i < slab->cnt; i++) {
		if (!GETBIT(slab->map, i)) {
			SETBIT(slab->map, i);
			slab->free--;
			return (void*)((char*)slab->addr + i * slab->chunk_size);
		}
	}

	return nullptr;
}

void *alloc(mm_pool_t *pool, const uint64_t &size)
{
	uint64_t _size = size;

	if (_size < MIN_CHUNK) {
		_size = MIN_CHUNK;
	}

	_size = ROUND_UP(_size, pool->min_slab);

	printf("size: %ld\n", _size);

	if (_size < pool->min_slab) {
		_size = pool->min_slab;
	}

	if (_size <= pool->max_slab) {
		return _alloc_mm(pool, _size);
	}

	return alloc_ext_mm(pool, _size);
}

void free(mm_pool_t *pool, void *addr)
{
	// 非法内存
	if (addr < pool->addr) {
		return;
	}

	// 额外内存
	if (addr > (void*)((char*)pool->addr + pool->pages * pool->page_size)) {
		mm_ext_t *ex = pool->ex;
		mm_ext_t *p = pool->ex;

		while (ex) {
			if (ex->addr == addr) {
				p->next = ex->next;
				::free(ex);
				return;
			}
			p = ex;
			ex = ex->next;
		}
		return;
	}

	// 预分配内存
	mm_used_t *used = pool->used;
	mm_used_t *p = pool->used;

	while (used) {
		// 在范围内
		if (addr >= used->slab_ptr->addr
			&& addr < ((char*)(used->slab_ptr->addr) + used->slab_ptr->pages * pool->page_size)) {
			mm_slab_t *s = used->slab_ptr;

			if (!s) {
				continue;
			}

			uint16_t idx = ((char*)addr - (char*)s->addr) / s->chunk_size;

			assert(idx < s->cnt);
			CLRBIT(s->map, idx);
			s->free++;

			// 释放顺序索引
			if (s->index == s->cnt) {
				s->index = idx;
			}

			// 该slab空闲,释放占用页
			if (s->free == s->cnt) {
				printf("free page\n");
				if (s->prev) {
					s->prev->next = s->next;
				}

				if (s->next) {
					s->next->prev = s->prev;
				}

				used->slab_ptr = nullptr;
				p->next = used->next;

				uint8_t *map = pool->map;

				// 清除占用页
				for (uint64_t i = s->page_no; i < pool->pages && s->pages; i++, s->pages--) {
					CLRBIT(map, i);
				}

				pool->free += s->pages;
			}

			break;
		}
	}
}

} // namespace wotsen
