#include "bintree.h"

int addNode(TreeNode *curNode, int id)
{
	if (curNode->id == id)
		return FALSE;
	else if (curNode->id > id)
		if (curNode->left == NULL)
		{
			curNode->left = (TreeNode *) malloc(sizeof(TreeNode));
			curNode->left->left = NULL;
			curNode->left->right = NULL;
			curNode->left->id = id;
		}
		else 
			return addNode(curNode->left, id);
	else
		if (curNode->right == NULL)
		{
			curNode->right = (TreeNode *) malloc(sizeof(TreeNode));
			curNode->right->left = NULL;
			curNode->right->right = NULL;
			curNode->right->id = id;
		}
		else 
			return addNode(curNode->right, id);
	return TRUE;
}

TreeNode *remNode(TreeNode *node)
{
	TreeNode *minRight, *parRight, *rVal; // use right side minimum in case of 2 child subs
	if (!node->left && !node->right)
	{
		free(node);
		return NULL;
	}
	else if ((!node->left && node->right) || (node->left && !node->right))
	{
		rVal = (!node->left) ? node->right : node->left;
		free(node);
		return rVal;
	}
	else 
	{
		minRight = node->right;
		parRight = minRight;
		while (minRight->left)
		{
			parRight = minRight;
			minRight = minRight->left;
		}
		// minRight and parent found - now mod and delete
		node->id = minRight->id;
		if (parRight == minRight)
			node->right = minRight->right;
		else
			parRight->left = NULL;
		free(minRight);
		return node;
	}
}

TreeNode *findParent(TreeNode *parent, int id)
{
	if (!parent->left && !parent->right)
		return NULL;
	else if ((parent->left && parent->left->id == id) || 
			(parent->right && parent->right->id == id))
		return parent;
	else if (parent->id < id)
		return findParent(parent->right, id);
	else
		return findParent(parent->left, id);
}

TreeNode *searchNode(TreeNode *curNode, int id)
{
	if (curNode->id == id)
		return curNode;
	else if (curNode->id > id)
		return (curNode->left != NULL) ? searchNode(curNode->left, id) : NULL;
	else
		return (curNode->right != NULL) ? searchNode(curNode->right, id) : NULL;
}
