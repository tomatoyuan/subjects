#ifndef SEMANTIC_H
#define SEMANTIC_H

#define HASH_TABLE_SIZE 0x3ffff
#define STACK_DEEP

#include "TreeNode.h"

typedef struct type *pType;
typedef struct fieldList *pFieldList;
typedef struct tableItem *pItem;
typedef struct hashTable *pHash;
typedef struct stack *pStack;
typedef struct table *pTable;

typedef struct type {
    Kind kind;
    union {
        // 基本类型
        BasicType basic;

        // 数据类型信息包括元素类型与数组大小构成
        struct {
            pType elem;
            int size;
        } array;

        // 结构体类型信息是一个链表
        struct {
            char *structName;
            pFieldList field;
        } structure;

        struct {
            int argc;
            pFieldList argv;
            pType returnType;
            int declaration_flag; // 函数定义后置为 1
            int firstFindLine; // 第一次出现该函数的位置
        } function;
    } u;
} Type;

typedef struct fieldList {
    char *name;         // 域的名字
    pType type;         // 域的类型
    boolean isArg;  // 实验3中结构体参数需要传址，此处需要做特殊处理
    pFieldList tail;    // 下一个域
} FieldList;

typedef struct tableItem {
    int symbolDepth;
    pFieldList field;
    pItem nextSymbol;   // 栈中同等深度的符号
    pItem nextHash;     // 处理hash冲突
} TableItem;

typedef struct hashTable {
    pItem *hashArray;   // 散列表
} HashTable;

typedef struct stack {
    pItem *stackArray;
    int curStackDepth;  // 记录当前元素在栈中的深度
} Stack;

typedef struct table { // 十字链表与 open hashing
    pHash hash;
    pStack stack;
    // 未命名结构体，此法分析中 id 不能为纯数字，所以将未命名结构体 id 暂时定义为 int
    int unNamedStructNum;
} Table;

extern pTable table;

// Type functions
pType newType(Kind kind, int argc, ...);
pType copyType(pType src);
void deleteType(pType type);
boolean checkType(pType type1, pType type2);
void printType(pType type);

pFieldList newFieldList(char *newName, pType newType);
pFieldList copyFieldList(pFieldList src);
void deleteFieldList(pFieldList fieldList);
void setFieldList(pFieldList p, char *newName);
void printFieldList(pFieldList fieldList);

pItem newItem(int symbolDepth, pFieldList pfield);
void deleteItem(pItem item);
boolean isStructDef(pItem src);

// Hash functions
pHash newHash();
void deleteHash(pHash hash);
pItem getHashHead(pHash hash, int index);
void setHashHead(pHash hash, int index, pItem newVal);

// Stack functions
pStack newStack();
void deleteStack(pStack stack);
void addStackDepth(pStack stack);
void minusStackDepth(pStack stack);
pItem getCurDepthStackHead(pStack stack);
void setCurDepthStackHead(pStack stack, pItem newVal);

// Table functions
pTable initTable();
void deleteTable(pTable table);
pItem searchTableItem(pTable table, char *name);
boolean checkTableItemConflict(pTable table, pItem item);
void addTableItem(pTable table, pItem item);
void deleteTableItem(pTable table, pItem item);
void clearCurDepthStackList(pTable table);
void printTable(pTable table);

// (12)
#define THREE_EIGHTH    12
// (2)
#define ONE_SIXTEENTH   2
// PJW 哈希算法
static inline unsigned int hash_pjw(char *name)
{
    unsigned int hash_value, i;
    for(hash_value = 0; *name; ++name) {
        // hash_value左移2位并在后8位加上name
        hash_value = (hash_value << ONE_SIXTEENTH) + *name;
        // 如果最高2位不全为零，即hash_value存放字符达到5个，第6个字符将无法存放。
        if ((i = hash_value & ~HASH_TABLE_SIZE) != 0 ) {
            //hash_value与自己右移12位的结果按位异或并将最高(2)位全置为0，结果存入hash_value
            hash_value = ( hash_value ^ (i >> THREE_EIGHTH) ) & HASH_TABLE_SIZE;
        }
    }
    return hash_value;
}

static inline void pError(ErrorType type, int line, char *msg)
{
    printf("Error type %d at Line %d: %s\n", type, line, msg);
} node;

void Traversal(pNode node);

// Generate symbol table functions 语法单元
void ExtDef(pNode node);
void ExtDecList(pNode node, pType specifier);
pType Specifier(pNode node);
pType StructSpecifier(pNode node);
pItem VarDec(pNode node, pType specifier);
// void FunDec(pNode node, pType returnType);
void FunDec(pNode node, pType returnType);
int isSameFunc(int curLine, pItem t1, pItem t2);
void checkFunc(pNode node, pTable table, pItem item);
void VarList(pNode node, pItem func);
pFieldList ParamDec(pNode node);
void CompSt(pNode node, pType returnType);
void StmtList(pNode node, pType returnType);
void Stmt(pNode node, pType returnType);
void DefList(pNode node, pItem structInfo);
void Def(pNode node, pItem structInfo);
void DecList(pNode node, pType specifier, pItem structInfo);
void Dec(pNode node, pType specifier, pItem structInfo);
pType Exp(pNode node);
void Args(pNode node, pItem funcInfo);
void checkFuncDefination(pTable table);

#endif