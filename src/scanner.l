/* $Id$
 *
 * BibTeX Convertor
 * Copyright (C) 2010 by Thomas Dreibholz
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact: dreibh@iem.uni-due.de
 */

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

[aA][rR][tT][iI][cC][lL][eE]                         { return(T_Article); }
[bB][oO][oO][kK]                                     { return(T_Book); }
[iI][nN][cC][oO][lL][lL][eE][cC][tT][iI][oO][nN]     { return(T_InCollection); }
[iI][nN][pP][rR][oO][cC][eE][eE][dD][iI][nN][gG][sS] { return(T_InProceedings); }
[mM][iI][sS][cC]                                     { return(T_Misc); }
[tT][eE][cC][hH][rR][eE][pP][oO][rR][tT]             { return(T_TechReport); }

[a-zA-Z0-9\-\.]+   { yylval.iText = strdup(yytext);
                     return(T_Keyword); }

\"                 { BEGIN STRING;
                     stringBufferPos = 0; }
<STRING>\\n        { stringBuffer[stringBufferPos++] = '\n'; }
<STRING>\\t        { stringBuffer[stringBufferPos++] = '\t'; }
<STRING>\\\"       { stringBuffer[stringBufferPos++] = '\"'; }
<STRING>\"         { stringBuffer[stringBufferPos++] = 0x00;
                     BEGIN 0;
                     yylval.iText = strdup(stringBuffer);
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
