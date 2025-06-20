// ==========================================================================
//                ____  _ _   _____   __  ______
//                | __ )(_) |_|_   _|__\ \/ / ___|___  _ ____   __
//                |  _ \| | '_ \| |/ _ \  / |   / _ \| '_ \ \ / /
//                | |_) | | |_) | |  __//  \ |__| (_) | | | \ V /
//                |____/|_|_.__/|_|\___/_/\_\____\___/|_| |_|\_/
//
//                          ---  BibTeX Converter  ---
//                   https://www.nntb.no/~dreibh/bibtexconv/
// ==========================================================================
//
// BibTeX Converter
// Copyright (C) 2010-2025 by Thomas Dreibholz
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Contact: thomas.dreibholz@gmail.com

%{
#include <stdio.h>
#include "node.h"

Node* bibTeXFile = NULL;
%}

%token T_AT
%token T_OpeningBrace
%token T_ClosingBrace
%token T_Comma
%token T_Equals

%token T_Article
%token T_Book
%token T_Booklet
%token T_Data
%token T_InBook
%token T_InProceedings
%token T_InCollection
%token T_Manual
%token T_MastersThesis
%token T_Misc
%token T_PhDThesis
%token T_Proceedings
%token T_TechReport
%token T_Unpublished

%token <iText>  T_Keyword
%token <iText>  T_String
%token <iText>  T_Comment

%destructor { free($$); } T_Keyword
%destructor { free($$); } T_String
%destructor { free($$); } T_Comment

%type <nodePtr> bibTeXFile
%type <nodePtr> publicationCollection
%type <nodePtr> publication
%type <nodePtr> publicationInfo
%type <nodePtr> publicationInfoItem
%type <nodePtr> publicationInfoJustComment
%type <nodePtr> publicationInfoItemJustComment

%union {
   char*        iText;
   struct Node* nodePtr;
}

%%

bibTeXFile
    : publicationCollection { bibTeXFile = $$; }
    ;

publicationCollection
    : publication publicationCollection  { $$ = makePublicationCollection($1, $2); }
    | publication                        { $$ = $1; }
    ;

publication
    : T_Comment
         { $$ = makePublication("Comment", NULL, makePublicationInfoItem("comment", $1)); free($1); }
    | T_AT T_Article T_OpeningBrace T_Keyword T_Comma publicationInfo T_ClosingBrace
         { $$ = makePublication("Article", $4, $6); free($4); }
    | T_AT T_Book T_OpeningBrace T_Keyword T_Comma publicationInfo T_ClosingBrace
         { $$ = makePublication("Book", $4, $6); free($4); }
    | T_AT T_Booklet T_OpeningBrace T_Keyword T_Comma publicationInfo T_ClosingBrace
         { $$ = makePublication("Booklet", $4, $6); free($4); }
    | T_AT T_Data T_OpeningBrace T_Keyword T_Comma publicationInfo T_ClosingBrace
         { $$ = makePublication("Data", $4, $6); free($4); }
    | T_AT T_InBook T_OpeningBrace T_Keyword T_Comma publicationInfo T_ClosingBrace
         { $$ = makePublication("InBook", $4, $6); free($4); }
    | T_AT T_InCollection T_OpeningBrace T_Keyword T_Comma publicationInfo T_ClosingBrace
         { $$ = makePublication("InCollection", $4, $6); free($4); }
    | T_AT T_InProceedings T_OpeningBrace T_Keyword T_Comma publicationInfo T_ClosingBrace
         { $$ = makePublication("InProceedings", $4, $6); free($4); }
    | T_AT T_Manual T_OpeningBrace T_Keyword T_Comma publicationInfo T_ClosingBrace
         { $$ = makePublication("Manual", $4, $6); free($4); }
    | T_AT T_MastersThesis T_OpeningBrace T_Keyword T_Comma publicationInfo T_ClosingBrace
         { $$ = makePublication("MastersThesis", $4, $6); free($4); }
    | T_AT T_Misc T_OpeningBrace T_Keyword T_Comma publicationInfo T_ClosingBrace
         { $$ = makePublication("Misc", $4, $6); free($4); }
    | T_AT T_PhDThesis T_OpeningBrace T_Keyword T_Comma publicationInfo T_ClosingBrace
         { $$ = makePublication("PhDThesis", $4, $6); free($4); }
    | T_AT T_TechReport T_OpeningBrace T_Keyword T_Comma publicationInfo T_ClosingBrace
         { $$ = makePublication("TechReport", $4, $6); free($4); }
    | T_AT T_Proceedings T_OpeningBrace T_Keyword T_Comma publicationInfo T_ClosingBrace
         { $$ = makePublication("Proceedings", $4, $6); free($4); }
    | T_AT T_Unpublished T_OpeningBrace T_Keyword T_Comma publicationInfo T_ClosingBrace
         { $$ = makePublication("Unpublished", $4, $6); free($4); }
    ;

publicationInfo
    : publicationInfoItem T_Comma publicationInfo              { $$ = makePublicationInfo($1, $3); }
    | publicationInfoItem T_Comment publicationInfoJustComment { $$ = $1; free($2); }   /* Comments at end of entry */
    | publicationInfoItem                                      { $$ = $1; }
    ;

publicationInfoItem
    : T_Keyword T_Equals T_Keyword { $$ = makePublicationInfoItem($1, $3); free($1); free($3); }
    | T_Keyword T_Equals T_String  { $$ = makePublicationInfoItem($1, $3); free($1); free($3); }
    | T_Keyword T_Equals T_OpeningBrace T_Keyword T_ClosingBrace { $$ = makePublicationInfoItem($1, $4); free($1); free($4); }
    | T_Keyword T_Equals T_OpeningBrace T_String T_ClosingBrace  { $$ = makePublicationInfoItem($1, $4); free($1); free($4); }
    | %empty { $$ = NULL; }
    ;

/* Just comments at end of entry */
publicationInfoJustComment
   : publicationInfoItemJustComment T_Comment publicationInfoJustComment { free($2); $$ = NULL; }
   | T_Comment { free($1); $$ = NULL; }
   ;

/* Just comments at end of entry */
publicationInfoItemJustComment
    : T_Comment { free($1); $$ = NULL; }
    ;
