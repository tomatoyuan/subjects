#include "TreeNode.h"
#include "syntax.tab.h"

extern int yylineno;
extern int yyparse();
extern void yyrestart(FILE* f);

int syntax_error = 0;
int lexical_error = 0;
extern pNode root;

int main(int argc, char **argv)
{
    int i;

    /* just read stdin */
    if (argc < 2) {
        printf("From Terminal:\n");
        yyparse();
        if (syntax_error == 0 && lexical_error == 0) {
            printTree(root, 1);
        }
        delTree(root);
        return 0;
    }

    /* read from files */
    for (i = 1; i < argc; i++) {
        printf("From file(%s):\n", argv[i]);

        FILE *f = fopen(argv[i], "r");
        if (!f) {
            perror(argv[i]);
            return 1; // 文件指针不空，说明正确打开，继续读
        }
        yyrestart(f); // yyrestart(f)函数是Flex提供的库函数，它可以让Flex将其输入文件的文件指针yyin设置为f
        yylineno = 1;
        lexical_error = 0;
        syntax_error = 0;
        yyparse();

        if (syntax_error == 0 && lexical_error == 0) {
            printTree(root, 1);
        }
        fclose(f);
        delTree(root);
    }

    return 0;
}