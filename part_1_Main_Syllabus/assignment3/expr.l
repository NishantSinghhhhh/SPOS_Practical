%{
#include "expr.tab.h"
int yywrap(void){ return 1; }
%}
%%
[ \t\r]+                   ;
[0-9]+                     { return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]*     { return ID; }
"+"                        { return '+'; }
"-"                        { return '-'; }
"*"                        { return '*'; }
"/"                        { return '/'; }
"^"                        { return '^'; }
"("                        { return '('; }
")"                        { return ')'; }
\n                         { return '\n'; }
.                          { return yytext[0]; }
%%
