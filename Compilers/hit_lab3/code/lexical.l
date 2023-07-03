    // ************* definitions part ******************
    // 定义方式: name definition
 // 增加该句不会报 undefined reference to `yywrap'
%option noyywrap
%option yylineno

%{
    // %{ %} :的这部分代码，会直接拷贝到 lex.yy.c 的最前面，可以放库引用、宏定义和一些变量定义
    #include <stdio.h>
    #include "TreeNode.h"
    #include "syntax.tab.h"

    extern int lexical_error_flag;  //词法错误标志
    int yycolumn = 1;
    #define YY_USER_ACTION \
    			yylloc.first_line = yylloc.last_line = yylineno; \
    			yylloc.first_column = yycolumn; \
    			yylloc.last_column = yycolumn + yyleng - 1; \
    			yycolumn += yyleng;

    int KeyWord(int TOKEN_TYPE, int token_type, char *name, char *val);
    int Operator(int TOKEN_TYPE, int token_type, char *name, char *val);
    int Border(int TOKEN_TYPE, int token_type, char *name, char *val);
    int identifier(int TOKEN_TYPE, int token_type, char *name, char *val);
    int Constant(int TOKEN_TYPE, int token_type, char *name, char *val);
    void illegalNumErrorA(char *type);
    void printErrorA(void);

%}

digit       [0-9]
letter      [_a-zA-Z]
 /* A sequence of digits without spaces */
INT         0|([1-9][0-9]*)
DECERROR    [0-9]+[a-wA-Wy-zY-Z]+[0-9a-dA-Df-zF-Z]*|[0-9]+[0-9]+[a-dA-Df-zF-Z]+[0-9]*[0-9a-dA-Df-zF-Z]*
OCT         0[0-7]+
OCTERROR    0[0-7]*[8-9]+[0-9]*
HEX         0[xX][0-9a-fA-F]+
HEXERROR    0[xX][0-9a-fA-F]*[g-zG-Z]+[0-9a-zA-Z]*
 /* A real number consisting of digits and one decimal point. The decimal
    point must be surrounded by at least one digit */
FLOAT       [0-9]+\.[0-9]+([eE][+-]?[0-9]+)?|[0-9]+[eE][+-]?[0-9]+|\.[0-9]+|[0-9]+\.|[eE][+-]?[0-9]+|[.][0-9]+[Ee][+-]?[0-9]+|[0-9]+\.[Ee][+-]?[0-9]+
FLOATERROR  \.[eE][+-]?[0-9]+|[0-9]*\.[0-9]+[eE][+-]?|[0-9]+\.[0-9]*[eE][+-]?|[0-9]+[Ee][+-]?|\.[eE][+-]?
 /* A character string consisting of 52 upper- or lower-case alphabetic, 10 numeric
    and one underscore characters. Besides, an identifier must not start with a digit */
Identifier  {letter}({letter}|{digit})*
RELOP       >|<|>=|<=|==|!=
TAB         [\t]
ENTER       [\n\r]
%%
    // ************* rules part ************************
    // 定义方法：pattern {action}，即：正则表达 {响应函数}
    // atoi() 把一个字符串表示的整数转化为int类型

    // Flex将按照这部分给出的内容依次尝试每一个规则，尽可能匹配最长的输入串。
    // 如果有些内容不匹配任何规则，Flex默认只将其拷贝到标准输出。
    // 想要修改这个默认行为的话只需要在所有规则的最后加上一条“.”（即匹配任何输入）规则，
    // 然后在其对应的action部分书写你想要的行为即可。

    // 变量yytext的类型为char*，它是Flex为我们提供的一个变量，里面保存了当前词法单元所对应的词素。

    // 关键的字的匹配要放在前面，否则会报错
int         { return KeyWord(TOKEN_TYPE, TYPE, "TYPE", yytext); }
float       { return KeyWord(TOKEN_TYPE, TYPE, "TYPE", yytext); }
struct      { return KeyWord(TOKEN_OTHER, STRUCT, "STRUCT", yytext); }
return      { return KeyWord(TOKEN_OTHER, RETURN, "RETURN", yytext); }
if          { return KeyWord(TOKEN_OTHER, IF, "IF", yytext); }
else        { return KeyWord(TOKEN_OTHER, ELSE, "ELSE", yytext); }
while       { return KeyWord(TOKEN_OTHER, WHILE, "WHILE", yytext); }

"="         { return Operator(TOKEN_OTHER, ASSIGNOP, "ASSIGNOP", yytext); }
{RELOP}     { return Operator(TOKEN_OTHER, RELOP, "RELOP", yytext); }
"+"         { return Operator(TOKEN_OTHER, PLUS, "PLUS", yytext); }
"-"         { return Operator(TOKEN_OTHER, MINUS, "MINUS", yytext); }
"*"         { return Operator(TOKEN_OTHER, STAR, "STAR", yytext); }
"/"         { return Operator(TOKEN_OTHER, DIV, "DIV", yytext); }
"&&"        { return Operator(TOKEN_OTHER, AND, "AND", yytext); }
"||"        { return Operator(TOKEN_OTHER, OR, "OR", yytext); }
"."         { return Operator(TOKEN_OTHER, DOT, "DOT", yytext); }
"!"         { return Operator(TOKEN_OTHER, NOT, "NOT", yytext); }

";"         { return Border(TOKEN_OTHER, SEMI, "SEMI", yytext); }
","         { return Border(TOKEN_OTHER, COMMA, "COMMA", yytext); }
"("         { return Border(TOKEN_OTHER, LP, "LP", yytext); }
")"         { return Border(TOKEN_OTHER, RP, "RP", yytext); }
"["         { return Border(TOKEN_OTHER, LB, "LB", yytext); }
"]"         { return Border(TOKEN_OTHER, RB, "RB", yytext); }
"{"         { return Border(TOKEN_OTHER, LC, "LC", yytext); }
"}"         { return Border(TOKEN_OTHER, RC, "RC", yytext); }
"\""        {  }
"'"         {  }
"#"         {  }
" "         {  }
{TAB}       { yycolumn += 3; }
{ENTER}     { yycolumn = 1; }

{OCT}       { return Constant(TOKEN_OCT, INT, "INT", yytext); }
{INT}       { return Constant(TOKEN_INT, INT, "INT", yytext); }
{HEX}       { return Constant(TOKEN_HEX, INT, "INT", yytext); }
{FLOAT}     { return Constant(TOKEN_FLOAT, FLOAT, "FLOAT", yytext); }
{Identifier} { return identifier(TOKEN_ID, ID, "ID", yytext); }

{OCTERROR}  { illegalNumErrorA("octal"); }
{DECERROR}  { illegalNumErrorA("decimal"); }
{HEXERROR}  { illegalNumErrorA("hexadecimal"); }
{FLOATERROR} { illegalNumErrorA("floating point"); }

"//"        { while(input() != '\n'); }
"/*"        {
                char ch1 = input();
                char ch2 = input();
                while (ch1 != '*' || ch2 != '/') {
                    ch1 = ch2;
                    ch2 = input();
                    if (ch2 == '\0') {
                        printf("Error type B at Line %d: Missing */.\n",yylineno);
                        break;
                    }
                }
            }
.   { printErrorA(); }

%%
    // ************* user subroutines ******************
    // 用户自定义代码部分： 这部分代码会被原封不动地拷贝到lex.yy.c中，以方便用户自定义所需要执行的函数。

// 1. 关键字：程序内置的关键字，如int、main等
int KeyWord(int TOKEN_TYPE, int token_type, char *name, char *val)
{
    yylval = createNode(yylineno, TOKEN_TYPE, name, val, 0);
    return token_type;
}

// 2. 运算符：+-/*等
int Operator(int TOKEN_TYPE, int token_type, char *name, char *val)
{
    yylval = createNode(yylineno, TOKEN_TYPE, name, val, 0);
    return token_type;
}

// 3. 界符：逗号，分号，、 等
int Border(int TOKEN_TYPE, int token_type, char *name, char *val)
{
    yylval = createNode(yylineno, TOKEN_TYPE, name, val, 0);
    return token_type;
}

// 4. 标识符：自己定义的变量名、函数名
int identifier(int TOKEN_TYPE, int token_type, char *name, char *val)
{
    yylval = createNode(yylineno, TOKEN_TYPE, name, val, 0);
    return token_type;
}

// 5. 常数：整数、浮点数、科学计数、字符串、布尔型
int Constant(int TOKEN_TYPE, int token_type, char *name, char *val)
{
    yylval = createNode(yylineno, TOKEN_TYPE, name, val, 0);
    return token_type;
}

void illegalNumErrorA(char *type)
{
    printf("Error type A at Line %d Column %d: Illegal %s number \'%s\'\n", yylineno, yycolumn, type, yytext);
    yylval = NULL;
    lexical_error_flag = 1;
    return ;
}

void printErrorA(void)
{
    printf("Error type A at Line %d Column %d: Mysterious characters \'%s\'\n", yylineno, yycolumn, yytext);
    lexical_error_flag = 1;
    return ;
}