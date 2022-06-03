/*
 * BibTeX Converter
 * Copyright (C) 2010-2022 by Thomas Dreibholz
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
#include <stdlib.h>
#include <string>

#include "grammar.hh"

std::string string;
std::string comment;
int         level;
%}

%option yylineno
%option nounput

%x STRING
%x COMMENT


%%


 /* ====== Basic tokens ================================================== */
"@"                                                  { return(T_AT); }
"{"                                                  { return(T_OpeningBrace); }
"}"                                                  { return(T_ClosingBrace); }
","                                                  { return(T_Comma); }
"="                                                  { return(T_Equals); }


 /* ====== Quoted strings ================================================ */
\"                                                   { BEGIN STRING; string = ""; level = 0; }
<STRING>\\n                                          { string += '\n';   }
<STRING>\\\"                                         { string += "\\\""; }
<STRING>\n                                           { string += '\n';   }
<STRING>\{                                           { string += '{'; level++; }
<STRING>\}                                           { string += '}'; level--; }
<STRING>\"                                           { if(level > 0) {
                                                          string += "\"";
                                                       }
                                                       else {
                                                          BEGIN 0;
                                                          yylval.iText = strdup(string.c_str());
                                                          // printf("S1=<%s> l=%d\n",yylval.iText, yylineno);
                                                          return(T_String);
                                                       }
                                                     }
<STRING>.                                            { string += *yytext; };


 /* ====== String in parentheses { this is an example } ================== */
"{"[^\n]+"}"                                         { yylval.iText = strndup((const char*)&yytext[1], strlen(yytext) - 2);
                                                       // printf("S2=<%s> l=%d\n",yylval.iText, yylineno);
                                                       return(T_String); }


 /* ====== Comment ======================================================= */
"%"|"\\%"                                            { BEGIN COMMENT; comment = ""; }
<COMMENT>\n                                          { BEGIN 0;
                                                       ++yylineno;
                                                       yylval.iText = strdup(comment.c_str());
                                                       // printf("C=<%s> l=%d\n",yylval.iText, yylineno);
                                                       return(T_Comment); }
<COMMENT>.                                           { comment += *yytext; }


 /* ====== Keywords ====================================================== */
[aA][rR][tT][iI][cC][lL][eE]                         { return(T_Article);       }
[bB][oO][oO][kK]                                     { return(T_Book);          }
[bB][oO][oO][kK][lL][eE][tT]                         { return(T_Booklet);       }
[cC][oO][nN][fF][eE][rR][eE][nN][cC][eE]             { return(T_InProceedings); }
[iI][nN][bB][oO][oO][kK]                             { return(T_InBook);        }
[iI][nN][cC][oO][lL][lL][eE][cC][tT][iI][oO][nN]     { return(T_InCollection);  }
[iI][nN][pP][rR][oO][cC][eE][eE][dD][iI][nN][gG][sS] { return(T_InProceedings); }
[mM][aA][nN][uU][aA][lL]                             { return(T_Manual);        }
[mM][aA][sS][tT][eE][rR][sS][tT][hH][eE][sS][iI][sS] { return(T_MastersThesis); }
[mM][iI][sS][cC]                                     { return(T_Misc);          }
[tT][eE][cC][hH][rR][eE][pP][oO][rR][tT]             { return(T_TechReport);    }
[pP][hH][dD][tT][hH][eE][sS][iI][sS]                 { return(T_PhDThesis);     }
[pP][rR][oO][cC][eE][eE][dD][iI][nN][gG][sS]         { return(T_Proceedings);   }
[uU][nN][pP][uU][bB][lL][iI][sS][hH][eE][dD]         { return(T_Unpublished);   }

[a-zA-Z0-9\-\.\+\:\_]+                               { yylval.iText = strdup(yytext);
                                                       // printf("K=<%s> l=%d\n",yylval.iText, yylineno);
                                                       return(T_Keyword); }


 /* ====== Miscellaneous ==================================================== */
" "                                                  { }
"\t"                                                 { }
"\r"                                                 { }
"\n"                                                 { }

%%


// ###### Line wrap #########################################################
int yywrap()
{
   return(1);
}


// ###### Print error #######################################################
void yyerror(const char* errorText)
{
   fprintf(stderr, "ERROR in line %d: %s\n", yylineno, errorText);
}
