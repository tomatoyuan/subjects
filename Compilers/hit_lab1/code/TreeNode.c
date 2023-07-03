#include "TreeNode.h"

pNode createNode(int _lineno, Type _type, char *_name, int args, ...)
{
    pNode curNode = (pNode)malloc(sizeof(TreeNode));
    assert(curNode != NULL);
    curNode->name = (char *)malloc( sizeof(char) * (strlen(_name) + 1) );
    assert(curNode->name != NULL);

    // 给节点赋值
    curNode->lineno = _lineno;
    curNode->type = _type;
    strncpy(curNode->name, _name, strlen(_name) + 1);
    curNode->child = NULL;
    curNode->sibling = NULL;

    if (args > 0) { // 如果有兄弟节点
        va_list ap;
        va_start(ap, args);
        pNode tempNode = va_arg(ap, pNode); // 取出第一个孩子节点
        curNode->child = tempNode;
        int i;
        for (i = 1; i < args; i++) { // 取出剩下的孩子节点
            tempNode->sibling = va_arg(ap, pNode);
            if (tempNode->sibling != NULL) { // 指针右移构建兄弟链
                tempNode = tempNode->sibling;
            }
        }
        va_end(ap);
    }

    return curNode;
}

void printTree(pNode root, int level)
{
    int n = level;
    while (root) {
        while (--n) printf("  ");
        if(root->type == non_terminal)
            printf("%s (%d)\n", root->name, root->lineno);
        else if(root->type == terminal_other)
            printf("%s\n", root->name);
        else if(root->type == terminal_type)
            printf("TYPE: %s\n", root->name);
        else if(root->type == terminal_int)
            printf("INT: %d\n", atoi(root->name));
	    else if(root->type == terminal_hex)
            printf("INT: %ld\n", strtol(root->name,NULL,16));
	    else if(root->type == terminal_oct)
            printf("INT: %ld\n", strtol(root->name,NULL,8));
        else if(root->type == terminal_float)
            printf("FLOAT: %f\n", atof(root->name));
        else if(root->type == terminal_id)
            printf("ID: %s\n", root->name);
        if (root->child) printTree(root->child, level + 1);
        root = root->sibling;
        n = level;
    }
}

void delTree(pNode root)
{
    if (root == NULL) return ;

    // 释放根节点
    pNode temp = root;
    root = root->child;
    free(temp);

    // 释放其所有子节点
    while (root) {
        temp = root->sibling;
        delTree(root);
        root = temp;
    }
}