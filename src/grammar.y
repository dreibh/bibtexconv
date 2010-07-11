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
#include "node.h"
#include <stdlib.h>
#include <stdio.h>
%}

%token T_AT
%token T_OpeningBrace
%token T_ClosingBrace
%token T_Comma
%token T_Equals
%token T_Quote

%token T_Article
%token T_Book
%token T_InProceedings
%token T_InCollection
%token T_Misc
%token T_TechReport

%token <iText>  T_Keyword
%token <iText>  T_String
%type <nodePtr> bibTeXFile
%type <nodePtr> publicationCollection
%type <nodePtr> publication
%type <nodePtr> publicationInfo
%type <nodePtr> publicationInfoItem

%union {
   char*        iText;
   struct Node* nodePtr;
}

%%

bibTeXFile
    : publicationCollection { dumpNode($$); freeNode($$); }
    |
    ;

publicationCollection
    : publicationCollection publication { $$ = makePublicationCollection($2, $1);   }
    | publication                       { $$ = makePublicationCollection($1, NULL); }
    ;

publication
    : T_AT T_Article T_OpeningBrace T_Keyword T_Comma publicationInfo T_ClosingBrace
         { $$ = makePublication("Article", $4, $6); }
    | T_AT T_Book T_OpeningBrace T_Keyword T_Comma publicationInfo T_ClosingBrace
         { $$ = makePublication("Book", $4, $6); }
    | T_AT T_InCollection T_OpeningBrace T_Keyword T_Comma publicationInfo T_ClosingBrace
         { $$ = makePublication("InCollection", $4, $6); }
    | T_AT T_InProceedings T_OpeningBrace T_Keyword T_Comma publicationInfo T_ClosingBrace
         { $$ = makePublication("InProceedings", $4, $6); }
    | T_AT T_Misc T_OpeningBrace T_Keyword T_Comma publicationInfo T_ClosingBrace
         { $$ = makePublication("Misc", $4, $6); }
    | T_AT T_TechReport T_OpeningBrace T_Keyword T_Comma publicationInfo T_ClosingBrace
         { $$ = makePublication("TechReport", $4, $6); }
    ;

publicationInfo
    : publicationInfo T_Comma publicationInfoItem { $$ = makePublicationInfo($3, $1);   }
    | publicationInfoItem                         { $$ = makePublicationInfo($1, NULL); }
    ;

publicationInfoItem
    : T_Keyword T_Equals T_Keyword { $$ = makePublicationInfoItem($1, $3); }
    | T_Keyword T_Equals T_String  { $$ = makePublicationInfoItem($1, $3); }
    | T_Keyword T_Equals T_OpeningBrace T_Keyword T_ClosingBrace { $$ = makePublicationInfoItem($1, $4); }
    | T_Keyword T_Equals T_OpeningBrace T_String T_ClosingBrace  { $$ = makePublicationInfoItem($1, $4); }
    ;
