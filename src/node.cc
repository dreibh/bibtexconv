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


// ###### Remove superflous whitespaces from a string #######################
static void trim(std::string& str)
{
   // ====== Remove whitespaces from beginning and end ======================
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

   // ====== Remove double whitespaces ======================================
   bool gotSpace = false;
   for(e = str.size() - 1; e >= 0; e--) {
      if(str[e] == ' ') {
         if(!gotSpace) {
            gotSpace = true;
         }
         else {
            str.erase(e, 1);
         }
      }
      else {
         gotSpace = false;
      }
   }
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


// ###### Unify "booktitle" section #########################################
static void unifyBookTitle(Node* node, Node* booktitle)
{
   size_t pos;
   while( (pos = booktitle->value.find(" (")) != std::string::npos ) {
      booktitle->value.replace(pos, 1, "~");
   }
}


// ###### Unify "isbn" section ##############################################
static void unifyISBN(Node* node, Node* isbn)
{
   // ====== Get pure number ================================================
   std::string number = "";
   for(size_t i = 0; i < isbn->value.size(); i++) {
      if( ((isbn->value[i] >= '0') &&
           (isbn->value[i] <= '9')) ||
           ((isbn->value[i] == 'X') && (i == isbn->value.size() - 1)) ) {
         number += isbn->value[i];
      }
      else if(isbn->value[i] == '-') {

      }
      else {
         fprintf(stderr, "WARNING: Entry %s has invalid characters in \"isbn\" section (isbn=%s)!\n" ,
                 node->label.c_str(), isbn->value.c_str());
         return;
      }
   }

   // ====== Validate =======================================================
   if(number.size() == 10) {
      unsigned int checksum = 0;
      for(size_t i = 0; i < 9; i++) {
         checksum += (10 - i) * ((number[i] == 'X') ? 10 : (number[i] - '0'));

      }
      checksum = 11 - checksum % 11;
      if(checksum == 11) {
         checksum = 0;
      }
      char value = ((checksum < 10) ? ((char)checksum + '0') : 'X');

      if(value != number[9]) {
         fprintf(stderr, "WARNING: Entry %s has invalid ISBN-10 in \"isbn\" section (isbn=%s; checksum=%c)\n" ,
                 node->label.c_str(), isbn->value.c_str(), value);
      }
   }
   else if(number.size() == 13) {
      printf("I13=%s\n",number.c_str());

   }
   else {
      fprintf(stderr, "WARNING: Entry %s has no ISBN-10 or ISBN-13 in \"isbn\" section (isbn=%s -> %s)\n" ,
              node->label.c_str(), isbn->value.c_str(), number.c_str());
      return;
   }
}


// ###### Unify "issn" section ##############################################
static void unifyISSN(Node* node, Node* issn)
{
}


// ###### Unify "year"/"month"/"day" sections ###############################
static void unifyDate(Node* node, Node* year, Node* month, Node* day)
{
   int yearNumber = 1;
   if(year != NULL) {
      yearNumber = atol(year->value.c_str());
      if((yearNumber < 1700) || (yearNumber > 2012)) {
         fprintf(stderr, "WARNING: Entry %s has probably invalid \"year\" section (year=%d?)!\n" ,
                 node->label.c_str(), yearNumber);
      }
      year->number = yearNumber;
   }
   else {
      fprintf(stderr, "WARNING: Entry %s has no \"year\" section, but \"month\" or \"day\"!\n" ,
              node->label.c_str());
   }

   int monthNumber = 0;
   int maxDays     = 0;
   if(month != NULL) {
      if(month->value == "jan") {
         monthNumber = 1;   maxDays = 31;
      }
      else if(month->value == "feb") {
         monthNumber = 2;
         if( ((yearNumber % 4) == 0) &&
             ( ((yearNumber % 100) != 0) ||
               ((yearNumber % 400) == 0) ) ) {
            maxDays = 29;
         }
         else {
            maxDays = 28;
         }
      }
      else if(month->value == "mar") {
         monthNumber = 3;   maxDays = 31;
      }
      else if(month->value == "apr") {
         monthNumber = 4;   maxDays = 30;
      }
      else if(month->value == "may") {
         monthNumber = 5;   maxDays = 31;
      }
      else if(month->value == "jun") {
         monthNumber = 6;   maxDays = 30;
      }
      else if(month->value == "jul") {
         monthNumber = 7;   maxDays = 30;
      }
      else if(month->value == "aug") {
         monthNumber = 8;   maxDays = 31;
      }
      else if(month->value == "sep") {
         monthNumber = 9;   maxDays = 30;
      }
      else if(month->value == "oct") {
         monthNumber = 10;   maxDays = 31;
      }
      else if(month->value == "nov") {
         monthNumber = 11;   maxDays = 30;
      }
      else if(month->value == "dec") {
         monthNumber = 12;   maxDays = 31;
      }
      else {
         fprintf(stderr, "WARNING: Entry %s has probably invalid \"month\" section (month=%s?)!\n" ,
                 node->label.c_str(), month->value.c_str());
      }
      month->number = monthNumber;
   }

   if(day != NULL) {
      day->number = atol(day->value.c_str());
      if(month == NULL) {
         fprintf(stderr, "WARNING: Entry %s has no \"month\" section, but \"day\"!\n" ,
               node->label.c_str());
      }
      else {
         if((day->number < 1) || (day->number > maxDays)) {
            fprintf(stderr, "WARNING: Entry %s has invalid \"day\" or \"month\" section (year=%d month=%d day=%d)!\n" ,
                  node->label.c_str(), yearNumber, monthNumber, day->number);
         }
      }
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

   puts("STOP!"); exit(1);


   do {
      printf("[%s] %s:\n", node->value.c_str(), node->label.c_str());
      child = node->child;
      while(child != NULL) {
//          printf("\t%s = %s\n", child->keyword.c_str(), child->value.c_str());
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

      Node* booktitle = findChildNode(node, "booktitle");
      if(booktitle != NULL) {
         unifyBookTitle(node, booktitle);
      }
      Node* isbn = findChildNode(node, "isbn");
      if(isbn != NULL) {
         unifyISBN(node, isbn);
      }
      Node* issn = findChildNode(node, "issn");
      if(issn != NULL) {
         unifyISSN(node, issn);
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
   trim(node->value);
   return(node);
}
