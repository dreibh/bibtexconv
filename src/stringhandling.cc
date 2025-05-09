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
#include <algorithm>

#include "stringhandling.h"


struct ReplaceTableEntry
{
   const std::string input;
   const std::string utf8Output;
   const std::string xmlOutput;
};

static const ReplaceTableEntry replaceTable[] = {
   { "{\\\"a}",    "ä",        "ä"       },   // &auml;
   { "{\\\"o}",    "ö",        "ö"       },   // &ouml;
   { "{\\\"u}",    "ü",        "ü"       },   // &uuml;
   { "{\\\"A}",    "Ä",        "Ä"       },   // &Auml;
   { "{\\\"O}",    "Ö",        "Ö"       },   // &Ouml;
   { "{\\\"U}",    "Ü",        "Ü"       },   // &Uuml;

   { "{\\'a}",     "á",        "á"       },
   { "{\\'A}",     "Á",        "Á"       },
   { "{\\'e}",     "é",        "é"       },
   { "{\\'E}",     "É",        "É"       },
   { "{\\'i}",     "í",        "í"       },
   { "{\\'I}",     "Í",        "Í"       },
   { "{\\\"i}",    "ï",        "ï"       },
   { "{\\\"I}",    "Ï",        "Ï"       },
   { "{\\'o}",     "ó",        "ó"       },
   { "{\\'O}",     "Ó",        "Ó"       },
   { "{\\'u}",     "ú",        "ú"       },
   { "{\\'U}",     "Ú",        "Ú"       },
   { "{\\'y}",     "ý",        "ý"       },
   { "{\\'Y}",     "Ý",        "Ý"       },
   { "{\\\"y}",    "ÿ",        "ÿ"       },
   { "{\\\"Y}",    "Ÿ",        "Ÿ"       },

   { "{\\`a}",     "à",        "à"       },
   { "{\\`A}",     "À",        "À"       },
   { "{\\`e}",     "è",        "è"       },
   { "{\\`E}",     "È",        "È"       },
   { "{\\`i}",     "ì",        "ì"       },
   { "{\\`I}",     "Ì",        "Ì"       },
   { "{\\`o}",     "ò",        "ò"       },
   { "{\\`O}",     "Ò",        "Ò"       },
   { "{\\`u}",     "ù",        "ù"       },
   { "{\\`U}",     "Ù",        "Ù"       },

   { "{^e}",       "ê",        "ê"       },
   { "{^E}",       "Ê",        "Ê"       },
   { "{^u}",       "û",        "û"       },
   { "{^U}",       "Û",        "Û"       },
   { "{^i}",       "î",        "î"       },
   { "{^I}",       "Î",        "Î"       },
   { "{^o}",       "ô",        "ô"       },
   { "{^O}",       "Ô",        "Ô"       },
   { "{^a}",       "â",        "â"       },
   { "{^A}",       "Â",        "Â"       },

   { "{~a}",       "ã",        "ã"       },
   { "{~A}",       "Ã",        "Ã"       },
   { "{~n}",       "ñ",        "ñ"       },
   { "{~N}",       "Ñ",        "Ñ"       },
   { "{~o}",       "õ",        "õ"       },
   { "{~O}",       "Õ",        "Õ"       },

   { "\\c{s}",     "ş",        "ş"       },
   { "\\c{S}",     "Ş",        "Ş"       },

   { "\\={u}",     "ū",        "ū"       },
   { "\\={U}",     "Ū",        "Ū"       },

   { "\\c{c}",     "ç",        "ç"       },
   { "\\C{C}",     "Ç",        "Ç"       },
   { "{\\'c}",     "ć",        "ć"       },
   { "{\\'C}",     "Ć",        "Ć"       },
   { "\\v{c}",     "č",        "č"       },
   { "\\v{C}",     "Č",        "Č"       },
   { "\\v{e}",     "ě",        "ě"       },
   { "\\v{E}",     "Ě",        "Ě"       },
   { "\\v{r}",     "ř",        "ř"       },
   { "\\v{R}",     "Ř",        "Ř"       },
   { "\\v{s}",     "š",        "š"       },
   { "\\v{S}",     "Š",        "Š"       },
   { "\\v{z}",     "ž",        "ž"       },
   { "\\v{Z}",     "Ž",        "Ž"       },

   { "{\\ae}",     "æ",        "æ"       },
   { "{\\AE}",     "Æ",        "Æ"       },
   { "{\\o}",      "ø",        "ø"       },
   { "{\\O}",      "Ø",        "Ø"       },
   { "\\r{a}",     "å",        "å"       },
   { "{\\aa}",     "å",        "å"       },
   { "\\r{A}",     "Å",        "Å"       },
   { "{\\AA}",     "Å",        "Å"       },

   { "<"  ,        "<",        "&lt;"    },
   { ">"  ,        ">",        "&gt;"    },
   { "\\\"" ,      "\"",       "&quot;"  },
   { "&"  ,        "&",        "&amp;"   },
   { "'"  ,        "'",        "&apos;"  },
   { "--",         "–",        "–"       },
   { "\\#",        "\\#",      "#"       }
};


struct LanguageTableEntry
{
   const char* xml;
   const char* latex;
};

// based on ISO 639-1 codes
// URL: http://en.wikipedia.org/wiki/List_of_ISO_639-1_codes
static const LanguageTableEntry languageTable[] = {
   { "de",    "german"     },
   { "de",    "ngerman"    },
   { "de-AT", "austrian"   },
   { "de-AT", "naustrian"  },
   { "en",    "english"    },

   { "en-US", "USenglish"  },
   { "en-US", "american"   },
   { "en-GB", "UKenglish"  },
   { "en-GB", "british"    },
   { "en-CA", "canadian"   },
   { "en-AU", "australian" },
   { "en-NZ", "newzealand" },

   { "nb-NO", "norsk"      },
   { "nn-NO", "nynorsk"    },

   { "fr",    "french"     },
   { "fr",    "francais"   },

   { "it",    "italian"    },
   { "es",    "spanish"    },
   { "pt",    "portuguese" },
   { "sv",    "swedish"    },
   { "da",    "danish"     },
   { "nl",    "dutch"      },
   { "la",    "latin"      },

   { "zh-CN", "chinese"    },
   { "ja-JP", "japanese"   },
   { "arb",   "arabic"     },
   { "fas",   "farsi"      },
   { "ko",    "korean"     },
   { "ru",    "russian"    },
   { "el",    "Greek"      }
};


// ###### Get XML language ##################################################
const char* getXMLLanguageFromLaTeX(const char* language)
{
   for(size_t i = 0; i < (sizeof(languageTable) / sizeof(LanguageTableEntry)); i++) {
      if(strcmp(languageTable[i].latex, language) == 0) {
         return(languageTable[i].xml);
      }
   }
   return(nullptr);
}


// ###### Convert ASCII string to UTF-8 #####################################
std::string string2utf8(const std::string& string,
                        const std::string& nbsp,
                        const std::string& lineBreak,
                        const bool         xmlStyle)
{
   std::string result(string);
   // std::cout << "IN= " << result << "\n";
   for(size_t i = 0; i < (sizeof(replaceTable) / sizeof(ReplaceTableEntry)); i++) {
      replaceAll(result,
                 replaceTable[i].input,
                 ((xmlStyle == true) ? replaceTable[i].xmlOutput : replaceTable[i].utf8Output));
   }
   if(nbsp.size() > 0) {
      replaceAll(result, "~",  nbsp);
   }
   replaceAll(result, "\n", lineBreak);
   // std::cout << "OUT= " << result << "\n";
   return(processBackslash(result));
}


// ###### Remove brackets { ... } and quotation " ... " #####################
std::string& removeBrackets(std::string& string)
{
   while( (string.substr(0, 1) == "{") &&
          (string.substr(string.size() - 1) == "}") ) {
      string = string.substr(1, string.size() - 2);
   }
   while( (string.substr(0, 1) == "\"") &&
          (string.substr(string.size() - 1) == "\"") ) {
      string = string.substr(1, string.size() - 2);
   }
   return(string);
}


// ###### Remove superflous whitespaces from a string #######################
std::string& trim(std::string& string)
{
   // ====== Remove whitespaces from beginning and end ======================
   const ssize_t length = string.size();
   ssize_t s, e;
   for(s = 0; s < length; s++) {
      if( (string[s] != ' ') && (string[s] != '\t')) {
         break;
      }
   }
   for(e = length - 1; e >= 0; e--) {
      if( (string[e] != ' ') && (string[e] != '\t')) {
         break;
      }
   }
   string = string.substr(s, length - s - (length - 1 - e) );

   // ====== Remove double whitespaces ======================================
   bool gotSpace = false;
   for(e = string.size() - 1; e >= 0; e--) {
      if( (string[e] == ' ') || (string[e] == '\t')) {
         if(!gotSpace) {
            gotSpace = true;
         }
         else {
            string.erase(e, 1);
         }
      }
      else {
         gotSpace = false;
      }
   }
   return(string);
}


// ###### Extract token from string #########################################
std::string extractToken(std::string& string, const std::string& delimiters)
{
   if(string[0] == '\"') {
      string = string.substr(1, string.size() - 1);
      if((string.size() > 0) && (string[0] != '\"')) {
         return(extractToken(string, "\""));
      }
      else {
         string = string.substr(1, string.size() - 1);
         return("");
      }
   }
   else if(string[0] == '#') {   // The rest of the line is a comment. Ignore it!
      string = "";
      return("");
   }
   for(size_t i = 0; i < string.size(); i++) {
      if(string[i] == '\\') {
         i++;
         continue;
      }
      for(size_t j = 0; j < delimiters.size(); j++) {
         if(string[i] == delimiters[j]) {
            const std::string result = string.substr(0, i);
            string = string.substr(i + 1, string.size() - i - 1);
            return(result);
         }
      }
   }
   const std::string result = string;
   string = "";
   return(result);
}


// ###### Split string into token by delimiter ##############################
void splitString(std::vector<std::string>& tokenVector,
                 const std::string&        input,
                 const std::string&        delimiter)
{
   size_t startPosition = 0;
   size_t endPosition   = input.find(delimiter);
   while(endPosition != std::string::npos) {
      tokenVector.push_back(
         input.substr(startPosition, endPosition - startPosition));
      startPosition = endPosition + delimiter.length();
      endPosition = input.find(delimiter, startPosition);
   }
   tokenVector.push_back(
      input.substr(startPosition, endPosition - startPosition));
}


// ###### Process backslash commands (newline, tab, etc.) ###################
std::string processBackslash(const std::string& string)
{
   const size_t size   = string.size();
   std::string  result = "";

   for(size_t i = 0; i < size; i++) {
      if( (string[i] == '\\') && (i + 1 < size) ) {
         switch(string[i + 1]) {
            case 'n':
               result += '\n';
             break;
            case 'x':
               if(i + 3 < size) {
                  std::string hex = "";
                  int         value;
                  hex += string[i + 2];
                  hex += string[i + 3];
                  if(sscanf(hex.c_str(), "%02x", &value) == 1) {
                     result += (char)value;
                     i += 2;
                  }
               }
             break;
            case '\\':
            case ' ':
            case '"':
            case '\'':
            case '{':
            case '}':
               result += string[i + 1];
             break;
            default:
               result += string[i + 0];
               result += string[i + 1];
             break;
         }
         i++;
      }
      else {
         result += string[i];
      }
   }
   return(result);
}


// ###### Convert LaTeX-compliant URL to URL ################################
std::string laTeXtoURL(const std::string& str)
{
   std::string result;
   bool        isPrefixed = false;
   for(size_t i = 0; i < str.size(); i++) {
      if(isPrefixed == false) {
         if(str[i] == '\\') {
            isPrefixed = true;
            continue;
         }
      }
      if(str[i] == ' ') {
         result += "%20";
      }
      else {
         result += str[i];
      }
      isPrefixed = false;
   }
   return(result);
}


// ###### Convert URL to LaTeX-compliant URL ################################
std::string urlToLaTeX(const std::string& str)
{
   std::string result;
   for(size_t i = 0; i < str.size(); i++) {
      if(str[i] == '%') {
         result += "\\";
      }
      result += str[i];
   }
   return(result);
}


// ###### Convert BibTeX label to HTML-compliant id attribute ###############
std::string labelToHTMLLabel(const std::string& string)
{
   std::string result(string);
   std::replace(result.begin(), result.end(), '+', '_');
   return(result);
}


// ###### Convert BibTeX label to XML-compliant target attribute ############
std::string labelToXMLLabel(const std::string& string)
{
   std::string result(string);
   result.erase(std::remove(result.begin(), result.end(), '+'), result.end());
   return(result);
}


// ###### Create formatted string (printf-like) #############################
std::string format(const char* fmt, ...)
{
   char buffer[16384];
   va_list va;
   va_start(va, fmt);
   vsnprintf(buffer, sizeof(buffer), fmt, va);
   va_end(va);
   return(std::string(buffer));
}


// ###### Replace all occurrences of fromString by toString in string #######
void replaceAll(std::string&       string,
                const std::string& fromString,
                const std::string& toString)
{
   const size_t fromStringLength = fromString.length();
   const size_t toStringLength   = toString.length();
   size_t       cursor           = string.find(fromString);
   while(cursor != std::string::npos) {
      string.replace(cursor, fromStringLength, toString);
      cursor = string.find(fromString, cursor + toStringLength);
   }
}


// ###### Check string for given prefix #####################################
bool hasPrefix(const std::string& string, const std::string& prefix, std::string& rest)
{
   if(string.substr(0, prefix.size()) == prefix) {
      rest = string.substr(prefix.size());
      return(true);
   }
   return(false);
}
