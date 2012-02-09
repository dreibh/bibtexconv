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
#include <assert.h>
#include <errno.h>
#include <iostream>
#include <curl/curl.h>
#include <curl/easy.h>
#include <openssl/md5.h>

#include "node.h"
#include "publicationset.h"
#include "stringhandling.h"


extern int   yyparse();
extern FILE* yyin;
extern Node* bibTeXFile;


// ###### Get current timer #################################################
unsigned long long getMicroTime()
{
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return(((unsigned long long)tv.tv_sec * (unsigned long long)1000000) +
         (unsigned long long)tv.tv_usec);
}


// ###### Check URLs ########################################################
unsigned int checkAllURLs(PublicationSet* publicationSet,
                          const char*     downloadDirectory,
                          const bool      checkNewURLsOnly)
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
         const Node* urlSize    = findChildNode(publication, "url.size");
         const Node* urlMime    = findChildNode(publication, "url.mime");
         const Node* urlChecked = findChildNode(publication, "url.checked");
         if( (urlSize != NULL) && (urlMime != NULL) && (urlChecked != NULL) ) {
            if(downloadDirectory != NULL) {
               const std::string downloadFileName =
                  PublicationSet::makeDownloadFileName(downloadDirectory,
                                                       publication->keyword,
                                                       urlMime->value);
               FILE* downloadFH = fopen(downloadFileName.c_str(), "rb");
               if(downloadFH != NULL) {
                  fclose(downloadFH);
                  fprintf(stderr, "Skipping URL of %s (already available as %s).\n",
                          publication->keyword.c_str(),
                          downloadFileName.c_str());
                  continue;
               }
            }
            else if(checkNewURLsOnly == true) {
               fprintf(stderr, "Skipping URL of %s (not a new entry).\n", publication->keyword.c_str());
               continue;
            }
         }

         fprintf(stderr, "Checking URL of %s ... ", publication->keyword.c_str());

         char downloadFileName[256];
         char mimeFileName[256];
         if(downloadDirectory != NULL) {
            snprintf((char*)&downloadFileName, sizeof(downloadFileName), "%s/%s", downloadDirectory, "/bibtexconv-dXXXXXX");
         }
         else {
            snprintf((char*)&downloadFileName, sizeof(downloadFileName), "%s", "/tmp/bibtexconv-dXXXXXX");
         }
         snprintf((char*)&mimeFileName, sizeof(mimeFileName), "%s",     "/tmp/bibtexconv-mXXXXXX");

         if( (mkstemp((char*)&downloadFileName) > 0) &&
             (mkstemp((char*)&mimeFileName) > 0) ) {
            FILE* downloadFH = fopen(downloadFileName, "w+b");
            if(downloadFH != NULL) {
               FILE* headerFH = tmpfile();
               if(headerFH != NULL) {
                  CURL* curl = curl_easy_init();
                  if(curl != NULL) {
                     curl_easy_setopt(curl, CURLOPT_URL,            url->value.c_str());
                     curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
                     curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
                     curl_easy_setopt(curl, CURLOPT_WRITEDATA,      downloadFH);
                     curl_easy_setopt(curl, CURLOPT_WRITEHEADER,    headerFH);
                     curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);   // follow redirects
                     curl_easy_setopt(curl, CURLOPT_AUTOREFERER,    1L);   // set referer on redirect

                     const CURLcode result = curl_easy_perform(curl);
                     if(result == CURLE_OK) {
                        unsigned long long totalSize = 0;
                        rewind(headerFH);
                        rewind(downloadFH);

                        // ====== Check HTTP result =========================
                        bool resultIsGood = true;
                        if( (strncmp(url->value.c_str(), "http", 4)) == 0) {
                           unsigned int v1, v2, httpErrorCode = 999;
                           char         header[8192];
                           while(!feof(headerFH)) {
                              // The actual result will be of the last request
                              // in the header file (may have been redirected!)
                              if(!fgets((char*)&header, sizeof(header) - 1, headerFH)) {
                                 break;
                              }
                              sscanf(header, "HTTP/%u.%u %u ", &v1, &v2, &httpErrorCode);
                           }
                           if(httpErrorCode != 200) {
                              resultIsGood = false;
                              fprintf(stderr, "FAILED %s - HTTP returns code %u!\n",
                                      url->value.c_str(), httpErrorCode);
/*
                              rewind(headerFH);
                              size_t r = fread((char*)&header, 1, sizeof(header) - 1, headerFH);
                              if(r > 0) {
                                 header[r] = 0x00;
                                 fputs(header, stderr);
                              }
*/
                              errors++;
                           }
                        }

                        if(resultIsGood) {
                           unsigned char md5[MD5_DIGEST_LENGTH];
                           MD5_CTX md5_ctx;
                           MD5_Init(&md5_ctx);

                           // ====== Compute size and MD5 ======================
                           while(!feof(downloadFH)) {
                              char input[16384];
                              const size_t bytesRead = fread(&input, 1, sizeof(input), downloadFH);
                              if(bytesRead > 0) {
                                 totalSize += (unsigned long long)bytesRead;
                                 MD5_Update(&md5_ctx, &input, bytesRead);
                              }
                           }

                           if(totalSize > 0) {
                              // ====== Compute mime type (using "file") =======
                              std::string mimeString;
                              std::string command = format("/usr/bin/file --mime-type -b %s >%s", downloadFileName, mimeFileName);
                              if(system(command.c_str()) == 0) {
                                 FILE* mimeFH = fopen(mimeFileName, "r");
                                 if(mimeFH != NULL) {
                                    char input[1024];
                                    if(fgets((char*)&input, sizeof(input) - 1, mimeFH) != NULL) {
                                       mimeString = std::string(input);
                                       if( (mimeString.size() > 0) &&
                                           (mimeString[mimeString.size() - 1] == '\n') ) {
                                          mimeString = mimeString.substr(0, mimeString.size() - 1);
                                       }
                                    }
                                    fclose(mimeFH);
                                 }
                              }
                              else {
                                 fprintf(stderr, "FAILED %s: failed to obtain mime type of download file!\n",
                                         url->value.c_str());
                              }

                              // ====== Compare size, mime type and MD5 ========
                              std::string sizeString = format("%llu", totalSize);
                              std::string md5String;
                              MD5_Final((unsigned char*)&md5, &md5_ctx);
                              for(unsigned int i = 0; i < MD5_DIGEST_LENGTH; i++) {
                                 md5String += format("%02x", (unsigned int)md5[i]);
                              }

                              const Node* urlSizeNode = findChildNode(publication, "url.size");
                              if((urlSizeNode != NULL) && (urlSizeNode->value != sizeString)) {
                                 fprintf(stderr, "FAILED %s: old size has been %s, new size is %s\n",
                                         url->value.c_str(),
                                         urlSizeNode->value.c_str(), sizeString.c_str());
                                 errors++;
                              }
                              const Node* urlMimeNode = findChildNode(publication, "url.mime");
                              if((urlMimeNode != NULL) && (urlMimeNode->value != mimeString)) {
                                 fprintf(stderr, "FAILED %s: old mime type has been %s, new type mime is %s\n",
                                         url->value.c_str(),
                                         urlMimeNode->value.c_str(), mimeString.c_str());
                                 errors++;
                              }
                              const Node* urlMD5Node = findChildNode(publication, "url.md5");
                              if((urlMD5Node != NULL) && (urlMD5Node->value != md5String)) {
                                 fprintf(stderr, "FAILED %s: old MD5 has been %s, new MD5 is %s\n",
                                         url->value.c_str(),
                                         urlMD5Node->value.c_str(), md5String.c_str());
                                 errors++;
                              }

                              // ====== Update size, mime type and MD5 =========
                              addOrUpdateChildNode(publication, "url.size", sizeString.c_str());
                              addOrUpdateChildNode(publication, "url.mime", mimeString.c_str());
                              addOrUpdateChildNode(publication, "url.md5",  md5String.c_str());

                              // ====== Update check time ======================
                              const unsigned long long microTime = getMicroTime();
                              const time_t             timeStamp = microTime / 1000000;
                              const tm*                timeptr   = localtime(&timeStamp);
                              char  checkTime[128];
                              strftime((char*)&checkTime, sizeof(checkTime), "%Y-%m-%d %H:%M:%S %Z", timeptr);
                              addOrUpdateChildNode(publication, "url.checked", checkTime);

                              fprintf(stderr, "OK: size=%sB; type=%s; MD5=%s\n",
                                      sizeString.c_str(), mimeString.c_str(), md5String.c_str());

                              if(downloadDirectory != NULL) {
                                 fclose(downloadFH);
                                 downloadFH = NULL;
                                 const std::string newFileName =
                                    PublicationSet::makeDownloadFileName(downloadDirectory, publication->keyword, mimeString);
                                 if(rename(downloadFileName, newFileName.c_str()) < 0) {
                                    unlink(downloadFileName);
                                    fprintf(stderr, "FAILED to store download file %s: %s!\n",
                                            newFileName.c_str(), strerror(errno));
                                    exit(1);
                                 }
                              }
                           }
                           else {
                              fprintf(stderr, "FAILED %s: size is zero!\n", url->value.c_str());
                              errors++;
                           }
                        }
                     }
                     else {
                        fprintf(stderr, "FAILED %s: %s!\n", url->value.c_str(), curl_easy_strerror(result));
                        errors++;
                     }
                     curl_easy_cleanup(curl);
                  }
                  else {
                     fputs("ERROR: Failed to initialize libcurl!\n", stderr);
                     exit(1);
                  }
                  fclose(headerFH);
               }
               else {
                  fputs("ERROR: Failed to create temporary header file!\n", stderr);
                  exit(1);
               }
               if(downloadFH != NULL) {
                  fclose(downloadFH);
                  unlink(downloadFileName);
               }
               unlink(mimeFileName);
            }
            else {
               fputs("ERROR: Failed to create temporary download file!\n", stderr);
               exit(1);
            }
         }
         else {
            fputs("ERROR: Failed to create temporary file name!\n", stderr);
            exit(1);
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
                       const char*     downloadDirectory,
                       const bool      checkURLs,
                       const bool      checkNewURLsOnly,
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
               result += checkAllURLs(&publicationSet, downloadDirectory, checkNewURLsOnly);
            }
            const char* namingTemplate = "%u";
            if(input[6] == ' ') {
               namingTemplate = (const char*)&input[7];
            }
            if(PublicationSet::exportPublicationSetToCustom(
                  &publicationSet, namingTemplate,
                  customPrintingHeader, customPrintingTrailer,
                  customPrintingTemplate, monthNames, nbsp, useXMLStyle,
                  downloadDirectory, stdout) == false) {
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
                  result += handleInput(includeFH, publicationSet,
                                        downloadDirectory, checkURLs, checkNewURLsOnly,
                                        recursionLevel + 1);
                  fclose(includeFH);
               }
               else {
                  fprintf(stderr, "ERROR: Unable to open include file '%s'!\n", includeFileName);
                  exit(1);
                  break;
               }
            }
            else {
               fprintf(stderr, "ERROR: Include file nesting level limit reached!\n");
               exit(1);
               break;
            }
         }
         else if((strncmp(input, "monthNames ", 11)) == 0) {
            std::string  s = (const char*)&input[11];
            unsigned int i = 0;
            while(s != "") {
               const std::string token = extractToken(trim(s), " ");
               monthNames[i] = token;
               if(i > 11) {
                  fputs("ERROR: There are only 12 month names possible in monthNames!\n", stderr);
                  exit(1);
                  break;
               }
               i++;
            }
         }
         else {
            fprintf(stderr, "ERROR: Bad command '%s'!\n", input);
            exit(1);
            break;
         }
      }
   }
   return(result);
}



// ###### Main program ######################################################
int main(int argc, char** argv)
{
   bool        interactive              = true;
   bool        checkURLs                = false;
   bool        checkNewURLsOnly         = false;
   bool        skipNotesWithISBNandISSN = true;
   bool        addNotesWithISBNandISSN  = false;
   bool        addUrlCommand            = false;
   const char* exportToBibTeX           = NULL;
   const char* exportToSeparateBibTeXs  = NULL;
   const char* exportToXML              = NULL;
   const char* exportToSeparateXMLs     = NULL;
   const char* exportToCustom           = NULL;
   const char* downloadDirectory        = NULL;

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
      fprintf(stderr, "Usage: %s [BibTeX file] {-export-to-bibtex=file} {-export-to-separate-bibtexs=prefix} {-export-to-xml=file} {-export-to-separate-xmls=prefix} {-export-to-custom=file} {-non-interactive} {-nbsp=string} {-check-urls} {-only-check-new-urls} {-add-url-command} {-skip-notes-with-isbn-and-issn} {-add-notes-with-isbn-and-issn} {-store-downloads=directory}\n", argv[0]);
      exit(1);
   }
   for(int i = 2; i < argc; i++) {
      if( strncmp(argv[i], "-export-to-bibtex=", 18) == 0 ) {
         exportToBibTeX = (const char*)&argv[i][18];
      }
      else if( strncmp(argv[i], "-export-to-separate-bibtexs=", 28) == 0 ) {
         exportToSeparateBibTeXs = (const char*)&argv[i][28];
      }
      else if( strncmp(argv[i], "-export-to-xml=", 15) == 0 ) {
         exportToXML = (const char*)&argv[i][15];
      }
      else if( strncmp(argv[i], "-export-to-separate-xmls=", 25) == 0 ) {
         exportToSeparateXMLs = (const char*)&argv[i][25];
      }
      else if( strncmp(argv[i], "-export-to-custom=", 18) == 0 ) {
         exportToCustom = (const char*)&argv[i][18];
      }
      else if( strncmp(argv[i], "-store-downloads=", 17) == 0 ) {
         downloadDirectory = (const char*)&argv[i][17];
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
      else if( strcmp(argv[i], "-only-check-new-urls") == 0 ) {
         checkNewURLsOnly = true;
      }
      else if( strcmp(argv[i], "-add-url-command") == 0 ) {
         addUrlCommand = true;
      }
      else if( strcmp(argv[i], "-skip-notes-with-isbn-and-issn") == 0 ) {
         skipNotesWithISBNandISSN = true;
      }
      else if( strcmp(argv[i], "-add-notes-with-isbn-and-issn") == 0 ) {
         skipNotesWithISBNandISSN = true;   // Drop old ones, if there are any
         addNotesWithISBNandISSN  = true;   // Compute new ones
      }
      else {
         fprintf(stderr, "ERROR: Bad argument %s!\n", argv[i]);
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
            result += checkAllURLs(&publicationSet, downloadDirectory, checkNewURLsOnly);
         }
      }
      else {
         fprintf(stderr, "Got %u publications from BibTeX file.\n",
                 (unsigned int)publicationSet.maxSize());
         result = handleInput(stdin, publicationSet,
                              downloadDirectory, checkURLs, checkNewURLsOnly);
         fprintf(stderr, "Done. %u errors have occurred.\n", result);
      }

      // ====== Export all to BibTeX ========================================
      if(exportToBibTeX) {
         if(PublicationSet::exportPublicationSetToBibTeX(
            &publicationSet, exportToBibTeX, false,
            skipNotesWithISBNandISSN, addNotesWithISBNandISSN, addUrlCommand) == false) {
            exit(1);
         }
      }
      if(exportToSeparateBibTeXs) {
         if(PublicationSet::exportPublicationSetToBibTeX(
            &publicationSet, exportToSeparateBibTeXs, true,
            skipNotesWithISBNandISSN, addNotesWithISBNandISSN, addUrlCommand) == false) {
            exit(1);
         }
      }

      // ====== Export all to XML ===========================================
      if(exportToXML) {
         if(PublicationSet::exportPublicationSetToXML(
            &publicationSet, exportToXML, false) == false) {
            exit(1);
         }
      }
      if(exportToSeparateXMLs) {
         if(PublicationSet::exportPublicationSetToXML(
            &publicationSet, exportToSeparateXMLs, true) == false) {
            exit(1);
         }
      }

      // ====== Export all to custom format =================================
      if(exportToCustom) {
         if(PublicationSet::exportPublicationSetToCustom(
               &publicationSet, "%u",
               customPrintingHeader, customPrintingTrailer,
               customPrintingTemplate, monthNames,
               nbsp, useXMLStyle, downloadDirectory,
               stdout) == false) {
            exit(1);
         }
      }
   }
   if(bibTeXFile) {
      freeNode(bibTeXFile);
      bibTeXFile = NULL;
   }

   return result;
}
