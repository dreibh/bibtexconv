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


// ###### Sort publications #################################################
void PublicationSet::sort()
{

   // FIXME ...

}


// ###### Export to BibTeX ##################################################
bool PublicationSet::exportPublicationSetToBibTeX(PublicationSet* publicationSet)
{
   for(size_t index = 0; index < publicationSet->size(); index++) {
      const Node* publication = publicationSet->get(index);
      if(publication->value == "Comment") {
         printf("%%%s\n\n", publication->keyword.c_str());
      }
      else {
         printf("@%s { %s, \n", publication->value.c_str(),
                                publication->keyword.c_str());

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
   }
   return(true);
}


// ###### Export to XML #####################################################
bool PublicationSet::exportPublicationSetToXML(PublicationSet* publicationSet)
{
   fputs("<?xml version='1.0' encoding='UTF-8'?>\n", stdout);

   for(size_t index = 0; index < publicationSet->size(); index++) {
      Node* publication = publicationSet->get(index);
      if(publication->value == "Comment") {
         printf("<!-- %s -->\n\n", publication->keyword.c_str());
      }
      else {
         Node* title     = findChildNode(publication, "title");
         Node* author    = findChildNode(publication, "author");
         Node* year      = findChildNode(publication, "year");
         Node* month     = findChildNode(publication, "month");
         Node* day       = findChildNode(publication, "day");
         Node* url       = findChildNode(publication, "url");
         Node* booktitle = findChildNode(publication, "booktitle");
         Node* journal   = findChildNode(publication, "journal");
         Node* volume    = findChildNode(publication, "volume");
         Node* number    = findChildNode(publication, "number");
         Node* pages     = findChildNode(publication, "pages");

         fprintf(stdout, "<reference anchor=\"%s\">\n", publication->keyword.c_str());
         fputs("\t<front>\n", stdout);
         if(title) {
            fprintf(stdout, "\t\t<title>%s</title>\n", string2xml(title->value).c_str());
         }
         if(author) {
            for(size_t authorIndex = 0; authorIndex < author->arguments.size(); authorIndex += 3) {
               fprintf(stdout,
                  "\t\t<author initials=\"%s\" surname=\"%s\" fullname=\"%s\" />\n",
                  string2xml(author->arguments[authorIndex + 2]).c_str(),
                  string2xml(author->arguments[authorIndex + 0]).c_str(),
                  string2xml(author->arguments[authorIndex + 1] +
                              ((author->arguments[authorIndex + 1] != "") ? "~" : "") +
                              author->arguments[authorIndex + 0]).c_str());
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
         if(booktitle) {
            seriesName =  booktitle->value;
         }
         if(journal) {
            seriesName =  journal->value;
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
            fprintf(stdout, "\t<format target=\"%s\" />\n",
                    url->value.c_str());
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

// ###### Export to custom ##################################################
bool PublicationSet::exportPublicationSetToCustom(PublicationSet*                 publicationSet,
                                                  const std::vector<std::string>& monthNames,
                                                  const std::string&              customPrintingHeader,
                                                  const std::string&              customPrintingTrailer,
                                                  const std::string&              printingTemplate,
                                                  const std::string&              nbsp,
                                                  const bool                      xmlStyle)
{
   const size_t printingTemplateSize = printingTemplate.size();

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

      std::string             result = "";
      std::vector<StackEntry> stack;
      Node*                   child;
      Node*                   author      = NULL;
      size_t                  authorIndex = 0;
      size_t                  authorBegin = std::string::npos;
      bool                    skip = false;
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
                     return(false);
                  }
                  author      = findChildNode(publication, "author");
                  authorIndex = 0;
                  authorBegin = i;
                break;
               case 'g':   // Current author given name initials
                  if(author) {
                     result += string2utf8(author->arguments[authorIndex + 2], nbsp, xmlStyle);
                  }
                break;
               case 'G':   // Current author given name
                  if(author) {
                     result += string2utf8(author->arguments[authorIndex + 1], nbsp, xmlStyle);
                  }
                break;
               case 'F':   // Current author family name
                  if(author) {
                     result += string2utf8(author->arguments[authorIndex + 0], nbsp, xmlStyle);
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
                     return(false);
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
               case 'B':   // Booktitle
                  child = findChildNode(publication, "booktitle");
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
                  if(child) { result += string2utf8(child->value, nbsp, xmlStyle); } else { skip = true; }
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
                  if(child) { result += string2utf8("ISBN" + (std::string)nbsp + child->value, nbsp, xmlStyle); } else { skip = true; }
                break;
               case 'i':   // ISSN
                  child = findChildNode(publication, "issn");
                  if(child) { result += string2utf8("ISSN"+ (std::string)nbsp + child->value, nbsp, xmlStyle); } else { skip = true; }
                break;
               case 'U':   // URL
                  child = findChildNode(publication, "url");
                  if(child) { result += string2utf8(child->value, nbsp, xmlStyle); } else { skip = true; }
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
                           return(false);
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
                              skip = ! (begin || end);
                            break;
                           case 'e':
                              skip = ! end;
                            break;
                        }
                     }
                     i++;
                  }
                break;
               default:
                  fprintf(stderr, "ERROR: Unexpected %% placeholder '%c' in custom printing template!",
                          printingTemplate[i + 1]);
                  return(false);
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
               return(false);
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
               return(false);
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

      fputs(string2utf8(processBackslash(customPrintingHeader), nbsp).c_str(), stdout);
      fputs(result.c_str(), stdout);
      fputs(string2utf8(processBackslash(customPrintingTrailer), nbsp).c_str(), stdout);
   }

   return(true);
}
