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
#include <assert.h>

#include <iostream>

#include "node.h"
#include "stringhandling.h"


extern int    yyparse();
extern FILE*  yyin;
extern Node*  bibTeXFile;


class PublicationSet
{
   public:
   PublicationSet(const size_t maxSize);
   ~PublicationSet();

   inline size_t size() const {
      return(entries);
   }
   inline size_t maxSize() const {
      return(maxEntries);
   }
   Node* get(const size_t index) const {
      assert(index < entries);
      return(publicationArray[index]);
   }

   bool add(Node* publication);
   void addAll(Node* publication);
   void sort();
   void clearAll();

   private:
   size_t maxEntries;
   size_t entries;
   Node** publicationArray;
};


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
bool exportPublicationSetToBibTeX(PublicationSet* publicationSet)
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
bool exportPublicationSetToXML(PublicationSet* publicationSet)
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
bool exportPublicationSetToCustom(PublicationSet*                 publicationSet,
                                  const std::vector<std::string>& monthNames,
                                  const std::string&              customPrintingHeader,
                                  const std::string&              customPrintingTrailer,
                                  const std::string&              printingTemplate,
                                  const std::string&              nbsp,
                                  const bool                      xmlStyle)
{
   const size_t printingTemplateSize = printingTemplate.size();

   for(size_t index = 0; index < publicationSet->size(); index++) {
      Node* publication = publicationSet->get(index);
      if(publication->value == "Comment") {
         continue;
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



// ###### Main program ######################################################
int main(int argc, char** argv)
{
   bool                     interactive            = true;
   bool                     useXMLStyle            = false;
   const char*              exportToBibTeX         = NULL;
   const char*              exportToXML            = NULL;
   const char*              exportToCustom         = NULL;
   std::string              nbsp                   = " ";
   std::string              customPrintingHeader   = "";
   std::string              customPrintingTrailer  = "";
   std::string              customPrintingTemplate =
      "\\[%C\\] %L\n %a\tAUTHOR: [[%fFIRST|%lLAST|%nNOT-FIRST]: initials=%g given=%G full=%F]\n%A\n";  // ", \"%T\"[, %B][, %J][, %?][, %$][, Volume~%V][, Number~%N][, pp.~%P][, %I][, %i][, %@][, [[%m, %D, |%m~]%Y].\\nURL: %U.\\n\\n";
   std::vector<std::string> monthNames;

   monthNames.push_back("January");
   monthNames.push_back("February");
   monthNames.push_back("March");
   monthNames.push_back("April");
   monthNames.push_back("May");
   monthNames.push_back("June");
   monthNames.push_back("July");
   monthNames.push_back("August");
   monthNames.push_back("September");
   monthNames.push_back("October");
   monthNames.push_back("November");
   monthNames.push_back("December");

   if(argc < 2) {
      fprintf(stderr, "Usage: %s [BibTeX file] {-export-to-bibtex=file} {-export-to-xml=file} {-export-to-custom=file}\n", argv[0]);
      exit(1);
   }
   for(int i = 2; i < argc; i++) {
      if( strncmp(argv[i], "-export-to-bibtex=", 18) == 0 ) {
         exportToBibTeX = (const char*)&argv[i][18];
      }
      else if( strncmp(argv[i], "-export-to-xml=", 15) == 0 ) {
         exportToXML = (const char*)&argv[i][15];
      }
      else if( strncmp(argv[i], "-export-to-custom=", 18) == 0 ) {
         exportToCustom = (const char*)&argv[i][18];
      }
      else if( strncmp(argv[i], "-nbsp=", 5) == 0 ) {
         nbsp = (const char*)&argv[i][5];
      }
      else if( strcmp(argv[i], "-non-interactive") == 0 ) {
         interactive = false;
      }
      else {
         fputs("ERROR: Bad arguments!\n", stderr);
         exit(1);
      }
   }

   yyin = fopen(argv[1], "r");
   if(yyin == NULL) {
      fprintf(stderr, "ERROR: Unable to open BibTeX input file %s!\n", argv[1]);
      exit(1);
   }
   int result = yyparse();
   fclose(yyin);

   if(result == 0) {
      PublicationSet publicationSet(countNodes(bibTeXFile));
      if(!interactive) {
         publicationSet.addAll(bibTeXFile);
         // publicationSet.sort();
         if(exportToBibTeX) {
            if(exportPublicationSetToBibTeX(&publicationSet) == false) {
               exit(1);
            }
         }
         if(exportToXML) {
            if(exportPublicationSetToXML(&publicationSet) == false) {
               exit(1);
            }
         }
         if(exportToCustom) {
            if(exportPublicationSetToCustom(
                  &publicationSet, monthNames,
                  customPrintingHeader, customPrintingTrailer,
                  customPrintingTemplate, nbsp, useXMLStyle) == false) {
               exit(1);
            }
         }
      }
      else {
         fprintf(stderr, "Got %u publications from BibTeX file.\n",
                 (unsigned int)publicationSet.maxSize());
         while(!feof(stdin)) {
            char input[65536];
            if(fgets((char*)&input, sizeof(input), stdin)) {
               // ====== Remove newline =====================================
               const size_t length = strlen(input);
               if(length > 0) {
                  input[length - 1] = 0x00;
               }

               // ====== Handle commands ====================================
               if(input[0] == 0x00) {
                  // Empty line
               }
               else if(input[0] == '#') {
                  // Comment
               }
               else if(strncmp(input, "citeAll", 7) == 0) {
                  publicationSet.addAll(bibTeXFile);
               }
               else if(strncmp(input, "cite ", 5) == 0) {
                  char* anchor = index((char*)&input[5], ' ');
                  if(anchor == NULL) {
                     anchor = index((char*)&input[5], '\t');
                  }
                  if(anchor != NULL) {
                     anchor[0] = 0x00;
                     anchor = (char*)&anchor[1];
                  }
                  std::string keyword = (const char*)&input[5];
                  trim(keyword);
                  Node* publication = findNode(bibTeXFile, keyword.c_str());
                  if(publication) {
                     if(anchor) {
                        publication->anchor = anchor;
                        trim(publication->anchor);
                     }
                     else {
                        char number[16];
                        snprintf((char*)&number, sizeof(number), "%u",
                                 (unsigned int)publicationSet.size());
                        publication->anchor = number;
                     }
                     if(!publicationSet.add(publication)) {
                        fprintf(stderr, "ERROR: Publication '%s' has already been added!\n",
                                (const char*)&input[5]);
                        result++;
                     }
                  }
                  else {
                     fprintf(stderr, "ERROR: Publication '%s' not found!\n",
                             (const char*)&input[5]);
                     result++;
                  }
               }
               else if((strncmp(input, "export", 5)) == 0) {
                  if(exportPublicationSetToCustom(
                        &publicationSet, monthNames,
                        customPrintingHeader, customPrintingTrailer,
                        customPrintingTemplate, nbsp, useXMLStyle) == false) {
                     result++;
                  }
               }
               else if((strncmp(input, "clear", 5)) == 0) {
                  publicationSet.clearAll();
               }
               else if((strncmp(input, "echo ", 5)) == 0) {
                  fputs(processBackslash(std::string((const char*)&input[5])).c_str(), stdout);
               }
               else if((strncmp(input, "header ", 7)) == 0) {
                  customPrintingHeader = (const char*)&input[7];
               }
               else if((strncmp(input, "trailer ", 8)) == 0) {
                  customPrintingTrailer = (const char*)&input[8];
               }
               else if((strncmp(input, "nbsp ", 5)) == 0) {
                  nbsp = (const char*)&input[5];
               }
               else if((strncmp(input, "utf8Style", 8)) == 0) {
                  useXMLStyle = false;
               }
               else if((strncmp(input, "xmlStyle", 8)) == 0) {
                  useXMLStyle = true;
               }
               else if((strncmp(input, "template ", 9)) == 0) {
                  customPrintingTemplate = (const char*)&input[9];
               }
               else {
                  fprintf(stderr, "ERROR: Bad command '%s'!\n", input);
                  result++;
               }
            }
         }
         fprintf(stderr, "Done. %u errors have occurred.\n", result);
      }
   }
   if(bibTeXFile) {
      freeNode(bibTeXFile);
      bibTeXFile = NULL;
   }

   return result;
}
