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
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include <openssl/md5.h>

#include "node.h"
#include "publicationset.h"
#include "stringhandling.h"


extern int    yyparse();
extern FILE*  yyin;
extern Node*  bibTeXFile;


// ###### Check URLs ########################################################
unsigned int checkAllURLs(PublicationSet* publicationSet)
{
   unsigned int errors = 0;
   for(size_t index = 0; index < publicationSet->size(); index++) {
      // ====== Get prev, current and next publications =====================
      if(publicationSet->get(index)->value == "Comment") {
         continue;
      }
      Node* publication = publicationSet->get(index);
      Node* url         = findChildNode(publication, "url");
      if(url != NULL) {
         printf("Checking URL of %s ... ", publication->keyword.c_str());
         fflush(stdout);

         CURL* curl = curl_easy_init();
         if(curl) {
            FILE* downloadFH = fopen("/tmp/x0", "w+b");
            if(downloadFH != NULL) {
               curl_easy_setopt(curl, CURLOPT_URL, url->value.c_str());
               curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
               curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
               curl_easy_setopt(curl, CURLOPT_WRITEDATA, downloadFH);
               const CURLcode result = curl_easy_perform(curl);
               if(result == 0) {
                  unsigned long long totalSize = 0;
                  rewind(downloadFH);

                  unsigned char md5[MD5_DIGEST_LENGTH];
                  MD5_CTX md5_ctx;
                  MD5_Init(&md5_ctx);

                  while(!feof(downloadFH)) {
                     char input[16384];
                     const size_t bytesRead = fread(&input, 1, sizeof(input), downloadFH);
                     if(bytesRead > 0) {
                        totalSize += (unsigned long long)bytesRead;
                        MD5_Update(&md5_ctx, &input, bytesRead);
                     }
                  }

                  if(totalSize > 0) {
                     MD5_Final((unsigned char*)&md5, &md5_ctx);
                     printf("OK: size=%lluB; MD5=", totalSize);
                     for(unsigned int i = 0; i < MD5_DIGEST_LENGTH; i++) {
                        printf("%02x", (unsigned int)md5[i]);
                     }
                     puts("");
                  }
                  else {
                     printf("FAILED %s - Size is zero!\n", url->value.c_str());
                     errors++;
                  }
               }
               else {
                  printf("FAILED %s - Failed to download!\n", url->value.c_str());
                  errors++;
               }
               fclose(downloadFH);
            }
            else {
               fputs("ERROR: Failed to store temporary download file!\n", stderr);
               errors++;
            }
            curl_easy_cleanup(curl);
         }
         else {
            fputs("ERROR: Failed to initialize libcurl!\n", stderr);
            errors++;
         }

      }
   }
   return(errors);
}


// ###### Handle interactive input ##########################################
static bool                     useXMLStyle            = false;
static std::string              nbsp                   = " ";
static std::string              customPrintingHeader   = "";
static std::string              customPrintingTrailer  = "";
static std::string              customPrintingTemplate =
   "\\[%C\\] %L\n %a\tAUTHOR: [[%fFIRST|%lLAST|%nNOT-FIRST]: initials=%g given=%G full=%F]\n%A\n";  // ", \"%T\"[, %B][, %J][, %?][, %$][, Volume~%V][, Number~%N][, pp.~%P][, %I][, %i][, %@][, [[%m, %D, |%m~]%Y].\\nURL: %U.\\n\\n";
static std::vector<std::string> monthNames;


static int handleInput(FILE*           fh,
                       PublicationSet& publicationSet,
                       const bool      checkURLs,
                       unsigned int    recursionLevel = 0)
{
   int result = 0;
   while(!feof(fh)) {
      char input[65536];
      if(fgets((char*)&input, sizeof(input), fh)) {
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
            std::string arguments = (const char*)&input[5];
            const std::string keyword = extractToken(trim(arguments), " \t");
            const std::string anchor  = extractToken(trim(arguments), " \t");
            Node* publication = findNode(bibTeXFile, keyword.c_str());
            if(publication) {
               if(anchor.size() > 0) {
                  publication->anchor = anchor;
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
               for(size_t i = 0; i < NODE_CUSTOM_ENTRIES; i++) {
                  publication->custom[i] = extractToken(trim(arguments), " \t");
               }
            }
            else {
               fprintf(stderr, "ERROR: Publication '%s' not found!\n",
                       (const char*)&input[5]);
               result++;
            }
         }
         else if((strncmp(input, "sort ", 5)) == 0) {
            const size_t maxSortLevels = 8;
            std::string sortKey[maxSortLevels];
            bool        sortAscending[maxSortLevels];
            std::string arguments = (const char*)&input[5];
            size_t sortLevels = 0;
            for(size_t i = 0; i < maxSortLevels; i++) {
               bool isAscending = true;
                std::string token = extractToken(trim(arguments), " \t");
                const size_t slash = token.find('/');
                if(slash != std::string::npos) {
                   const std::string order = token.substr(slash + 1, token.size() - slash - 1);
                   token = token.substr(0, slash);
                   if( (order == "ascending") || (order == "A") ) {
                     isAscending = true;
                   }
                   else if( (order == "descending") || (order == "D") ) {
                     isAscending = false;
                   }
                   else {
                      fprintf(stderr, "ERROR: Bad sorting order '%s' for key '%s'!\n",
                              order.c_str(), token.c_str());
                      result++;
                      break;
                   }
                }
                if(token != "") {
                   sortKey[sortLevels]       = token;
                   sortAscending[sortLevels] = isAscending;
                   sortLevels++;
                }
            }
            publicationSet.sort((const std::string*)&sortKey,
                                (const bool*)&sortAscending,
                                sortLevels);
         }
         else if((strncmp(input, "export", 5)) == 0) {
            if(checkURLs) {
               result += checkAllURLs(&publicationSet);
            }
            if(PublicationSet::exportPublicationSetToCustom(
                  &publicationSet,
                  customPrintingHeader, customPrintingTrailer,
                  customPrintingTemplate, monthNames, nbsp, useXMLStyle) == false) {
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
         else if((strncmp(input, "templatenew", 11)) == 0) {
            customPrintingTemplate = "";
         }
         else if((strncmp(input, "template ", 9)) == 0) {
            customPrintingTemplate = (const char*)&input[9];
         }
         else if((strncmp(input, "template+ ", 10)) == 0) {
            customPrintingTemplate += (const char*)&input[10];
         }
         else if((strncmp(input, "include ", 8)) == 0) {
            if(recursionLevel <= 9) {
               const char* includeFileName = (const char*)&input[8];
               FILE* includeFH = fopen(includeFileName, "r");
               if(includeFH != NULL) {
                  result += handleInput(includeFH, publicationSet, checkURLs, recursionLevel + 1);
                  fclose(includeFH);
               }
               else {
                  fprintf(stderr, "ERROR: Unable to open include file '%s'!\n", includeFileName);
                  result++;
                  break;
               }
            }
            else {
               fprintf(stderr, "ERROR: Include file nesting level limit reached!\n");
               result++;
               break;
            }
         }
         else {
            fprintf(stderr, "ERROR: Bad command '%s'!\n", input);
            result++;
            break;
         }
      }
   }
   return(result);
}



// ###### Main program ######################################################
int main(int argc, char** argv)
{
   bool        interactive    = true;
   bool        checkURLs      = false;
   const char* exportToBibTeX = NULL;
   const char* exportToXML    = NULL;
   const char* exportToCustom = NULL;

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
      else if( strcmp(argv[i], "-check-urls") == 0 ) {
         checkURLs = true;
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
         if(checkURLs) {
            result += checkAllURLs(&publicationSet);
         }
         if(exportToBibTeX) {
            if(PublicationSet::exportPublicationSetToBibTeX(&publicationSet) == false) {
               exit(1);
            }
         }
         if(exportToXML) {
            if(PublicationSet::exportPublicationSetToXML(&publicationSet) == false) {
               exit(1);
            }
         }
         if(exportToCustom) {
            if(PublicationSet::exportPublicationSetToCustom(
                  &publicationSet,
                  customPrintingHeader, customPrintingTrailer,
                  customPrintingTemplate, monthNames,
                  nbsp, useXMLStyle) == false) {
               exit(1);
            }
         }
      }
      else {
         fprintf(stderr, "Got %u publications from BibTeX file.\n",
                 (unsigned int)publicationSet.maxSize());
         result = handleInput(stdin, publicationSet, checkURLs);
         fprintf(stderr, "Done. %u errors have occurred.\n", result);
      }
   }
   if(bibTeXFile) {
      freeNode(bibTeXFile);
      bibTeXFile = NULL;
   }

   return result;
}
