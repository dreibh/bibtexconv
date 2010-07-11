%{
#include "y.tab.h"
#include <stdlib.h>

#define returntoken(token) return(token);


%}

%%

"@"                { returntoken(T_AT); }
"{"                { puts("open"); returntoken(T_OpeningBrace); }
"}"                { returntoken(T_ClosingBrace); }
","                { puts("comma"); returntoken(T_Comma); }
"\""               { returntoken(T_Quote); }
"article"          { returntoken(T_Article); }
"inproceedings"    { returntoken(T_Proceedings); }
"book"             { returntoken(T_Book); }
"techreport"       { returntoken(T_TechReport); }

[a-zA-Z0-9\-\.]+        { printf("KEY=<%s>\n",yytext); returntoken(T_Keyword); }

%%

int yywrap()
{
   return(1);
}

void yyerror(char* errorText)
{
   printf("ERROR in line %d: %s\n", yylineno, errorText);
   exit(1);
}

int main(int argc, char** argv)
{
   yyparse();
   return 0;
}