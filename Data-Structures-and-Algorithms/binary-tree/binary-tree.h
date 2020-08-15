/**
 * @file binary-tree.h
 * @author yuwangliang (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-08-15
 * 
 * @copyright Copyright (c) 2020 yuwangliang
 * 
 */

#ifndef __wotsen_BINARY_TREE_H__
#define __wotsen_BINARY_TREE_H__

#include <type_traits>
#include <functional>
#include <utility>

namespace wotsen
{
	
/**
 * @brief 节点数据类型萃取器
 * 
 * @tparam T 数据类型
 */
template <typename T>
struct TreeNodeDataType
{
	typedef T tree_data_t;
};

/**
 * @brief 节点数据类型萃取器
 * @details 为void时定义为0字节的结构体
 * 
 * @tparam  
 */
template <>
struct TreeNodeDataType<void>
{
	typedef struct { char _[0]; } tree_data_t;
};

template <typename K, typename D>
struct IBinaryTreeNode
{
	typedef typename std::decay<K>::type tree_key_t;
	typedef typename TreeNodeDataType<typename std::decay<D>::type>::tree_data_t tree_data_t;
};

template <>
struct IBinaryTreeNode<void, void>
{
};

template <typename D>
struct IBinaryTreeNode<void, D>
{
};

/**
 * @brief 拷贝仿函数
 * 
 * @tparam D 数据类型
 * @tparam bool 是否菲方类型，非法时调用将不起作用
 */
template <typename D, bool>
struct CopyTreeNodeData
{
	typedef D tree_data_t;

	void operator()(const tree_data_t &src, tree_data_t &dst)
	{
		dst = src;
	}

	void operator()(tree_data_t &&src, tree_data_t &dst)
	{
		dst = src;
	}
};

template <typename D>
struct CopyTreeNodeData<D, true>
{
	typedef D tree_data_t;

	void operator()(const tree_data_t &src, tree_data_t &dst)
	{
	}

	void operator()(tree_data_t &&src, tree_data_t &dst)
	{
	}
};

template <typename K, typename D, bool>
struct NodeDelete
{
	typedef std::function<void(K&, D&)> release_fn_t;
};

template <typename K, typename D>
struct NodeDelete<K, D, true>
{
	typedef std::function<void(K&)> release_fn_t;
};

template <typename K, typename D>
class IBinaryTree
{
public:
	typedef IBinaryTreeNode<K, D> tree_node_t;
	typedef typename tree_node_t::tree_key_t tree_key_t;
	typedef typename tree_node_t::tree_data_t tree_data_t;

	typedef typename NodeDelete<tree_key_t, tree_data_t, std::is_void<typename std::decay<D>::type>::value>::release_fn_t release_fn_t;

	IBinaryTree(release_fn_t fn=nullptr) : release_data_(fn) {}

	IBinaryTree(const IBinaryTree&) = delete;
	IBinaryTree& operator=(const IBinaryTree&) = delete;

	virtual ~IBinaryTree() {}

	virtual bool empty(void) = 0;
	virtual size_t count(void) = 0;

	virtual bool insert(const tree_key_t& key) = 0;
	virtual bool insert(const tree_key_t& key, const tree_data_t& data) = 0;
	virtual bool insert(const tree_key_t& key, tree_data_t&& data) = 0;
	virtual bool insert(tree_key_t&& key) = 0;
	virtual bool insert(tree_key_t&& key, const tree_data_t& data) = 0;
	virtual bool insert(tree_key_t&& key, tree_data_t&& data) = 0;

	virtual bool search(const tree_key_t& key) = 0;
	virtual bool search(const tree_key_t& key, tree_data_t& data) = 0;

	virtual bool del(const tree_key_t& key) = 0;
	virtual bool del(const tree_key_t& key, tree_data_t &data) = 0;

	virtual void clear(void) = 0;

	// TODO:条件删除

	// TODO:迭代

	virtual void copy(const tree_data_t &src, tree_data_t &dst)
	{
		copy_(src, dst);
	}

	virtual	void copy(tree_data_t &&src, tree_data_t &dst)
	{
		copy_(std::forward<tree_data_t>(src), dst);
	}

public:
	release_fn_t release_data_;

private:
	CopyTreeNodeData<tree_data_t, std::is_void<typename std::decay<D>::type>::value> copy_;
};

} // !namespace wotsen

#endif