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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unification.h"
#include "stringhandling.h"


// ###### Extract initials from given name(s) ###############################
static void extractAuthorInitials(const std::string& givenNameFull,
                                  std::string&       givenNameInitials)
{
   const size_t length  = givenNameFull.size();
   bool         extract = true;
   bool         empty   = true;

   givenNameInitials = "";
   for(size_t i = 0;i < length;i++) {
      if( (givenNameFull[i] == ' ') || (givenNameFull[i] == '~') ) {
         extract = true;
      }
      else {
         if(extract == true) {
            if(!empty) {
               givenNameInitials += '~';
            }
            if( ( (((unsigned char)givenNameFull[i]) & 0xE0) == 0xC0 ) && (i + 1 < length) ) {
               // Two-byte UTF-8 character
               givenNameInitials += givenNameFull[i];
               givenNameInitials += givenNameFull[++i];
            }
            else if( ( (((unsigned char)givenNameFull[i]) & 0xF0) == 0xE0 ) && (i + 2 < length) ) {
               // Three-byte UTF-8 character
               givenNameInitials += givenNameFull[i];
               givenNameInitials += givenNameFull[++i];
               givenNameInitials += givenNameFull[++i];
            }
            else if( ( (((unsigned char)givenNameFull[i]) & 0xF8) == 0xF0 ) && (i + 3 < length) ) {
               // Four-byte UTF-8 character
               givenNameInitials += givenNameFull[i];
               givenNameInitials += givenNameFull[++i];
               givenNameInitials += givenNameFull[++i];
               givenNameInitials += givenNameFull[++i];
            }
            else if( (((unsigned char)givenNameFull[i]) & 0x80) == 0 ) {
               // Regular 1-byte character
              givenNameInitials += givenNameFull[i];
            }
            else {
               // Invalid!
            }
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
   size_t pos;

   // Clean up author string first.
   trim(author);
   while( (pos = author.find("~")) != std::string::npos ) {
      author.replace(pos, 1, " ");
   }

   if( author[0] == '{') {   // Special name in brackets, e.g. "{R Development Core Team}".
      familyName    = author;
      givenNameFull = givenNameInitials = "";
   }
   else if( (pos = author.find(",")) != std::string::npos ) {   // Name, Given Name(s)
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
      if(givenNameFull.rfind(".") != std::string::npos) {   // Given first name + initial
         // Replace spaces by non-breakable spaces.
         while( (pos = givenNameFull.find(" ")) != std::string::npos ) {
            givenNameFull.replace(pos, 1, "~");
         }
      }
   }
   trim(givenNameFull);
   trim(familyName);

/*
   printf("\t-> %s:\tA=<%s>\t->\tI=<%s> G=<%s> F=<%s>\n", author.c_str(),
          author.c_str(),
          givenNameInitials.c_str(), givenNameFull.c_str(), familyName.c_str());
*/

   if(givenNameFull != "") {
      if(givenNameFull == givenNameInitials) {   // Given name == initials
         author = givenNameInitials + "~" + familyName;
      }
      else {
         author = givenNameFull + " " + familyName;
      }
      // printf("\t\t=> A=<%s>\n", author.c_str());
   }
   else {
      author = familyName;
   }
}


// ###### Unify "author" section ############################################
void unifyAuthor(Node* publication, Node* author)
{
   std::string currentAuthor;
   std::string givenNameFull;
   std::string givenNameInitials;
   std::string familyName;

   size_t argumentsIndex = 0;
   author->arguments.clear();

   // ====== Iterator from author 1 to author n-1 (for n authors) ===========
   std::string allAuthors = author->value;
   bool        empty      = true;
   size_t      pos;
   author->value = "";
   while( (pos = allAuthors.find(" and ")) != std::string::npos ) {
      currentAuthor = allAuthors.substr(0, pos);

      // ====== Extract current author ======================================
      splitAuthor(currentAuthor, givenNameFull, givenNameInitials, familyName);
      author->value += ((!empty) ? " and " : "") + currentAuthor;
      empty = false;

      // ====== Store extracted name strings into Node's arguments vector ===
      author->arguments.resize(argumentsIndex + 3);
      author->arguments[argumentsIndex++] = familyName;
      author->arguments[argumentsIndex++] = givenNameFull;
      author->arguments[argumentsIndex++] = givenNameInitials;

      pos += 5;
      allAuthors = allAuthors.substr(pos, allAuthors.size() - pos);
   }

   // ====== Extract last author ============================================
   splitAuthor(allAuthors, givenNameFull, givenNameInitials, familyName);
   author->value += ((!empty) ? " and " : "") + allAuthors;

   // ====== Store extracted name strings into Node's arguments vector ======
   author->arguments.resize(argumentsIndex + 3);
   author->arguments[argumentsIndex++] = familyName;
   author->arguments[argumentsIndex++] = givenNameFull;
   author->arguments[argumentsIndex++] = givenNameInitials;
}


// ###### Unify "booktitle" section #########################################
void unifyBookTitle(Node* publication, Node* booktitle)
{
   size_t pos;
   while( (pos = booktitle->value.find(" (")) != std::string::npos ) {
      booktitle->value.replace(pos, 1, "~");
   }
}


// ###### Unify "isbn" section ##############################################
void unifyISBN(Node* publication, Node* isbn)
{
   // ====== Get pure number ================================================
   std::string number = "";
   size_t      length = isbn->value.size();
   for(size_t i = 0; i < length; i++) {
      if((isbn->value[i] < 0) && (i + 1 < length)) {
         i++;
      }
      else if( ((isbn->value[i] >= '0') && (isbn->value[i] <= '9')) ||
               ((isbn->value[i] == 'X') && (i == length - 1)) ) {
         number += isbn->value[i];
      }
      else if(isbn->value[i] == '-') {

      }
      else {
         fprintf(stderr, "WARNING: Entry %s has invalid characters in \"isbn\" section (isbn=%s)!\n" ,
                 publication->keyword.c_str(), isbn->value.c_str());
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
                 publication->keyword.c_str(), isbn->value.c_str(), value);
      }
   }
   else if(number.size() == 13) {
      unsigned int checksum = 10 - (
         (number[0] - '0') +
         3 * (number[1] - '0') +
         (number[2] - '0') +
         3 * (number[3] - '0') +
         (number[4] - '0') +
         3 * (number[5] - '0') +
         (number[6] - '0') +
         3 * (number[7] - '0') +
         (number[8] - '0') +
         3 * (number[9] - '0') +
         (number[10] - '0') +
         3 * (number[11] - '0')) % 10;
      if(checksum == 10) {
         checksum = 0;
      }
      char value = (char)checksum + '0';

      if(value != number[12]) {
         fprintf(stderr, "WARNING: Entry %s has invalid ISBN-13 in \"isbn\" section (isbn=%s; checksum=%c)\n" ,
                 publication->keyword.c_str(), isbn->value.c_str(), value);
      }
   }
   else {
      fprintf(stderr, "WARNING: Entry %s has no ISBN-10 or ISBN-13 in \"isbn\" section (isbn=%s -> %s)\n" ,
              publication->keyword.c_str(), isbn->value.c_str(), number.c_str());
      return;
   }
}


// ###### Unify "issn" section ##############################################
void unifyISSN(Node* publication, Node* issn)
{
   // ====== Get pure number ================================================
   std::string number = "";
   size_t      length = issn->value.size();
   for(size_t i = 0; i < length; i++) {
      if((issn->value[i] < 0) && (i + 1 < length)) {
         i++;
      }
      else if( ((issn->value[i] >= '0') && (issn->value[i] <= '9')) ||
               ((issn->value[i] == 'X') && (i == issn->value.size() - 1)) ) {
         number += issn->value[i];
      }
      else if(issn->value[i] == '-') {

      }
      else {
         fprintf(stderr, "WARNING: Entry %s has invalid characters in \"issn\" section (issn=%s)!\n" ,
                 publication->keyword.c_str(), issn->value.c_str());
         return;
      }
   }

   // ====== Validate =======================================================
   if(number.size() == 8) {
      unsigned int checksum = 0;
      for(size_t i = 0; i < 7; i++) {
         checksum += (8 - i) * ((number[i] == 'X') ? 10 : (number[i] - '0'));

      }
      checksum = 11 - checksum % 11;
      if(checksum == 11) {
         checksum = 0;
      }
      char value = ((checksum < 10) ? ((char)checksum + '0') : 'X');

      if(value != number[7]) {
         fprintf(stderr, "WARNING: Entry %s has invalid ISSN-10 in \"issn\" section (issn=%s; checksum=%c)\n" ,
                 publication->keyword.c_str(), issn->value.c_str(), value);
      }
   }
   else {
      fprintf(stderr, "WARNING: Entry %s has no ISSN in \"issn\" section (issn=%s -> %s)\n" ,
              publication->keyword.c_str(), issn->value.c_str(), number.c_str());
      return;
   }
}


// ###### Unify "year"/"month"/"day" sections ###############################
void unifyDate(Node* publication, Node* year, Node* month, Node* day)
{
   int yearNumber = 1;
   if(year != nullptr) {
      yearNumber = atol(year->value.c_str());
      if((yearNumber < 1700) || (yearNumber > 2030)) {
         fprintf(stderr, "WARNING: Entry %s has probably invalid \"year\" section (year=%d?)!\n" ,
                 publication->keyword.c_str(), yearNumber);
      }
      year->number = yearNumber;
      year->value  = format("%04d", yearNumber);
   }
   else {
      fprintf(stderr, "WARNING: Entry %s has no \"year\" section, but \"month\" or \"day\"!\n" ,
              publication->keyword.c_str());
   }

   int monthNumber = 0;
   int maxDays     = 0;
   if(month != nullptr) {
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
                 publication->keyword.c_str(), month->value.c_str());
      }
      month->number = monthNumber;
      month->value  = format("%02d", monthNumber);
   }

   if(day != nullptr) {
      day->number = atol(day->value.c_str());
      if(month == nullptr) {
         fprintf(stderr, "WARNING: Entry %s has no \"month\" section, but \"day\"!\n" ,
                 publication->keyword.c_str());
      }
      else {
         if((day->number < 1) || (day->number > maxDays)) {
            fprintf(stderr, "WARNING: Entry %s has invalid \"day\" or \"month\" section (year=%d month=%d day=%d)!\n" ,
                    publication->keyword.c_str(), yearNumber, monthNumber, day->number);
         }
      }
      day->value = format("%04d", day->number);
   }
}


// ###### Unify "url" section ###############################################
void unifyURL(Node* publication, Node* url)
{
   // ====== Remove deprecated \url{...} ====================================
   if( (url->value.substr(0, 5) == "\\url{") &&
       (url->value.substr(url->value.size() - 1) == "}") ) {
      url->value = url->value.substr(5, url->value.size() - 6);
   }

   // ====== Fix IEEExplore URLs ============================================
   if( (url->value.substr(0, 27) == "http://ieeexplore.ieee.org/") ||
       (url->value.substr(0, 28) == "https://ieeexplore.ieee.org/") ) {
      const size_t is = url->value.find("&isnumber=");
      if(is == std::string::npos) {   // URL would otherwise not point to PDF download!
         url->value += "&isnumber=";
      }
   }

   url->value = laTeXtoURL(url->value);
}


// ###### Unify "pages" section #############################################
void unifyPages(Node* publication, Node* pages)
{
   // ====== Get pure numbers ===============================================
   std::string numbers = "";
   size_t      length  = pages->value.size();
   for(size_t i = 0; i < length; i++) {
      if((pages->value[i] < 0) && (i + 1 < length)) {
         i++;
      }
      else if( (pages->value[i] >= '0') &&
               (pages->value[i] <= '9') ) {
         numbers += pages->value[i];
      }
      else if(pages->value[i] == '-') {
         numbers += ' ';
      }
      else {
         fprintf(stderr, "WARNING: Entry %s has invalid characters in \"pages\" section (pages=%s)!\n" ,
                 publication->keyword.c_str(), pages->value.c_str());
         return;
      }
   }

   unsigned int a;
   unsigned int b;
   if(sscanf(numbers.c_str(), "%u %u", &a, &b) != 2) {
      if(sscanf(numbers.c_str(), "%u", &a) != 1) {
         a = b = 0;
      }
      else {
         b = a;
      }
   }
   if((a != 0) && (a <= b)) {
      char pagesString[64];
      if(a != b) {
         snprintf((char*)&pagesString, sizeof(pagesString), "%u--%u", a, b);
      }
      else {
         snprintf((char*)&pagesString, sizeof(pagesString), "%u", a);
      }
      pages->value = pagesString;

      Node* numpages = findChildNode(publication, "numpages");
      if(numpages) {
         unsigned int n = atol(numpages->value.c_str());
         if(n != 1 + (b - a)) {
            fprintf(stderr, "WARNING: Entry %s has inconsistent invalid page numbers and number of pages (pages=%s; numpages=%s)!\n" ,
                    publication->keyword.c_str(), pages->value.c_str(), numpages->value.c_str());
         }
      }
      addOrUpdateChildNode(publication, "numpages", format("%u", 1 + (b - a)).c_str());
   }
   else {
      fprintf(stderr, "WARNING: Entry %s has possibly invalid page numbers in \"pages\" section (pages=%s)!\n" ,
              publication->keyword.c_str(), pages->value.c_str());
   }
}


// ###### Unify "numpages" section #############################################
void unifyNumPages(Node* publication, Node* numpages)
{
   const unsigned int numberOfPages = atol(numpages->value.c_str());
   if( (numberOfPages < 1) || (numberOfPages >= 999999) ) {
      fprintf(stderr, "WARNING: Entry %s has invalid page of numbers in \"numpages\" section (numpages=%s)!\n" ,
              publication->keyword.c_str(), numpages->value.c_str());
   }
   numpages->value = format("%u", numberOfPages);
}
