/**
 * @file avl-tree.h
 * @author yuwangliang (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-08-15
 * 
 * @copyright Copyright (c) 2020 yuwangliang
 * 
 */
#ifndef __wotsen_AVL_TREE_H__
#define __wotsen_AVL_TREE_H__

#include <stdio.h>
#include "binary-tree.h"

namespace wotsen
{

enum AvlBf
{
	AVL_EH = 0,
	AVL_LH = 1,
	AVL_RH = -1	
};
	
/**
 * @brief 树节点
 * 
 * @tparam K key类型，为void时将编译不过
 * @tparam D 数据类型
 */
template <typename K, typename D>
struct AvlTreeNode : public IBinaryTreeNode<K, D>
{
	typedef IBinaryTreeNode<K, D> base_node_t;
	typedef typename base_node_t::tree_key_t tree_key_t;
	typedef typename base_node_t::tree_data_t tree_data_t;
	tree_key_t key;			///< key值
	tree_data_t data;		///< 数据段
	AvlBf bf;				///< 平衡因子
	AvlTreeNode<K, D> *lchild, *rchild;		///< 指针域
};

template <typename K, typename D=void>
class AvlTree : public IBinaryTree<K, D>
{
public:
	typedef IBinaryTree<K, D> base_tree_t;
	typedef AvlTreeNode<K, D> tree_node_t;
	typedef typename base_tree_t::tree_key_t tree_key_t;
	typedef typename base_tree_t::tree_data_t tree_data_t;

	AvlTree(typename base_tree_t::release_fn_t fn=nullptr) : base_tree_t(fn), tree_(nullptr) {}
	AvlTree(const AvlTree&) = delete;
	AvlTree(AvlTree&& old_tree)
	{
		tree_ = old_tree.tree_;
		old_tree.tree_ = nullptr;
	}

	AvlTree& operator=(const AvlTree&) = delete;

	// TODO:允许移动复制，先释放，后移动
	AvlTree& operator=(AvlTree&&) = delete;

	virtual ~AvlTree() {
		// TODO:使用后序遍历删除
	}

	virtual bool empty(void){
		return false;
	}

	virtual size_t count(void){
		return 0;
	}

	virtual bool insert(const tree_key_t& key){
		tree_key_t _key = key;
		tree_data_t _data;
		return insert(std::move(_key), std::move(_data));
	}

	virtual bool insert(const tree_key_t& key, const tree_data_t& data){
		tree_key_t _key = key;
		tree_data_t _data = data;
		return insert(std::move(_key), std::move(_data));
	}

	virtual bool insert(const tree_key_t& key, tree_data_t&& data){
		tree_key_t _key = key;
		return insert(std::move(_key), std::forward<tree_data_t>(data));
	}

	virtual bool insert(tree_key_t&& key){
		tree_data_t _data;
		return insert(std::forward<tree_key_t>(key), std::move(_data));
	}

	virtual bool insert(tree_key_t&& key, const tree_data_t& data){
		tree_data_t _data = data;
		return insert(std::forward<tree_key_t>(key), std::move(_data));
	}

	virtual bool insert(tree_key_t&& key, tree_data_t&& data){
		bool taller = false;
		return insert(&tree_, std::forward<tree_key_t>(key), std::forward<tree_data_t>(data), taller);
	}

	virtual bool search(const tree_key_t& key){
		return true;
	}
	virtual bool search(const tree_key_t& key, tree_data_t& data){
		return true;
	}

	virtual bool del(const tree_key_t& key){
		return true;
	}
	virtual bool del(const tree_key_t& key, tree_data_t &data){
		return true;
	}

	virtual void clear(void) {}

private:
	// 右旋
	virtual void r_rotate(tree_node_t** node)
	{
		tree_node_t *l = (*node)->lchild;
		(*node)->lchild = l->rchild;
		l->rchild = (*node);
		(*node) = l;	// 原来的左子树上移
	}

	// 左旋
	virtual void l_rotate(tree_node_t** node)
	{
		tree_node_t *r = (*node)->rchild;
		(*node)->rchild = r->lchild;
		r->rchild = (*node);
		(*node) = r;	// 原来的右子树上移
	}

	// 左子树平衡处理
	virtual void left_balance(tree_node_t** node)
	{
		tree_node_t *l;
		tree_node_t *lr;

		l = (*node)->lchild;

		switch (l->bf)
		{
		// 当前节点的左子树增高(此节点原本的平衡因子都为1了，插入后，他的左子树的平衡因子又为1，那么此节点的平衡因子为2，单右旋)
		// l必定没有右孩子
		case AVL_LH:
			/* code */
			// 单右旋能完全平衡，因为此时只有左斜的三个节点
			// NOTE:注意node的平很因子不会加到二，因为l新增了左节点后，就会去判断要不要调整
			/*
			 *             O(node, 1)                      O(l, 0)
			 *            /          ->                 /     \
			 *           O(l, 1)                       O(new)  O(node, 0)
			 *          /
			 *         O(new)
			 *
			 */
			(*node)->bf = l->bf = AVL_EH;
			r_rotate(node);
			break;
		// 左子树增高，但是左子树的平衡因子为-1，说明新增节点在做孩子的右子树，那么进行左右旋
		case AVL_RH:
			// 判断左孩子的右孩子的平衡因子
			// 如果lr原本的平衡因子不为0，那么l必定有左右孩子
			lr = l->rchild;
			switch (lr->bf)
			{

			case AVL_LH:
				(*node)->bf = AVL_RH;
				l->bf = AVL_EH;
				break;
			
			case AVL_EH:
				(*node)->bf = l->bf = AVL_EH;
				break;

			case AVL_RH:
				(*node)->bf = AVL_EH;
				l->bf = AVL_LH;
				break;
			
			default:
				break;
			}

			lr->bf = AVL_EH;

			// 左右旋
			l_rotate(&(*node)->lchild);
			r_rotate(node);
		
		default:
			break;
		}
	}

	// 右子树平衡处理
	virtual void right_balance(tree_node_t** node)
	{
		tree_node_t *r;
		tree_node_t *rl;

		r = (*node)->rchild;

		switch (r->bf)
		{
		case AVL_RH:
			(*node)->bf = r->bf = AVL_EH;
			l_rotate(node);
			break;
		case AVL_LH:
			rl = r->lchild;
			switch (rl->bf)
			{

			case AVL_RH:
				(*node)->bf = AVL_LH;
				r->bf = AVL_EH;
				break;
			
			case AVL_EH:
				(*node)->bf = r->bf = AVL_EH;
				break;

			case AVL_LH:
				(*node)->bf = AVL_EH;
				r->bf = AVL_RH;
				break;
			
			default:
				break;
			}

			rl->bf = AVL_EH;

			// 右左旋
			r_rotate(&(*node)->rchild);
			l_rotate(node);
		
		default:
			break;
		}
	}

	virtual bool insert(tree_node_t** root, tree_key_t&& key, tree_data_t&& data, bool &taller)
	{
		if (!*root)
		{
			// 新建节点
			*root = new tree_node_t;
			(*root)->key = std::move(key);
			base_tree_t::copy(std::forward<tree_data_t>(data), (*root)->data);
			(*root)->bf = AVL_EH;
			(*root)->lchild = (*root)->rchild = nullptr;

			// 树长高
			taller = true;
		}
		else
		{
			// 节点冲突
			if (key == (*root)->key)
			{
				taller = false;
				return false;
			}

			// 左子树插入
			if (key < (*root)->key)
			{
				if (!insert(&(*root)->lchild, std::forward<tree_key_t>(key), std::forward<tree_data_t>(data), taller))
				{
					return false;
				}

				// 已经插入了，并且树长高了
				if (taller)
				{
					switch ((*root)->bf)
					{
					case AVL_LH:
						// 原本左子树高，现在在左子树插入，需要左平衡
						// 注意，原本平衡的情况下，将局部调整后，整棵树会平衡
						left_balance(root);
						taller = false;
						break;
					
					case AVL_EH:
						// 原本一样高，现在左边增高
						(*root)->bf = AVL_LH;
						taller = true;
						break;
					
					case AVL_RH:
						// 原本右边高，现在等高
						(*root)->bf = AVL_EH;
						taller = false;
						break;
					
					default:
						return false;
						break;
					}
				}
			}
			else
			{
				// 右子树插入
				if (!insert(&(*root)->rchild, std::forward<tree_key_t>(key), std::forward<tree_data_t>(data), taller))
				{
					return false;
				}

				// 已经插入了，并且树长高了
				if (taller)
				{
					switch ((*root)->bf)
					{
					case AVL_LH:
						(*root)->bf = AVL_EH;
						taller = false;
						break;
					
					case AVL_EH:
						/* code */
						(*root)->bf = AVL_RH;
						taller = true;
						break;
					
					case AVL_RH:
						right_balance(root);
						taller = false;
						break;
					
					default:
						return false;
						break;
					}
				}
			}
		}
		
		return true;
	}

private:
	tree_node_t *tree_;
};

} // namespace wotsen


#endif // !__wotsen_AVL_TREE_H__