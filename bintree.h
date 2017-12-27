#include <stdio.h>
#include <stdlib.h>

#ifndef BINTREE_H_
#define BINTREE_H_

#define TRUE	1
#define FALSE	0

typedef struct TreeNode
{
	int id;
	struct TreeNode *left, *right;
}TreeNode;

extern int addNode(TreeNode *, int);
extern TreeNode *remNode(TreeNode *);
extern TreeNode *findParent(TreeNode *, int);
extern TreeNode *searchNode(TreeNode *, int);

#endif
