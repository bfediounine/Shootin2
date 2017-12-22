#ifndef BINTREE_H_
#define BINTREE_H_

typedef struct TreeNode
{
	int id;
	struct TreeNode *left, *right;
}TreeNode;

int addNode(TreeNode *, int);
TreeNode *remNode(TreeNode *, int);
TreeNode *findParent(TreeNode *, int);
TreeNode *searchNode(TreeNode *, int);

#endif
