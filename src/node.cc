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


// ###### Remove brackets { ... } ###########################################
static void removeBrackets(std::string& str)
{
   while( (str.substr(0, 1) == "{") &&
       (str.substr(str.size() - 1) == "}") ) {
      str = str.substr(1, str.size() - 2);
   }
}


// ###### Remove whitespaces at front and back of a string ##################
static void trim(std::string& str)
{
   const ssize_t length = str.size();
   ssize_t s, e;
   for(s = 0; s < length; s++) {
      if(str[s] != ' ') {
         break;
      }
   }
   for(e = length - 1; e >= 0; e--) {
      if(str[e] != ' ') {
         break;
      }
   }
   str = str.substr(s, length - s - (length - 1 - e) );
}


// ###### Extract initials from given name(s) ###############################
static void extractAuthorInitials(const std::string& givenNameFull,
                                  std::string&       givenNameInitials)
{
   const size_t length  = givenNameFull.size();
   bool         extract = true;
   bool         empty   = true;

   givenNameInitials = "";
   for(size_t i = 0;i < length;i++) {
      if( (givenNameFull[i] == ' ') ||
          (givenNameFull[i] == '~') ) {
         extract = true;
      }
      else {
         if(extract == true) {
            if(!empty) {
               givenNameInitials += '~';
            }
            givenNameInitials += (const char)givenNameFull[i];
            givenNameInitials += '.';
            extract = false;
            empty   = false;
         }
      }
   }

}


// ###### Split author name into its parts ##################################
static void splitAuthor(std::string& author,
                        std::string& givenNameFull,
                        std::string& givenNameInitials,
                        std::string& familyName)
{
   trim(author);

   size_t pos;
   if( (pos = author.find(",")) != std::string::npos ) {   // Name, Given Name(s)
      givenNameFull = author.substr(pos + 1, author.size() - pos - 1);
      familyName    = author.substr(0, pos);
      extractAuthorInitials(givenNameFull, givenNameInitials);
   }
   else {   // Given Name(s) + Family Name
      pos = author.rfind(" ");
      if(pos == std::string::npos) {
         pos = author.rfind("~");
      }
      if(pos == std::string::npos) {   // Family Name only
         familyName     = author;
         givenNameFull = givenNameInitials = "";
      }
      else {   // Given Name(s) + Family Name
         familyName    = author.substr(pos + 1, author.size() - pos - 1);
         givenNameFull = author.substr(0, pos);
         extractAuthorInitials(givenNameFull, givenNameInitials);
      }
   }
   trim(givenNameFull);
   trim(familyName);
/*
   printf("   -> %s:\tI=<%s> G=<%s> F=<%s>\n", author.c_str(),
          givenNameInitials.c_str(), givenNameFull.c_str(), familyName.c_str());
*/
   if(givenNameFull != "") {
      author = givenNameFull + ((givenNameFull.rfind(".") == givenNameFull.size() - 1) ? "~" : " ") + familyName;
   }
   else {
      author = familyName;
   }
}


// ###### Unify "author" section ############################################
static void unifyAuthor(Node* node, Node* author)
{
   std::string currentAuthor;
   std::string givenNameFull;
   std::string givenNameInitials;
   std::string familyName;

   // ====== Extract authors ================================================
   std::string allAuthors = author->value;
   size_t      pos;
   bool        empty = true;
   author->value = "";
   while( (pos = allAuthors.find(" and ")) != std::string::npos ) {
      currentAuthor = allAuthors.substr(0, pos);

      splitAuthor(currentAuthor, givenNameFull, givenNameInitials, familyName);
      author->value += ((!empty) ? " and " : "") + currentAuthor;
      empty = false;

      pos += 5;
      allAuthors = allAuthors.substr(pos, allAuthors.size() - pos);
   }

   // ====== Extract last author ============================================
   splitAuthor(allAuthors, givenNameFull, givenNameInitials, familyName);
   author->value += ((!empty) ? " and " : "") + allAuthors;
}


// ###### Unify "year"/"month"/"day" sections ###############################
static void unifyDate(Node* node, Node* year, Node* month, Node* day)
{
   if(year != NULL) {
      year->number = atol(year->value.c_str());
      if((year->number < 1700) || (year->number > 2012)) {
         fprintf(stderr, "WARNING: Entry %s has probably invalid \"year\" section (year=%d?)!\n" ,
               node->label.c_str(), year->number);
      }
   }
   else {
      fprintf(stderr, "WARNING: Entry %s has no \"year\" section, but \"month\" or \"day\"!\n" ,
              node->label.c_str());
   }

   int monthNumber = 0;
   if(monthNumber) {
      if(month->value == "jan") {
         monthNumber = 1;
      }
      else if(month->value == "feb") {
         monthNumber = 2;
      }
      else if(month->value == "mar") {
         monthNumber = 3;
      }
      else if(month->value == "apr") {
         monthNumber = 4;
      }
      else if(month->value == "may") {
         monthNumber = 5;
      }
      else if(month->value == "jun") {
         monthNumber = 6;
      }
      else if(month->value == "jul") {
         monthNumber = 7;
      }
      else if(month->value == "aug") {
         monthNumber = 8;
      }
      else if(month->value == "sep") {
         monthNumber = 9;
      }
      else if(month->value == "oct") {
         monthNumber = 10;
      }
      else if(month->value == "nov") {
         monthNumber = 11;
      }
      else if(month->value == "dec") {
         monthNumber = 12;
      }
      else {
         fprintf(stderr, "WARNING: Entry %s has probably invalid \"monthNumber\" section (monthNumber=%s?)!\n" ,
                 node->label.c_str(), month->value.c_str());
      }
      month->number = monthNumber;
   }
}

// ###### Unify "url" section ###############################################
static void unifyURL(Node* node, Node* url)
{
   if( (url->value.substr(0, 5) == "\\url{") &&
       (url->value.substr(url->value.size() - 1) == "}") ) {
      url->value = url->value.substr(5, url->value.size() - 6);
   }
}




// ###### Allocate node #####################################################
static Node* createNode(const char* label)
{
   Node* node = new Node;
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


// ###### Dump nodes ########################################################
void dumpNode(Node* node)
{
   Node* child;

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


// ###### Make publication ##################################################
Node* makePublication(const char* type, const char* label, Node* publicationInfo)
{
   Node* node = createNode(label);
   node->child = publicationInfo;
   node->value = type;

   if(node->value != "Comment") {
      Node* author = findChildNode(node, "author");
      if(author != NULL) {
         unifyAuthor(node, author);
      }
      else {
         fprintf(stderr, "WARNING: Entry %s has no \"author\" section!\n" , label);
      }

      Node* year =  findChildNode(node, "year");
      Node* month = findChildNode(node, "month");
      Node* day   = findChildNode(node, "day");
      if( (year != NULL) || (month != NULL) || (day != NULL) ) {
         unifyDate(node, year, month, day);
      }

      Node* url = findChildNode(node, "url");
      if(url != NULL) {
         unifyURL(node, url);
      }
   }

   return(node);
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

   for(i = 0;i < keywordLength;i++) {
      keywordString[i] = tolower(keyword[i]);
   }
   keywordString[keywordLength] = 0x00;

   node->keyword = keywordString;
   node->value   = value;
   removeBrackets(node->value);
   return(node);
}
