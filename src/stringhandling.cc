/* $Id$
 *
 * BibTeX Converter
 * Copyright (C) 2010-2012 by Thomas Dreibholz
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
   { "{\\\"i}",    "ï",        "ï"       },
   { "{\\\"y}",    "ÿ",        "ÿ"       },

   { "{\\´a}",     "á",        "á"       },
   { "{\\´e}",     "é",        "é"       },
   { "{\\´i}",     "í",        "í"       },
   { "{\\´u}",     "ú",        "ú"       },
   { "{\\´y}",     "ý",        "ý"       },

   { "{\\'a}",     "á",        "á"       },
   { "{\\'e}",     "é",        "é"       },
   { "{\\'i}",     "í",        "í"       },
   { "{\\'o}",     "ó",        "ó"       },
   { "{\\'u}",     "ú",        "ú"       },
   { "{\\'y}",     "ý",        "ý"       },

   { "{\\`a}",     "à",        "à"       },
   { "{\\`e}",     "è",        "è"       },
   { "{\\`i}",     "ì",        "ì"       },
   { "{\\`o}",     "ò",        "ò"       },
   { "{\\`u}",     "ù",        "ù"       },

   { "{^e}",       "ê",        "ê"       },
   { "{^u}",       "û",        "û"       },
   { "{^i}",       "î",        "î"       },
   { "{^o}",       "ô",        "ô"       },
   { "{^a}",       "â",        "â"       },

   { "{~o}",       "õ",        "õ"       },
   { "{~a}",       "ã",        "ã"       },
   { "{~n}",       "ñ",        "ñ"       },
   { "{~o}",       "õ",        "õ"       },

   { "\\c{S}",     "Ş",        "Ş"       },
   { "\\c{s}",     "ş",        "ş"       },

   { "\\v{c}",     "č",        "č"       },

   { "\\r{a}",     "å",        "å"       },
   { "\\r{A}",     "Å",        "Å"       },

   { "<"  ,        "<",        "&lt;"    },
   { ">"  ,        ">",        "&gt;"    },
   { "\\\"" ,      "\"",       "&quot;"  },
   { "&"  ,        "&",        "&amp;"   },
   { "'"  ,        "'",        "&apos;"  },
   { "--"  ,       "–",        "–"       }
};


// ###### Convert ASCII string to UTF-8 #####################################
std::string string2utf8(const std::string& string,
                        const std::string& nbsp,
                        const bool         xmlStyle)
{
   std::string result(string);
   size_t      pos = 0;
   while(pos < result.size()) {
      for(size_t i = 0; i < (sizeof(replaceTable) / sizeof(ReplaceTableEntry)); i++) {
         if(result.substr(pos, replaceTable[i].input.size()) == replaceTable[i].input) {
            result.replace(pos, replaceTable[i].input.size(),
                                ((xmlStyle == true) ? replaceTable[i].xmlOutput : replaceTable[i].utf8Output));
            pos += ((xmlStyle == true) ? replaceTable[i].xmlOutput.size() : replaceTable[i].utf8Output.size()) - 1;
            break;
         }
      }

      if( (nbsp.size() > 0) && (result.substr(pos, 1) == "~") ) {
         result.replace(pos, 1, nbsp);
      }

      pos++;
   }
/*
   if(result.find("{") != std::string::npos) {
      fprintf(stderr, "!!! <%s>\n", result.c_str());
   }
*/
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


// ###### Process backslash comments (newline, tab, etc.) ###################
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
            case 't':
               result += '\t';
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
            default:
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
