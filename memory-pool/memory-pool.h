/**
 * @file memory-pool.h
 * @author yuwangliang (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-08-27
 * 
 * @copyright Copyright (c) 2020 yuwangliang
 * 
 */
#ifndef __wotsen_MEMORY_POOL_H__
#define __wotsen_MEMORY_POOL_H__

#include <inttypes.h>

namespace wotsen
{

///< 8字节
#define MIN_CHUNK 8

///< 128k
#define MAX_CHUNK (128 * 1024)

typedef struct mm_pool_s mm_pool_t;

/**
 * @brief Create a mm pool object
 * 
 * @param pool_size 内存池大小
 * @param pool[out] 内存池
 * @param min_chunk 最小块
 * @param max_chunk 最大块
 * @return true 成功
 * @todo 失败情况很多，需要使用错误码
 * @return false 失败
 */
bool create_mm_pool(const uint64_t &pool_size, mm_pool_t **pool, const uint64_t &min_chunk = MIN_CHUNK, const uint64_t &max_chunk = MAX_CHUNK);

/**
 * @brief 销毁内存池
 * 
 * @param pool 
 */
void destroy_mm_pool(mm_pool_t **pool);

/**
 * @brief 分配内存
 * 
 * @param pool 内存池
 * @param size 内存大小
 * @return void* 内存地址
 */
void *alloc(mm_pool_t *pool, const uint64_t &size);

/**
 * @brief 释放内存
 * 
 * @param pool 内存池
 * @param addr 地址
 */
void free(mm_pool_t *pool, void *addr);

} // namespace wotsen

#endif // !__wotsen_MEMORY_POOL_H__