/**
 * @file bloom_filter.cpp
 * @author yuwangliang (wotsen@outlook.com)
 * @brief 布隆过滤器，其原理是使用哈希函数计算散列值，将其记录到位阵列当中(哈希值%位阵列长度，确定在位阵列当中的位置)。
 * 优势在于减少空间占有，插入和查询时间复杂度为O(1)。而效率很大程度上取决于哈希函数的计算时间，位阵列长度和哈希函数的优良性影响防碰撞能力
 * @version 0.1
 * @date 2020-06-06
 * 
 * @copyright Copyright (c) 2020 yuwangliang. All rights reserved.
 * 
 */

#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned int (*hashfunc_t)(const char *);

/**
 * @brief 布隆过滤器
 * 
 */
typedef struct
{
	size_t asize;			///< 位空间大小
	unsigned char *a;		///< 位阵列
	size_t nfuncs;			///< 哈希函数数量
	hashfunc_t *funcs;		///< 哈希函数列表
} BLOOM;

///< 创建
BLOOM *bloom_create(size_t size, size_t nfuncs, ...);

///< 销毁
int bloom_destroy(BLOOM *bloom);

///< 新增
int bloom_add(BLOOM *bloom, const char *s);

///< 校验
int bloom_check(BLOOM *bloom, const char *s);

///< 记录在位阵列当中
#define SETBIT(a, n) (a[n / CHAR_BIT] |= (1 << (n % CHAR_BIT)))

///< 获取记录
#define GETBIT(a, n) (a[n / CHAR_BIT] & (1 << (n % CHAR_BIT)))

BLOOM *bloom_create(size_t size, size_t nfuncs, ...)
{
	BLOOM *bloom;
	va_list l;
	int n;

	if (!(bloom = (BLOOM *)malloc(sizeof(BLOOM))))
		return NULL;
	if (!(bloom->a = (unsigned char*)calloc((size + CHAR_BIT - 1) / CHAR_BIT, sizeof(char))))
	{
		free(bloom);
		return NULL;
	}
	if (!(bloom->funcs = (hashfunc_t *)malloc(nfuncs * sizeof(hashfunc_t))))
	{
		free(bloom->a);
		free(bloom);
		return NULL;
	}

	va_start(l, nfuncs);
	for (n = 0; n < nfuncs; ++n)
	{
		bloom->funcs[n] = va_arg(l, hashfunc_t);
	}
	va_end(l);

	bloom->nfuncs = nfuncs;
	bloom->asize = size;

	return bloom;
}

int bloom_destroy(BLOOM *bloom)
{
	free(bloom->a);
	free(bloom->funcs);
	free(bloom);

	return 0;
}

int bloom_add(BLOOM *bloom, const char *s)
{
	size_t n;

	for (n = 0; n < bloom->nfuncs; ++n)
	{
		///< 使用多个哈希函数计算散列值并进行记录，那么检查记录时就进行多次检查，&&操作，加强防碰撞能力
		SETBIT(bloom->a, bloom->funcs[n](s) % bloom->asize);
	}

	return 0;
}

int bloom_check(BLOOM *bloom, const char *s)
{
	size_t n;

	for (n = 0; n < bloom->nfuncs; ++n)
	{
		///< 如果有一个哈希函数表明未记录则未记录
		if (!(GETBIT(bloom->a, bloom->funcs[n](s) % bloom->asize)))
			return 0;
	}

	return 1;
}

unsigned int sax_hash(const char *key)
{
	unsigned int h = 0;

	while (*key)
		h ^= (h << 5) + (h >> 2) + (unsigned char)*key++;

	return h;
}

unsigned int sdbm_hash(const char *key)
{
	unsigned int h = 0;
	while (*key)
		h = (unsigned char)*key++ + (h << 6) + (h << 16) - h;
	return h;
}

int main(int argc, char *argv[])
{
	const char *const test[] = {
		"yuwangliang",
		"c++",
		"github.com"};
	BLOOM *bloom = nullptr;

	// 使用两个哈希函数
	if (!(bloom = bloom_create(2500000, 2, sax_hash, sdbm_hash)))
	{
		fprintf(stderr, "ERROR: Could not create bloom filter\n");
		return -1;
	}

	for (int i = 0; i < 3; i++)
	{
		bloom_add(bloom, test[i]);
	}

	printf("[%s] check [%s].\n", test[0], bloom_check(bloom, test[0]) ? "ok" : "false");
	printf("[%s] check [%s].\n", test[1], bloom_check(bloom, test[1]) ? "ok" : "false");
	printf("[%s] check [%s].\n", test[2], bloom_check(bloom, test[2]) ? "ok" : "false");
	printf("[%s] check [%s].\n", "test str", bloom_check(bloom, "test str") ? "ok" : "false");

	bloom_destroy(bloom);

	return 0;
}
