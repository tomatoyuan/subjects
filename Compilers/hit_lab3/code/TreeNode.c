#include "TreeNode.h"

pNode createNode(int _lineno, NodeType _type, char *_name, char *_value, int args, ...)
{
    pNode curNode = (pNode)malloc(sizeof(TreeNode));
    assert(curNode != NULL);
    curNode->name = (char *)malloc( sizeof(char) * (strlen(_name) + 1) );
    assert(curNode->name != NULL);

    // 给节点赋值
    curNode->lineno = _lineno;
    curNode->type = _type;
    strncpy(curNode->name, _name, strlen(_name) + 1);

    if (args == 0) {
        curNode->val = newString(_value); // _value = yytex 为 char* 类型
        assert(curNode->val != NULL);
        curNode->child = NULL;
        curNode->sibling = NULL;
        return curNode;
    } else { // 如果有兄弟节点
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
        printf("%s: ", root->name);
        if (root->type == NOT_A_TOKEN) {
            printf("(%d)", root->lineno);
        } else if (root->type == TOKEN_TYPE || root->type == TOKEN_ID) {
            printf("%s", root->val);
        } else if (root->type == TOKEN_INT) {
            printf("%d", atoi(root->val));
        } else if (root->type == TOKEN_HEX) {
            printf("%d", (int)strtol(root->val, NULL, 16));
        } else if (root->type == TOKEN_OCT) {
            printf("%d", (int)strtol(root->val, NULL, 8));
        } else if (root->type == TOKEN_FLOAT) {
            printf("%f", atof(root->val));
        }
        printf("\n");
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

void delNode(pNode* node) {
    if (node == NULL) return;
    pNode p = *node;
    while (p->child != NULL) {
        pNode temp = p->child;
        p->child = p->child->sibling;
        delNode(&temp);
    }
    free(p->name);
    free(p->val);
    free(p);
    p = NULL;
}
