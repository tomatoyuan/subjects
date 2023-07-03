#include "semantic.h"
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
    int i;

    /* read from files */
    if (argc > 1) {
        for (i = 1; i < argc; i++) {
            // printf("From file(%s):\n", argv[i]);

            FILE *f = fopen(argv[i], "r");
            if (!f) {
                perror(argv[i]);
                return 1; // 文件指针不空，说明正确打开，继续读
            }
            yyrestart(f); // yyrestart(f)函数是Flex提供的库函数，它可以让Flex将其输入文件的文件指针yyin设置为f
            yylineno = 1;
            lexical_error_flag = 0;
            syntax_error_flag = 0;
            yyparse();

            if (syntax_error_flag == 0 && lexical_error_flag == 0) {
                table = initTable();
                // printTree(root, 1);
                Traversal(root);
                // check 所有的函数声明是否都有定义了
                checkFuncDefination(table);
                deleteTable(table);
            }
            fclose(f);
            delTree(root);
        }
    }

    return 0;
}