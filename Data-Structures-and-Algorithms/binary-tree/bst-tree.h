/**
 * @file bst-tree.h
 * @author yuwangliang (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-08-15
 * 
 * @copyright Copyright (c) 2020 yuwangliang
 * 
 */
#ifndef __wotsen_BST_TREE_H__
#define __wotsen_BST_TREE_H__

#include "binary-tree.h"

namespace wotsen
{

/**
 * @brief 树节点
 * 
 * @tparam K key类型，为void时将编译不过
 * @tparam D 数据类型
 */
template <typename K, typename D>
struct BstTreeNode : public IBinaryTreeNode<K, D>
{
	typedef IBinaryTreeNode<K, D> base_node_t;
	typedef typename base_node_t::tree_key_t tree_key_t;
	typedef typename base_node_t::tree_data_t tree_data_t;
	tree_key_t key;			///< key值
	tree_data_t data;		///< 数据段
	BstTreeNode<K, D> *lchild, *rchild;		///< 指针域
};

/**
 * @brief 二叉搜索树
 * 
 * @tparam K key类型
 * @tparam D=void 数据类型，默认void
 */
template <typename K, typename D=void>
class BstTree : public IBinaryTree<K, D>
{
public:
	typedef IBinaryTree<K, D> base_tree_t;
	typedef BstTreeNode<K, D> tree_node_t;
	typedef typename base_tree_t::tree_key_t tree_key_t;
	typedef typename base_tree_t::tree_data_t tree_data_t;

	BstTree(typename base_tree_t::release_fn_t fn=nullptr) : base_tree_t(fn), tree_(nullptr) {}
	BstTree(const BstTree&) = delete;
	BstTree(BstTree&& old_tree)
	{
		tree_ = old_tree.tree_;
		old_tree.tree_ = nullptr;
	}

	BstTree& operator=(const BstTree&) = delete;

	// TODO:允许移动复制，先释放，后移动
	BstTree& operator=(BstTree&&) = delete;

	virtual ~BstTree() {
		// TODO:使用后序遍历删除
	}

	// 搜索时如果需要数据，则使用参数来返回数据

	/**
	 * @brief 搜索
	 * 
	 * @param key 
	 * @param data[out] 用于返回数据
	 * @return true 搜索失败
	 * @return false 成功
	 */
	virtual bool search(const tree_key_t& key, tree_data_t& data)
	{
		tree_node_t *item = nullptr;
		return search(tree_, key, data, nullptr, &item);
	}

	virtual bool search(const tree_key_t& key)
	{
		tree_data_t data;
		return search(key, data);
	}

	/**
	 * @brief 插入节点
	 * 
	 * @param key 
	 * @param data 
	 * @return true 成功
	 * @return false 失败
	 */
	virtual bool insert(const tree_key_t& key, const tree_data_t& data)
	{
		tree_key_t _key = key;
		tree_data_t _data = data;
		return insert(std::move(_key), std::move(_data));
	}

	virtual bool insert(const tree_key_t& key, tree_data_t&& data)
	{
		tree_key_t _key = key;
		return insert(std::move(_key), std::forward<tree_data_t>(data));
	}

	virtual bool insert(tree_key_t&& key, const tree_data_t& data)
	{
		tree_data_t _data = data;
		return insert(std::forward<tree_key_t>(key), std::move(_data));
	}

	virtual bool insert(const tree_key_t& key)
	{
		tree_key_t _key = key;
		tree_data_t _data;
		return insert(std::move(_key), std::move(_data));
	}

	virtual bool insert(tree_key_t&& key)
	{
		tree_data_t _data;
		return insert(std::forward<tree_key_t>(key), std::move(_data));
	}

	virtual bool insert(tree_key_t&& key, tree_data_t&& data)
	{
		tree_node_t *item;
		tree_data_t _data;
		tree_key_t _key = key;

		if (!search(tree_, _key, _data, nullptr, &item))
		{
			tree_node_t *new_node = new tree_node_t;

			base_tree_t::copy(std::forward<tree_data_t>(data), new_node->data);
			new_node->key = std::move(_key);
			new_node->lchild = new_node->rchild = nullptr;

			if (!item)
			{
				tree_ = new_node;
			}
			else if (new_node->key < item->key)
			{
				item->lchild = new_node;
			}
			else
			{
				item->rchild = new_node;
			}

			return true;
		}

		return false;
	}

	/**
	 * @brief 删除
	 * 
	 * @param key 
	 * @param data 
	 * @return true 
	 * @return false 
	 */
	virtual bool del(const tree_key_t& key, tree_data_t &data)
	{
		return del(&tree_, key, data);
	}

	virtual bool del(const tree_key_t& key)
	{
		tree_data_t data;
		return del(&tree_, key, data);
	}

	virtual bool empty(void) {
		// TODO:
		return false;
	}
	virtual size_t count(void)
	{
		return 0;
	}
	virtual void clear(void)
	{

	}

	// TODO:返回节点数
	// TODO:遍历

protected:
	virtual bool search(tree_node_t *root, const tree_key_t& key, tree_data_t& data, tree_node_t *parent, tree_node_t **item)
	{
		if (!root)
		{
			*item = parent;
			return false;
		}
		else if (key == root->key)
		{
			base_tree_t::copy(root->data, data);
			*item = root;
			return true;
		}
		else if (key < root->key)
		{
			return search(root->lchild, key, data, root, item);
		}
		else
		{
			return search(root->rchild, key, data, root, item);
		}
	}

	virtual bool del_node(tree_node_t **node)
	{
		tree_node_t *tmp = nullptr;

		// 左子树为空，则只需要用右子节点代替该节点
		if ((*node)->lchild == nullptr)
		{
			tmp = *node;
			*node = tmp->rchild;
			delete tmp;
		}
		// 右子树为空，则只需要用左子节点代替该节点
		else if ((*node)->rchild == nullptr)
		{
			tmp = *node;
			*node = tmp->lchild;
			delete tmp;
		}
		else
		{
			// 查找左子树当中的最大节点代替该节点
			tmp = *node;
			// 做大节点必定是左子树当中的最右节点
			tree_node_t *max_node = (*node)->lchild;

			// 找到最右节点
			while (max_node->rchild)
			{
				tmp = max_node;
				max_node = max_node->rchild;
			}

			(*node)->key = std::move(max_node->key);

			// 直接复用要删除的节点位置，把本来要移到此处的节点删除掉就可以了，可以减少很多指针赋值
			base_tree_t::copy(std::move(max_node->data), (*node)->data);

			// 左子树有右节点，则将最右节点的左子树接到其父节点的右边
			if (tmp != *node)
			{
				tmp->rchild = max_node->lchild;
			}
			// 左子树没有右节点，则左节点本身就是最大的，将他的左子树接上来就好了
			else
			{
				tmp->lchild = max_node->lchild;
			}

			// 最后删除这个最大节点
			delete max_node;

			return true;
		}
		
		return false;
	}

	// 删除时返回值数据，让用户自己删除
	virtual bool del(tree_node_t **root, const tree_key_t& key, tree_data_t &data)
	{
		if (!*root)
		{
			return false;
		}

		// 节点匹配则删除该节点
		if (key == (*root)->key)
		{
			base_tree_t::copy(std::move((*root)->data), data);
			return del_node(root);
		}
		else if (key < (*root)->key)
		{
			// 在左子树
			return del(&(*root)->lchild, key, data);
		}
		else
		{
			// 在右子树
			return del(&(*root)->rchild, key, data);
		}
		
		return false;
	}

private:
	tree_node_t *tree_;
};

}

#endif // !__wotsen_BST_TREE_H__