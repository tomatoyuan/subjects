#include "semantic.h"

pTable table; // 全局符号表

const char *myTypeName[4] = {
    "BASIC",
    "ARRAY",
    "STRUCTURE",
    "FUNCTION"
};

const char *myBasicTypeName[2] = {
    "int",
    "float"
};

// Type functions
pType newType(Kind kind, int argc, ...)
{
    pType p = (pType)malloc(sizeof(Type));
    assert(p != NULL);
    p->kind = kind;
    va_list vaList;
    assert(kind == BASIC || kind == ARRAY || kind == STRUCTURE || kind == FUNCTION);
    switch (kind)
    {
    case BASIC:
        va_start(vaList, argc);
        p->u.basic = va_arg(vaList, BasicType);
        break;
    case ARRAY:
        va_start(vaList, argc);
        p->u.array.elem = va_arg(vaList, pType);
        p->u.array.size = va_arg(vaList, int);
        break;
    case STRUCTURE:
        va_start(vaList, argc);
        p->u.structure.structName = va_arg(vaList, char *);
        p->u.structure.field = va_arg(vaList, pFieldList);
        break;
    case FUNCTION:
        va_start(vaList, argc);
        p->u.function.argc = va_arg(vaList, int);
        p->u.function.argv = va_arg(vaList, pFieldList);
        p->u.function.returnType = va_arg(vaList, pType);
        p->u.function.declaration_flag = va_arg(vaList, int);
        p->u.function.firstFindLine = va_arg(vaList, int);
        break;
    }
    va_end(vaList);
    return p;
}

pType copyType(pType src)
{
    if (src == NULL) return NULL;
    pType p = (pType)malloc(sizeof(Type));
    assert(p != NULL);
    p->kind = src->kind;
    assert(p->kind == BASIC || p->kind == ARRAY || p->kind == STRUCTURE || p->kind == FUNCTION);
    switch (p->kind) {
        case BASIC:
            p->u.basic = src->u.basic;
            break;
        case ARRAY:
            p->u.array.elem = copyType(src->u.array.elem);
            p->u.array.size = src->u.array.size;
            break;
        case STRUCTURE:
            p->u.structure.structName = newString(src->u.structure.structName);
            p->u.structure.field = copyFieldList(src->u.function.argv);
            break;
        case FUNCTION:
            p->u.function.argc = src->u.function.argc;
            p->u.function.argv = copyFieldList(src->u.function.argv);
            p->u.function.returnType = copyType(src->u.function.returnType);
            p->u.function.declaration_flag = src->u.function.declaration_flag;
            p->u.function.firstFindLine = src->u.function.firstFindLine;
            break;
    }
    return p;
}

void deleteType(pType type)
{
    assert(type != NULL);
    assert(type->kind == BASIC || type->kind == ARRAY ||
           type->kind == STRUCTURE || type->kind == FUNCTION);
    pFieldList temp = NULL;

    // pFieldList tDelete = NULL;
    switch (type->kind) {
        case BASIC:
            break;
        case ARRAY:
            deleteType(type->u.array.elem);
            type->u.array.elem = NULL;
            break;
        case STRUCTURE:
            if (type->u.structure.structName) {
                free(type->u.structure.structName);
            }
            type->u.structure.structName = NULL;

            temp = type->u.structure.field;
            while (temp) {
                pFieldList tDelete = temp;
                temp = temp->tail;
                deleteFieldList(tDelete);
            }
            type->u.structure.field = NULL;
            break;
        case FUNCTION:
            deleteType(type->u.function.returnType);
            type->u.function.returnType = NULL;
            temp = type->u.function.argv;
            while (temp) {
                pFieldList tDelete = temp;
                temp = temp->tail;
                deleteFieldList(tDelete);
            }
            type->u.function.argv = NULL;
            break;
    }
    free(type);
}

boolean checkType(pType type1, pType type2)
{
    if (type1 == NULL || type2 == NULL) return TRUE;
    if (type1->kind == FUNCTION || type2->kind == FUNCTION) return FALSE;
    if (type1->kind != type2->kind) return FALSE;
    else {
        assert(type1->kind == BASIC || type1->kind == ARRAY ||
               type1->kind == STRUCTURE);
        switch (type1->kind) {
            case BASIC:
                return type1->u.basic == type2->u.basic;
            case ARRAY:
                return checkType(type1->u.array.elem, type2->u.array.elem);
            case STRUCTURE:
                return !strcmp(type1->u.structure.structName,
                               type2->u.structure.structName);
        }
    }
}

void printType(pType type) {
    if (type == NULL) {
        printf("type is NULL.\n");
    } else {
        printf("type kind: %s\n", myTypeName[type->kind]);
        switch (type->kind) {
            case BASIC:
                printf("type basic: %s\n", myBasicTypeName[type->u.basic]);
                break;
            case ARRAY:
                printf("array size: %s\n", myBasicTypeName[type->u.array.size]);
                printType(type->u.array.elem);
                break;
            case STRUCTURE:
                if (!type->u.structure.structName)
                    printf("struct name is NULL\n");
                else {
                    printf("struct name is %s\n", type->u.structure.structName);
                }
                printf("{\n");
                printFieldList(type->u.structure.field);
                printf("}\n");
                break;
            case FUNCTION:
                printf("function argc is %d\n", type->u.function.argc);
                printf("function args:\n");
                printf("{\n");
                printFieldList(type->u.function.argv);
                printf("}\n");
                printf("function return type:\n");
                printType(type->u.function.returnType);
                break;
        }
    }
}

// FieldList functions
pFieldList newFieldList(char *newName, pType newType)
{
    pFieldList p = (pFieldList)malloc(sizeof(FieldList));
    assert(p != NULL);
    p->name = newString(newName);
    p->type = newType;
    p->tail = NULL;
    return p;
}

pFieldList copyFieldList(pFieldList src)
{
    assert(src != NULL);
    pFieldList head = NULL, cur = NULL;
    pFieldList temp = src;

    while (temp) {
        if (!head) {
            head = newFieldList(temp->name, copyType(temp->type));
            cur = head;
            temp = temp->tail;
        } else {
            cur->tail = newFieldList(temp->name, copyType(temp->type));
            cur = cur->tail;
            temp = temp->tail;
        }
    }
    return head;
}

void deleteFieldList(pFieldList fieldList)
{
    assert(fieldList != NULL);
    if (fieldList->name) {
        free(fieldList->name);
        fieldList->name = NULL;
    }
    if (fieldList->type) deleteType(fieldList->type);
    fieldList->type = NULL;
    free(fieldList);
}

void setFieldListName(pFieldList p, char *newName)
{
    assert(p != NULL && newName != NULL);
    if (p->name != NULL) {
        free(p->name);
    }

    p->name = newString(newName);
}

void printFieldList(pFieldList fieldList)
{
    if (fieldList == NULL) {
        printf("fieldList is NULL\n");
    } else {
        printf("fieldList name is: %s\n", fieldList->name);
        printf("FieldList Type:\n");
        printType(fieldList->type);
        printFieldList(fieldList->tail);
    }
}

// tableItem functions
pItem newItem(int symbolDepth, pFieldList pfield)
{
    pItem p = (pItem)malloc(sizeof(TableItem));
    assert(p != NULL);
    p->symbolDepth = symbolDepth;
    p->field = pfield;
    p->nextHash = NULL;
    p->nextSymbol = NULL;
    return p;
}

void deleteItem(pItem item)
{
    assert(item != NULL);
    if (item->field != NULL) deleteFieldList(item->field);
    free(item);
}

// Hash functions
pHash newHash()
{
    pHash p = (pHash)malloc(sizeof(HashTable));
    assert(p != NULL);
    p->hashArray = (pItem*)malloc(sizeof(pItem) * HASH_TABLE_SIZE);
    assert(p->hashArray != NULL);
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        p->hashArray[i] = NULL;
    }
    return p;
}

void deleteHash(pHash hash)
{
    assert(hash != NULL);
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        pItem temp = hash->hashArray[i];
        while (temp) {
            pItem tdelete = temp;
            temp = temp->nextHash;
            deleteItem(tdelete);
        }
        hash->hashArray[i] = NULL;
    }
    free(hash->hashArray);
    hash->hashArray = NULL;
    free(hash);
}

pItem getHashHead(pHash hash, int index)
{
    assert(hash != NULL);
    return hash->hashArray[index];
}

void setHashHead(pHash hash, int index, pItem newVal)
{
    assert(hash != NULL);
    hash->hashArray[index] = newVal;
}

// Table functions
pTable initTable()
{
    pTable table = (pTable)malloc(sizeof(Table));
    assert(table != NULL);
    table->hash = newHash();
    table->stack = newStack();
    table->unNamedStructNum = 0;
    return table;
}

void deleteTable(pTable table)
{
    deleteHash(table->hash);
    table->hash = NULL;
    deleteStack(table->stack);
    table->stack = NULL;
    free(table);
}

pItem searchTableItem(pTable table, char *name)
{
    unsigned int hashCode = hash_pjw(name);
    // printf("name = %s   hashCode = %u\n", name, hashCode);
    pItem temp = getHashHead(table->hash, hashCode);
    if (temp == NULL) return NULL;
    while (temp) {
        if (!strcmp(temp->field->name, name)) return temp;
        temp = temp->nextHash;
    }
    return NULL;
}

// 无冲突返回 FALSE，有冲突返回 TRUE
boolean checkTableItemConflict(pTable table, pItem item) {
    pItem temp = searchTableItem(table, item->field->name);
    if (temp == NULL) return FALSE;
    while (temp) {
        if (!strcmp(temp->field->name, item->field->name)) {
            if (temp->field->type->kind == STRUCTURE ||
                item->field->type->kind == STRUCTURE)
                return TRUE;
            if (temp->symbolDepth == table->stack->curStackDepth) return TRUE;
        }
        temp = temp->nextHash;
    }
    return FALSE;
}

// 插入符号表
void addTableItem(pTable table, pItem item)
{
    assert(table != NULL && item != NULL);
    unsigned hashCode = hash_pjw(item->field->name); // 获得散列值
    pHash hash = table->hash;
    pStack stack = table->stack;
    item->nextSymbol = getCurDepthStackHead(stack); // 栈中同等深度
    setCurDepthStackHead(stack, item); // 更新栈顶符号

    item->nextHash = getHashHead(hash, hashCode); // 同一作用域，槽
    setHashHead(hash, hashCode, item);
}

void deleteTableItem(pTable table, pItem item)
{
    assert(table != NULL && item != NULL);
    unsigned hashCode = hash_pjw(item->field->name);
    if (item == getHashHead(table->hash, hashCode)) {
        setHashHead(table->hash, hashCode, item->nextHash);
    } else {
        pItem cur = getHashHead(table->hash, hashCode);
        pItem last = cur;
        while (cur != item) {
            last = cur;
            cur = cur->nextHash;
        }
        last->nextHash = cur->nextHash;
    }
    deleteItem(item);
}

boolean isStructDef(pItem src)
{
    if (src == NULL) return FALSE;
    if (src->field->type->kind != STRUCTURE) return FALSE;
    if (src->field->type->u.structure.structName) return FALSE;
    return TRUE;
}

void clearCurDepthStackList(pTable table)
{
    assert(table != NULL);
    pStack stack = table->stack;
    pItem temp = getCurDepthStackHead(stack);
    while (temp) {
        pItem tDelete = temp;
        temp = temp->nextSymbol;
        deleteTableItem(table, tDelete);
    }
    setCurDepthStackHead(stack, NULL);
    minusStackDepth(stack);
}

// for Debug
void printTable(pTable table)
{
    printf("----------------hash_table----------------\n");
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        pItem item = getHashHead(table->hash, i);
        if (item) {
            printf("[%d]", i);
            while (item) {
                printf(" -> name: %s depth: %d\n", item->field->name, item->symbolDepth);
                printf("========FiledList========\n");
                printFieldList(item->field);
                printf("===========End===========\n");
                item = item->nextHash;
            }
        }
    }
    printf("-------------------end--------------------\n");
}

// Stack functions
pStack newStack()
{
    pStack p = (pStack)malloc(sizeof(Stack));
    assert(p != NULL);
    p->stackArray = (pItem *)malloc(sizeof(pItem) * HASH_TABLE_SIZE);
    assert(p->stackArray != NULL);
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        p->stackArray[i] = NULL;
    }
    p->curStackDepth = 0;
    return p;
}

void deleteStack(pStack stack)
{
    assert(stack != NULL);
    free(stack->stackArray);
    stack->stackArray = NULL;
    stack->curStackDepth = 0;
    free(stack);
}

void addStackDepth(pStack stack)
{
    assert(stack != NULL);
    stack->curStackDepth++;
}

void minusStackDepth(pStack stack)
{
    assert(stack != NULL);
    stack->curStackDepth--;
}

pItem getCurDepthStackHead(pStack stack)
{
    assert(stack != NULL);
    return stack->stackArray[stack->curStackDepth];
}

// 栈顶添加元素
void setCurDepthStackHead(pStack stack, pItem newVal)
{
    assert(stack != NULL);
    stack->stackArray[stack->curStackDepth] = newVal;
}

// Global function
void Traversal(pNode node)
{
    if (node == NULL) return ;
    // 处理符号一定进入“ExtDef”
    if (!strcmp(node->name, "ExtDef")) ExtDef(node);
    // 递归进行语义分析
    Traversal(node->child);
    Traversal(node->sibling);
}

// Generate symbol table functions
void ExtDef(pNode node)
{
    // ExtDef -> Specifier ExtDecList SEMI
    //         | Specifier SEMI
    //         | Specifier FunDec SEMI
    //         | Specifier FunDec CompSt
    assert(node != NULL);
    pType specifierType = Specifier(node->child); // ExtDef -> Specifier SEMI 结构体声明也包含在里面
    char *secondName = node->child->sibling->name;

    // ExtDef -> Specifier ExtDecList SEMI
    if (!strcmp(secondName, "ExtDecList")) {
        ExtDecList(node->child->sibling, specifierType);
    }

    // ExtDef -> Specifier FunDec SEMI
    //         | Specifier FunDef CompSt
    else if (!strcmp(secondName, "FunDec")) {
        char *afterFuncName = node->child->sibling->sibling->name;
        if (!strcmp(afterFuncName, "SEMI")) { // 函数声明
            FunDec(node->child->sibling, specifierType, 0);
            // FunDec(node->child->sibling, specifierType, 0);
        } else if (!strcmp(afterFuncName, "CompSt")) { // 函数定义
            FunDec(node->child->sibling, specifierType, 1);
            CompSt(node->child->sibling->sibling, specifierType);
        }
    }
    if (specifierType) deleteType(specifierType);
}

void ExtDecList(pNode node, pType Specifier)
{
    assert(node != NULL);
    pNode temp = node;
    while (temp) {
        pItem item = VarDec(temp->child, Specifier);
        if (checkTableItemConflict(table, item)) {
            char msg[100] = {0};
            sprintf(msg, "Redefined variable \"%s\".", item->field->name);
            pError(REDEF_VAR, temp->lineno, msg);
            deleteItem(item);
        } else {
            addTableItem(table, item);
        }
        if (temp->child->sibling) {
            temp = temp->sibling->sibling->child;
        } else {
            break;
        }
    }
}

pType Specifier(pNode node)
{
    assert(node != NULL);
    pNode t = node->child;
    if (!strcmp(t->name, "TYPE")) {
        if (!strcmp(t->val, "float")) {
            return newType(BASIC, 1, FLOAT_TYPE);
        } else {
            return newType(BASIC, 1, INT_TYPE);
        }
    } else {
        return StructSpecifier(t);
    }
}

pType StructSpecifier(pNode node)
{
    assert(node != NULL);
    pType returnType = NULL;
    pNode t = node->child->sibling;
    if (strcmp(t->name, "Tag")) {
        pItem structItem =
            newItem(table->stack->curStackDepth,
                    newFieldList("", newType(STRUCTURE, 2, NULL, NULL)));
        if (!strcmp(t->name, "OptTag")) {
            setFieldListName(structItem->field, t->child->val);
            t = t->sibling;
        } else {
            table->unNamedStructNum++;
            char structName[20] = {0};
            sprintf(structName, "%d", table->unNamedStructNum);
            setFieldListName(structItem->field, structName);
        }
        // 进入结构体
        if (!strcmp(t->sibling->name, "DefList")) {
            DefList(t->sibling, structItem);
        }

        if (checkTableItemConflict(table, structItem)) {
            char msg[100] = {0};
            sprintf(msg, "Duplicated name \"%s\".", structItem->field->name);
            pError(DUPLICATED_NAME, node->lineno, msg);
            deleteItem(structItem);
        } else {
            returnType = newType(
                STRUCTURE, 
                2,
                newString(structItem->field->name),
                copyFieldList(structItem->field->type->u.structure.field)
            );
            if (!strcmp(node->child->sibling->name, "OptTag")) {
                addTableItem(table, structItem);
            } else {
                deleteItem(structItem);
            }
        }
    // 结束
    }

    // StructSpecifier->STRUCT Tag
    else {
        pItem structItem = searchTableItem(table, t->child->val);
        if (structItem == NULL || !isStructDef(structItem)) {
            char msg[100] = {0};
            sprintf(msg, "Undefined structure \"%s\".", t->child->val);
            pError(UNDEF_STRUCT, node->lineno, msg);
        } else {
            returnType = newType(
                STRUCTURE, 
                2,
                newString(structItem->field->name),
                copyFieldList(structItem->field->type->u.structure.field)
            );
        }
    }
    return returnType;
}

pItem VarDec(pNode node, pType specifier)
{
    assert(node != NULL);
    // VarDec -> ID
    //         | VarDec LB INT RB
    pNode id = node;
    // get ID
    while (id->child) id = id->child;
    pItem p = newItem(table->stack->curStackDepth, newFieldList(id->val, NULL));

    // VarDec -> ID
    if (!strcmp(node->child->name, "ID")) {
        // printf("copy type tp %s.\n", node->child->val);
        p->field->type = copyType(specifier);
    }
    // VarDec -> VarDec LB INT RB
    else {
        pNode varDec = node->child;
        pType temp = specifier;

        while (varDec->sibling) {
            p->field->type =
                newType(ARRAY, 2, copyType(temp), atoi(varDec->sibling->sibling->val));

            temp = p->field->type;
            varDec = varDec->child;
        }
    }
    return p;
}

void FunDec(pNode node, pType returnType, int declaration_flag)
{
    assert(node != NULL);
    // FunDec -> ID LP VarList RP
    //         | ID LP RP
    pItem p =
        newItem(table->stack->curStackDepth,
                newFieldList(node->child->val,
                             newType(FUNCTION, 5, 0, NULL, copyType(returnType), declaration_flag, node->lineno)));

    // FunDec -> ID LP VarList RP
    if (!strcmp(node->child->sibling->sibling->name, "VarList")) {
        VarList(node->child->sibling->sibling, p);
    }

    // FunDec -> ID LP RP ： don't need process

    if ( !checkTableItemConflict(table, p) ) { // 函数第一次定义或者声明
        addTableItem(table, p);

    } else { // 函数的声明或者定义再次出现时
        checkFunc(node, table, p);

    }

}

// 检查函数声明与定义是否正确
void checkFunc(pNode node, pTable table, pItem item)
{
    pItem temp = searchTableItem(table, item->field->name);
    // pItem record = temp;
    assert(temp != NULL); // 能到这里，说明可以在符号表中找到该函数声明或定义
    while (temp) {
        if (!strcmp(temp->field->name, item->field->name)) {
            if (temp->field->type->kind == FUNCTION
                && item->field->type->kind == FUNCTION) {

                // printf("function conflict\n");
                // printFieldList(temp->field);
                // printf("\n");
                // printFieldList(item->field);
                // 重复定义函数
                if (temp->field->type->u.function.declaration_flag
                    && item->field->type->u.function.declaration_flag) {
                    char msg[100] = {0};
                    sprintf(msg, "Redefined function \"%s\".", item->field->name);
                    pError(REDEF_FUNC, node->lineno, msg);
                    return ;
                }
                // 定义过的函数遇到了函数声明
                else if (temp->field->type->u.function.declaration_flag
                           && !item->field->type->u.function.declaration_flag) {
                    // 检查声明的函数参数和返回值是否相同
                    // printf("定义过的函数遇到了函数声明\n");
                    pItem t1 = temp, t2 = item;
                    if (!isSameFunc(node->lineno, t1, t2)) {
                        // char msg[100] = {0};
                        // sprintf(msg, "定义过的函数遇到了函数声明 \"%s\".", item->field->name);
                        // pError(REDEF_FUNC, node->lineno, msg);
                    }
                }
                // 没定义过的函数，又遇到了函数声明
                else if (!temp->field->type->u.function.declaration_flag
                         && !item->field->type->u.function.declaration_flag) {
                    // 检查声明的函数参数和返回值是否相同
                    // printf("没定义过的函数，又遇到了函数声明\n");
                    pItem t1 = temp, t2 = item;
                    if (isSameFunc(node->lineno, t1, t2) == FALSE) {
                        // char msg[100] = {0};
                        // sprintf(msg, "没定义过的函数，又遇到了函数声明 \"%s\".", item->field->name);
                        // pError(REDEF_FUNC, node->lineno, msg);
                    }
                }
                // 声明过的函数遇到了函数定义
                else if (!temp->field->type->u.function.declaration_flag
                         && item->field->type->u.function.declaration_flag) {
                    // 检查声明的函数参数和返回值是否相同，相同的话将哈希表中的 declaration_flag 置为 1
                    // printf("声明过的函数遇到了函数定义\n");
                    pItem t1 = temp, t2 = item;
                    if (isSameFunc(node->lineno, t1, t2) == 0) {
                        // char msg[100] = {0};
                        // sprintf(msg, "声明过的函数遇到了函数定义 \"%s\".", item->field->name);
                        // pError(REDEF_FUNC, node->lineno, msg);
                    } else {
                        temp->field->type->u.function.declaration_flag = 1;
                    }
                }
                // 释放空间
                deleteItem(item);
                item = NULL;
            }
        }
        temp = temp->nextHash;
    }
}

int isSameFunc(int curLine, pItem t1, pItem t2)
{
    if (t1->field->type->u.function.returnType->kind != t2->field->type->u.function.returnType->kind) {
        char msg[200] = {0};
        sprintf(msg, "Inconsistent declaration of function \"%s\". \n\tDetails: 返回值类型不一致! func1.returnType = %s, func2.returnType = %s", 
                     t1->field->name, myTypeName[t1->field->type->u.function.returnType->kind], myTypeName[t2->field->type->u.function.returnType->kind]);
        pError(MULTI_DECLARA_CONFLICT, curLine, msg);
        return FALSE;
    } else if (t1->field->type->u.function.argc != t2->field->type->u.function.argc) {
        char msg[200] = {0};
        sprintf(msg, "Inconsistent declaration of function \"%s\". \n\tDetails: 参数数量不一致！func1() need %d, but func2() need %d", 
                t1->field->name, t1->field->type->u.function.argc, t2->field->type->u.function.argc);
        pError(MULTI_DECLARA_CONFLICT, curLine, msg);
        return FALSE;
    } else {
        int func_argc = t1->field->type->u.function.argc;
        for (int i = 0; i < func_argc; i++) {
            // 参数种类不同
            if (t1->field->type->u.function.argv->type->kind != t2->field->type->u.function.argv->type->kind) {
                char msg[200] = {0};
                sprintf(msg, "Inconsistent declaration of function \"%s\". \n\tDetails: 参数类型不一致！ func1.paramType = %s, func2.paramType = %s", 
                        t1->field->name, myTypeName[t1->field->type->u.function.argv->type->kind], myTypeName[t2->field->type->u.function.argv->type->kind]);
                pError(MULTI_DECLARA_CONFLICT, curLine, msg);
                return FALSE;
            } 
            // 参数的 Basic 类型不匹配
            else if (t1->field->type->u.function.argv->type->kind == BASIC) {
                if (t1->field->type->u.function.argv->type->u.basic != t2->field->type->u.function.argv->type->u.basic) {
                    char msg[200] = {0};
                    sprintf(msg, "Inconsistent declaration of function \"%s\". \n\tDetails: 参数类型不一致！ func1.paramType = %s, func2.paramType = %s", 
                            t1->field->name, myBasicTypeName[t1->field->type->u.function.argv->type->u.basic], myBasicTypeName[t2->field->type->u.function.argv->type->u.basic]);
                    pError(MULTI_DECLARA_CONFLICT, curLine, msg);
                    return FALSE;
                }
            } 
            // 参数的 Struct 类型不匹配
            else if (t1->field->type->u.function.argv->type->kind == STRUCTURE) {
                char *structName1 = t1->field->type->u.function.argv->type->u.structure.structName;
                char *structName2 = t2->field->type->u.function.argv->type->u.structure.structName;
                if ( strcmp(structName1, structName2) ) {
                    char msg[200] = {0};
                    sprintf(msg, "Inconsistent declaration of function \"%s\". \n\tDetails: 参数结构体不一致！ func1.structType = %s, func2.structType = %s", 
                            t1->field->name, structName1, structName2);
                    pError(MULTI_DECLARA_CONFLICT, curLine, msg);
                    return FALSE;
                }
            } else {
                t1 = t1->nextHash;
                t2 = t2->nextHash;
            }
        }
    }
    return TRUE;
}

void VarList(pNode node, pItem func)
{
    assert(node != NULL);
    // VarList -> ParamDec COMMA VarList
    //          | ParamDec
    addStackDepth(table->stack);
    int argc = 0;
    pNode temp = node->child;
    pFieldList cur = NULL;

    // VarList -> ParamDec
    pFieldList paramDec = ParamDec(temp, func);
    func->field->type->u.function.argv = copyFieldList(paramDec);
    cur = func->field->type->u.function.argv;
    argc++;

    // VarList -> ParamDec COMMA VarList
    while (temp->sibling) {
        temp = temp->sibling->sibling->child;
        paramDec = ParamDec(temp, func);
        if (paramDec) {
            cur->tail = copyFieldList(paramDec);
            cur = cur->tail;
            argc++;
        }
    }

    func->field->type->u.function.argc = argc;

    minusStackDepth(table->stack);
}

pFieldList ParamDec(pNode node, pItem func)
{
    assert(node != NULL);
    // ParamDec -> Specifier VarDec
    pType specifierType = Specifier(node->child);
    pItem p = VarDec(node->child->sibling, specifierType);
    if (specifierType) deleteType(specifierType);
    if (checkTableItemConflict(table, p)) {
        pItem temp = searchTableItem(table, func->field->name);
        // 遇到函数声明
        if (func->field->type->u.function.declaration_flag == 0) {
            return p->field;
        }
        // 遇到函数第一次定义
        else if (temp->field->type->u.function.declaration_flag == 0 && func->field->type->u.function.declaration_flag) {
            // temp->field->type->u.function.declaration_flag = 1;
            return p->field;
        }
        // 其余情况正常报错
        char msg[100] = {0};
        sprintf(msg, "Redefined variable \"%s\".", p->field->name);
        pError(REDEF_VAR, node->lineno, msg);
        deleteItem(p);
        return NULL;
    } else {
        addTableItem(table, p);
        return p->field;
    }
}

void funcDeclaration(pNode node, pType returnType)
{
    assert(node != NULL);
    // CompSt -> LC DefList StmtList RC
    // printTree(node, 0);
    addStackDepth(table->stack);
    pNode temp = node->child->sibling;
    if (!strcmp(temp->name, "DefList")) {
        DefList(temp, NULL);
        temp = temp->sibling;
    }
    if (!strcmp(temp->name, "StmtList")) {
        StmtList(temp, returnType);
    }

    clearCurDepthStackList(table);
}

void CompSt(pNode node, pType returnType)
{
    assert(node != NULL);
    // CompSt -> LC DefList StmtList RC
    // printTree(node, 0);
    addStackDepth(table->stack);
    pNode temp = node->child->sibling;
    if (!strcmp(temp->name, "DefList")) {
        DefList(temp, NULL);
        temp = temp->sibling;
    }
    if (!strcmp(temp->name, "StmtList")) {
        StmtList(temp, returnType);
    }

    clearCurDepthStackList(table);
}

void StmtList(pNode node, pType returnType)
{
    // StmtList -> Stmt StmtList
    //           | epsilon
    while (node) {
        Stmt(node->child, returnType);
        node = node->child->sibling;
    }
}

void Stmt(pNode node, pType returnType)
{
    assert(node != NULL);
    // Stmt -> Exp SEMI
    //       | CompSt
    //       | RETURN Exp SEMI
    //       | IF LP Exp RP Stmt
    //       | IF LP Exp RP Stmt ELSE Stmt
    //       | WHILE LP Exp RP Stmt

    pType expType = NULL;
    // Stmt -> Exp SEMI
    if (!strcmp(node->child->name, "Exp")) expType = Exp(node->child);

    // Stmt -> CompSt
    else if (!strcmp(node->child->name, "CompSt")) {
        CompSt(node->child, returnType);
    }

    // Stmt -> RETURN Exp SEMI
    else if (!strcmp(node->child->name, "RETURN")) {
        expType = Exp(node->child->sibling);

        // check return type
        if (!checkType(returnType, expType)) {
            pError(TYPE_MISMATCH_RETURN, node->lineno,
                   "Type mismatched for return.");
        }
    }

    // Stmt -> IF LP Exp RP Stmt
    else if (!strcmp(node->child->name, "IF")) {
        pNode stmt = node->child->sibling->sibling->sibling->sibling;
        expType = Exp(node->child->sibling->sibling);
        Stmt(stmt, returnType);
        // Stmt -> IF LP Exp RP Stmt ELSE Stmt
        if (stmt->sibling != NULL) Stmt(stmt->sibling->sibling, returnType);
    }

    // Stmt -> WHILE LP Exp RP Stmt
    else if (!strcmp(node->child->name, "WHILE")) {
        expType = Exp(node->child->sibling->sibling);
        Stmt(node->child->sibling->sibling->sibling->sibling, returnType);
    }

    if (expType) deleteType(expType);
}

void DefList(pNode node, pItem structInfo)
{
    // assert(node != NULL);
    // DefList -> Def DefList
    //          | epsilon
    while (node) {
        Def(node->child, structInfo);
        node = node->child->sibling;
    }
}

void Def(pNode node, pItem structInfo)
{
    assert(node != NULL);
    // Def -> Specifier DecList SEMI
    // TODO: 调用接口
    pType dectype = Specifier(node->child);
    DecList(node->child->sibling, dectype, structInfo);
    if (dectype) deleteType(dectype);
}

void DecList(pNode node, pType specifier, pItem structInfo)
{
    assert(node != NULL);
    // DecList -> Dec
    //          | Dec COMMA DecList
    pNode temp = node;
    while (temp) {
        Dec(temp->child, specifier, structInfo);
        if (temp->child->sibling) {
            temp = temp->child->sibling->sibling;
        } else {
            break;
        }
    }
}

void Dec(pNode node, pType specifier, pItem structInfo)
{
    assert(node != NULL);
    // Dec -> VarDec
    //      | VarDec ASSIGNOP Exp

    // Dec -> VarDec
    if (node->child->sibling == NULL) {
        if (structInfo != NULL) {
            // 结构体内，将 VarDec 返回的 Item 中的 filedList
            // Copy 判断是否重定义，无错则倒结构体链表尾，记得 delete 掉 Item
            pItem decitem = VarDec(node->child, specifier);
            pFieldList payload = decitem->field;
            pFieldList structField = structInfo->field->type->u.structure.field;
            pFieldList last = NULL;
            while (structField != NULL) {
                // then we have to check
                if (!strcmp(payload->name, structField->name)) {
                    // 出现重定义，报错
                    char msg[100] = {0};
                    sprintf(msg, "Redefined field \"%s\".",
                            decitem->field->name);
                    pError(REDEF_FEILD, node->lineno, msg);
                    deleteItem(decitem);
                    return ;
                } else {
                    last = structField;
                    structField = structField->tail;
                }
            }
            // 新建一个 fieldlist，删除之前的 item
            if (last == NULL) {
                // that is goog
                structInfo->field->type->u.structure.field =
                    copyFieldList(decitem->field);
            } else {
                last->tail = copyFieldList(decitem->field);
            }
            deleteItem(decitem);
        } else {
            // 非结构体内，判断返回的 item 有无冲突，无冲突放入表中，有冲突报错并 delete
            pItem decitem = VarDec(node->child, specifier);
            if (checkTableItemConflict(table, decitem)) {
                // 出现冲突，报错
                char msg[100] = {0};
                sprintf(msg, "Redefined variable \"%s\".",
                        decitem->field->name);
                pError(REDEF_VAR, node->lineno, msg);
                deleteItem(decitem);
            } else {
                addTableItem(table, decitem);
            }
        }
    }

    // Dec -> VarDec ASSIGNOP Exp
    else {
        if (structInfo != NULL) {
            // 结构体内不能赋值，报错
            pError(REDEF_FEILD, node->lineno,
                   "Illegal initialize variable in struct.");
        } else {
            // 判断赋值类型是否相等
            // 如果成功，注册该符号
            pItem decitem = VarDec(node->child, specifier);
            pType exptype = Exp(node->child->sibling->sibling);
            if (checkTableItemConflict(table, decitem)) {
                // 出现冲突，报错
                char msg[100] = {0};
                sprintf(msg, "Redefined variable \"%s\".", 
                        decitem->field->name);
                pError(REDEF_VAR, node->lineno, msg);
                deleteItem(decitem);
            }
            if (!checkType(decitem->field->type, exptype)) {
                // 类型不相符
                // 报错
                pError(TYPE_MISMATCH_ASSIGN, node->lineno, 
                       "Type mismatched for assignment.");
                deleteItem(decitem);
            }
            if (decitem->field->type && decitem->field->type->kind == ARRAY) {
                // 报错，对非 BASIC 类型赋值
                pError(TYPE_MISMATCH_ASSIGN, node->lineno,
                       "Illegal initialize variable.");
                deleteItem(decitem);
            } else {
                addTableItem(table, decitem);
            }
            // exp 不出意外应该返回一个无用的 type，删除即可
            if (exptype) deleteType(exptype);
        }
    }
}

pType Exp(pNode node)
{
    assert(node != NULL);
    // Exp -> Exp ASSIGNOP Exp
    //      | Exp AND Exp
    //      | Exp OR Exp
    //      | Exp RELOP Exp
    //      | Exp PLUS Exp
    //      | Exp MINUS Exp
    //      | Exp STAR Exp
    //      | Exp DIV Exp
    //      | LP Exp RP
    //      | MINUS Exp
    //      | NOT Exp
    //      | ID LP Args RP
    //      | ID LP RP
    //      | Exp LB Exp RB
    //      | Exp DOT ID
    //      | ID
    //      | INT
    //      | FLOAT
    pNode t = node->child;
    // exp will only check if the cal ir right

    // 二值运算
    if (!strcmp(t->name, "Exp")) {
        // 基本数学运算符
        if (strcmp(t->sibling->name, "LB") && strcmp(t->sibling->name, "DOT")) {
            pType p1 = Exp(t);
            pType p2 = Exp(t->sibling->sibling);
            pType returnType = NULL;

            // Exp -> Exp ASSIGNOP Exp
            if (!strcmp(t->sibling->name, "ASSIGNOP")) {
                // 检查左值
                pNode tchild = t->child;

                if (!strcmp(tchild->name, "FLOAT") || !strcmp(tchild->name, "INT")) {
                    // 报错，左值
                    pError(LEFT_VAR_ASSIGN, t->lineno,
                           "The left-hand side of an assignment must be "
                           "avariable.");
                } else if (!strcmp(tchild->name, "ID") ||
                           !strcmp(tchild->sibling->name, "LB") ||
                           !strcmp(tchild->sibling->name, "DOT")) {
                    if (!checkType(p1, p2)) {
                        // 报错，类型不匹配
                        pError(TYPE_MISMATCH_ASSIGN, t->lineno,
                               "Type mismatched for assignment.");
                    } else {
                        returnType = copyType(p1);
                    }
                } else {
                    // 报错，左值
                    pError(LEFT_VAR_ASSIGN, t->lineno,
                           "The left-hand side of an assignment must be "
                           "avariable.");
                }
            }
            // Exp -> Exp AND Exp
            //      | Exp OR Exp
            //      | Exp RELOP Exp
            //      | Exp PLUS Exp
            //      | Exp MINUS Exp
            //      | Exp STAR Exp
            //      | Exp DIV Exp
            else {
                if (p1 && p2 && (p1->kind == ARRAY || p2->kind == ARRAY)) {
                    // 报错，数组，结构体运算
                    pError(TYPE_MISMATCH_OP, t->lineno,
                           "Type mismatched for operands.");
                } else if (!checkType(p1, p2)) {
                    // 报错，类型不匹配
                    pError(TYPE_MISMATCH_OP, t->lineno,
                           "Type mismatched for operands.");
                } else {
                    if (p1 && p2) {
                        returnType = copyType(p1);
                    }
                }
            }

            if (p1) deleteType(p1);
            if (p2) deleteType(p2);
            return returnType;
        }
        // 数组和结构体访问
        else {
            // Exp -> Exp LB Exp RB
            if (!strcmp(t->sibling->name, "LB")) {
                // 数组
                pType p1 = Exp(t);
                pType p2 = Exp(t->sibling->sibling);
                pType returnType = NULL;

                if (!p1) {
                    // 第一个 exp 为 null，上层报错，这里不用再管
                } else if (p1 && p1->kind != ARRAY) {
                    // 报错，非数组使用 []
                    char msg[100] = {0};
                    sprintf(msg, "\"%s\" is not an array.", t->child->val);
                    pError(NOT_A_ARRAY, t->lineno, msg);
                } else if (!p2 || p2->kind != BASIC ||
                        p2->u.basic != INT_TYPE) {
                    // 报错，不用 int 索引 []
                    char msg[100] = {0};
                    sprintf(msg, "\"%s\" is not an integer.",
                            t->sibling->sibling->child->val);
                    pError(NOT_A_INT, t->lineno, msg);
                } else {
                    returnType = copyType(p1->u.array.elem);
                }
                if (p1) deleteType(p1);
                if (p2) deleteType(p2);
                return returnType;
            }
            // Exp -> Exp DOT ID
            else {
                pType p1 = Exp(t);
                pType returnType = NULL;
                if (!p1 || p1->kind != STRUCTURE ||
                    !p1->u.structure.structName) {
                    // 报错，对非结构体使用 . 运算符
                    pError(ILLEGAL_USE_DOT, t->lineno, "Illegal use of \".\".");
                } else {
                    pNode ref_id = t->sibling->sibling;
                    pFieldList structfield = p1->u.structure.field;
                    while (structfield != NULL) {
                        if (!strcmp(structfield->name, ref_id->val)) {
                            break;
                        }
                        structfield = structfield->tail;
                    }
                    if (structfield == NULL) {
                        // 报错，没有可以匹配的域名
                        char msg[100] = {0};
                        sprintf(msg, "Non-existent field \"%s\".", ref_id->val);
                        pError(NONEXISTFIELD, t->lineno, msg);
                    } else {
                        returnType = copyType(structfield->type);
                    }
                }
                if (p1) deleteType(p1);
                return returnType;
            }
        }
    }
    //单目运算符
    // Exp -> MINUS Exp
    //      | NOT Exp
    else if (!strcmp(t->name, "MINUS") || !strcmp(t->name, "NOT")) {
        pType p1 = Exp(t->sibling);
        pType returnType = NULL;
        if (!p1 || p1->kind != BASIC) {
            // 报错，数组、结构体运算
            printf("Error type %d at Line %d: %s.\n", 7, t->lineno,
                   "TYPE_MISMATCH_OP");
        } else {
            returnType = copyType(p1);
        }
        if (p1) deleteType(p1);
        return returnType;
    } else if (!strcmp(t->name, "LP")) {
        return Exp(t->sibling);
    }
    // Exp -> ID LP Args RP
    //      | ID LP RP
    else if (!strcmp(t->name, "ID") && t->sibling) {
        pItem funcInfo = searchTableItem(table, t->val);

        // function not find
        if (funcInfo == NULL) {
            char msg[100] = {0};
            sprintf(msg, "Undefined function \"%s\".", t->val);
            pError(UNDEF_FUNC, node->lineno, msg);
            return NULL;
        } else if (funcInfo->field->type->kind != FUNCTION) {
            char msg[100] = {0};
            sprintf(msg, "\"%s\" is not a function.", t->val);
            pError(NOT_A_FUNC, node->lineno, msg);
            return NULL;
        }
        // Exp -> ID LP Args RP
        else if (!strcmp(t->sibling->sibling->name, "Args")) {
            Args(t->sibling->sibling, funcInfo);
            return copyType(funcInfo->field->type->u.function.returnType);
        }
        // Exp -> ID LP RP
        else {
            if (funcInfo->field->type->u.function.argc != 0) {
                char msg[100] = {0};
                sprintf(msg,
                        "too few arguments to function \"%s\", except %d args.",
                        funcInfo->field->name,
                        funcInfo->field->type->u.function.argc);
                pError(FUNC_AGRC_MISMATCH, node->lineno, msg);
            }
            return copyType(funcInfo->field->type->u.function.returnType);
        }
    }
    // Exp -> ID
    else if (!strcmp(t->name, "ID")) {
        pItem tp = searchTableItem(table, t->val);
        if (tp == NULL || isStructDef(tp)) {
            char msg[100] = {0};
            sprintf(msg, "Undefined variable \"%s\".", t->val);
            pError(UNDEF_VAR, t->lineno, msg);
            return NULL;
        } else {
            // good
            return copyType(tp->field->type);
        }
    } else {
        // Exp -> FLOAT
        if (!strcmp(t->name, "FLOAT")) {
            return newType(BASIC, 1, FLOAT_TYPE);
        }
        // Exp -> INT
        else {
            return newType(BASIC, 1, INT_TYPE);
        }
    }
}

void Args(pNode node, pItem funcInfo)
{
    assert(node != NULL);
    // Args -> Exp COMMA Args
    //       | Exp
    pNode temp = node;
    pFieldList arg = funcInfo->field->type->u.function.argv;

    while (temp) {
        if (arg == NULL) {
            char msg[100] = {0};
            sprintf(
                msg, "too many arguments to function \"%s\", except %d args.",
                funcInfo->field->name, funcInfo->field->type->u.function.argc);
            pError(FUNC_AGRC_MISMATCH, node->lineno, msg);
            break;
        }
        pType realType = Exp(temp->child);

        if (!checkType(realType, arg->type)) {
            char msg[100] = {0};
            sprintf(msg, "Function \"%s\" is not applicable for arguments.",
                    funcInfo->field->name);
            pError(FUNC_AGRC_MISMATCH, node->lineno, msg);
            if (realType) deleteType(realType);
            return;
        }
        if (realType) deleteType(realType);

        arg = arg->tail;
        if (temp->child->sibling) {
            temp = temp->child->sibling->sibling;
        } else {
            break;
        }
    }
    if (arg != NULL) {
        char msg[100] = {0};
        sprintf(msg, "too few arguments to function \"%s\", except %d args.",
                funcInfo->field->name, funcInfo->field->type->u.function.argc);
        pError(FUNC_AGRC_MISMATCH, node->lineno, msg);
    }
}

void checkFuncDefination(pTable table)
{
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        pItem item = getHashHead(table->hash, i);
        if (item) {
            // printf("[%d]", i);
            while (item) {
                if (item->field->type->kind == FUNCTION && !item->field->type->u.function.declaration_flag) {
                    // printf("func_flag = %d, line = %d\n",
                    //         item->field->type->u.function.declaration_flag,
                    //         item->field->type->u.function.firstFindLine);
                    char msg[100] = {0};
                    sprintf(msg, "Undefined function \"%s\".",
                            item->field->name);
                    pError(UNDEF_DECLARA_FUNC, item->field->type->u.function.firstFindLine, msg);
                }
                item = item->nextHash;
            }
        }
    }
}