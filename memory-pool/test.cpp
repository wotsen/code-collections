/**
 * @file test.cpp
 * @author yuwangliang (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-08-29
 * 
 * @copyright Copyright (c) 2020 yuwangliang. All rights reserved.
 * 
 */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "memory-pool.h"

using namespace wotsen;

int main(void)
{
	clock_t s = 0;
	clock_t n = 0;
	mm_pool_t *pool = nullptr;

	s = clock();
	char *str2 = (char*)malloc(16);
	n = clock();

	printf("sys : %ld\n", n - s);

	create_mm_pool(16 * 1024 * 1024, &pool);

	char *str = nullptr;
	for (uint32_t i = 0; i < 33; i++) {
		s = clock();
		str = (char*)alloc(pool, 16);
		n = clock();
		printf("my : %ld\n", n - s);
	}

	sprintf(str, "hello world");

	printf("%s\n", str);

	// free(pool, str);

	char *str3 = (char*)alloc(pool, 17);

	sprintf(str3, "hello world");

	printf("%s\n", str3);

	return 0;
}