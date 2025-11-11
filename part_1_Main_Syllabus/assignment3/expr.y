%{
#include <stdio.h>
#include <stdlib.h>
int yylex(void);
void yyerror(const char *s){ /* keep quiet */ }
%}
%token NUMBER ID
%left '+' '-'
%left '*' '/'
%right '^'
%right UMINUS
%define parse.error verbose
%%
input
    : /* empty */
    | input line
    ;
line
    : expr '\n'           { printf("Valid\n"); }
    | '\n'
    | error '\n'          { printf("Invalid\n"); yyerrok; }
    ;
expr
    : expr '+' expr
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | expr '^' expr
    | '(' expr ')'
    | '-' expr %prec UMINUS
    | NUMBER
    | ID
    ;
%%
int main(void){ return yyparse(); }
