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

#include "mappings.h"
#include "node.h"
#include "publicationset.h"
#include "package-version.h"
#include "stringhandling.h"

#include <getopt.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <openssl/evp.h>


extern int   yyparse();
extern FILE* yyin;
extern Node* bibTeXFile;


// ###### Get current timer #################################################
static unsigned long long getMicroTime()
{
  struct timeval tv;
  gettimeofday(&tv,nullptr);
  return(((unsigned long long)tv.tv_sec * (unsigned long long)1000000) +
         (unsigned long long)tv.tv_usec);
}


// ###### Download file via libcurl #########################################
static bool downloadFile(CURL*         curl,
                         const char*   url,
                         FILE*         headerFH,
                         FILE*         downloadFH,
                         unsigned int& errors)
{
   if( (ftruncate(fileno(headerFH), 0) != 0) ||
       (ftruncate(fileno(downloadFH), 0) != 0) ) {
      perror("Unable to truncate output files");
      return(false);
   }

   curl_easy_setopt(curl, CURLOPT_URL,            url);
   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1L);
   curl_easy_setopt(curl, CURLOPT_WRITEDATA,      downloadFH);
   curl_easy_setopt(curl, CURLOPT_WRITEHEADER,    headerFH);
   curl_easy_setopt(curl, CURLOPT_USERAGENT,      "bibtexconv/1.1 (AmigaOS; MC680x0)");
   curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);   // follow redirects
   curl_easy_setopt(curl, CURLOPT_AUTOREFERER,    1L);   // set referer on redirect
   curl_easy_setopt(curl, CURLOPT_COOKIEFILE,     "");   // enable cookies
   curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);   // 30s connect timeout

   bool  resultIsGood    = false;
   const CURLcode result = curl_easy_perform(curl);
   if(result == CURLE_OK) {
      rewind(headerFH);
      rewind(downloadFH);

      // ====== Check HTTP result =========================
      if( (strncmp(url, "http", 4)) == 0) {
         unsigned httpErrorCode = 999;
         char     header[8192];
         while(!feof(headerFH)) {
            // The actual result will be of the last request
            // in the header file (may have been redirected!)
            if(!fgets((char*)&header, sizeof(header) - 1, headerFH)) {
               break;
            }
            sscanf(header, "HTTP/%*[^ ] %u ", &httpErrorCode);
         }
         if(httpErrorCode == 200) {
             resultIsGood = true;
         }
         if(httpErrorCode != 200) {
            fprintf(stderr, "FAILED %s - HTTP returns code %u!\n",
                    url, httpErrorCode);

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
   }
   else {
      fprintf(stderr, "FAILED %s: %s!\n", url, curl_easy_strerror(result));
      errors++;
   }

   return(resultIsGood);
}


// ###### Dynamic URL handling ##############################################
static bool handleDynamicURL(CURL*             curl,
                             const std::string url,
                             FILE*             headerFH,
                             FILE*             downloadFH,
                             unsigned int&     errors)
{
   std::string rest;
   std::string newURL = "";

   // ====== IEEExplore database ============================================
   if( (hasPrefix(url, "http://ieeexplore.ieee.org/", rest)) ||
       (hasPrefix(url, "https://ieeexplore.ieee.org/", rest)) ) {
      char buffer[65536];
      size_t r = fread((char*)&buffer, 1, sizeof(buffer) - 1, downloadFH);
      if((r > 0) && (r < sizeof(buffer) - 1)) {
         buffer[r] = 0x00;

         fprintf(stderr, "[IEEExplore");

         const std::string inputString(buffer);
         const size_t      framePos = inputString.rfind("<frame src=\"");
         // fputs(inputString.c_str(),stderr);
         if(framePos != std::string::npos) {
            const size_t a = inputString.find("\"", framePos);
            const size_t b = inputString.find("\"", a + 1);
            if( (a != std::string::npos) && (b != std::string::npos) ) {
               newURL = inputString.substr(a + 1, b - a - 1);
               fprintf(stderr, "->%s", newURL.c_str());
            }
         }

         fprintf(stderr, "] ");
      }
   }

   rewind(headerFH);
   rewind(downloadFH);
   if(newURL.size() > 0) {
      // printf("NEW=<%s>\n", newURL.c_str());
      return(downloadFile(curl, newURL.c_str(), headerFH, downloadFH, errors));
   }

   return(true);
}


// ###### Check URLs ########################################################
static unsigned int checkAllURLs(PublicationSet* publicationSet,
                                 const char*     downloadDirectory,
                                 const bool      checkNewURLsOnly,
                                 const bool      ignoreUpdatesForHTML)
{
   if(downloadDirectory != nullptr) {
      if( (mkdir(downloadDirectory, S_IRWXU|S_IXGRP|S_IRGRP|S_IXOTH|S_IROTH) < 0) &&
          (errno != EEXIST) ) {
         fprintf(stderr, "ERROR: Failed to create download directory: %s!\n",
                 strerror(errno));
         exit(1);
      }
   }

   CURL* curl = curl_easy_init();
   if(curl == nullptr) {
      fputs("ERROR: Failed to initialize libcurl!\n", stderr);
      exit(1);
   }

   unsigned int errors = 0;
   for(size_t index = 0; index < publicationSet->size(); index++) {
      // ====== Get prev, current and next publications =====================
      if(publicationSet->get(index)->value == "Comment") {
         continue;
      }
      Node* publication = publicationSet->get(index);
      Node* url         = findChildNode(publication, "url");
      if(url != nullptr) {
         const Node* urlSize    = findChildNode(publication, "url.size");
         const Node* urlMime    = findChildNode(publication, "url.mime");
         const Node* urlChecked = findChildNode(publication, "url.checked");
         if( (urlSize != nullptr) && (urlMime != nullptr) && (urlChecked != nullptr) ) {
            if(downloadDirectory != nullptr) {
               const std::string downloadFileName =
                  PublicationSet::makeDownloadFileName(downloadDirectory,
                                                       publication->keyword,
                                                       urlMime->value);
               FILE* downloadFH = fopen(downloadFileName.c_str(), "rb");
               if(downloadFH != nullptr) {
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
         char metaFileName[256];
         if(downloadDirectory != nullptr) {
            snprintf((char*)&downloadFileName, sizeof(downloadFileName), "%s/%s", downloadDirectory, "/bibtexconv-dXXXXXX");
         }
         else {
            snprintf((char*)&downloadFileName, sizeof(downloadFileName), "%s", "/tmp/bibtexconv-dXXXXXX");
         }
         snprintf((char*)&mimeFileName, sizeof(mimeFileName), "%s",      "/tmp/bibtexconv-mXXXXXX");
         snprintf((char*)&metaFileName, sizeof(metaFileName), "%s",      "/tmp/bibtexconv-pXXXXXX");

         const int dfd = mkstemp((char*)&downloadFileName);
         const int mfd = mkstemp((char*)&mimeFileName);
         if( (dfd > 0) && (mfd > 0) ) {
            FILE* downloadFH = fopen(downloadFileName, "w+b");
            if(downloadFH != nullptr) {
               FILE* headerFH = tmpfile();
               if(headerFH != nullptr) {
                  bool resultIsGood = downloadFile(curl, url->value.c_str(), headerFH, downloadFH, errors);
                  if(resultIsGood) {
                     // Special handling for dynamic URLs of some publishers
                     resultIsGood = handleDynamicURL(curl, url->value, headerFH, downloadFH, errors);
                  }
                  if(resultIsGood) {
                     unsigned long long totalSize = 0;

                     unsigned int  md5Length = EVP_MD_size(EVP_md5());
                     unsigned char md5[EVP_MD_size(EVP_md5())];
                     EVP_MD_CTX*   md5Context = EVP_MD_CTX_new();
                     EVP_DigestInit_ex(md5Context, EVP_md5(), nullptr);

                     // ====== Compute size and MD5 =========================
                     while(!feof(downloadFH)) {
                        char input[16384];
                        const size_t bytesRead = fread(&input, 1, sizeof(input), downloadFH);
                        if(bytesRead > 0) {
                           totalSize += (unsigned long long)bytesRead;
                           EVP_DigestUpdate(md5Context, &input, bytesRead);
                        }
                     }

                     if(totalSize > 0) {
                        // ====== Compute mime type (using "file") =======
                        std::string mimeString;
                        std::string command = format("/usr/bin/file --mime-type -b %s >%s", downloadFileName, mimeFileName);
                        if(system(command.c_str()) == 0) {
                           FILE* mimeFH = fopen(mimeFileName, "r");
                           if(mimeFH != nullptr) {
                              char input[1024];
                              if(fgets((char*)&input, sizeof(input) - 1, mimeFH) != nullptr) {
                                 mimeString = std::string(input);
                                 if( (mimeString.size() > 0) &&
                                     (mimeString[mimeString.size() - 1] == '\n') ) {
                                    mimeString = mimeString.substr(0, mimeString.size() - 1);
                                 }

                                 // RFCs/I-Ds are sometimes misidentified as source code:
                                 if( (mimeString == "text/x-pascal") ||
                                     (mimeString == "text/x-c") ||
                                     (mimeString == "text/x-c++") ) {
                                    mimeString = "text/plain";
                                 }
                              }
                              fclose(mimeFH);
                           }
                        }
                        else {
                           fprintf(stderr, "WARNING %s: failed to obtain mime type of download file!\n",
                                   url->value.c_str());
                        }

                        // ====== Compare size, mime type and MD5 ===========
                        std::string sizeString = format("%llu", totalSize);
                        std::string md5String;
                        EVP_DigestFinal_ex(md5Context, (unsigned char*)&md5, &md5Length);
                        for(unsigned int i = 0; i < md5Length; i++) {
                           md5String += format("%02x", (unsigned int)md5[i]);
                        }
                        const Node* urlMimeNode = findChildNode(publication, "url.mime");
                        const Node* urlSizeNode = findChildNode(publication, "url.size");
                        const Node* urlMD5Node  = findChildNode(publication, "url.md5");

                        bool failed = false;
                        if((urlMimeNode != nullptr) && (urlMimeNode->value != mimeString)) {
                           if( (urlMimeNode->value == "text/html") &&
                               (mimeString == "application/pdf") ) {
                              fprintf(stderr, "\nNOTE: change from HTML to PDF -> just updating entry\n");
                              urlSizeNode = nullptr;
                              urlMD5Node  = nullptr;
                           }
                           else {
                              fprintf(stderr, "UPDATED %s: old mime type has been %s, new type mime is %s\n",
                                      url->value.c_str(),
                                      urlMimeNode->value.c_str(), mimeString.c_str());
                           }
                        }
                        if( (!failed) && (urlSizeNode != nullptr) && (urlSizeNode->value != sizeString) ) {
                            if( (ignoreUpdatesForHTML == true) &&
                                ((urlMimeNode != nullptr) &&
                                 ((urlMimeNode->value == "text/html") ||
                                  (urlMimeNode->value == "application/xml"))) ) {
                               md5String = "ignore";
                               fprintf(stderr, "[Size change for HTML/XML document -> setting url.md5=\"ignore\"] ");
                            }
                            else {
                              fprintf(stderr, "UPDATED %s: old size has been %s, new size is %s\n",
                                      url->value.c_str(),
                                      urlSizeNode->value.c_str(), sizeString.c_str());
                            }
                        }
                        if( (!failed) && (urlMD5Node != nullptr) && (urlMD5Node->value != "ignore") &&
                           (urlMD5Node->value != md5String)) {
                            if( (ignoreUpdatesForHTML == true) &&
                                ((urlMimeNode != nullptr) &&
                                 ((urlMimeNode->value == "text/html") ||
                                  (urlMimeNode->value == "application/xml"))) ) {
                               md5String = "ignore";
                               fprintf(stderr, "[MD5 change for HTML/XML document -> setting url.md5=\"ignore\"] ");
                            }
                            else {
                               fprintf(stderr, "UPDATED %s: old MD5 has been %s, new MD5 is %s\n",
                                       url->value.c_str(),
                                       urlMD5Node->value.c_str(), md5String.c_str());
                            }
                        }

                        // ====== Check PDF metadata ========================
                        if(mimeString == "application/pdf") {
                           std::string command = format("/usr/bin/pdfinfo %s >%s", downloadFileName, metaFileName);
                           if(system(command.c_str()) == 0) {
                              FILE* metaFH = fopen(metaFileName, "r");
                              if(metaFH != nullptr) {
                                 while(!feof(metaFH)) {
                                    char input[1024];
                                    if(fgets((char*)&input, sizeof(input) - 1, metaFH) != nullptr) {
                                       // printf("IN=%s",input);
                                       if(strncmp(input, "Pages:", 6) == 0) {
                                          addOrUpdateChildNode(publication, "numpages", format("%u", atol((const char*)&input[6])).c_str());
                                       }
                                       else if(strncmp(input, "Keywords:", 9) == 0) {
                                          Node* keywords = findChildNode(publication, "keywords");
                                          if(keywords == nullptr) {
                                             // If there are no "keywords", add "url.keywords".
                                             // They can be renamed manually after a check.
                                             std::string keywords((const char*)&input[9]);
                                             addOrUpdateChildNode(publication, "url.keywords",
                                                                  string2utf8(trim(keywords), "~", "").c_str());
                                          }
                                       }
                                       else if(strncmp(input, "Page size:", 10) == 0) {
                                          std::string pagesize((const char*)&input[10]);
                                          addOrUpdateChildNode(publication, "url.pagesize",
                                                               string2utf8(trim(pagesize), "~", "").c_str());
                                       }
                                    }
                                 }
                                 fclose(metaFH);
                              }
                           }

                        }

                        // ====== Update metadata ===========================
                        if(!failed) {
                           // ====== Update size, mime type and MD5 =========
                           addOrUpdateChildNode(publication, "url.size", sizeString.c_str());
                           addOrUpdateChildNode(publication, "url.mime", mimeString.c_str());
                           if( (urlMD5Node == nullptr) || (urlMD5Node->value != "ignore")) {
                              addOrUpdateChildNode(publication, "url.md5",  md5String.c_str());
                           }

                           // ====== Update check time ======================
                           const unsigned long long microTime = getMicroTime();
                           const time_t             timeStamp = microTime / 1000000;
                           const tm*                timeptr   = localtime(&timeStamp);
                           char  checkTime[128];
                           strftime((char*)&checkTime, sizeof(checkTime), "%Y-%m-%d %H:%M:%S %Z", timeptr);
                           addOrUpdateChildNode(publication, "url.checked", checkTime);

                           fprintf(stderr, "OK: size=%sB;\ttype=%s;\tMD5=%s\n",
                                   sizeString.c_str(), mimeString.c_str(), md5String.c_str());

                           // ====== Move downloaded file ===================
                           if(downloadDirectory != nullptr) {
                              fclose(downloadFH);
                              downloadFH = nullptr;
                              const std::string newFileName =
                                 PublicationSet::makeDownloadFileName(downloadDirectory, publication->keyword, mimeString);
                              if(rename(downloadFileName, newFileName.c_str()) < 0) {
                                 unlink(downloadFileName);
                                 fprintf(stderr, "\nFAILED to store download file %s: %s!\n",
                                         newFileName.c_str(), strerror(errno));
                                 errors++;
                              }
                           }
                        }
                     }
                     else {
                        fprintf(stderr, "\nFAILED %s: size is zero!\n", url->value.c_str());
                        errors++;
                     }
                  }
                  fclose(headerFH);
                  headerFH = nullptr;
               }
               else {
                  fputs("ERROR: Failed to create temporary header file!\n", stderr);
                  errors++;
               }
               if(downloadFH != nullptr) {
                  fclose(downloadFH);
                  downloadFH = nullptr;
                  unlink(downloadFileName);
               }
               unlink(mimeFileName);
            }
            else {
               fputs("ERROR: Failed to create temporary download file!\n", stderr);
               errors++;
            }
         }
         else {
            fputs("ERROR: Failed to create temporary file name!\n", stderr);
            errors++;
         }
         if(dfd >= 0) {
            close(dfd);
         }
         if(mfd >= 0) {
            close(mfd);
         }
      }
   }

   curl_easy_cleanup(curl);
   curl = nullptr;

   return(errors);
}


// ###### Handle interactive input ##########################################
static bool                     useXMLStyle            = false;
static std::string              nbsp                   = " ";
static std::string              lineBreak              = "\n";
static std::string              customPrintingHeader   = "";
static std::string              customPrintingTrailer  = "";
static std::string              customPrintingTemplate =
   "\\[%C\\] %L\n %a\tAUTHOR: [[%fFIRST|%lLAST|%nNOT-FIRST]: initials=%g given=%G full=%F]\n%A\n";  // ", \"%T\"[, %B][, %J][, %?][, %$][, Volume~%V][, Number~%N][, pp.~%P][, %I][, %i][, %@][, [[%m, %D, |%m~]%Y].\\nURL: %U.\\n\\n";
static std::vector<std::string> monthNames;

static int handleInput(FILE*           fh,
                       PublicationSet& publicationSet,
                       const char*     downloadDirectory,
                       const Mappings& mappings,
                       const bool      checkURLs,
                       const bool      checkNewURLsOnly,
                       const bool      ignoreUpdatesForHTML,
                       const char*     exportToBibTeX,
                       const char*     exportToSeparateBibTeXs,
                       const char*     exportToXML,
                       const char*     exportToSeparateXMLs,
                       const bool      skipNotesWithISBNandISSN,
                       const bool      addNotesWithISBNandISSN,
                       const bool      addUrlCommand,
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
               fprintf(stderr, "ERROR: Publication '%s' not found!\n", keyword.c_str());
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
               result += checkAllURLs(&publicationSet, downloadDirectory, checkNewURLsOnly, ignoreUpdatesForHTML);
            }
            const char* namingTemplate = "%u";
            if(input[6] == ' ') {
               namingTemplate = (const char*)&input[7];
            }

            // ====== Export all to custom ==================================
            if(PublicationSet::exportPublicationSetToCustom(
                  &publicationSet, namingTemplate,
                  customPrintingHeader, customPrintingTrailer,
                  customPrintingTemplate, monthNames, nbsp, lineBreak, useXMLStyle,
                  downloadDirectory, mappings, stdout) == false) {
               result++;
            }

            // ====== Export all to BibTeX ==================================
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

            // ====== Export all to XML =====================================
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
         else if((strncmp(input, "linebreak ", 10)) == 0) {
            lineBreak = (const char*)&input[10];
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
               if(includeFH != nullptr) {
                  result += handleInput(includeFH, publicationSet,
                                        downloadDirectory, mappings,
                                        checkURLs, checkNewURLsOnly, ignoreUpdatesForHTML,
                                        exportToBibTeX, exportToSeparateBibTeXs,
                                        exportToXML, exportToSeparateXMLs,
                                        skipNotesWithISBNandISSN, addNotesWithISBNandISSN,
                                        addUrlCommand,
                                        recursionLevel + 1);
                  fclose(includeFH);
               }
               else {
                  fprintf(stderr, "ERROR: Unable to open include file '%s'!\n", includeFileName);
                  exit(1);
               }
            }
            else {
               fprintf(stderr, "ERROR: Include file nesting level limit reached!\n");
               exit(1);
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
               }
               i++;
            }
         }
         else {
            fprintf(stderr, "ERROR: Bad command '%s'!\n", input);
            exit(1);
         }
      }
   }
   return(result);
}


// ###### Version ###########################################################
[[ noreturn ]] static void version()
{
   printf("BibTeXConv %s\n", BIBTEXCONV_VERSION);
   exit(0);
}


// ###### Usage #############################################################
[[ noreturn ]] static void usage(const char* program, const int exitCode)
{
   fprintf(stderr, "Usage: %s BibTeX_file [...] "
      "[-B bibtex_file_name | --export-to-bibtex bibtex_file_name]"
      "[-b xml_file_prefix| --export-to-separate-bibtexs xml_file_prefix]"
      "[-X xml_file_name | --export-to-xml xml_file_name]"
      "[-x xml_file_prefix | --export-to-separate-xmls xml_file_prefix]"
      "[-C custom_file | --export-to-custom custom_file]"
      "[-D directory | --store-downloads directory]"
      "[-m name:mapping_file:key_column:value_column | --mapping name:mapping_file:key_column:value_column]"
      "[-s string | --nbsp string]"
      "[-l string | --linebreak string]"
      "[-n | --non-interactive]"
      "[-U | --check-urls]"
      "[-u | --only-check-new-urls]"
      "[-w | --ignore-updates-for-html]"
      "[-a | --add-url-command]"
      "[-i | --skip-notes-with-isbn-and-issn]"
      "[-I | --add-notes-with-isbn-and-issn]"
      "[-h | --help]"
      "[-v | --version]"
      "\n", program);
   exit(exitCode);
}



// ###### Main program ######################################################
int main(int argc, char** argv)
{
   bool        interactive              = true;
   bool        checkURLs                = false;
   bool        checkNewURLsOnly         = false;
   bool        ignoreUpdatesForHTML     = false;
   bool        skipNotesWithISBNandISSN = false;
   bool        addNotesWithISBNandISSN  = false;
   bool        addUrlCommand            = false;
   const char* exportToBibTeX           = nullptr;
   const char* exportToSeparateBibTeXs  = nullptr;
   const char* exportToXML              = nullptr;
   const char* exportToSeparateXMLs     = nullptr;
   const char* exportToCustom           = nullptr;
   const char* downloadDirectory        = nullptr;
   Mappings    mappings;

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

   const static struct option long_options[] = {
      { "export-to-bibtex",              required_argument, 0, 'B' },
      { "export-to-separate-bibtexs",    required_argument, 0, 'b' },
      { "export-to-xml",                 required_argument, 0, 'X' },
      { "export-to-separate-xmls",       required_argument, 0, 'x' },
      { "export-to-custom",              required_argument, 0, 'C' },
      { "store-downloads",               required_argument, 0, 'D' },
      { "mapping",                       required_argument, 0, 'm' },

      { "nbsp",                          required_argument, 0, 's' },
      { "linebreak",                     required_argument, 0, 'l' },
      { "non-interactive",               no_argument,       0, 'n' },
      { "check-urls",                    no_argument,       0, 'U' },
      { "only-check-new-urls",           no_argument,       0, 'u' },
      { "ignore-updates-for-html",       no_argument,       0, 'w' },
      { "add-url-command",               no_argument,       0, 'a' },
      { "skip-notes-with-isbn-and-issn", no_argument,       0, 'i' },
      { "add-notes-with-isbn-and-issn",  no_argument,       0, 'I' },

      { "help",                          no_argument,       0, 'h' },
      { "version",                       no_argument,       0, 'v' },
      {  nullptr,                        0,                 0, 0   }
   };

   int option;
   int longIndex;
   while( (option = getopt_long(argc, argv, "B:b:X:x:C:D:m:s:l:nUuwaiIhv", long_options, &longIndex)) != -1 ) {
      switch(option) {
         case 'B':
            exportToBibTeX = optarg;
          break;
         case 'b':
            exportToSeparateBibTeXs = optarg;
          break;
         case 'X':
            exportToXML = optarg;
          break;
         case 'x':
            exportToSeparateXMLs = optarg;
          break;
         case 'C':
            exportToCustom = optarg;
          break;
         case 'D':
            downloadDirectory = optarg;
          break;
         case 'm': {
            std::vector<std::string> mappingArguments;
            splitString(mappingArguments, std::string(optarg));
            if(mappingArguments.size() == 4) {
               if(!mappings.addMapping(mappingArguments[0], mappingArguments[1],
                                      mappingArguments[2], mappingArguments[3])) {
                  exit(1);
               }
            }
            else {
               fprintf(stderr, "ERROR: Bad mapping specification %s!\n", optarg);
               exit(1);
            }
          }
          break;
         case 's':
            nbsp = optarg;
          break;
         case 'l':
            lineBreak = optarg;
          break;
         case 'n':
            interactive = false;
          break;
         case 'U':
            checkURLs = true;
          break;
         case 'u':
            checkNewURLsOnly = true;
          break;
         case 'w':
            ignoreUpdatesForHTML = true;
          break;
         case 'a':
            addUrlCommand = true;
          break;
         case 'i':
            skipNotesWithISBNandISSN = true;
          break;
         case 'I':
            skipNotesWithISBNandISSN = true;   // Drop old ones, if there are any
            addNotesWithISBNandISSN  = true;   // Compute new ones
          break;
         case 'v':
            version();
          break;
         case 'h':
            usage(argv[0], 0);
          break;
         default:
          break;
      }
   }

   int result = 0;
   if(optind < argc) {
      do {
         yyin = fopen(argv[optind], "r");
         if(yyin == nullptr) {
            fprintf(stderr, "ERROR: Unable to open BibTeX input file %s!\n", argv[optind]);
            exit(1);
         }
         result = yyparse();
         fclose(yyin);
         if(result != 0) {
            break;
         }
         optind++;
      } while(optind < argc);
   }
   else {
      fputs("ERROR: No BibTeX input file provided!\n", stderr);
      exit(1);
   }

   if(result == 0) {
      PublicationSet publicationSet(countNodes(bibTeXFile));
      if(!interactive) {
         publicationSet.addAll(bibTeXFile);
         if(checkURLs) {
            result += checkAllURLs(&publicationSet, downloadDirectory, checkNewURLsOnly, ignoreUpdatesForHTML);
         }

         // ====== Export all to BibTeX =====================================
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

         // ====== Export all to XML ========================================
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

         // ====== Export all to custom format ==============================
         if(exportToCustom) {
            if(PublicationSet::exportPublicationSetToCustom(
                  &publicationSet, "%u",
                  customPrintingHeader, customPrintingTrailer,
                  customPrintingTemplate, monthNames,
                  nbsp, lineBreak, useXMLStyle, downloadDirectory, mappings,
                  stdout) == false) {
               exit(1);
            }
         }
      }
      else {
         fprintf(stderr, "Got %u publications from BibTeX file.\n",
                 (unsigned int)publicationSet.maxSize());
         result = handleInput(stdin, publicationSet,
                              downloadDirectory, mappings,
                              checkURLs, checkNewURLsOnly, ignoreUpdatesForHTML,
                              exportToBibTeX, exportToSeparateBibTeXs,
                              exportToXML, exportToSeparateXMLs,
                              skipNotesWithISBNandISSN, addNotesWithISBNandISSN,
                              addUrlCommand);
         fprintf(stderr, "Done. %u errors have occurred.\n", result);
      }
   }
   if(bibTeXFile) {
      freeNode(bibTeXFile);
      bibTeXFile = nullptr;
   }

   return result;
}
