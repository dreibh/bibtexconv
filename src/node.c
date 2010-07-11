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
#include <string.h>
#include <ctype.h>

#include "node.h"


extern void yyerror(char* errorText);


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
   struct Node* next;
   struct Node* child;
   struct Node* nextChild;

   while(node != NULL) {
      next = node->next;
      child = node->child;
      while(child != NULL) {
         nextChild = child->next;
         free(child->string);
         if(child->keyword) {
            free(child->keyword);
         }
         if(child->value) {
            free(child->value);
         }
         free(child);
         child = nextChild;
      }
      free(node->string);
      if(node->keyword) {
         free(node->keyword);
      }
      if(node->value) {
         free(node->value);
      }
      free(node);
      node = next;
   }
}

void dumpNode(struct Node* node)
{
   struct Node* child;

   puts("---- DUMP ----");
   do {
      printf("[%s] %s:\n", node->value, node->string);
      child = node->child;
      while(child != NULL) {
         printf("\t%s = %s\n", child->keyword, child->value);
         child = child->next;
      }
      node = node->next;
   } while(node != NULL);
   puts("--------------");
}

struct Node* makePublicationCollection(struct Node* node1, struct Node* node2)
{
   node1->next = node2;
   return(node1);
}

struct Node* makePublication(char* type, char* label, struct Node* publicationInfo)
{
   struct Node* node = createNode(label);
   node->child = publicationInfo;
   node->value = strdup(type);
   return(node);
}

struct Node* makePublicationInfo(struct Node* node1, struct Node* node2)
{
   node1->next = node2;
   return(node1);
}

struct Node* makePublicationInfoItem(char* keyword, char* value)
{
   struct Node* node          = createNode("PublicationInfoItem");
   const size_t keywordLength = strlen(keyword);
   size_t       i;

   node->keyword = strdup(keyword);
   node->value   = strdup(value);
   for(i = 0;i < keywordLength;i++) {
      node->keyword[i] = tolower(node->keyword[i]);
   }
//    printf("\t%p:\tKEY=%s\tVALUE=%s\n", node, node->keyword, node->value);
   return(node);
}
