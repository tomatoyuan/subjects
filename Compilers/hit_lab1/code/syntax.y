%{
    #define YYSTYPE TreeNode* // 定义 yylval 的类型为 TreeNode*

    #include "TreeNode.h"
    #include "lex.yy.c"

    void yyerror(const char *);
    void printRecovering(int line, int column, const char *msg);

    int error_column;
    extern int syntax_error;   // 语法错误标志
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
Program : ExtDefList { $$ = createNode(@$.first_line, non_terminal, "Program", 1, $1); root = $$; }
    ;
ExtDefList : ExtDef ExtDefList { $$ = createNode(@$.first_line, non_terminal, "ExtDefList", 2, $1, $2); }
    | { $$ = NULL; } // epsilon
    ;
         // 全局变量，例如“int global1, global2;”
ExtDef : Specifier ExtDecList SEMI { $$ = createNode(@$.first_line, non_terminal, "ExtDef", 3, $1, $2, $3); }
    // 专门为结构体准备的
    | Specifier SEMI { $$ = createNode(@$.first_line, non_terminal, "ExtDef", 2, $1, $2); }
    // 函数
    | Specifier FunDec CompSt { $$ = createNode(@$.first_line, non_terminal, "ExtDef", 3, $1, $2, $3); }
    | error SEMI { syntax_error = MissSEMI; if(!lexical_error) printRecovering(@$.first_line, @$.last_column, "Missing2 \";\""); } // Missing ";"
    | Specifier error SEMI { syntax_error = MissSEMI; if(!lexical_error) printRecovering(@$.first_line, @$.last_column, "Missing3 \";\""); } // Missing ";"
    | error Specifier SEMI { syntax_error = NormalSyntaxError; }
    ;
ExtDecList : VarDec { $$ = createNode(@$.first_line, non_terminal, "ExtDecList", 1, $1); }
    | VarDec COMMA ExtDecList { $$ = createNode(@$.first_line, non_terminal, "ExtDecList", 3, $1, $2, $3); }
    | VarDec error ExtDefList { syntax_error = NormalSyntaxError; }
    ;

// Specifiers ：与变量的类型有关
Specifier : TYPE { $$ = createNode(@$.first_line, non_terminal, "Specifier", 1, $1); }
    // | error TYPE { printf("type error\n"); }
    // 结构体
    | StructSpecifier { $$ = createNode(@$.first_line, non_terminal, "Specifier", 1, $1); }
    ;
StructSpecifier : STRUCT OptTag LC DefList RC { $$ = createNode(@$.first_line, non_terminal, "StructSpecifier", 5, $1, $2, $3, $4, $5); }
    // 之前已经定义过某个结构体,直接用来定义变量 truct Complex a, b;
    | STRUCT Tag { $$ = createNode(@$.first_line, non_terminal, "StructSpecifier", 2, $1, $2); }
    ;
OptTag : ID { $$ = createNode(@$.first_line, non_terminal, "OptTag", 1, $1); } // 结构体名
    | { $$ = NULL; } // epsilon 结构体可以匿名
    ;
Tag : ID { $$ = createNode(@$.first_line, non_terminal, "Tag", 1, $1); }
    ;

// Declarators ：与变量和函数的定义有关
VarDec : ID { $$ = createNode(@$.first_line, non_terminal, "VarDec", 1, $1); } // 可以是变量名
    // 可以是数组
    | VarDec LB INT RB { $$ = createNode(@$.first_line, non_terminal, "VarDec", 4, $1, $2, $3, $4); }
    | VarDec LB INT error RB { syntax_error = MissRB; if(!lexical_error) printRecovering(@$.first_line, @$.last_column, "Missing4.0 \"]\""); } // Missing "]"
    | VarDec LB error RB { syntax_error = MissRB; if(!lexical_error) printRecovering(@$.first_line, @$.last_column, "Missing4 \";\""); } // Missing "]"
    | error RB { syntax_error = MissRB; if(!lexical_error) printRecovering(@$.first_line, @$.last_column, "Missing5 \";\""); } // Missing "]"
    | error LB INT RB { syntax_error = NormalSyntaxError; }
    ;
    // 传入参数列表的函数
FunDec : ID LP VarList RP { $$ = createNode(@$.first_line, non_terminal, "FunDec", 4, $1, $2, $3, $4); }
    // 不传入参数的函数
    | ID LP RP { $$ = createNode(@$.first_line, non_terminal, "FunDec", 3, $1, $2, $3); }
    | ID LP error RP { syntax_error = MissRP; if(!lexical_error) printRecovering(@$.first_line, @$.last_column, "Missing6 \";\""); } // Missing ")"
    | error LP VarList RP { syntax_error = NormalSyntaxError; }
    ;
VarList : ParamDec COMMA VarList { $$ = createNode(@$.first_line, non_terminal, "VarList", 3, $1, $2, $3); }
    // 形参定义
    | ParamDec { $$ = createNode(@$.first_line, non_terminal, "VarList", 1, $1); }
    ;
ParamDec : Specifier VarDec { $$ = createNode(@$.first_line, non_terminal, "ParamDec", 2, $1, $2); }
    ;

// Statements ：与语句有关
CompSt : LC DefList StmtList RC { $$ = createNode(@$.first_line, non_terminal, "CompSt", 4, $1, $2, $3, $4); } // { 语句1; 语句2; }
    | error RC { syntax_error = MissRC;  if(!lexical_error) printRecovering(@$.first_line, @$.last_column, "Missing7 \"}\""); } // Missing "}"
    ;
StmtList : Stmt StmtList { $$ = createNode(@$.first_line, non_terminal, "StmtList", 2, $1, $2); } // 0或多条语句
    | { $$ = NULL; } // epsilon
    ;
Stmt : Exp SEMI { $$ = createNode(@$.first_line, non_terminal, "Stmt", 2, $1, $2); } // 一个表达式
    | CompSt { $$ = createNode(@$.first_line, non_terminal, "Stmt", 1, $1); } // 一个语句块

    | RETURN Exp SEMI { $$ = createNode(@$.first_line, non_terminal, "Stmt", 3, $1, $2, $3); } // 一条返回语句

    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE   { $$ = createNode(@$.first_line, non_terminal, "Stmt", 5, $1, $2, $3, $4, $5); } // if 语句
    | IF LP Exp RP Stmt ELSE Stmt { $$ = createNode(@$.first_line, non_terminal, "Stmt", 7, $1, $2, $3, $4, $5, $6, $7); } // if else 语句
    | WHILE LP Exp RP Stmt { $$ = createNode(@$.first_line, non_terminal, "Stmt", 5, $1, $2, $3, $4, $5); } // while(Exp)

    | error SEMI { syntax_error = MissSEMI; if(!lexical_error) printRecovering(@$.first_line, @$.last_column, "Missing8 \";\""); } // Missing ";"
    | Exp error SEMI { syntax_error = MissSEMI; if(!lexical_error) printRecovering(@$.first_line, @$.last_column, "Missing9 \";\""); } // Missing ";"
    | RETURN Exp error SEMI { syntax_error = MissSEMI; if(!lexical_error) printRecovering(@$.first_line, @$.last_column, "Missing10 \";\""); } // Missing ";"
    | RETURN error SEMI  { syntax_error = MissSEMI; if(!lexical_error) printRecovering(@$.first_line, @$.last_column, "Missing11 \";\""); } // Missing ";"

    ;

// Local Definitions
DefList : Def DefList { $$ = createNode(@$.first_line, non_terminal, "DefList", 2, $1, $2); } // 定义参数列表
    | { $$ = NULL; } // epsilon
    // | error Def DefList { printf("eroor!\n"); }
    ;
Def : Specifier DecList SEMI { $$ = createNode(@$.first_line, non_terminal, "Def", 3, $1, $2, $3); }
    | Specifier error SEMI {syntax_error = MissSEMI; if(!lexical_error) if(!lexical_error) printRecovering(@$.first_line, @$.last_column, "Missing12 \";\""); } // Missing ";"
    | Specifier DecList error {syntax_error = MissSEMI; if(!lexical_error) if(!lexical_error) printRecovering(@$.first_line, @$.last_column, "Missing13 \";\""); } // Missing ";"
    // | error Specifier DecList SEMI {syntax_error = MissSEMI; if(!lexical_error) if(!lexical_error) printRecovering(@$.first_line, @$.last_column, "Missing12.0 \";\""); } // Missing ";"
    ;
DecList : Dec { $$ = createNode(@$.first_line, non_terminal, "DecList", 1, $1); }
    | Dec COMMA DecList { $$ = createNode(@$.first_line, non_terminal, "DecList", 3, $1, $2, $3); }
    ;
Dec : VarDec { $$ = createNode(@$.first_line, non_terminal, "Dec", 1, $1); }
    | VarDec ASSIGNOP Exp { $$ = createNode(@$.first_line, non_terminal, "Dec", 3, $1, $2, $3); } // 可以给局部变量初始化
    ;

// Expressions
Exp : Exp ASSIGNOP Exp { $$ = createNode(@$.first_line, non_terminal, "Exp", 3, $1, $2, $3); }
    | Exp AND Exp   { $$ = createNode(@$.first_line, non_terminal, "Exp", 3, $1, $2, $3); }
    | Exp OR Exp    { $$ = createNode(@$.first_line, non_terminal, "Exp", 3, $1, $2, $3); }
    | Exp RELOP Exp { $$ = createNode(@$.first_line, non_terminal, "Exp", 3, $1, $2, $3); }
    | Exp PLUS Exp  { $$ = createNode(@$.first_line, non_terminal, "Exp", 3, $1, $2, $3); }
    | Exp MINUS Exp { $$ = createNode(@$.first_line, non_terminal, "Exp", 3, $1, $2, $3); }
    | Exp STAR Exp  { $$ = createNode(@$.first_line, non_terminal, "Exp", 3, $1, $2, $3); }
    | Exp DIV Exp   { $$ = createNode(@$.first_line, non_terminal, "Exp", 3, $1, $2, $3); }
    | LP Exp RP { $$ = createNode(@$.first_line, non_terminal, "Exp", 3, $1, $2, $3); }
    | MINUS Exp { $$ = createNode(@$.first_line, non_terminal, "Exp", 2, $1, $2); }
    | NOT Exp   { $$ = createNode(@$.first_line, non_terminal, "Exp", 2, $1, $2); }
    // 函数表达式 Func(Args)
    | ID LP Args RP { $$ = createNode(@$.first_line, non_terminal, "Exp", 4, $1, $2, $3, $4); }
    | ID LP RP  { $$ = createNode(@$.first_line, non_terminal, "Exp", 3, $1, $2, $3); }
    | Exp LB Exp RB { $$ = createNode(@$.first_line, non_terminal, "Exp", 4, $1, $2, $3, $4); }
    | Exp LB Exp error RB { syntax_error = MissRB; if(!lexical_error) printRecovering(@$.first_line, @$.last_column, "Missing14 \"]\""); } //  Missing ']'
    | Exp DOT ID    { $$ = createNode(@$.first_line, non_terminal, "Exp", 3, $1, $2, $3); }
    | ID    { $$ = createNode(@$.first_line, non_terminal, "Exp", 1, $1); }
    | INT   { $$ = createNode(@$.first_line, non_terminal, "Exp", 1, $1); }
    | FLOAT { $$ = createNode(@$.first_line, non_terminal, "Exp", 1, $1); }
    ;
Args : Exp COMMA Args { $$ = createNode(@$.first_line, non_terminal, "Args", 3, $1, $2, $3); }
    | Exp { $$ = createNode(@$.first_line, non_terminal, "Args", 1, $1); }
    ;

%%
// ^_^
void yyerror(const char *msg)
{
    if (lexical_error == 0) {
        fprintf(stderr, "Error type B at Line %d Column %d : Syntax error.\n", yylloc.first_line, yylloc.first_column);
    }

    return ;
}

void printRecovering(int line, int column, const char *msg)
{
    fprintf(stderr, "Error type B at Line %d Column %d : %s\n", yylloc.first_line, yylloc.first_column, msg);
}