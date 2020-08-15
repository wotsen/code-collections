/**
 * @file test.cpp
 * @author yuwangliang (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-08-12
 * 
 * @copyright Copyright (c) 2020 yuwangliang
 * 
 */

#include <stdio.h>
#include "bst-tree.h"
#include "avl-tree.h"

using namespace wotsen;

int main(void)
{
	BstTree<int, int> tree([](int& key, int& data){
		printf("%d, %d\n", key, data);
	});
	BstTree<int> tree2;

	printf("insert %s\n", tree2.insert(54) ? "true" : "false");
	printf("insert %s\n", tree2.insert(54) ? "true" : "false");
	printf("insert %s\n", tree2.insert(52) ? "true" : "false");
	printf("search %s\n", tree2.search(52) ? "true" : "false");

	const int a[10] = {62, 88, 58, 47, 35, 73, 51, 99, 37, 93};

	for (int i = 0; i < 10; i++)
	{
		printf("insert %s\n", tree.insert(std::move(a[i]), a[i]) ? "true" : "false");
	}

	int b = 0;
	int c = 0;

	tree.search(62, b);
	printf("b : %d, c : %d\n", b, c);
	tree.del(62, c);

	printf("b : %d, c : %d\n", b, c);

	printf("del %s\n", tree.del(62, b) ? "true" : "false");

	IBinaryTree<int, int> *tree3 = &tree;
	printf("del %s\n", tree3->del(88, b) ? "true" : "false");
	printf("b : %d, c : %d\n", b, c);

	AvlTree<int, int> tree4;
	for (int i = 0; i < 10; i++)
	{
		printf("insert %s\n", tree4.insert(a[i], a[i]) ? "true" : "false");
	}

	tree3 = &tree4;
	int aa = 100;
	int bb = 101;
	printf("insert %s\n", tree3->insert(aa, bb) ? "true" : "false");
	printf("insert %s\n", tree3->insert(101, 120) ? "true" : "false");

	typedef char _type[0];

	struct __type
	{
		char a[0];
	};

	printf("%zu\n", sizeof(_type));
	printf("%zu\n", sizeof(__type));

	return 0;
}
