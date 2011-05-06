/* $Id$
 *
 * BibTeX Converter
 * Copyright (C) 2010-2011 by Thomas Dreibholz
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

#include <algorithm>
#include <string>
#include <set>
#include <vector>

#include "publicationset.h"


// ###### Constructor #######################################################
PublicationSet::PublicationSet(const size_t maxSize)
{
   maxEntries = maxSize;
   publicationArray = new Node*[maxEntries];
   assert(publicationArray != NULL);
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
      publicationArray[i] = NULL;
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
   while(publication != NULL) {
      if(add(publication)) {
         publication->anchor = publication->keyword;
      }
      publication = publication->next;
   }
}


// NOTE: PublicationSet::sort() will *NOT* be thread-safe!
static const std::string* gSortKey       = NULL;
static const bool*        gSortAscending = NULL;
static size_t             gMaxSortLevels = 0;

static int publicationNodeComparisonFunction(const void* ptr1, const void* ptr2)
{
   const Node* node1 = *((const Node**)ptr1);
   const Node* node2 = *((const Node**)ptr2);

   for(size_t i = 0; i < gMaxSortLevels; i++) {
      const Node* child1 = findChildNode((Node*)node1, gSortKey[i].c_str());
      const Node* child2 = findChildNode((Node*)node2, gSortKey[i].c_str());
      int result = 0;
      if( (child1 == NULL) && (child2 != NULL) ) {
         result = -1;
      }
      if( (child1 != NULL) && (child2 == NULL) ) {
         result = 1;
      }
      else if( (child1 != NULL) && (child2 != NULL) ) {
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

   gSortKey       = NULL;
   gSortAscending = NULL;
}


// ###### Export to BibTeX ##################################################
bool PublicationSet::exportPublicationSetToBibTeX(PublicationSet* publicationSet,
                                                  FILE*           fh,
                                                  const bool      skipNotesWithISBNandISSN,
                                                  const bool      addNotesWithISBNandISSN)
{
   for(size_t index = 0; index < publicationSet->size(); index++) {
      const Node* publication = publicationSet->get(index);
      if(publication->value == "Comment") {
         fprintf(fh, "%%%s\n\n", publication->keyword.c_str());
      }
      else {
         fprintf(fh, "@%s{ %s, \n", publication->value.c_str(),
                                    publication->keyword.c_str());

         bool  empty           = true;
         Node* child           = publication->child;
         const Node* issn      = NULL;
         const Node* isbn      = NULL;
         const char* separator = "";
         while(child != NULL) {
            if(!empty) {
               separator = ",\n";
            }
            empty = false;

            if( (child->keyword == "title") ||
                (child->keyword == "booktitle") ||
                (child->keyword == "series") ||
                (child->keyword == "journal") ) {
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
               fprintf(fh, "%s\t%s = \"\\url{%s}\"", separator, child->keyword.c_str(), urlToLaTeX(child->value).c_str());
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
             ((isbn != NULL) || (issn != NULL)) ) {
            if(isbn) {
               fprintf(fh, "%s\tnote = \"{ISBN} %s\"", separator, isbn->value.c_str());
               separator = ",\n";
            }
            if(issn) {
               fprintf(fh, "%s\tnote = \"{ISSN} %s\"", separator, issn->value.c_str());
            }
         }

         fputs("\n}\n\n", fh);
      }
   }
   return(true);
}


// ###### Export to XML #####################################################
bool PublicationSet::exportPublicationSetToXML(PublicationSet* publicationSet,
                                               FILE*           fh)
{
   fputs("<?xml version='1.0' encoding='UTF-8'?>\n", stdout);

   for(size_t index = 0; index < publicationSet->size(); index++) {
      Node* publication = publicationSet->get(index);
      if(publication->value == "Comment") {
         printf("<!-- %s -->\n\n", publication->keyword.c_str());
      }
      else {
         const Node* title        = findChildNode(publication, "title");
         const Node* author       = findChildNode(publication, "author");
         const Node* year         = findChildNode(publication, "year");
         const Node* month        = findChildNode(publication, "month");
         const Node* day          = findChildNode(publication, "day");
         const Node* url          = findChildNode(publication, "url");
         const Node* urlMime      = findChildNode(publication, "url.mime");
         const Node* howpublished = findChildNode(publication, "howpublished");
         const Node* booktitle    = findChildNode(publication, "booktitle");
         const Node* journal      = findChildNode(publication, "journal");
         const Node* volume       = findChildNode(publication, "volume");
         const Node* number       = findChildNode(publication, "number");
         const Node* pages        = findChildNode(publication, "pages");

         fprintf(stdout, "<reference anchor=\"%s\">\n", publication->keyword.c_str());
         fputs("\t<front>\n", stdout);
         if(title) {
            fprintf(stdout, "\t\t<title>%s</title>\n", string2xml(title->value).c_str());
         }
         if(author) {
            for(size_t authorIndex = 0; authorIndex < author->arguments.size(); authorIndex += 3) {
               std::string familyName = author->arguments[authorIndex + 0];
               std::string givenName  = author->arguments[authorIndex + 1];
               std::string initials   = author->arguments[authorIndex + 2];
               removeBrackets(familyName);
               removeBrackets(givenName);
               removeBrackets(initials);
               fprintf(stdout,
                  "\t\t<author initials=\"%s\" surname=\"%s\" fullname=\"%s\" />\n",
                  string2xml(initials).c_str(), string2xml(familyName).c_str(),
                  string2xml(givenName +
                             ((givenName != "") ? "~" : "") +
                             familyName).c_str());
            }
         }
         if(year || month || day) {
            fputs("\t\t<date ", stdout);
            if(day) {
               fprintf(stdout, "day=\"%u\" ", day->number);
            }
            if(month) {
               fprintf(stdout, "month=\"%u\" ", month->number);
            }
            if(year) {
               fprintf(stdout, "year=\"%u\" ", year->number);
            }
            fputs("/>\n", stdout);
         }
         fputs("\t</front>\n", stdout);

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
         if(volume) {
            seriesValue += "Volume " + volume->value;
         }
         if(number) {
            seriesValue += "Number " + number->value;
         }
         if(pages) {
            seriesValue += "Pages " + pages->value;
         }
         if((seriesName != "") || (seriesValue != "")) {
            fprintf(stdout, "\t<seriesInfo name=\"%s\" value=\"%s\" />\n",
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
            fprintf(stdout, "\t<format type=\"%s\" target=\"%s\" />\n",
                    type.c_str(), url->value.c_str());
         }
         fputs("</reference>\n\n", stdout);
      }
   }
   return(true);
}


struct StackEntry {
   size_t pos;
   bool   skip;
};


// ###### Apply printing template to publication ############################
std::string PublicationSet::applyTemplate(Node*                           publication,
                                          Node*                           prevPublication,
                                          Node*                           nextPublication,
                                          const std::string&              printingTemplate,
                                          const std::vector<std::string>& monthNames,
                                          const std::string&              nbsp,
                                          const bool                      xmlStyle,
                                          FILE*                           fh)
{
   std::string             result;
   std::vector<StackEntry> stack;
   Node*                   child;
   Node*                   author               = NULL;
   size_t                  authorIndex          = 0;
   size_t                  authorBegin          = std::string::npos;
   bool                    skip                 = false;
   const size_t            printingTemplateSize = printingTemplate.size();
   for(size_t i = 0; i < printingTemplateSize; i++) {
      if( (printingTemplate[i] == '%') && (i + 1 < printingTemplateSize) ) {
         switch(printingTemplate[i + 1]) {
            case 'L':   // Original BibTeX label
               result += string2utf8(publication->keyword, nbsp, xmlStyle);
               break;
            case 'C':   // Anchor
               result += string2utf8(publication->anchor, nbsp, xmlStyle);
               break;
            case 'a':   // Author LOOP BEGIN
               if(authorBegin != std::string::npos) {
                  fputs("ERROR: Unexpected author loop begin %a -> an author loop is still open!\n", stderr);
                  return("");
               }
               author      = findChildNode(publication, "author");
               authorIndex = 0;
               authorBegin = i;
               break;
            case 'g':   // Current author given name initials
               if(author) {
                  std::string initials   = author->arguments[authorIndex + 2];
                  removeBrackets(initials);                  
                  if(initials != "") {
                     result += string2utf8(initials, nbsp, xmlStyle);
                  }
                  else {
                     skip = true;
                  }
               }
               break;
            case 'G':   // Current author given name
               if(author) {
                  std::string givenName  = author->arguments[authorIndex + 1];
                  removeBrackets(givenName);
                  if(givenName != "") {
                     result += string2utf8(givenName, nbsp, xmlStyle);
                  }
                  else {
                     skip = true;
                  }
               }
               break;
            case 'F':   // Current author family name
               if(author) {
                  std::string familyName = author->arguments[authorIndex + 0];
                  removeBrackets(familyName);
                  result += string2utf8(familyName, nbsp, xmlStyle);
               }
               break;
            case 'f':   // IS first author
               skip = ! (authorIndex == 0);
               break;
            case 'n':   // IS not first author
               skip = ! ((author != NULL) &&
                           (authorIndex > 0));
               break;
            case 'l':   // IS last author
               skip = ! ((author != NULL) && (authorIndex + 3 >= author->arguments.size()));
               break;
            case 'A':   // Author LOOP END
               if(authorBegin == std::string::npos) {
                  fputs("ERROR: Unexpected author loop end %A -> %a author loop begin needed first!\n", stderr);
                  return("");
               }
               authorIndex += 3;
               if(authorIndex < author->arguments.size()) {
                  i = authorBegin;
               }
               else {
                  author      = NULL;
                  authorIndex = 0;
               }
               break;
            case 'T':   // Title
               child = findChildNode(publication, "title");
               if(child) { result += string2utf8(child->value, nbsp, xmlStyle); } else { skip = true; }
               break;
            case 'H':   // HowPublished
               child = findChildNode(publication, "howpublished");
               if(child) { result += string2utf8(child->value, nbsp, xmlStyle); } else { skip = true; }
               break;
            case 'B':   // Booktitle
               child = findChildNode(publication, "booktitle");
               if(child) { result += string2utf8(child->value, nbsp, xmlStyle); } else { skip = true; }
               break;
            case 'r':   // Series
               child = findChildNode(publication, "series");
               if(child) { result += string2utf8(child->value, nbsp, xmlStyle); } else { skip = true; }
               break;
            case 'J':   // Journal
               child = findChildNode(publication, "journal");
               if(child) { result += string2utf8(child->value, nbsp, xmlStyle); } else { skip = true; }
               break;
            case 'E':   // Edition
               child = findChildNode(publication, "edition");
               if(child) { result += string2utf8(child->value, nbsp, xmlStyle); } else { skip = true; }
               break;
            case 'V':   // Volume
               child = findChildNode(publication, "volume");
               if(child) { result += string2utf8(child->value, nbsp, xmlStyle); } else { skip = true; }
               break;
            case 't':   // Type
               child = findChildNode(publication, "type");
               if(child) { result += string2utf8(child->value, nbsp, xmlStyle); } else { skip = true; }
               break;
            case 'N':   // Number
               child = findChildNode(publication, "number");
               if(child) { result += string2utf8(child->value, nbsp, xmlStyle); } else { skip = true; }
               break;
            case 'P':   // Pages
               child = findChildNode(publication, "pages");
               if(child) { result += string2utf8(child->value, nbsp, xmlStyle); } else { skip = true; }
               break;
            case '@':   // Address
               child = findChildNode(publication, "address");
               if(child) { result += string2utf8(child->value, nbsp, xmlStyle); } else { skip = true; }
               break;
            case 'Y':   // Year
               child = findChildNode(publication, "year");
               if(child) { result += string2utf8(child->value, nbsp, xmlStyle); } else { skip = true; }
               break;
            case 'M':   // Month as name
               child = findChildNode(publication, "month");
               if(child) {
                  if( (child->number >= 1) && (child->number <= 12) ) {
                     result += string2utf8(monthNames[child->number - 1], nbsp, xmlStyle);
                  } else { skip = true; }
               } else { skip = true; }
               break;
            case 'm':   // Month as number
               child = findChildNode(publication, "month");
               if(child) {
                  char month[16];
                  snprintf((char*)&month, sizeof(month), "%d", child->number);
                  result += string2utf8(month, nbsp, xmlStyle);
               } else { skip = true; }
               break;
            case 'D':   // Day
               child = findChildNode(publication, "day");
               if(child) {
                  char day[16];
                  snprintf((char*)&day, sizeof(day), "%d", child->number);
                  result += string2utf8(day, nbsp, xmlStyle);
               } else { skip = true; }
               break;
            case '$':   // Publisher
               child = findChildNode(publication, "publisher");
               if(child) { result += string2utf8(child->value, nbsp, xmlStyle); } else { skip = true; }
               break;
            case 'S':   // School
               child = findChildNode(publication, "school");
               if(child) { result += string2utf8(child->value, nbsp, xmlStyle); } else { skip = true; }
               break;
            case '?':   // Institution
               child = findChildNode(publication, "institution");
               if(child) { result += string2utf8(child->value, nbsp, xmlStyle); } else { skip = true; }
               break;
            case 'I':   // ISBN
               child = findChildNode(publication, "isbn");
               if(child) { result += string2utf8("ISBN~" + child->value, nbsp, xmlStyle); } else { skip = true; }
               break;
            case 'i':   // ISSN
               child = findChildNode(publication, "issn");
               if(child) { result += string2utf8("ISSN~" + child->value, nbsp, xmlStyle); } else { skip = true; }
               break;
            case 'U':   // URL
               child = findChildNode(publication, "url");
               if(child) { result += string2utf8(child->value, "", xmlStyle); } else { skip = true; }
               break;
            case 'd':   // DOI
               child = findChildNode(publication, "doi");
               if(child) { result += string2utf8("DOI~" + child->value, "", xmlStyle); } else { skip = true; }
               break;
            case 'y':   // URL mime type
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
                  else {
                     result += child->value;
                  }
               } else { skip = true; }
               break;
            case 's':   // URL size
               child = findChildNode(publication, "url.size");
               if(i + 2 < printingTemplateSize) {
                  switch(printingTemplate[i + 2]) {
                     case 'B':   // B
                        if(child) { result += string2utf8(format("%llu", atoll(child->value.c_str())), nbsp, xmlStyle); } else { skip = true; }
                      break;
                     case 'K':   // KiB
                        if(child) { result += string2utf8(format("%llu", atoll(child->value.c_str()) / 1024), nbsp, xmlStyle); } else { skip = true; }
                      break;
                  }
                  i++;
               }
               break;
            case 'X':   // Note
               child = findChildNode(publication, "note");
               if(child) {
                  if( (strncmp(child->value.c_str(), "ISBN", 4) == 0) ||
                      (strncmp(child->value.c_str(), "ISSN", 4) == 0) ||
                      (strncmp(child->value.c_str(), "{ISBN}", 6) == 0) ||
                      (strncmp(child->value.c_str(), "{ISSN}", 6) == 0) ) {
                     skip = true;
                  }
                  else {
                     result += string2utf8(child->value, nbsp, xmlStyle);
                  }
               } else { skip = true; }
               break;
            case '%':
               result += '%';
               break;
            case 'b':
            case 'w':
            case 'e':   // Begin/Within/End of subdivision
               if(i + 2 < printingTemplateSize) {
                  const char* type = NULL;
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
                        fprintf(stderr, "ERROR: Unexpected %% placeholder '%c' in subdivision part of custom printing template!",
                        printingTemplate[i + 2]);
                        return("");
                        break;
                  }
                  if(type != NULL) {
                     const Node* prevChild = (prevPublication != NULL) ? findChildNode(prevPublication, type) : NULL;
                     child                 = findChildNode(publication, type);
                     const Node* nextChild = (nextPublication != NULL) ? findChildNode(nextPublication, type) : NULL;

                     bool begin = (prevChild == NULL) ||
                                 ( (prevChild != NULL) && (child != NULL) && (prevChild->value != child->value) );
                     bool end = (nextChild == NULL) ||
                                 ( (child != NULL) && (nextChild != NULL) && (child->value != nextChild->value) );
                     switch(printingTemplate[i + 1]) {
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
                  i++;
               }
               break;
            case '1':   // Custom #1
            case '2':   // Custom #2
            case '3':   // Custom #3
            case '4':   // Custom #4
            case '5':   // Custom #5
               {
                  const unsigned int id = printingTemplate[i + 1] - '1';
                  if(publication->custom[id] != "") {
                     result += string2utf8(publication->custom[id], nbsp, xmlStyle);
                  }
                  else {
                     skip = true;
                  }
               }
               break;
            default:
               fprintf(stderr, "ERROR: Unexpected %% placeholder '%c' in custom printing template!",
                        printingTemplate[i + 1]);
               return("");
               break;
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
      else if(printingTemplate[i] == '[') {
         if(stack.empty()) {
            skip = false;   // Up to now, everything will be accepted
         }
         struct StackEntry entry = { result.size(), skip };
         stack.push_back(entry);
      }
      else if(printingTemplate[i] == ']') {
         if(!stack.empty()) {
            StackEntry entry = stack.back();
            stack.pop_back();
            if(skip == true) {
               result.erase(entry.pos);
               skip = entry.skip;
            }
         }
         else {
            fputs("ERROR: Unexpected ']' in custom printing template!\n", stderr);
            return("");
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
            return("");
         }
      }
      else {
         std::string character = "";

#ifdef USE_UTF8
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
#endif
            character += printingTemplate[i];
#ifdef USE_UTF8
         }
         else {
            // Invalid!
         }
#endif

         // Add current character. We may *not* use XML style encoding here,
         // since the character may be itself part of XML tags!
         result += string2utf8(character, nbsp);
      }
   }
   return(result);
}


// ###### Export to custom ##################################################
bool PublicationSet::exportPublicationSetToCustom(PublicationSet*                 publicationSet,
                                                  const std::string&              customPrintingHeader,
                                                  const std::string&              customPrintingTrailer,
                                                  const std::string&              printingTemplate,
                                                  const std::vector<std::string>& monthNames,
                                                  const std::string&              nbsp,
                                                  const bool                      xmlStyle,
                                                  FILE*                           fh)
{
   Node* publication = NULL;
   for(size_t index = 0; index < publicationSet->size(); index++) {
      // ====== Get prev, current and next publications =====================
      if(publicationSet->get(index)->value == "Comment") {
         continue;
      }
      Node* prevPublication = publication;
      publication = publicationSet->get(index);
      size_t nextPublicationIndex = 1;
      Node* nextPublication = (index + nextPublicationIndex< publicationSet->size()) ? publicationSet->get(index + nextPublicationIndex) : NULL;
      while( (nextPublication != NULL) && (nextPublication->value == "Comment")) {
         nextPublicationIndex++;
         nextPublication = (index + nextPublicationIndex< publicationSet->size()) ? publicationSet->get(index + nextPublicationIndex) : NULL;
      }
      const std::string result = applyTemplate(publication, prevPublication, nextPublication,
                                               printingTemplate,
                                               monthNames, nbsp, xmlStyle, fh);

      fputs(string2utf8(processBackslash(customPrintingHeader), nbsp).c_str(), stdout);
      fputs(result.c_str(), stdout);
      fputs(string2utf8(processBackslash(customPrintingTrailer), nbsp).c_str(), stdout);
   }

   return(true);
}
