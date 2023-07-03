#!/bin/bash
flex lexical.l
bison -v -d syntax.y
flex lexical.l
gcc main.c syntax.tab.c TreeNode.c -lfl -o parser