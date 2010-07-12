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

#include <stdio.h>
#include <stdlib.h>

#include "node.h"


extern int   yyparse();
extern FILE* yyin;
extern Node* bibTeXFile;


// ###### Export to BiBTeX ##################################################
void exportToBiBTeX(Node* publication)
{
   while(publication != NULL) {
      if(publication->value == "Comment") {
         printf("%%%s\n\n", publication->label.c_str());
      }
      else {
         printf("@%s { %s, \n", publication->value.c_str(), publication->label.c_str());

         bool empty  = true;
         Node* child = publication->child;
         while(child != NULL) {
            if(!empty) {
               printf(",\n");
            }
            empty = false;

            if( (child->keyword == "title") ||
                (child->keyword == "booktitle") ||
                (child->keyword == "journal") ) {
               printf("\t%s = \"{%s}\"", child->keyword.c_str(), child->value.c_str());
            }
            else if( (child->keyword == "month") ) {
               printf("\t%s = %s", child->keyword.c_str(), child->value.c_str());
            }
            else if( (child->keyword == "url") ) {
               printf("\t%s = \"\\url{%s}\"", child->keyword.c_str(), child->value.c_str());
            }
            else {
               printf("\t%s = \"%s\"", child->keyword.c_str(), child->value.c_str());
            }
            child = child->next;
         }

         puts("\n}\n");
      }

      publication = publication->next;
   };
}


int main(int argc, char** argv)
{
   if(argc < 2) {
      fprintf(stderr, "Usage: %s [BiBTeX file]!\n", argv[0]);
      exit(1);
   }

   yyin = fopen(argv[1], "r");
   if(yyin == NULL) {
      fprintf(stderr, "ERROR: Unable to open BibTeX input file %s!\n", argv[1]);
      exit(1);
   }
   int result = yyparse();
   fclose(yyin);

   if(result == 0) {
       exportToBiBTeX(bibTeXFile);
   }
   if(bibTeXFile) {
      freeNode(bibTeXFile);
      bibTeXFile = NULL;
   }

   return result;
}
