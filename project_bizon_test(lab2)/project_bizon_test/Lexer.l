%{
#include <stdlib.h> 
#include <string.h> 
#include "ast.h"
#include "Parser.tab.h"
%}

%%
-?[0-9]+ { yylval.ival = atoi(yytext); return INTVAL; }
true        { yylval.ival = 1; return BOOLVAL; }
false       { yylval.ival = 0; return BOOLVAL; }
-?[0-9]+"."[0-9]* { yylval.fval = atof(yytext); return DOUBLEVAL; }
'(\\.|''|[^'\n])*' { yylval.sval = _strdup(yytext); return STRINGVAL; }

table_num {return TABLENUM;}
raw_size {return RAWSIZE;}
table {return TABLE;}
into {return INTO;}
with {return WITH;}
for {return FOR;}
in {return IN;}
join {return JOIN;}
to {return TO;}
delete {return DELETE;}
on {return ON;}

\( {return OPENBR_CIR;}
\) {return CLOSEBR_CIR;}
\{ {return OPENBR_FIG;}
\} {return CLOSEBR_FIG;}
\; {return DOT_COMA;}
: {return DOUBLE_POINT;}
"." {return DOT;}

"==" {return EQ;}
"!=" {return NEQ;}
">" {return GREATER;}
"<" {return LESSER;}
">=" {return GREATER_EQ;}
"<=" {return LESSER_EQ;}

"," {return COMA;}

"&&" {return AND;}
and {return AND;}
"||" {return OR;}
or {return OR;}


drop {return DROP;}
create {return CREATE;}
insert {return INSERT;}
filter {return FILTER;}
return {return RETURN;}
remove {return REMOVE;}
update {return UPDATE;}
select {return SELECT;}
from {return FROM;}
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.sval = _strdup(yytext); return VARNAME; }
[ \t\r]+ { }
%%

int yywrap() {
    return 1;
}
