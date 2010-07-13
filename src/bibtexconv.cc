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

#include "node.h"
#include "stringhandling.h"


extern int    yyparse();
extern FILE*  yyin;
extern Node*  bibTeXFile;


struct PublicationSet
{
   size_t entries;
   Node** publicationArray;
};


// ###### Select specific publications from collection ######################
PublicationSet* filterPublicationSet(Node* publication)
{
   const size_t    publications   = countNodes(publication);
   PublicationSet* publicationSet = new PublicationSet;
   if(publicationSet) {
      publicationSet->entries          = 0;
      publicationSet->publicationArray = new Node*[publications];
      if(publicationSet->publicationArray) {
         while(publication != NULL) {

            // FIXME: Hier sollte gefiltert werden ...

            publicationSet->publicationArray[publicationSet->entries++] = publication;
            publication = publication->next;
         }
      }
      else {
         delete publicationSet;
         publicationSet = NULL;
      }
   }
   return(publicationSet);
}


// ###### Sort publications #################################################
void sortPublicationSet(PublicationSet* publicationSet)
{

   // FIXME ...

}


// ###### Free publication set ##############################################
void freePublicationSet(PublicationSet* publicationSet)
{
   delete publicationSet->publicationArray;
   publicationSet->publicationArray = NULL;
   publicationSet->entries          = 0;
   delete publicationSet;
}


// ###### Export to BibTeX ##################################################
bool exportPublicationSetToBibTeX(PublicationSet* publicationSet)
{
   for(size_t index = 0; index < publicationSet->entries; index++) {
      Node* publication = publicationSet->publicationArray[index];
      if(publication->value == "Comment") {
         printf("%%%s\n\n", publication->keyword.c_str());
      }
      else {
         printf("@%s { %s, \n", publication->value.c_str(), publication->keyword.c_str());

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

   for(size_t index = 0; index < publicationSet->entries; index++) {
      Node* publication = publicationSet->publicationArray[index];
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


// ###### Export to custom ##################################################
bool exportPublicationSetToCustom(PublicationSet* publicationSet,
                                  const char*     printingTemplate)
{
   const size_t printingTemplateSize = strlen(printingTemplate);

   for(size_t index = 0; index < publicationSet->entries; index++) {
      Node* publication = publicationSet->publicationArray[index];
      if(publication->value == "Comment") {
         continue;
      }

//       fprintf(stdout, "<reference anchor=\"%s\">\n", publication->keyword.c_str());

      Node* child;

      for(size_t i = 0; i < printingTemplateSize; i++) {
         if( (printingTemplate[i] == '%') && (i + 1 < printingTemplateSize) ) {
            switch(printingTemplate[i + 1]) {
               case 'L':   // Original BibTeX label
                  fputs(string2utf8(publication->keyword).c_str(), stdout);
                  break;
               case 'A':   // Author
                  child = findChildNode(publication, "author");
                  if(child) {
                     fputs(string2utf8(child->value).c_str(), stdout);
                  }
                  break;
               case 'T':   // Title
                  child = findChildNode(publication, "title");
                  if(child) {
                     fputs(string2utf8(child->value).c_str(), stdout);
                  }
                  break;
               case 'B':   // Booktitle
                  child = findChildNode(publication, "booktitle");
                  if(child) {
                     fputs(string2utf8(child->value).c_str(), stdout);
                  }
                  break;
               case 'J':   // Journal
                  child = findChildNode(publication, "journal");
                  if(child) {
                     fputs(string2utf8(child->value).c_str(), stdout);
                  }
                  break;
               case 'V':   // Volume
                  child = findChildNode(publication, "volume");
                  if(child) {
                     fputs(string2utf8(child->value).c_str(), stdout);
                  }
                  break;
               case 'N':   // Number
                  child = findChildNode(publication, "number");
                  if(child) {
                     fputs(string2utf8(child->value).c_str(), stdout);
                  }
                  break;
               case 'P':   // Pages
                  child = findChildNode(publication, "pages");
                  if(child) {
                     fputs(string2utf8(child->value).c_str(), stdout);
                  }
                  break;
               case 'D':   // Address
                  child = findChildNode(publication, "address");
                  if(child) {
                     fputs(string2utf8(child->value).c_str(), stdout);
                  }
                  break;
               case 'U':   // URL
                  child = findChildNode(publication, "url");
                  if(child) {
                     fputs(string2utf8(child->value).c_str(), stdout);
                  }
                  break;
               case '%':
                  fputc('%', stdout);
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
                  fputs("\n", stdout);
                  break;
               case 't':
                  fputc('\t', stdout);
                  break;
               default:
                  fputc(printingTemplate[i + 1], stdout);
                  break;
            }
            i++;
         }
         else if(printingTemplate[i] == '[') {

         }
         else if(printingTemplate[i] == ']') {

         }
         else {
            fputc(printingTemplate[i], stdout);
         }
      }
   }
   return(true);
}



// ###### Main program ######################################################
int main(int argc, char** argv)
{
   const char* exportToBibTeX         = NULL;
   const char* exportToXML            = NULL;
   const char* exportToCustom         = NULL;
   const char* customPrintingTemplate = "\\[%L\\] %A, \"%T\", %B%J[, Volume %V][, Number %N][, pp. %P][, %D].\\nURL: %U.\\n\\n";

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
      PublicationSet* publicationSet = filterPublicationSet(bibTeXFile);
      if(publicationSet) {
         sortPublicationSet(publicationSet);

         if(exportToBibTeX) {
            if(exportPublicationSetToBibTeX(publicationSet) == false) {
               exit(1);
            }
         }
         if(exportToXML) {
            if(exportPublicationSetToXML(publicationSet) == false) {
               exit(1);
            }
         }
         if(exportToCustom) {
            if(exportPublicationSetToCustom(publicationSet, customPrintingTemplate) == false) {
               exit(1);
            }
         }

         freePublicationSet(publicationSet);
      }
   }
   if(bibTeXFile) {
      freeNode(bibTeXFile);
      bibTeXFile = NULL;
   }

   return result;
}
