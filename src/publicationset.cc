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
#include <math.h>

#include <algorithm>
#include <string>
#include <vector>

#include "publicationset.h"


// ###### Constructor #######################################################
PublicationSet::PublicationSet(const size_t maxSize)
{
   maxEntries = maxSize;
   publicationArray = new Node*[maxEntries];
   assert(publicationArray != nullptr);
   clearAll();
}


// ###### Destructor ########################################################
PublicationSet::~PublicationSet()
{
   delete [] publicationArray;
   maxEntries = 0;
   entries    = 0;
}


// ###### Clear complete set ################################################
void PublicationSet::clearAll()
{
   entries = 0;
   for(size_t i = 0;i < maxEntries; i++) {
      publicationArray[i] = nullptr;
   }
}


// ###### Add a single node #################################################
bool PublicationSet::add(Node* publication)
{
   assert(entries + 1 <= maxEntries);
   for(size_t i = 0; i < entries; i++) {
      if(publicationArray[entries] == publication) {
         return(false);
      }
   }
   publicationArray[entries] = publication;
   entries++;
   return(true);
}


// ###### Add all nodes from collection #####################################
void PublicationSet::addAll(Node* publication)
{
   while(publication != nullptr) {
      if(add(publication)) {
         publication->anchor = publication->keyword;
      }
      publication = publication->next;
   }
}


// NOTE: PublicationSet::sort() will *NOT* be thread-safe!
static const std::string* gSortKey       = nullptr;
static const bool*        gSortAscending = nullptr;
static size_t             gMaxSortLevels = 0;

// ###### Node comparison function for qsort() ##############################
static int publicationNodeComparisonFunction(const void* ptr1, const void* ptr2)
{
   const Node* node1 = *((const Node**)ptr1);
   const Node* node2 = *((const Node**)ptr2);

   for(size_t i = 0; i < gMaxSortLevels; i++) {
      const Node* child1 = findChildNode((Node*)node1, gSortKey[i].c_str());
      const Node* child2 = findChildNode((Node*)node2, gSortKey[i].c_str());
      int result = 0;
      if( (child1 == nullptr) && (child2 != nullptr) ) {
         result = 1;
      }
      if( (child1 != nullptr) && (child2 == nullptr) ) {
         result = -1;
      }
      else if( (child1 != nullptr) && (child2 != nullptr) ) {
         if(child1->value < child2->value) {
            result = -1;
         }
         else if(child1->value > child2->value) {
            result = 1;
         }
      }

      if(!gSortAscending[i]) {
         result *= -1;
      }

      if(result != 0) {
         return(result);
      }
   }
   return(0);
}

// ###### Sort publications #################################################
void PublicationSet::sort(const std::string* sortKey,
                          const bool*        sortAscending,
                          const size_t       maxSortLevels)
{
   gMaxSortLevels = maxSortLevels;
   gSortKey       = sortKey;
   gSortAscending = sortAscending;

   qsort(publicationArray, entries, sizeof(Node*), publicationNodeComparisonFunction);

   gSortKey       = nullptr;
   gSortAscending = nullptr;
}



// ###### Generate name for file download ###################################
std::string PublicationSet::makeDownloadFileName(const char*        downloadDirectory,
                                                 const std::string& anchor,
                                                 const std::string& mimeString)
{
   std::string extension = "data";
   if(mimeString == "application/pdf") {
      extension = ".pdf";
   }
   else if(mimeString == "application/xml") {
      extension = ".xml";
   }
   else if(mimeString == "text/html") {
      extension = ".html";
   }
   else if(mimeString == "text/plain") {
      extension = ".txt";
   }

   if( (downloadDirectory != nullptr) && (strlen(downloadDirectory) != 0) ) {
      return((std::string)downloadDirectory + "/" + anchor + extension);
   }
   return(anchor + extension);
}


// ###### Export to BibTeX ##################################################
bool PublicationSet::exportPublicationSetToBibTeX(PublicationSet* publicationSet,
                                                  const char*     fileNamePrefix,
                                                  const bool      separateFiles,
                                                  const bool      skipNotesWithISBNandISSN,
                                                  const bool      addNotesWithISBNandISSN,
                                                  const bool      addUrlCommand)
{
   FILE* fh = nullptr;
   if(!separateFiles) {
      fh = fopen(fileNamePrefix, "w");
      if(fh == nullptr) {
         fprintf(stderr, "ERROR: Unable to create BibTeX file %s!\n", fileNamePrefix);
         return(false);
      }
   }

   for(size_t index = 0; index < publicationSet->size(); index++) {
      const Node* publication = publicationSet->get(index);
      if(publication->value == "Comment") {
         if(fh != nullptr) {
            fprintf(fh, "%%%s\n\n", publication->keyword.c_str());
         }
      }
      else {
         if(separateFiles) {
            char fileName[1024];
            snprintf((char*)&fileName, sizeof(fileName), "%s%s.bib", fileNamePrefix, publication->keyword.c_str());
            fh = fopen(fileName, "w");
            if(fh == nullptr) {
               fprintf(stderr, "ERROR: Unable to create XML file %s!\n", fileName);
               return(false);
            }
         }

         fprintf(fh, "@%s{ %s,\n", publication->value.c_str(),
                                   publication->keyword.c_str());

         bool  empty           = true;
         Node* child           = publication->child;
         const Node* issn      = nullptr;
         const Node* isbn      = nullptr;
         const char* separator = "";
         while(child != nullptr) {
            if(!empty) {
               separator = ",\n";
            }
            empty = false;

            if( (child->keyword == "title")     ||
                (child->keyword == "booktitle") ||
                (child->keyword == "series")    ||
                (child->keyword == "journal")   ||
                (child->keyword == "abstract") ) {
               fprintf(fh, "%s\t%s = \"{%s}\"", separator, child->keyword.c_str(), child->value.c_str());
            }
            else if( (child->keyword == "day") ||
                     (child->keyword == "year") ) {
               fprintf(fh, "%s\t%s = \"%u\"", separator, child->keyword.c_str(), child->number);
            }
            else if( (child->keyword == "month") ) {
               static const char* bibtexMonthNames[12] = {"jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"};
               if((child->number >= 1) && (child->number <= 12)) {
                  fprintf(fh, "%s\t%s = %s", separator, child->keyword.c_str(), bibtexMonthNames[child->number - 1]);
               }
            }
            else if( (child->keyword == "url") ) {
               if(addUrlCommand) {
                  fprintf(fh, "%s\t%s = \"\\url{%s}\"", separator, child->keyword.c_str(), urlToLaTeX(child->value).c_str());
               }
               else {
                  fprintf(fh, "%s\t%s = \"%s\"", separator, child->keyword.c_str(), urlToLaTeX(child->value).c_str());
               }
            }
            else if( (child->keyword == "doi") ) {
               fprintf(fh, "%s\t%s = \"%s\"", separator, child->keyword.c_str(), urlToLaTeX(child->value).c_str());
            }
            else if( (child->keyword == "note") ) {
               if( (skipNotesWithISBNandISSN == false) ||
                   ((strncmp(child->value.c_str(), "ISBN", 4) != 0) &&
                    (strncmp(child->value.c_str(), "ISSN", 4) != 0) &&
                    (strncmp(child->value.c_str(), "{ISBN}", 6) != 0) &&
                    (strncmp(child->value.c_str(), "{ISSN}", 6) != 0)) ) {
                  fprintf(fh, "%s\t%s = \"%s\"", separator, child->keyword.c_str(), child->value.c_str());
               }
            }
            else if( (child->keyword == "removeme") ) {
               // Skip this entry. Useful for combining BibTeXConv with "sed" filtering.
            }
            else {
               if(child->keyword == "isbn") {
                  isbn = child;
               }
               else if(child->keyword == "issn") {
                  issn = child;
               }
               fprintf(fh, "%s\t%s = \"%s\"", separator, child->keyword.c_str(), child->value.c_str());
            }
            child = child->next;
         }

         if( (addNotesWithISBNandISSN) &&
             ((isbn != nullptr) || (issn != nullptr)) ) {
            if(isbn) {
               fprintf(fh, "%s\tnote = \"{ISBN} %s\"", separator, isbn->value.c_str());
            }
            else if(issn) {
               fprintf(fh, "%s\tnote = \"{ISSN} %s\"", separator, issn->value.c_str());
            }
         }

         fputs("\n}\n\n", fh);
      }

      if( (separateFiles) && (fh != nullptr)) {
         fclose(fh);
         fh = nullptr;
      }
   }

   if(!separateFiles) {
      fclose(fh);
   }
   return(true);
}


// ###### Export to XML #####################################################
bool PublicationSet::exportPublicationSetToXML(PublicationSet* publicationSet,
                                               const char*     fileNamePrefix,
                                               const bool      separateFiles)
{
   FILE* fh = nullptr;
   if(!separateFiles) {
      fh = fopen(fileNamePrefix, "w");
      if(fh == nullptr) {
         fprintf(stderr, "ERROR: Unable to create XML file %s!\n", fileNamePrefix);
         return(false);
      }
      fputs("<?xml version='1.0' encoding='UTF-8'?>\n", fh);
      fputs("<!DOCTYPE rfc PUBLIC '-//IETF//DTD RFC 2629//EN' 'http://xml.resource.org/authoring/rfc2629.dtd'>\n", fh);
   }

   for(size_t index = 0; index < publicationSet->size(); index++) {
      Node* publication = publicationSet->get(index);

      if(publication->value == "Comment") {
         if(fh != nullptr) {
            fprintf(fh, "<!-- %s -->\n\n", publication->keyword.c_str());
         }
      }
      else {
         if(separateFiles) {
            char fileName[1024];
            snprintf((char*)&fileName, sizeof(fileName), "%s%s.xml", fileNamePrefix, publication->keyword.c_str());
            fh = fopen(fileName, "w");
            if(fh == nullptr) {
               fprintf(stderr, "ERROR: Unable to create XML file %s!\n", fileName);
               return(false);
            }
            fputs("<?xml version='1.0' encoding='UTF-8'?>\n", fh);
            fputs("<!DOCTYPE rfc PUBLIC '-//IETF//DTD RFC 2629//EN' 'http://xml.resource.org/authoring/rfc2629.dtd'>\n", fh);
         }

         const Node* title        = findChildNode(publication, "title");
         const Node* author       = findChildNode(publication, "author");
         const Node* year         = findChildNode(publication, "year");
         const Node* month        = findChildNode(publication, "month");
         const Node* day          = findChildNode(publication, "day");
         const Node* url          = findChildNode(publication, "url");
         const Node* urlMime      = findChildNode(publication, "url.mime");
         const Node* urlSize      = findChildNode(publication, "url.size");
         const Node* type         = findChildNode(publication, "type");
         const Node* howpublished = findChildNode(publication, "howpublished");
         const Node* booktitle    = findChildNode(publication, "booktitle");
         const Node* journal      = findChildNode(publication, "journal");
         const Node* volume       = findChildNode(publication, "volume");
         const Node* number       = findChildNode(publication, "number");
         const Node* pages        = findChildNode(publication, "pages");
         const Node* isbn         = findChildNode(publication, "isbn");
         const Node* issn         = findChildNode(publication, "issn");
         const Node* doi          = findChildNode(publication, "doi");

         if(url == nullptr) {
            fprintf(fh, "<reference anchor=\"%s\">\n",
                    labelToXMLLabel(publication->keyword).c_str());
         }
         else {
            fprintf(fh, "<reference anchor=\"%s\" target=\"%s\">\n",
                    labelToXMLLabel(publication->keyword).c_str(),
                    url->value.c_str());
         }
         fputs("\t<front>\n", fh);
         if(title) {
            fprintf(fh, "\t\t<title>%s</title>\n", string2xml(title->value).c_str());
         }
         if(author) {
            for(size_t authorIndex = 0; authorIndex < author->arguments.size(); authorIndex += 3) {
               std::string familyName = author->arguments[authorIndex + 0];
               std::string givenName  = author->arguments[authorIndex + 1];
               std::string initials   = author->arguments[authorIndex + 2];
               removeBrackets(familyName);
               removeBrackets(givenName);
               removeBrackets(initials);
               fprintf(fh,
                  "\t\t<author initials=\"%s\" surname=\"%s\" fullname=\"%s\" />\n",
                  string2xml(initials).c_str(), string2xml(familyName).c_str(),
                  string2xml(givenName +
                             ((givenName != "") ? "~" : "") +
                             familyName).c_str());
            }
         }
         if(year || month || day) {
            fputs("\t\t<date ", fh);
            if(day) {
               fprintf(fh, "day=\"%u\" ", day->number);
            }
            if(month) {
               static const char* xmlMonthNames[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
               if((month->number >= 1) && (month->number <= 12)) {
                  fprintf(fh, "month=\"%s\" ", xmlMonthNames[month->number - 1]);
               }
            }
            if(year) {
               fprintf(fh, "year=\"%u\" ", year->number);
            }
            fputs("/>\n", fh);
         }
         fputs("\t</front>\n", fh);

         std::string seriesName  = "";
         std::string seriesValue = "";
         if(howpublished) {
            seriesName = howpublished->value;
         }
         if(booktitle) {
            seriesName = booktitle->value;
         }
         if(journal) {
            seriesName = journal->value;
         }
         if(type) {
            seriesName = type->value;
            if(number) {
               if(seriesValue != "") {
                  seriesValue += ", ";
               }
               seriesValue += number->value;
               number = nullptr;
            }
         }
         if(number) {
            if(seriesValue != "") {
               seriesValue += ", ";
            }
            seriesValue += "Number " + number->value;
         }
         if(volume) {
            if(seriesValue != "") {
               seriesValue += ", ";
            }
            seriesValue += "Volume " + volume->value;
         }
         if(pages) {
            if(seriesValue != "") {
               seriesValue += ", ";
            }
            seriesValue += "Pages " + pages->value;
         }
         if(issn) {
            if(seriesValue != "") {
               seriesValue += ", ";
            }
            seriesValue += "ISSN~" + issn->value;
         }
         if(isbn) {
            if(seriesValue != "") {
               seriesValue += ", ";
            }
            seriesValue += "ISBN~" + isbn->value;
         }
         if(doi) {
            if(seriesValue != "") {
               seriesValue += ", ";
            }
            seriesValue += "DOI~" + doi->value;
         }
         if((seriesName != "") || (seriesValue != "")) {
            if(seriesValue == "") {
               // This would produce an ugly space.
               seriesValue = seriesName;
               seriesName  = "";
            }
            fprintf(fh, "\t<seriesInfo name=\"%s\" value=\"%s\" />\n",
                    string2xml(seriesName).c_str(),
                    string2xml(seriesValue).c_str());
         }

         if(url) {
            std::string type = "";
            if(urlMime) {
               const size_t slash = urlMime->value.find("/");
               if(slash != std::string::npos) {
                  type = urlMime->value.substr(slash + 1, urlMime->value.size() - slash);
                  std::transform(type.begin(), type.end(), type.begin(),
                                 (int(*)(int))std::toupper);
                  if(type == "PLAIN") {
                     type = "TXT";
                  }
               }
            }
            type = " type=\"" + type + "\"";

            std::string octets = "";
            if(urlSize) {
               octets = format(" octets=\"%u\"", atol(urlSize->value.c_str()));
            }

            fprintf(fh, "\t<format%s%s target=\"%s\" />\n",
                    type.c_str(), octets.c_str(),
                    url->value.c_str());
         }
         fputs("</reference>\n\n", fh);
      }

      if( (separateFiles) && (fh != nullptr)) {
         fclose(fh);
         fh = nullptr;
      }
   }

   if(!separateFiles) {
      fclose(fh);
   }
   return(true);
}


struct StackEntry {
   size_t pos;
   bool   skip;
};

// NOTE: PublicationSet::applyTemplate() will *NOT* be thread-safe!
static unsigned int gNumber      = 0;
static unsigned int gTotalNumber = 0;


// ###### Get next action ##################################################
inline static std::string getNextAction(const char* inputString, size_t& counter)
{
   std::string result;
   if(inputString[0] == '{') {
      std::string input((const char*)&inputString[1]);
      result = extractToken(input, std::string("}"));
      std::transform(result.begin(), result.end(), result.begin(), ::tolower);
      counter += result.size() + 1;
   }
   else {
      char str[2];
      str[0] = inputString[0];
      str[1] = 0x00;
      result = std::string(str);
   }
   // fprintf(stderr,"R=<%s>\n",result.c_str());
   return(result);
}


// ###### Apply printing template to publication ############################
std::string PublicationSet::applyTemplate(Node*                           publication,
                                          Node*                           prevPublication,
                                          Node*                           nextPublication,
                                          const char*                     namingTemplate,
                                          const std::string&              printingTemplate,
                                          const std::vector<std::string>& monthNames,
                                          const std::string&              nbsp,
                                          const std::string&              lineBreak,
                                          const bool                      xmlStyle,
                                          const char*                     downloadDirectory,
                                          const Mappings&                 mappings,
                                          FILE*                           fh)
{
   std::string             result;
   std::vector<StackEntry> stack;
   Node*                   child;
   Node*                   author               = nullptr;
   size_t                  authorIndex          = 0;
   size_t                  authorBegin          = std::string::npos;
   bool                    skip                 = false;
   const size_t            printingTemplateSize = printingTemplate.size();
   std::string             type                 = std::string("");

   gNumber++;
   gTotalNumber++;

   for(size_t i = 0; i < printingTemplateSize; i++) {
      if( (printingTemplate[i] == '%') && (i + 1 < printingTemplateSize) ) {
         const std::string action = getNextAction((const char*)&printingTemplate[i + 1], i);
         if( (action == "L") || (action == "label") ) {   // Original BibTeX label
            result += string2utf8(publication->keyword, nbsp, lineBreak, xmlStyle);
         }
         else if(action == "html-label") {   // Original BibTeX label
            result += labelToHTMLLabel(string2utf8(publication->keyword, nbsp, lineBreak, xmlStyle));
         }
         else if( (action == "C") || (action == "anchor") ) {   // Anchor
            result += string2utf8(publication->anchor, nbsp, lineBreak, xmlStyle);
         }
         else if( (action == "c") || (action == "class") ) {   // Class (e.g. TechReport, InProceedings, etc.)
            result += string2utf8(publication->value, nbsp, lineBreak, xmlStyle);
         }
         else if( (action == "Z") || (action == "name") ) {   // Name based on naming template
            size_t p;
            size_t begin      = 0;
            size_t len        = strlen(namingTemplate);
            bool   inTemplate = false;
            for(p = 0; p < len; p++) {
               if(inTemplate == false) {
                  if(namingTemplate[p] == '%') {
                     inTemplate = true;
                     char str[p + 1];
                     if(p > begin) {
                        memcpy((char*)&str, &namingTemplate[begin], p - begin);
                     }
                     str[p - begin] = 0x00;
                     result += string2utf8(str, nbsp, lineBreak, xmlStyle);
                     begin = p + 1;
                  }
               }
               else {
                  if(namingTemplate[p] == '%') {
                     result += "%";
                     inTemplate = false;
                     begin      = p + 1;
                  }
                  else if(isdigit(namingTemplate[p])) {
                     // Number
                  }
                  else if( (namingTemplate[p] == 'n') ||
                           (namingTemplate[p] == 'N') ) {
                     char str[p + 3];
                     str[0] = '%';
                     if(p > begin) {
                        memcpy((char*)&str[1], &namingTemplate[begin], p - begin);
                     }
                     str[p - begin + 1] = 'u';
                     str[p - begin + 2] = 0x00;
                     if(namingTemplate[p] == 'n') {
                        result += format(str, gNumber);
                     }
                     else if(namingTemplate[p] == 'N') {
                        result += format(str, gTotalNumber);
                     }
                     inTemplate = false;
                     begin      = p + 1;
                  }
                  else {
                     fprintf(stderr, "ERROR: Bad naming template \"%s\"!\n", namingTemplate);
                     exit(1);
                  }
               }
            }
            if(begin < p) {
               result += string2utf8(&namingTemplate[begin], nbsp, lineBreak, xmlStyle);
            }
         }
         else if( (action == "#") || (action == "download-file-name") ) {   // Download file name
            child = findChildNode(publication, "url.mime");
            result += makeDownloadFileName(downloadDirectory, publication->keyword,
                                           (child != nullptr) ? child->value : "");
         }
         else if( (action == "a") || (action == "begin-author-loop") ) {   // Author LOOP BEGIN
            if(authorBegin != std::string::npos) {
               fputs("ERROR: Unexpected author loop begin %a -> an author loop is still open!\n", stderr);
               exit(1);
            }
            author      = findChildNode(publication, "author");
            authorIndex = 0;
            authorBegin = i;
         }
         else if( (action == "g") || (action == "author-initials") ) {   // Current author given name initials
            if(author) {
               std::string initials   = author->arguments[authorIndex + 2];
               removeBrackets(initials);
               if(initials != "") {
                  result += string2utf8(initials, nbsp, lineBreak, xmlStyle);
               }
               else {
                  skip = true;
               }
            }
         }
         else if( (action == "G") || (action == "author-give-name") ) {   // Current author given name
            if(author) {
               std::string givenName  = author->arguments[authorIndex + 1];
               removeBrackets(givenName);
               if(givenName != "") {
                  result += string2utf8(givenName, nbsp, lineBreak, xmlStyle);
               }
               else {
                  skip = true;
               }
            }
         }
         else if( (action == "F") || (action == "author-family-name") ) {   // Current author family name
            if(author) {
               std::string familyName = author->arguments[authorIndex + 0];
               removeBrackets(familyName);
               result += string2utf8(familyName, nbsp, lineBreak, xmlStyle);
            }
         }
         else if( (action.substr(0, 3) == "is?") ||
                  (action.substr(0, 7) == "is-not?") ||
                  (action.substr(0, 13) == "is-less-than?") ||
                  (action.substr(0, 22) == "is-less-than-or-equal?") ||
                  (action.substr(0, 16) == "is-greater-than?") ||
                  (action.substr(0, 25) == "is-greater-than-or-equal?") ) {   // IS string
            if(i + 1 < printingTemplateSize) {
               StackEntry        entry         = stack.back();
               const std::string writtenString = result.substr(entry.pos);

               if(skip == true) {
                   // Text will already be skipped ...
               }
               else if(action.substr(0, 3) == "is?") {
                  const std::string comparisonString = action.substr(3);
                  skip = ! (writtenString == comparisonString);
               }
               else if(action.substr(0, 7) == "is-not?") {
                  const std::string comparisonString = action.substr(7);
                  skip = ! (writtenString != comparisonString);
               }
               else if(action.substr(0, 13) < "is-less-than?") {
                  const std::string comparisonString = action.substr(13);
                  skip = ! (writtenString == comparisonString);
               }
               else if(action.substr(0, 22) <= "is-less-than-or-equal?") {
                  const std::string comparisonString = action.substr(22);
                  skip = ! (writtenString == comparisonString);
               }
               else if(action.substr(0, 16) > "is-greater-than?") {
                  const std::string comparisonString = action.substr(16);
                  skip = ! (writtenString == comparisonString);
               }
               else if(action.substr(0, 25) >= "is-greater-than-or-equal?") {
                  const std::string comparisonString = action.substr(25);
                  skip = ! (writtenString == comparisonString);
               }

               result.erase(entry.pos);   // Remove the written "test" string.
            }
         }
         else if( (action == "f") || (action == "is-first-author?") ) {       // IS first author
            if(skip == false) {
               skip = ! (authorIndex == 0);
            }
         }
         else if( (action == "n") || (action == "is-not-first-author?") ) {   // IS not first author
            if(skip == false) {
               skip = ! ((author != nullptr) && (authorIndex > 0));
            }
         }
         else if( (action == "l") || (action == "is-last-author?") ) {        // IS last author
            if(skip == false) {
               skip = ! ((author != nullptr) && (authorIndex + 3 >= author->arguments.size()));
            }
         }
         else if( (action == "A") || (action == "end-author-loop") ) {   // Author LOOP EBD
            if(authorBegin == std::string::npos) {
               fputs("ERROR: Unexpected author loop end %A -> %a author loop begin needed first!\n", stderr);
               exit(1);
            }
            authorIndex += 3;
            if( (author != nullptr) && (authorIndex < author->arguments.size()) ) {
               i = authorBegin;
            }
            else {
               author      = nullptr;
               authorIndex = 0;
            }
         }
         else if( (action == "T") || (action == "title") ) {   // Title
            child = findChildNode(publication, "title");
            if(child) { result += string2utf8(child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if( (action == "H") || (action == "how-published") ) {   // HowPublished
            child = findChildNode(publication, "howpublished");
            if(child) { result += string2utf8(child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if( (action == "B") || (action == "booktitle") ) {   // Booktitle
            child = findChildNode(publication, "booktitle");
            if(child) { result += string2utf8(child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if( (action == "r") || (action == "series") ) {   // Series
            child = findChildNode(publication, "series");
            if(child) { result += string2utf8(child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if( (action == "J") || (action == "journal") ) {   // Journal
            child = findChildNode(publication, "journal");
            if(child) { result += string2utf8(child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if( (action == "E") || (action == "edition") ) {   // Edition
            child = findChildNode(publication, "edition");
            if(child) { result += string2utf8(child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if( (action == "V") || (action == "volume") ) {   // Volume
            child = findChildNode(publication, "volume");
            if(child) { result += string2utf8(child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if( (action == "t") || (action == "type") ) {   // Type
            child = findChildNode(publication, "type");
            if(child) { result += string2utf8(child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if( (action == "N") || (action == "number") ) {   // Number
            child = findChildNode(publication, "number");
            if(child) { result += string2utf8(child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if( (action == "P") || (action == "pages") ) {   // Pages
            child = findChildNode(publication, "pages");
            if(child) { result += string2utf8(child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if(action == "numpages") {   // Number of pages
            child = findChildNode(publication, "numpages");
            if(child) { result += string2utf8(child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if( (action == "@") || (action == "address") ) {   // Address
            child = findChildNode(publication, "address");
            if(child) { result += string2utf8(child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if( (action == "Y") || (action == "year") ) {   // Year
            child = findChildNode(publication, "year");
            if(child) { result += string2utf8(child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if( (action == "M") || (action == "month-name") ) {   // Month as name
            child = findChildNode(publication, "month");
            if(child) {
               if( (child->number >= 1) && (child->number <= 12) ) {
                  result += string2utf8(monthNames[child->number - 1], nbsp, lineBreak, xmlStyle);
               } else { skip = true; }
            } else { skip = true; }
         }
         else if( (action == "m") || (action == "month-number") ) {   // Month as number
            child = findChildNode(publication, "month");
            if(child) {
               char month[16];
               snprintf((char*)&month, sizeof(month), "%d", child->number);
               result += string2utf8(month, nbsp, lineBreak, xmlStyle);
            } else { skip = true; }
         }
         else if( (action == "D") || (action == "day") ) {   // Day
            child = findChildNode(publication, "day");
            if(child) {
               char day[16];
               snprintf((char*)&day, sizeof(day), "%d", child->number);
               result += string2utf8(day, nbsp, lineBreak, xmlStyle);
            } else { skip = true; }
         }
         else if( (action == "$") || (action == "publisher") ) {   // Publisher
            child = findChildNode(publication, "publisher");
            if(child) { result += string2utf8(child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if( (action == "S") || (action == "school") ) {   // School
            child = findChildNode(publication, "school");
            if(child) { result += string2utf8(child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if( (action == "?") || (action == "institution") ) {   // Institution
            child = findChildNode(publication, "institution");
            if(child) { result += string2utf8(child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if( (action == "I") || (action == "isbn") ) {   // ISBN
            child = findChildNode(publication, "isbn");
            if(child) { result += string2utf8("ISBN~" + child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if( (action == "i") || (action == "issn") ) {   // ISSN
            child = findChildNode(publication, "issn");
            if(child) { result += string2utf8("ISSN~" + child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if( (action == "x") || (action == "language") ) {   // Language
            child = findChildNode(publication, "language");
            if(child) { result += string2utf8(child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if( (action == "O") || (action == "content-language") ) {   // Content Language
            child = findChildNode(publication, "content-language");
            if(child == nullptr) {   // No content language -> try same as "language" instead:
               child = findChildNode(publication, "language");
            }
            if(child) { result += string2utf8(child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if( (action == "x") || (action == "xml-language") ) {   // Language
            child = findChildNode(publication, "language");
            if(child) {
               const char* language = getXMLLanguageFromLaTeX(child->value.c_str());
               if(language != nullptr) {
                  result += std::string(language);
               } else { skip = true; }
            } else { skip = true; }
         }
         else if( (action == "U") || (action == "url") ) {   // URL
            child = findChildNode(publication, "url");
            if(child) { result += string2utf8(child->value, "", "", xmlStyle); } else { skip = true; }
         }
         else if( (action == "d") || (action == "doi") ) {   // DOI
            child = findChildNode(publication, "doi");
            if(child) { result += string2utf8(child->value, "", "", xmlStyle); } else { skip = true; }
         }
         else if( (action == "q") || (action == "urn") ) {   // URN
            child = findChildNode(publication, "urn");
            if(child) { result += string2utf8(child->value, "", "", xmlStyle); } else { skip = true; }
         }
         else if(action == "keywords") {   // Keywords
            child = findChildNode(publication, "keywords");
            if(child) { result += string2utf8(child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if(action == "abstract") {   // Abstract
            child = findChildNode(publication, "abstract");
            if(child) { result += string2utf8(child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if(action == "url-md5") {   // MD5
            child = findChildNode(publication, "url.md5");
            if(child) { result += string2utf8(child->value, nbsp, lineBreak, xmlStyle); } else { skip = true; }
         }
         else if( (action == "z") || (action == "url-mime") ) {   // URL mime type
            child = findChildNode(publication, "url.mime");
            if(child) { result += string2utf8(child->value, "", "", xmlStyle); } else { skip = true; }
         }
         else if( (action == "y") || (action == "url-type") ) {   // URL type
            child = findChildNode(publication, "url.mime");
            if(child) {
               if(child->value == "application/pdf") {
                  result += "PDF";
               }
               else if(child->value == "application/xml") {
                  result += "XML";
               }
               else if(child->value == "text/html") {
                  result += "HTML";
               }
               else if(child->value == "text/plain") {
                  result += "TXT";
               }
               else if(child->value == "image/svg+xml") {
                  result += "SVG";
               }
               else {
                  result += child->value;
               }
            } else { skip = true; }
         }
         else if( (action == "s") || (hasPrefix(action, "url-size-",  type)) ) {   // URL size
            if((action.size() == 1) && (i + 2 < printingTemplateSize)) {
               switch(printingTemplate[i + 2]) {
                  case 'K':   // KiB
                     type = "kib";
                   break;
                  case 'M':   // MiB
                     type = "mib";
                   break;
                  default:   // Bytes
                     type = "";
                   break;
               }
               i++;
            }
            child = findChildNode(publication, "url.size");
            if( (child) && (atoll(child->value.c_str()) != 0) ) {
               double divisor;
               if(type == "kib") {
                  divisor = 1024.0;
               }
               else if(type == "mib") {
                  divisor = 1024.0 * 1024.0;
               }
               else {
                  divisor = 1.0;
               }
               result += string2utf8(format("%1.0f", ceil(atoll(child->value.c_str()) / divisor)), nbsp, lineBreak, xmlStyle);
            }
            else { skip = true; }
         }
         else if( (action == "X") || (action == "note") ) {   // Note
            child = findChildNode(publication, "note");
            if(child) {
               if( (strncmp(child->value.c_str(), "ISBN", 4) == 0) ||
                   (strncmp(child->value.c_str(), "ISSN", 4) == 0) ||
                   (strncmp(child->value.c_str(), "{ISBN}", 6) == 0) ||
                   (strncmp(child->value.c_str(), "{ISSN}", 6) == 0) ) {
                  skip = true;
               }
               else {
                  result += string2utf8(child->value, nbsp, lineBreak, xmlStyle);
               }
            } else { skip = true; }
         }
         else if(action == "%") {   // %
            result += '%';
         }
         else if( (action == "b") || (hasPrefix(action, "begin-subdivision-",  type)) ||
                  (action == "w") || (hasPrefix(action, "within-subdivision-", type)) ||
                  (action == "e") || (hasPrefix(action, "end-subdivision-",    type)) ) {   // Begin/Within/End of subdivision
            if(i + 2 < printingTemplateSize) {
               if ((action.size() == 1) && (i + 2 < printingTemplateSize) ) {
                  switch(printingTemplate[i + 2]) {
                     case 'D':
                        type = "day";
                      break;
                     case 'm':
                     case 'M':
                        type = "month";
                      break;
                     case 'Y':
                        type = "year";
                      break;
                     default:
                        fprintf(stderr, "ERROR: Unexpected %% placeholder '%c' in subdivision part of custom printing template!\n",
                        printingTemplate[i + 2]);
                        exit(1);
                  }
                  i++;
               }
               else {
                  if( (type != "day") && (type != "month") && (type != "year") ) {
                     fprintf(stderr, "ERROR: Unexpected %% placeholder '%s' in subdivision part of custom printing template!\n",
                     action.c_str());
                     exit(1);
                  }
               }
               const Node* prevChild = (prevPublication != nullptr) ? findChildNode(prevPublication, type.c_str()) : nullptr;
               child                 = findChildNode(publication, type.c_str());
               const Node* nextChild = (nextPublication != nullptr) ? findChildNode(nextPublication, type.c_str()) : nullptr;

               bool begin = (prevChild == nullptr) ||
                           ( (prevChild != nullptr) && (child != nullptr) && (prevChild->value != child->value) );
               bool end = (nextChild == nullptr) ||
                           ( (child != nullptr) && (nextChild != nullptr) && (child->value != nextChild->value) );
               switch(action[0]) {
                  case 'b':
                     skip = ! begin;
                     break;
                  case 'w':
                     skip = (begin || end);
                     break;
                  case 'e':
                     skip = ! end;
                     break;
               }
            }
         }
         else if( (action == "1") || (action == "custom-1") ||
                  (action == "2") || (action == "custom-2") ||
                  (action == "3") || (action == "custom-3") ||
                  (action == "4") || (action == "custom-4") ||
                  (action == "5") || (action == "custom-5") ||
                  (action == "6") || (action == "custom-6") ||
                  (action == "7") || (action == "custom-7") ||
                  (action == "8") || (action == "custom-8") ||
                  (action == "9") || (action == "custom-9") ) {   // Custom #1..9
            const unsigned int id = action[action.size() - 1] - '1';
            if(publication->custom[id] != "") {
               result += string2utf8(publication->custom[id], nbsp, lineBreak, xmlStyle);
            }
            else {
               skip = true;
            }
         }
         else if( (action == "custom-1-as-is") ||
                  (action == "custom-2-as-is") ||
                  (action == "custom-3-as-is") ||
                  (action == "custom-4-as-is") ||
                  (action == "custom-5-as-is") ||
                  (action == "custom-6-as-is") ||
                  (action == "custom-7-as-is") ||
                  (action == "custom-8-as-is") ||
                  (action == "custom-9-as-is") ) {
            const unsigned int id = action[7] - '1';
            if(publication->custom[id] != "") {
               result += publication->custom[id];
            }
            else {
               skip = true;
            }
         }
         else if(action.substr(0, 4) == "map:") {   // Map from mappings
            // fputs("Y1\n", stderr);
            const std::string mappingName(action.substr(4));
            const MappingEntry* mappingEntry = mappings.findMapping(mappingName);
            if(mappingEntry == nullptr) {
               fprintf(stderr, "ERROR: Mapping \"%s\" does not exist! Forgot parameter \"--mapping %s:mapping_file:key_column:value_column\"?\n", mappingName.c_str(), mappingName.c_str());
               exit(1);
            }
            StackEntry        entry = stack.back();
            const std::string key  = result.substr(entry.pos);
            std::string       value;
            skip = !mappings.map(mappingEntry, key, value);
            if(!skip) {
               result.erase(entry.pos);   // Remove the written key string.
               result += value;
            }
         }
         else if(action == "exec") {   // Execute command and pipe in the result
            if(i + 1 < printingTemplateSize) {
               StackEntry        entry = stack.back();
               const std::string call  = result.substr(entry.pos);

               if(skip != true) {
                   // Text will already be skipped ...
                  result.erase(entry.pos);   // Remove the written "exec" string.

                  FILE* pipe = popen(call.c_str(), "r");
                  if(pipe == nullptr) {
                     fprintf(stderr, "Unable to run %s!\n", call.c_str());
                     exit(1);
                  }

                  skip = true;
                  char buffer[16384];
                  ssize_t inputBytes;
                  while( (inputBytes = fread((char*)&buffer, 1, sizeof(buffer) - 1, pipe)) > 0 ) {
                     if(inputBytes > 0) {
                         buffer[inputBytes] = 0x00;
                         result += buffer;
                         skip = false;
                     }
                     else {
                        fprintf(stderr, "Reading from run of %s failed!\n", call.c_str());
                        exit(1);
                     }
                  }

                  const int returnCode = pclose(pipe);
                  if(returnCode != 0) {
                     fprintf(stderr, "Run of %s failed with code %d!\n", call.c_str(), returnCode);
                     exit(1);
                  }
               }
            }
         }
         else {
            fprintf(stderr, "ERROR: Unexpected %% placeholder '%s' in custom printing template!\n",
                     action.c_str());
            exit(1);
         }
         i++;
      }
      else if( (printingTemplate[i] == '\\') && (i + 1 < printingTemplateSize) ) {
         switch(printingTemplate[i + 1]) {
            case 'n':
               result += '\n';
             break;
            case 't':
               result += '\t';
             break;
            default:
               result += printingTemplate[i + 1];
             break;
         }
         i++;
      }
      else if( (printingTemplate[i] == '[') ||
               (printingTemplate[i] == '(') ) {
         if(stack.empty()) {
            skip = false;   // Up to now, everything will be accepted
         }
         struct StackEntry entry = { result.size(), skip };
         stack.push_back(entry);
      }
      else if( (printingTemplate[i] == ']') ||
               (printingTemplate[i] == ')') ) {
         if(!stack.empty()) {
            StackEntry entry = stack.back();
            stack.pop_back();
            if(skip == true) {
               result.erase(entry.pos);
               if(printingTemplate[i] == ']') {
                  skip = entry.skip;
               }
            }
         }
         else {
            fputs("ERROR: Unexpected ']' in custom printing template!\n", stderr);
            exit(1);
         }
      }
      else if(printingTemplate[i] == '|') {
         if(!stack.empty()) {
            StackEntry entry = stack.back();
            stack.pop_back();
            // ====== Failed => try alternative ==========================
            if(skip == true) {
               result.erase(entry.pos);
               skip = entry.skip;
               stack.push_back(entry);
            }
            // ====== Successful => skip alternative(s) ==================
            else {
               skip = entry.skip;
               int levels = 1;
               for(   ; i < printingTemplateSize; i++) {
                  if(printingTemplate[i] == '\\') {
                     i++;
                  }
                  else {
                     if(printingTemplate[i] == '[') {
                        levels++;
                     }
                     if(printingTemplate[i] == ']') {
                        levels--;
                        if(levels == 0) {
                           break;
                        }
                     }
                  }
               }
            }
         }
         else {
            fputs("ERROR: Unexpected '|' in custom printing template!\n", stderr);
            exit(1);
         }
      }
      else {
         std::string character = "";

         if( ( (((unsigned char)printingTemplate[i]) & 0xE0) == 0xC0 ) &&
               (i + 1 < printingTemplateSize) ) {
            // Two-byte UTF-8 character
            character += printingTemplate[i];
            character += printingTemplate[++i];
         }
         else if( ( (((unsigned char)printingTemplate[i]) & 0xF0) == 0xE0 ) &&
                  (i + 2 < printingTemplateSize) ) {
            // Three-byte UTF-8 character
            character += printingTemplate[i];
            character += printingTemplate[++i];
            character += printingTemplate[++i];
         }
         else if( ( (((unsigned char)printingTemplate[i]) & 0xF8) == 0xF0 ) &&
                  (i + 3 < printingTemplateSize) ) {
            // Four-byte UTF-8 character
            character += printingTemplate[i];
            character += printingTemplate[++i];
            character += printingTemplate[++i];
            character += printingTemplate[++i];
         }
         else if( (((unsigned char)printingTemplate[i]) & 0x80) == 0 ) {
            // Regular 1-byte character
            character += printingTemplate[i];
         }
         else {
            // Invalid!
         }

         // Add current character. We may *not* use XML style encoding here,
         // since the character may be itself part of XML tags!
         result += string2utf8(character, nbsp);
      }
   }
   return(result);
}


// ###### Export to custom ##################################################
bool PublicationSet::exportPublicationSetToCustom(PublicationSet*                 publicationSet,
                                                  const char*                     namingTemplate,
                                                  const std::string&              customPrintingHeader,
                                                  const std::string&              customPrintingTrailer,
                                                  const std::string&              printingTemplate,
                                                  const std::vector<std::string>& monthNames,
                                                  const std::string&              nbsp,
                                                  const std::string&              lineBreak,
                                                  const bool                      xmlStyle,
                                                  const char*                     downloadDirectory,
                                                  const Mappings&                 mappings,
                                                  FILE*                           fh)
{
   Node* publication = nullptr;
   gNumber           = 0;
   for(size_t index = 0; index < publicationSet->size(); index++) {
      // ====== Get prev, current and next publications =====================
      if(publicationSet->get(index)->value == "Comment") {
         continue;
      }
      Node* prevPublication = publication;
      publication = publicationSet->get(index);
      size_t nextPublicationIndex = 1;
      Node* nextPublication = (index + nextPublicationIndex< publicationSet->size()) ? publicationSet->get(index + nextPublicationIndex) : nullptr;
      while( (nextPublication != nullptr) && (nextPublication->value == "Comment")) {
         nextPublicationIndex++;
         nextPublication = (index + nextPublicationIndex< publicationSet->size()) ? publicationSet->get(index + nextPublicationIndex) : nullptr;
      }

      const std::string result = applyTemplate(publication, prevPublication, nextPublication,
                                               namingTemplate,
                                               printingTemplate,
                                               monthNames, nbsp, lineBreak, xmlStyle,
                                               downloadDirectory,
                                               mappings,
                                               fh);

      fputs(string2utf8(processBackslash(customPrintingHeader), nbsp).c_str(), stdout);
      fputs(result.c_str(), stdout);
      fputs(string2utf8(processBackslash(customPrintingTrailer), nbsp).c_str(), stdout);
   }

   return(true);
}
