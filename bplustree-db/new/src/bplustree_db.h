/**
 * @file bplustree_db.h
 * @author yuwangliang (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-07-26
 * 
 * @copyright Copyright (c) 2020 yuwangliang
 * 
 */
#ifndef __wotsen_BPLUSTREE_DB_H__
#define __wotsen_BPLUSTREE_DB_H__

#include <inttypes.h>

namespace wotsen
{

typedef struct bplustree_db_s bplustree_db_t;

struct bplustree_db_attr_s
{
	uint32_t max_slot;			///< 最大容量，默认最大1000万
	bool nodata;				///< 无数据模式，默认有数据
};

bool creat_db(const char *name, struct bplustree_db_attr_s *attr);

bplustree_db_t *open_db(const char *name);

///< 插入
///< 查询，精确与范围
///< 删除
///< 修改
	
}

#endif // !__wotsen_BPLUSTREE_DB_H__