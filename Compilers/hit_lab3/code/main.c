#include "inter.h"
#include "syntax.tab.h"

extern int yylineno;
extern int yyparse();
extern void yyrestart(FILE* f);
extern pNode root;

int syntax_error_flag = 0;
int lexical_error_flag = 0;
int recover_error_flag = 0;

int main(int argc, char **argv)
{
    if(argc <= 1) {
	    yyparse();
        return 1;
    }

    FILE* fr = fopen(argv[1], "r");
    if (!fr) {
        perror(argv[1]);
        return 1;
    }

    FILE* fw = fopen(argv[2], "wt+");
    if (!fw) {
        perror(argv[2]);
        return 1;
    }

    yyrestart(fr); // yyrestart(f)函数是Flex提供的库函数，它可以让Flex将其输入文件的文件指针yyin设置为f
    yylineno = 1;
    lexical_error_flag = 0;
    syntax_error_flag = 0;
    yyparse();

    if (syntax_error_flag == 0 && lexical_error_flag == 0) {
        table = initTable();
        // printTree(root, 1);
        Traversal(root);
        // check 所有的函数声明是否都有定义了
        // checkFuncDefination(table);

        // printTable(table);

        interCodeList = newInterCodeList();
        genInterCodes(root);
        if (!interError) {
            //printInterCode(NULL, interCodeList);
            printInterCode(fw, interCodeList);
        }

        deleteTable(table);

        // delTree(root);
    }
    fclose(fr);
    fclose(fw);
    delNode(&root);

    return 0;
}