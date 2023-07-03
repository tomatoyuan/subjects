#ifndef TREENODE_H
#define TREENODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

typedef enum Type {
    terminal_int,
    terminal_hex,
    terminal_oct,
    terminal_float,
    terminal_id,
    terminal_type,
    terminal_other,
    non_terminal
} Type;

enum SyntaxError {
    NormalSyntaxError = 1,
    MissRB = 2,
    MissRP,
    MissRC,
    MissSEMI,

};

typedef struct TreeNode {
    int lineno; // 行数
    Type type; // 元素类型
    char *name; // 推导式中的元素名称
    struct TreeNode *child, *sibling; // 指向孩子和兄弟的指针
} TreeNode;

typedef TreeNode* pNode;

// 自顶向下构建语法树
pNode createNode(int _lineno, Type _type, char *_name, int args, ...);

// 先序遍历语法树
void printTree(pNode root, int level);

// 打印完毕，释放语法树空间
void delTree(pNode root);
#endif
