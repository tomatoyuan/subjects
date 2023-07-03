# 实验一 词法分析与语法分析

## 1. 实验完成情况

完成所有必做内容以及选做所有内容：测试用例 `test1-test10` 均通过

**完成C--语言文法的词法分析：** 

1.   识别所有合法词法单元（包括选做任务的单多行注释）;
2.   识别特定非法词法单元（包括非选做任务的八进制数、十六进制数、科学计数法表示的浮点数，以及错误以数字开头的标识符），以及未在C--文法中定义的词素（如&、！等）。

**完成C--语言文法的所有语法分析：** 

1.   归约所有合法的语法单元，识别特定的非法语法（如分号缺失、大括号缺失）;
2.   识别概括性的特定语法错误（如无效的Extern Definition等）；
3.   最重要的，在if-then-else语句中增加了特定语法以消除二义性；
4.   完成语法分析树的创建;
5.   完成语法树的正确输出。

## 2. 编译方法

首先 `cd code` 到 `code` 文件夹下

<font color=blue size=4>make 方法</font>

编译 `make`

运行样例 `make test`

删除中间文件 `make clean`

<font color=blue size=4>bash 脚本方法</font>

编译：`./compile.sh`

运行所有样例：`./runtest.sh`

删除中间文件：`./fileclean.sh`

## 3. 实验亮点

<font color=blue size=4>1. 编译方法</font>

学习了 `Makefile` 和 `shell` 脚本的基本语法，完成了代码的编译、样例测试、中间文件清理的功能。

<font color=blue size=4>2. 多叉树的创建</font>

在 `syntax.y` 文件中，使用 `<stdarg.h>` 中提供的 `va_list` 类，通过 `va_start` 、 `va_arg` 、 `va_end` 函数能传递可变参数的特性，更方便的实现了不同归约产生式的节点（非终结符或终结符）的传递。更方便的实现了子树某一层的构建。

<font color=blue size=4>3. 对多行注释部分的识别</font>

```c
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
```

如果中间的判据是 `if (ch2 == EOF)` ，那么如下的样例将无法通过：

```c
/* hello world hello world
嘿嘿啊哈哈哈
没有注释结束符哦
```

改用 `if (ch2 == '\0')` 可正常执行。

<font color=blue size=4>4. 区分十进制、八进制、十六进制的具体错误类型</font>

编写十进制、八进制、十六进制整型正确以及错误的正则表达式，利用错误表达式来判断词法任务分析时，关于十进制、八进制、十六进制的可能错误。可以具体输出词法错误单元的类型。

```c
INT         0|([1-9][0-9]*)
DECERROR    [0-9]+[a-wA-Wy-zY-Z]+[0-9a-dA-Df-zF-Z]*|[0-9]+[0-9]+[a-dA-Df-zF-Z]+[0-9]*[0-9a-dA-Df-zF-Z]*
OCT         0[0-7]+
OCTERROR    0[0-7]*[8-9]+[0-9]*
HEX         0[xX][0-9a-fA-F]+
HEXERROR    0[xX][0-9a-fA-F]*[g-zG-Z]+[0-9a-zA-Z]*
```

<font color=blue size=4>5. 将词法分析的正则规则部分按照类型划分，封装为为函数，使代码结果更清晰</font>

```c
// 1. 关键字：程序内置的关键字，如int、main等
int KeyWord(int terminal_type, int token_type, char *name)
{
    yylval = createNode(yylineno, terminal_type, name, 0);
    return token_type;
}

// 2. 运算符：+-/*等
int Operator(int terminal_type, int token_type, char *name) {...}

// 3. 界符：逗号，分号，、 等
int Border(int terminal_type, int token_type, char *name) {...}

// 4. 标识符：自己定义的变量名、函数名
int identifier(int terminal_type, int token_type, char *name) {...}

// 5. 常数：整数、浮点数、科学计数、字符串、布尔型
int Constant(int terminal_type, int token_type, char *name) {...}
```

<font color=blue size=4>6. 错误提示包含行位置和列位置</font>

使用 `flex` 内置的 `yylloc` 和 `yylineno` ，来更新行和列的位置。

注意遇到 `Tab` 键列数要加 3 ，遇到 `Enter` 列数要回归到 0 （遇到一个字母再加1）。