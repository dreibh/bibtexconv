/* $Id$
 *
 * BibTeX Converter
 * Copyright (C) 2010-2014 by Thomas Dreibholz
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
#include <assert.h>
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
   node->keyword  = label;
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
size_t countNodes(const Node* node)
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


// ###### Count child nodes #################################################
size_t countChildNodes(const Node* node, const char* childKeyword)
{
   const Node*       child;
   const std::string keywordToFind(childKeyword);
   size_t            count = 0;

   child = node->child;
   while(child != NULL) {
      if(child->keyword == keywordToFind) {
         count++;
      }
      child = child->next;
   }
   return(count);
}


// ###### Make publication collection #######################################
Node* makePublicationCollection(Node* node1, Node* node2)
{
   // ====== If there is already an existing node, clear and use it =========
   Node* n = node2;
   while(n != NULL) {
      if(n->keyword == node1->keyword) {
         // fprintf(stderr, "NOTE: Duplicate: %s\n", n->keyword.c_str());

         const Node* oldTitle = findChildNode(node1, "title");
         Node*       newTitle = findChildNode(n, "title");
         if( (oldTitle != NULL) && (newTitle != NULL) && (oldTitle->value != newTitle->value) ) {
            fprintf(stderr, "NOTE: Keeping old title:\nOld = \"%s\"\nNew = \"%s\"\n",
                    oldTitle->value.c_str(),
                    newTitle->value.c_str());
            newTitle->value = oldTitle->value;            
         }

         // node1 is old. Remove its contents, but reuse it for newer data.
         freeNode(node1->child);
         node1->child = n->child;
         n->child     = NULL;
         
         // Get rid of old node n.
         if(n->prev) {
            n->prev->next = n->next;
         }
         if(n->next) {
            n->next->prev = n->prev;
         }
         delete n;
         break;
      }
      n = n->next;
   }

   // ====== Add a new node =================================================
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


// ###### Find existing or create new child node ############################
Node* addOrUpdateChildNode(Node* node, const char* childKeyword, const char* value)
{
   Node* child = findChildNode(node, childKeyword);
   if(child == NULL) {
      child = makePublicationInfoItem(childKeyword, value);
      assert(child != NULL);
      child->next = node->child;
      node->child = child;
      sortChildren(node);
   }
   else {
      child->value = value;
   }
   return(child);
}


// ###### Check number of occurrences for a field ###########################
static bool requiresField(const Node* publication,
                          const char* field,
                          const size_t minimum,
                          const size_t maximum)
{
   const size_t count = countChildNodes(publication, field);
   if(count < minimum) {
      fprintf(stderr, "WARNING: Entry %s has no \"%s\" section!\n",
              publication->keyword.c_str(),
              field);
      return(false);
   }
   else if(count > maximum) {
      fprintf(stderr, "WARNING: Entry %s has %u \"%s\" sections!\n",
              publication->keyword.c_str(),
              (unsigned int)count, field);
      return(false);
   }
   return(true);
}


// ###### Make publication ##################################################
Node* makePublication(const char* type, const char* label, Node* publicationInfo)
{
   Node* publication = createNode(label);
   publication->child = publicationInfo;
   publication->value = type;

   sortChildren(publication);

   if(publication->value != "Comment") {
      requiresField(publication, "title",        1, 1);
      requiresField(publication, "author",       1, 1);
      requiresField(publication, "year",         1, 1);
      requiresField(publication, "isbn",         0, 1);
      requiresField(publication, "issn",         0, 1);
      requiresField(publication, "doi",          0, 1);
      requiresField(publication, "url",          0, 1);
      requiresField(publication, "url.size",     0, 1);
      requiresField(publication, "url.mime",     0, 1);
      requiresField(publication, "url.md5",      0, 1);
      requiresField(publication, "url.checked",  0, 1);
      requiresField(publication, "urn",          0, 1);
      requiresField(publication, "pages",        0, 1);
      requiresField(publication, "numpages",     0, 1);
      requiresField(publication, "day",          0, 1);
      requiresField(publication, "month",        0, 1);
      requiresField(publication, "address",      0, 1);
      requiresField(publication, "location",     0, 1);
      requiresField(publication, "note",         0, 1);
      requiresField(publication, "howpublished", 0, 1);
      requiresField(publication, "publisher",    0, 1);
      requiresField(publication, "school",       0, 1);
      requiresField(publication, "institution",  0, 1);
      requiresField(publication, "type",         0, 1);
      requiresField(publication, "number",       0, 1);
      requiresField(publication, "issue",        0, 1);
      requiresField(publication, "volume",       0, 1);
      requiresField(publication, "abstract",     0, 1);
      requiresField(publication, "keywords",     0, 1);
      if(publication->value == "Article") {
         requiresField(publication, "journal", 1, 1);
      }
      else if(publication->value == "Book") {
         requiresField(publication, "publisher", 1, 1);
      }
      else if(publication->value == "InProceedings") {
         requiresField(publication, "booktitle", 1, 1);
      }
      else if(publication->value == "TechReport") {
         requiresField(publication, "institution", 1, 1);
      }

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
      Node* howPublished = findChildNode(publication, "howPublished");
      if(howPublished != NULL) {
         unifyBookTitle(publication, howPublished);
      }
      Node* journal = findChildNode(publication, "journal");
      if(journal != NULL) {
         unifyBookTitle(publication, journal);   // Same as for booktitle!
      }
      Node* pages = findChildNode(publication, "pages");
      if(pages != NULL) {
         unifyPages(publication, pages);
      }
      Node* numpages = findChildNode(publication, "numpages");
      if(numpages != NULL) {
         unifyNumPages(publication, numpages);
      }

      Node* isbn = findChildNode(publication, "isbn");
      if(isbn != NULL) {
         unifyISBN(publication, isbn);
      }
      Node* issn = findChildNode(publication, "issn");
      if(issn != NULL) {
         unifyISSN(publication, issn);
      }

      Node* year  = findChildNode(publication, "year");
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
   if(node1 != NULL) {
      node2->prev = node1;
      node1->next = node2;
      return(node1);
   }
   else {
      return(node2);
   }
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
   if( (node->keyword != "author") ) {   // Brackets must remain for author string!
      removeBrackets(node->value);
      trim(node->value);
   }

   if(node->value == "") {   // Empty content -> This item is useless
      node->keyword = "removeme";
   }

   // ====== Set priorities for well-known keyword fields ===================
   if(node->keyword == "author") {
      node->priority = 255;
   }
   else if(node->keyword == "title") {
      node->priority = 254;
   }

   else if(node->keyword == "howpublished") {
      node->priority = 252;
   }
   else if(node->keyword == "booktitle") {
      node->priority = 251;
   }
   else if(node->keyword == "series") {
      node->priority = 250;
   }
   else if(node->keyword == "journal") {
      node->priority = 249;
   }
   else if(node->keyword == "type") {
      node->priority = 248;
   }
   else if(node->keyword == "volume") {
      node->priority = 247;
   }
   else if(node->keyword == "issue") {
      node->priority = 246;
   }
   else if(node->keyword == "number") {
      node->priority = 245;
   }
   else if(node->keyword == "edition") {
      node->priority = 244;
   }
   else if(node->keyword == "editor") {
      node->priority = 243;
   }
   else if(node->keyword == "pages") {
      node->priority = 242;
   }
   else if(node->keyword == "numpages") {
      node->priority = 241;
   }

   else if(node->keyword == "day") {
      node->priority = 239;
   }
   else if(node->keyword == "month") {
      node->priority = 238;
   }
   else if(node->keyword == "year") {
      node->priority = 237;
   }

   else if(node->keyword == "organization") {
      node->priority = 235;
   }
   else if(node->keyword == "school") {
      node->priority = 234;
   }
   else if(node->keyword == "institution") {
      node->priority = 233;
   }
   else if(node->keyword == "location") {
      node->priority = 232;
   }
   else if(node->keyword == "publisher") {
      node->priority = 231;
   }
   else if(node->keyword == "address") {
      node->priority = 230;
   }

   else if(node->keyword == "language") {
      node->priority = 226;
   }
   else if(node->keyword == "content-language") {
      node->priority = 225;
   }
   else if(node->keyword == "isbn") {
      node->priority = 224;
   }
   else if(node->keyword == "issn") {
      node->priority = 223;
   }
   else if(node->keyword == "urn") {
      node->priority = 222;
   }
   else if(node->keyword == "doi") {
      node->priority = 221;
   }
   else if(node->keyword == "note") {
      node->priority = 220;
   }

   else if(node->keyword == "keywords") {
      node->priority = 211;
   }
   else if(node->keyword == "abstract") {
      node->priority = 210;
   }
   
   else if(node->keyword == "url") {
      node->priority = 199;
   }
   else if(node->keyword == "url.size") {
      node->priority = 198;
   }
   else if(node->keyword == "url.md5") {
      node->priority = 197;
   }
   else if(node->keyword == "url.mime") {
      node->priority = 196;
   }
   else if(node->keyword == "url.pagesize") {
      node->priority = 195;
   }
   else if(node->keyword == "url.checked") {
      node->priority = 194;
   }
   else if(node->keyword == "url.keywords") {
      node->priority = 193;
   }

   else {
      // printf("UNKNOWN=<%s>\n", node->keyword.c_str());
   }

   return(node);
}
