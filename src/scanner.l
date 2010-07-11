%{
#include "y.tab.h"
#include <stdlib.h>

char   stringBuffer[1024];
size_t stringBufferPos = 0;
%}

%x STRING

%%

"@"                { return(T_AT); }
"{"                { return(T_OpeningBrace); }
"}"                { return(T_ClosingBrace); }
","                { return(T_Comma); }
"="                { return(T_Equals); }
"article"          { return(T_Article); }
"inproceedings"    { return(T_Proceedings); }
"book"             { return(T_Book); }
"techreport"       { return(T_TechReport); }

[a-zA-Z0-9\-\.]+   { return(T_Keyword); }

\"                 { BEGIN STRING;
                     stringBufferPos = 0; }
<STRING>\\n        { stringBuffer[stringBufferPos++] = '\n'; }
<STRING>\\t        { stringBuffer[stringBufferPos++] = '\t'; }
<STRING>\\\"       { stringBuffer[stringBufferPos++] = '\"'; }
<STRING>\"         { stringBuffer[stringBufferPos++] = 0x00;
                     BEGIN 0;
                     yylval = strdup(stringBuffer);
                     printf("INFO: <%s>\n", stringBuffer);
                     return(T_String); }
<STRING>\n         { }
<STRING>.          { stringBuffer[stringBufferPos++] = *yytext; };

" "                { }
"\t"               { }
"\n"               { ++yylineno; }

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