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


struct Node {
   struct Node* next;
   struct Node* child;
   char*        string;
   char*        keyword;
   char*        value;
};

%}

%x STRING

%%

"@"                { return(T_AT); }
"{"                { return(T_OpeningBrace); }
"}"                { return(T_ClosingBrace); }
","                { return(T_Comma); }
"="                { return(T_Equals); }
"article"          { return(T_Article); }
"book"             { return(T_Book); }
"incollection"     { return(T_InCollection); }
"inproceedings"    { return(T_InProceedings); }
"misc"             { return(T_Misc); }
"techreport"       { return(T_TechReport); }

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


struct Node* createNode(const char* label)
{
   struct Node* node = (struct Node*)malloc(sizeof(struct Node));
   if(node == NULL) {
      yyerror("out of memory");
   }
   node->string  = strdup(label);
   node->next    = NULL;
   node->child   = NULL;
   node->keyword = NULL;
   node->value   = NULL;

   return(node);
}

void freeNode(struct Node* node)
{
   free(node->string);
   node->string = NULL;
   puts("-------- FREE !!!!!!! ");
   free(node);
}

void dumpNode(struct Node* node)
{
   struct Node* child;

   puts("---- DUMP ----");
   do {
      printf("NODE: s=%s\n", node->string);
      child = node->child;
      while(child != NULL) {
         printf("C=%p\n", child);
      /*   printf("\tKEY=%s\tVALUE=%s\n", child->keyword, child->value); */
         child = child->next;
      }
      node = node->next;
   } while(node != NULL);
   puts("---- DUMP ----   ende!");
}


struct Node* makePublication(char* type, char* label, struct Node* publicationInfo)
{
   printf("MAKE=%s\n",label);
   struct Node* node = createNode(label);
   node->child = publicationInfo;
   node->value = strdup(type);
   return(node);
}

struct Node* makePublicationInfo(struct Node* node1, struct Node* node2)
{
   node1->next = node2;
   printf("MKINFO=%p\n", node1);
   return(node1);
}

struct Node* makePublicationInfoItem(char* keyword, char* value)
{
   struct Node* node = createNode("PublicationInfoItem");
   node->keyword = strdup(keyword);
   node->value   = strdup(value);
   printf("\t%p:\tKEY=%s\tVALUE=%s\n", node, node->keyword, node->value);
   printf("MKINFOITEM=%p\n", node);
   return(node);
}



int main(int argc, char** argv)
{
   yyparse();
   return 0;
}
