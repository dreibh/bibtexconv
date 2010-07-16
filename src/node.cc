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
#include <set>

#include "node.h"
#include "unification.h"
#include "stringhandling.h"


// ###### Allocate node #####################################################
static Node* createNode(const char* label)
{
   Node* node = new Node;
   if(node == NULL) {
      yyerror("out of memory");
   }
   node->keyword    = label;
   node->number   = 0;
   node->prev     = NULL;
   node->next     = NULL;
   node->child    = NULL;
   node->priority = 0;
   return(node);
}


// ###### Free nodes ########################################################
void freeNode(Node* node)
{
   Node* next;
   Node* child;
   Node* nextChild;

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


// ###### Count nodes in chain ##############################################
size_t countNodes(Node* node)
{
   size_t count = 0;
   while(node != NULL) {
      count++;
      node = node->next;
   }
   return(count);
}


// ###### Dump nodes ########################################################
void dumpNode(Node* node)
{
   Node* child;

   puts("---- DUMP ----");
   do {
      printf("[%s] %s:\n", node->value.c_str(), node->keyword.c_str());
      child = node->child;
      while(child != NULL) {
         printf("\t%s = %s\n", child->keyword.c_str(), child->value.c_str());
         child = child->next;
      }
      node = node->next;
   } while(node != NULL);
   puts("--------------");
}


// ###### Find node #########################################################
Node* findNode(Node* node, const char* keyword)
{
   const std::string keywordToFind(keyword);

   while(node != NULL) {
      if(node->keyword == keywordToFind) {
         return(node);
      }
      node = node->next;
   }
   return(NULL);
}


// ###### Find child node ###################################################
Node* findChildNode(Node* node, const char* childKeyword)
{
   Node*             child;
   const std::string keywordToFind(childKeyword);

   child = node->child;
   while(child != NULL) {
      if(child->keyword == keywordToFind) {
         return(child);
      }
      child = child->next;
   }
   return(NULL);
}


// ###### Make publication collection #######################################
Node* makePublicationCollection(Node* node1, Node* node2)
{
   node2->prev = node1;
   node1->next = node2;
   return(node1);
}


// ###### Node comparison function ##########################################
int nodeComparisonFunction(const void* node1ptr, const void* node2ptr)
{
   const Node* node1 = *((Node**)node1ptr);
   const Node* node2 = *((Node**)node2ptr);
   if(node1->priority > node2->priority) {
      return(-1);
   }
   else if(node1->priority < node2->priority) {
      return(1);
   }
   if(node1->keyword < node2->keyword) {
      return(-1);
   }
   else if(node1->keyword > node2->keyword) {
      return(1);
   }
   return(0);
}


// ###### Sort children of node #############################################
static void sortChildren(Node* node)
{
   Node* child = node->child;
   if(child) {
      const size_t children = countNodes(child);
      Node*        sortedChildrenSet[children];
      size_t       i = 0;
      while(child != NULL) {
         sortedChildrenSet[i++] = child;
         child = child->next;
      }

      qsort((void*)&sortedChildrenSet[0], children, sizeof(sortedChildrenSet[0]), nodeComparisonFunction);

      for(i = 0; i < children; i++) {
         if(i < children - 1) {
            sortedChildrenSet[i]->next = sortedChildrenSet[i + 1];
         }
         else {
            sortedChildrenSet[i]->next = NULL;
         }
         if(i > 0) {
            sortedChildrenSet[i]->prev = sortedChildrenSet[i - 1];
         }
         else {
            sortedChildrenSet[i]->prev = NULL;
         }
      }
      node->child = sortedChildrenSet[0];
   }
}


// ###### Make publication ##################################################
Node* makePublication(const char* type, const char* label, Node* publicationInfo)
{
   Node* publication = createNode(label);
   publication->child = publicationInfo;
   publication->value = type;

   sortChildren(publication);

   if(publication->value != "Comment") {
      Node* author = findChildNode(publication, "author");
      if(author != NULL) {
         unifyAuthor(publication, author);
      }
      else {
         fprintf(stderr, "WARNING: Entry %s has no \"author\" section!\n" , label);
      }

      Node* booktitle = findChildNode(publication, "booktitle");
      if(booktitle != NULL) {
         unifyBookTitle(publication, booktitle);
      }
      Node* journal = findChildNode(publication, "journal");
      if(journal != NULL) {
         unifyBookTitle(publication, journal);   // Same as for booktitle!
      }
      Node* pages = findChildNode(publication, "pages");
      if(pages != NULL) {
         unifyPages(publication, pages);
      }

      Node* isbn = findChildNode(publication, "isbn");
      if(isbn != NULL) {
         unifyISBN(publication, isbn);
      }
      Node* issn = findChildNode(publication, "issn");
      if(issn != NULL) {
         unifyISSN(publication, issn);
      }

      Node* year =  findChildNode(publication, "year");
      Node* month = findChildNode(publication, "month");
      Node* day   = findChildNode(publication, "day");
      if( (year != NULL) || (month != NULL) || (day != NULL) ) {
         unifyDate(publication, year, month, day);
      }

      Node* url = findChildNode(publication, "url");
      if(url != NULL) {
         unifyURL(publication, url);
      }
   }

   return(publication);
}


// ###### Make publication info #############################################
Node* makePublicationInfo(Node* node1, Node* node2)
{
   node2->prev = node1;
   node1->next = node2;
   return(node1);
}


// ###### Make publication info item ########################################
Node* makePublicationInfoItem(const char* keyword, const char* value)
{
   Node*        node          = createNode("PublicationInfoItem");
   const size_t keywordLength = strlen(keyword);
   char         keywordString[keywordLength + 1];
   size_t       i;

   // ====== Create new entry ===============================================
   for(i = 0;i < keywordLength;i++) {
      keywordString[i] = tolower(keyword[i]);
   }
   keywordString[keywordLength] = 0x00;

   node->keyword = keywordString;
   node->value   = value;
   removeBrackets(node->value);
   trim(node->value);

   // ====== Set priorities for well-known keyword fields ===================
   if(node->keyword == "author") {
      node->priority = 255;
   }
   else if(node->keyword == "title") {
      node->priority = 254;
   }
   else if(node->keyword == "booktitle") {
      node->priority = 253;
   }
   else if(node->keyword == "journal") {
      node->priority = 253;
   }
   else if(node->keyword == "volume") {
      node->priority = 250;
   }
   else if(node->keyword == "number") {
      node->priority = 249;
   }
   else if(node->keyword == "pages") {
      node->priority = 248;
   }
   else if(node->keyword == "institution") {
      node->priority = 242;
   }
   else if(node->keyword == "publisher") {
      node->priority = 241;
   }
   else if(node->keyword == "address") {
      node->priority = 240;
   }
   else if(node->keyword == "day") {
      node->priority = 232;
   }
   else if(node->keyword == "month") {
      node->priority = 231;
   }
   else if(node->keyword == "year") {
      node->priority = 230;
   }
   else if(node->keyword == "isbn") {
      node->priority = 222;
   }
   else if(node->keyword == "issn") {
      node->priority = 221;
   }
   else if(node->keyword == "note") {
      node->priority = 220;
   }
   else if(node->keyword == "url") {
      node->priority = 200;
   }
   return(node);
}
