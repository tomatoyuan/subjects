%{
    #define YYSTYPE TreeNode* // 定义 yylval 的类型为 TreeNode*

    #include "TreeNode.h"
    #include "lex.yy.c"

    void yyerror(const char *);
    void printRecovering(int line, int column, const char *msg);

    int error_column;
    extern int syntax_error_flag;   // 语法错误标志
    extern int recover_error_flag;  // 错误恢复标志位，防止一个 B 类错误多次恢复
    pNode root;
%}

/* declared tokens */
%token INT FLOAT TYPE
%token ID
%token RELOP // >|<|>=|<=|==|!=
%token ASSIGNOP // =
%token PLUS MINUS STAR DIV // operator
%token AND OR NOT // logical operator
%token DOT COMMA SEMI LP RP LB RB LC RC // punctuation
%token STRUCT RETURN IF ELSE WHILE    // keyword

/* declared non-terminals */
%type Program ExtDefList ExtDef ExtDecList   //  High-level Definitions
%type Specifier StructSpecifier OptTag Tag   //  Specifiers
%type VarDec FunDec VarList ParamDec         //  Declarators
%type CompSt StmtList Stmt                   //  Statements
%type DefList Def Dec DecList                //  Local Definitions
%type Exp Args                               //  Expressions


/* precedence and associativity 优先级和关联性 */
%start Program
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS UMINUS // 负号
%left STAR DIV
%right NOT
%left DOT
%left LB RB
%left LP RP
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

// High-level Definitions
Program : ExtDefList { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Program", NULL, 1, $1); root = $$; }
    ;
ExtDefList : ExtDef ExtDefList { $$ = createNode(@$.first_line, NOT_A_TOKEN, "ExtDefList", NULL, 2, $1, $2); }
    | { $$ = NULL; } // epsilon
    ;
         // 全局变量，例如“int global1, global2;”
ExtDef : Specifier ExtDecList SEMI { $$ = createNode(@$.first_line, NOT_A_TOKEN, "ExtDef", NULL, 3, $1, $2, $3); }
    // 专门为结构体准备的
    | Specifier SEMI { $$ = createNode(@$.first_line, NOT_A_TOKEN, "ExtDef", NULL, 2, $1, $2); }
    // 函数
    // 函数声明
    | Specifier FunDec SEMI { $$ = createNode(@$.first_line, NOT_A_TOKEN, "ExtDef", NULL, 3, $1, $2, $3); }
    // 函数定义
    | Specifier FunDec CompSt { $$ = createNode(@$.first_line, NOT_A_TOKEN, "ExtDef", NULL, 3, $1, $2, $3); }
    | error SEMI { syntax_error_flag = MissSEMI; if(!lexical_error_flag && !recover_error_flag) printRecovering(@$.first_line, @$.last_column, "Missing \";\""); } // Missing ";"
    | Specifier error SEMI { syntax_error_flag = MissSEMI; if(!lexical_error_flag && !recover_error_flag) printRecovering(@$.first_line, @$.last_column, "Missing \";\""); } // Missing ";"
    | error Specifier SEMI { syntax_error_flag = NormalSyntaxError; }
    ;
ExtDecList : VarDec { $$ = createNode(@$.first_line, NOT_A_TOKEN, "ExtDecList", NULL, 1, $1); }
    | VarDec COMMA ExtDecList { $$ = createNode(@$.first_line, NOT_A_TOKEN, "ExtDecList", NULL, 3, $1, $2, $3); }
    | VarDec error ExtDefList { syntax_error_flag = NormalSyntaxError; }
    ;

// Specifiers ：与变量的类型有关
Specifier : TYPE { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Specifier", NULL, 1, $1); }
    // | error TYPE { printf("type error\n"); }
    // 结构体
    | StructSpecifier { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Specifier", NULL, 1, $1); }
    ;
StructSpecifier : STRUCT OptTag LC DefList RC { $$ = createNode(@$.first_line, NOT_A_TOKEN, "StructSpecifier", NULL, 5, $1, $2, $3, $4, $5); }
    // 之前已经定义过某个结构体,直接用来定义变量 truct Complex a, b;
    | STRUCT Tag { $$ = createNode(@$.first_line, NOT_A_TOKEN, "StructSpecifier", NULL, 2, $1, $2); }
    ;
OptTag : ID { $$ = createNode(@$.first_line, NOT_A_TOKEN, "OptTag", NULL, 1, $1); } // 结构体名
    | { $$ = NULL; } // epsilon 结构体可以匿名
    ;
Tag : ID { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Tag", NULL, 1, $1); }
    ;

// Declarators ：与变量和函数的定义有关
VarDec : ID { $$ = createNode(@$.first_line, NOT_A_TOKEN, "VarDec", NULL, 1, $1); } // 可以是变量名
    // 可以是数组
    | VarDec LB INT RB { $$ = createNode(@$.first_line, NOT_A_TOKEN, "VarDec", NULL, 4, $1, $2, $3, $4); }
    | VarDec LB INT error RB { syntax_error_flag = MissRB; if(!lexical_error_flag && !recover_error_flag) printRecovering(@$.first_line, @$.last_column, "Missing \"]\""); } // Missing "]"
    | VarDec LB error RB { syntax_error_flag = MissRB; if(!lexical_error_flag && !recover_error_flag) printRecovering(@$.first_line, @$.last_column, "Missing \"]\""); } // Missing "]"
    | error RB { syntax_error_flag = MissRB; if(!lexical_error_flag && !recover_error_flag) printRecovering(@$.first_line, @$.last_column, "Missing \"]\""); } // Missing "]"
    | error LB INT RB { syntax_error_flag = NormalSyntaxError; }
    ;
    // 传入参数列表的函数
FunDec : ID LP VarList RP { $$ = createNode(@$.first_line, NOT_A_TOKEN, "FunDec", NULL, 4, $1, $2, $3, $4); }
    // 不传入参数的函数
    | ID LP RP { $$ = createNode(@$.first_line, NOT_A_TOKEN, "FunDec", NULL, 3, $1, $2, $3); }
    | ID LP error RP { syntax_error_flag = MissRP; if(!lexical_error_flag && !recover_error_flag) printRecovering(@$.first_line, @$.last_column, "Missing \")\""); } // Missing ")"
    | error LP VarList RP { syntax_error_flag = NormalSyntaxError; }
    ;
VarList : ParamDec COMMA VarList { $$ = createNode(@$.first_line, NOT_A_TOKEN, "VarList", NULL, 3, $1, $2, $3); }
    // 形参定义
    | ParamDec { $$ = createNode(@$.first_line, NOT_A_TOKEN, "VarList", NULL, 1, $1); }
    ;
ParamDec : Specifier VarDec { $$ = createNode(@$.first_line, NOT_A_TOKEN, "ParamDec", NULL, 2, $1, $2); }
    ;

// Statements ：与语句有关
CompSt : LC DefList StmtList RC { $$ = createNode(@$.first_line, NOT_A_TOKEN, "CompSt", NULL, 4, $1, $2, $3, $4); } // { 语句1; 语句2; }
    | error RC { syntax_error_flag = MissRC;  if(!lexical_error_flag && !recover_error_flag) printRecovering(@$.first_line, @$.last_column, "Missing \"}\""); } // Missing "}"
    ;
StmtList : Stmt StmtList { $$ = createNode(@$.first_line, NOT_A_TOKEN, "StmtList", NULL, 2, $1, $2); } // 0或多条语句
    | { $$ = NULL; } // epsilon
    ;
Stmt : Exp SEMI { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Stmt", NULL, 2, $1, $2); } // 一个表达式
    | CompSt { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Stmt", NULL, 1, $1); } // 一个语句块

    | RETURN Exp SEMI { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Stmt", NULL, 3, $1, $2, $3); } // 一条返回语句

    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE   { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Stmt", NULL, 5, $1, $2, $3, $4, $5); } // if 语句
    | IF LP Exp RP Stmt ELSE Stmt { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Stmt", NULL, 7, $1, $2, $3, $4, $5, $6, $7); } // if else 语句
    | WHILE LP Exp RP Stmt { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Stmt", NULL, 5, $1, $2, $3, $4, $5); } // while(Exp)

    | error SEMI { syntax_error_flag = MissSEMI; if(!lexical_error_flag && !recover_error_flag) printRecovering(@$.first_line, @$.last_column, "Missing \";\""); } // Missing ";"
    | Exp error SEMI { syntax_error_flag = MissSEMI; if(!lexical_error_flag && !recover_error_flag) printRecovering(@$.first_line, @$.last_column, "Missing \";\""); } // Missing ";"
    | RETURN Exp error SEMI { syntax_error_flag = MissSEMI; if(!lexical_error_flag && !recover_error_flag) printRecovering(@$.first_line, @$.last_column, "Missing \";\""); } // Missing ";"
    | RETURN error SEMI  { syntax_error_flag = MissSEMI; if(!lexical_error_flag && !recover_error_flag) printRecovering(@$.first_line, @$.last_column, "Missing \";\""); } // Missing ";"

    ;

// Local Definitions
DefList : Def DefList { $$ = createNode(@$.first_line, NOT_A_TOKEN, "DefList", NULL, 2, $1, $2); } // 定义参数列表
    | { $$ = NULL; } // epsilon
    // | error Def DefList { printf("eroor!\n"); }
    ;
Def : Specifier DecList SEMI { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Def", NULL, 3, $1, $2, $3); }
    | Specifier error SEMI {syntax_error_flag = MissSEMI; if(!lexical_error_flag && !recover_error_flag) printRecovering(@$.first_line, @$.last_column, "Missing \";\""); } // Missing ";"
    | Specifier DecList error {syntax_error_flag = MissSEMI; if(!lexical_error_flag && !recover_error_flag) printRecovering(@$.first_line, @$.last_column, "Missing \";\""); } // Missing ";"
    // | error Specifier DecList SEMI {syntax_error_flag = MissSEMI; if(!lexical_error_flag) printRecovering(@$.first_line, @$.last_column, "Missing12.0 \";\""); } // Missing ";"
    ;
DecList : Dec { $$ = createNode(@$.first_line, NOT_A_TOKEN, "DecList", NULL, 1, $1); }
    | Dec COMMA DecList { $$ = createNode(@$.first_line, NOT_A_TOKEN, "DecList", NULL, 3, $1, $2, $3); }
    ;
Dec : VarDec { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Dec", NULL, 1, $1); }
    | VarDec ASSIGNOP Exp { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Dec", NULL, 3, $1, $2, $3); } // 可以给局部变量初始化
    ;

// Expressions
Exp : Exp ASSIGNOP Exp { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Exp", NULL, 3, $1, $2, $3); }
    | Exp AND Exp   { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Exp", NULL, 3, $1, $2, $3); }
    | Exp OR Exp    { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Exp", NULL, 3, $1, $2, $3); }
    | Exp RELOP Exp { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Exp", NULL, 3, $1, $2, $3); }
    | Exp PLUS Exp  { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Exp", NULL, 3, $1, $2, $3); }
    | Exp MINUS Exp { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Exp", NULL, 3, $1, $2, $3); }
    | Exp STAR Exp  { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Exp", NULL, 3, $1, $2, $3); }
    | Exp DIV Exp   { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Exp", NULL, 3, $1, $2, $3); }
    | LP Exp RP { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Exp", NULL, 3, $1, $2, $3); }
    | MINUS Exp { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Exp", NULL, 2, $1, $2); }
    | NOT Exp   { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Exp", NULL, 2, $1, $2); }
    // 函数表达式 Func(Args)
    | ID LP Args RP { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Exp", NULL, 4, $1, $2, $3, $4); }
    | ID LP RP  { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Exp", NULL, 3, $1, $2, $3); }
    | Exp LB Exp RB { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Exp", NULL, 4, $1, $2, $3, $4); }
    | Exp LB Exp error RB { syntax_error_flag = MissRB; if(!lexical_error_flag && !recover_error_flag) printRecovering(@$.first_line, @$.last_column, "Missing \"]\""); } //  Missing ']'
    | Exp DOT ID    { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Exp", NULL, 3, $1, $2, $3); }
    | ID    { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Exp", NULL, 1, $1); }
    | INT   { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Exp", NULL, 1, $1); }
    | FLOAT { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Exp", NULL, 1, $1); }
    ;
Args : Exp COMMA Args { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Args", NULL, 3, $1, $2, $3); }
    | Exp { $$ = createNode(@$.first_line, NOT_A_TOKEN, "Args", NULL, 1, $1); }
    ;

%%
// ^_^ 打印发现语法错误的位置
void yyerror(const char *msg)
{
    if (lexical_error_flag == 0) {
        fprintf(stderr, "Error type B at Line %d Column %d : Syntax error.\n", yylloc.first_line, yylloc.first_column);
        recover_error_flag = 0; // 发现一个 B 类错误，将错误恢复标志为清0，表示可以恢复
    }

    return ;
}

// 打印错误恢复的位置
void printRecovering(int line, int column, const char *msg)
{
    fprintf(stderr, "Recover type B at Line %d Column %d : %s\n", yylloc.first_line, yylloc.first_column, msg);
    recover_error_flag = 1; // 该 B 类已经被恢复了，标志为置为 1，防止重复恢复
}