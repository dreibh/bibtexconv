/*
 * ==========================================================================
 *                ____  _ _   _____   __  ______
 *                | __ )(_) |_|_   _|__\ \/ / ___|___  _ ____   __
 *                |  _ \| | '_ \| |/ _ \  / |   / _ \| '_ \ \ / /
 *                | |_) | | |_) | |  __//  \ |__| (_) | | | \ V /
 *                |____/|_|_.__/|_|\___/_/\_\____\___/|_| |_|\_/
 *
 *                          ---  BibTeX Converter  ---
 *                   https://www.nntb.no/~dreibh/bibtexconv/
 * ==========================================================================
 *
 * BibTeX Converter
 * Copyright (C) 2010-2026 by Thomas Dreibholz
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact: thomas.dreibholz@gmail.com
 */

%{
#include <stdlib.h>
#include <string>

#include "grammar.hh"

std::string string;
std::string comment;
int         level;
%}

%option 8bit
%option yylineno
%option nounput

%x QUOTED_STRING
%x BRACED_STRING
%x COMMENT

LATIN    [a-zA-Z]
EURO     (\xc3[\x80-\xbf]|[\xc4\xc5][\x80-\xbf])
CYRILLIC (\xd0[\x80-\xbf]|\xd1[\x80-\xbf])
GREEK    (\xce[\x80-\xbf]|\xcf[\x80-\xbf])
CJK      ([\xe3-\xe9][\x80-\xbf][\x80-\xbf])


%%

 /* ====== Basic tokens ================================================================== */
"@"                     { return T_AT;           }
"{"                     { return T_OpeningBrace; }
"}"                     { return T_ClosingBrace; }
","                     { return T_Comma;        }


 /* ====== Assignment of quoted string =================================================== */
=[ \t]*\"               {
                           BEGIN QUOTED_STRING;
                           string = "";
                           level = 0;
                        }
<QUOTED_STRING>\\\"     { string += "\\\"";       }
<QUOTED_STRING>\{       { string += '{'; level++; }
<QUOTED_STRING>\}       { string += '}'; level--; }
<QUOTED_STRING>\"       {
                           if(level > 0) {
                              string += "\"";
                           }
                           else {
                              BEGIN 0;
                              yylval.iText = strdup(string.c_str());
                              // printf("SQ=<%s> l=%d\n",yylval.iText, yylineno);
                              return T_String;
                           }
                        }
<QUOTED_STRING>\n       { string += "\n";         }
<QUOTED_STRING>.        { string += *yytext; }


 /* ====== Assignment of braced string =================================================== */
=[ \t]*\{               {
                           BEGIN BRACED_STRING;
                           string = "";
                           level = 1;
                        }
<BRACED_STRING>\{       { string += '{'; level++; }
<BRACED_STRING>\}       {
                           level--;
                           if(level > 0) {
                              string += '}';
                           }
                           else {
                              BEGIN 0;
                              yylval.iText = strdup(string.c_str());
                              // printf("SB=<%s> l=%d\n",yylval.iText, yylineno);
                              return T_String;
                           }
                        }
<BRACED_STRING>\n       { string += "\n";         }
<BRACED_STRING>.        { string += *yytext;      }


 /* ====== Assignment of keyword ========================================================= */
=[ \t]*({LATIN}|[0-9\-\.\+\:\_])+ {
                           const char* p = strchr(yytext, '=') + 1;
                           while(isspace(*p)) { p++; }
                           yylval.iText = strdup(p);
                           // printf("SK=<%s> l=%d\n",yylval.iText, yylineno);
                           return T_String;
                        }



 /* ====== Comment ======================================================================= */
"%"|"\\%"               { BEGIN COMMENT; comment = ""; }
<COMMENT>\n             {
                           BEGIN 0;
                           ++yylineno;
                           yylval.iText = strdup(comment.c_str());
                           // printf("C=<%s> l=%d\n",yylval.iText, yylineno);
                           return T_Comment;
                        }
<COMMENT>.              { comment += *yytext; }


 /* ====== Keywords ====================================================================== */
[aA][rR][tT][iI][cC][lL][eE]                                 { return T_Article;         }
[bB][oO][oO][kK]                                             { return T_Book;            }
[bB][oO][oO][kK][lL][eE][tT]                                 { return T_Booklet;         }
[dD][aA][tT][aA][sS][eE][tT]                                 { return T_Dataset;         }
[dD][aA][tT][aA]                                             { return T_Data;            }
[cC][oO][nN][fF][eE][rR][eE][nN][cC][eE]                     { return T_InProceedings;   }
[iI][nN][bB][oO][oO][kK]                                     { return T_InBook;          }
[iI][nN][cC][oO][lL][lL][eE][cC][tT][iI][oO][nN]             { return T_InCollection;    }
[iI][nN][pP][rR][oO][cC][eE][eE][dD][iI][nN][gG][sS]         { return T_InProceedings;   }
[mM][aA][nN][uU][aA][lL]                                     { return T_Manual;          }
[mM][aA][sS][tT][eE][rR][sS][tT][hH][eE][sS][iI][sS]         { return T_MastersThesis;   }
[mM][iI][sS][cC]                                             { return T_Misc;            }
[oO][nN][lL][iI][nN][eE]                                     { return T_Online;          }
[tT][eE][cC][hH][rR][eE][pP][oO][rR][tT]                     { return T_TechReport;      }
[pP][hH][dD][tT][hH][eE][sS][iI][sS]                         { return T_PhDThesis;       }
[pP][rR][oO][cC][eE][eE][dD][iI][nN][gG][sS]                 { return T_Proceedings;     }
[uU][nN][pP][uU][bB][lL][iI][sS][hH][eE][dD]                 { return T_Unpublished;     }

[sS][oO][fF][tT][wW][aA][rR][eE][mM][oO][dD][uU][lL][eE]     { return T_SoftwareModule;  }
[sS][oO][fF][tT][wW][aA][rR][eE][vV][eE][rR][sS][iI][oO][nN] { return T_SoftwareVersion; }
[sS][oO][fF][tT][wW][aA][rR][eE]                             { return T_Software;        }
[cC][oO][dD][eE][fF][rR][aA][gG][mM][eE][nN][tT]             { return T_CodeFragment;    }

({LATIN}|{EURO}|{CYRILLIC}|{GREEK}|{CJK}|[0-9\-\.\+\:\_])+ {
   yylval.iText = strdup(yytext);
   // printf("K=<%s> l=%d\n",yylval.iText, yylineno);
   return T_Keyword;
}


 /* ====== Miscellaneous ==================================================== */
[ \t\r\n]   { }

%%


// ###### Line wrap #########################################################
int yywrap()
{
   return 1;
}


// ###### Print error #######################################################
void yyerror(const char* errorText)
{
   fprintf(stderr, "ERROR in line %d: %s\n", yylineno, errorText);
}
