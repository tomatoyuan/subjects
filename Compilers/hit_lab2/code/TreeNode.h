#ifndef TREENODE_H
#define TREENODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "enum.h"

#define TRUE    1
#define FALSE   0

enum SyntaxError {
    NormalSyntaxError = 1,
    MissRB,
    MissRP,
    MissRC,
    MissSEMI,
};

typedef struct TreeNode {
    int lineno; // 行数
    NodeType type; // 元素类型
    char *name; // 推导式中的元素名称
    char *val; // value of the lexical unit
    struct TreeNode *child, *sibling; // 指向孩子和兄弟的指针
} TreeNode;

typedef TreeNode* pNode;
typedef unsigned boolean;

// 自顶向下构建语法树
pNode createNode(int _lineno, NodeType _type, char *_name, char *_value, int args, ...);

// 先序遍历语法树
void printTree(pNode root, int level);

// 打印完毕，释放语法树空间
void delTree(pNode root);

static inline char *newString(char *src)
{
    if (src == NULL) return NULL;
    int length = strlen(src) + 1;
    char *p = (char *)malloc(sizeof(char) * length);
    assert(p != NULL);
    strncpy(p, src, length);
    return p;
}

#endif
