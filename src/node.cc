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

#include "node.h"


// ###### Allocate node #####################################################
struct Node* createNode(const char* label)
{
   struct Node* node = new Node;
   if(node == NULL) {
      yyerror("out of memory");
   }
   node->label  = label;
   node->number = 0;
   node->prev   = NULL;
   node->next   = NULL;
   node->child  = NULL;
   return(node);
}


// ###### Free nodes ########################################################
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
         delete child;
         child = nextChild;
      }
      delete node;
      node = next;
   }
}


// ###### Dump nodes ########################################################
void dumpNode(struct Node* node)
{
   struct Node* child;

   puts("---- DUMP ----");
   do {
      printf("[%s] %s:\n", node->value.c_str(), node->label.c_str());
      child = node->child;
      while(child != NULL) {
         printf("\t%s = %s\n", child->keyword.c_str(), child->value.c_str());
         child = child->next;
      }
      node = node->next;
   } while(node != NULL);
   puts("--------------");
}


// ###### Make publication collection #######################################
struct Node* makePublicationCollection(struct Node* node1, struct Node* node2)
{
   node2->prev = node1;
   node1->next = node2;
   return(node1);
}


// ###### Make publication ##################################################
struct Node* makePublication(const char* type, const char* label, struct Node* publicationInfo)
{
   struct Node* node = createNode(label);
   node->child = publicationInfo;
   node->value = type;
   return(node);
}


// ###### Make publication info #############################################
struct Node* makePublicationInfo(struct Node* node1, struct Node* node2)
{
   node2->prev = node1;
   node1->next = node2;
   return(node1);
}


// ###### Make publication info item ########################################
struct Node* makePublicationInfoItem(const char* keyword, const char* value)
{
   struct Node* node          = createNode("PublicationInfoItem");
   const size_t keywordLength = strlen(keyword);
   char         keywordString[keywordLength + 1];
   size_t       i;

   for(i = 0;i < keywordLength;i++) {
      keywordString[i] = tolower(keyword[i]);
   }
   keywordString[keywordLength] = 0x00;

   node->keyword = keywordString;
   node->value   = value;
   return(node);
}
