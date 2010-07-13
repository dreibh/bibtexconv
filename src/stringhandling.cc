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

#include "stringhandling.h"


struct ReplaceTableEntry
{
   const std::string input;
   const std::string utf8Output;
   const std::string xmlOutput;
};

static const ReplaceTableEntry replaceTable[] = {
   { "{\"a}",      "ä",        "ä"       },   // &auml;
   { "{\"o}",      "ö",        "ö"       },   // &ouml;
   { "{\"u}",      "ü",        "ü"       },   // &uuml;
   { "{\"A}",      "Ä",        "Ä"       },   // &Auml;
   { "{\"O}",      "Ö",        "Ö"       },   // &Ouml;
   { "{\"U}",      "Ü",        "Ü"       },   // &Uuml;
   { "{\"i}",      "ï",        "ï"       },
   { "{\"y}",      "ÿ",        "ÿ"       },

   { "{´e}",       "é",        "é"       },
   { "{´u}",       "ú",        "ú"       },
   { "{´i}",       "í",        "í"       },
   { "{´a}",       "á",        "á"       },
   { "{´y}",       "ý",        "ý"       },

   { "{'e}",       "é",        "é"       },
   { "{'u}",       "ú",        "ú"       },
   { "{'i}",       "í",        "í"       },
   { "{'a}",       "á",        "á"       },
   { "{'y}",       "ý",        "ý"       },

   { "{`e}",       "è",        "è"       },
   { "{`u}",       "ù",        "ù"       },
   { "{`i}",       "ì",        "ì"       },
   { "{`o}",       "ò",        "ò"       },
   { "{`a}",       "à",        "à"       },

   { "{^e}",       "ê",        "ê"       },
   { "{^u}",       "û",        "û"       },
   { "{^i}",       "î",        "î"       },
   { "{^o}",       "ô",        "ô"       },
   { "{^a}",       "â",        "â"       },

   { "{~o}",       "õ",        "õ"       },
   { "{~a}",       "ã",        "ã"       },
   { "{~n}",       "ñ",        "ñ"       },
   { "{~o}",       "õ",        "õ"       },

   { "<"  ,        "<",        "&lt;"    },
   { ">"  ,        ">",        "&gt;"    },
   { "\\\"" ,      "\"",       "&quot;"  },
   { "&"  ,        "&",        "&amp;"   },
   { "'"  ,        "'",        "&apos;"  },
   { "~",          " ",        "&#160;"  }    // &nbsp;
};


// ###### Convert ASCII string to UTF-8 #####################################
std::string string2utf8(const std::string& string)
{
   std::string result(string);
   size_t      pos = 0;
   while(pos < result.size()) {
      for(size_t i = 0; i < (sizeof(replaceTable) / sizeof(ReplaceTableEntry)); i++) {
         if(result.substr(pos, replaceTable[i].input.size()) == replaceTable[i].input) {
            result.replace(pos, replaceTable[i].input.size(),
                                replaceTable[i].utf8Output);
            pos += replaceTable[i].utf8Output.size() - 1;
            break;
         }
      }
      pos++;
   }
   return(result);
}


// ###### Convert ASCII string to XML-compliant UTF-8 #######################
std::string string2xml(const std::string& string)
{
   std::string result(string);
   size_t      pos = 0;
   while(pos < result.size()) {
      for(size_t i = 0; i < (sizeof(replaceTable) / sizeof(ReplaceTableEntry)); i++) {
         if(result.substr(pos, replaceTable[i].input.size()) == replaceTable[i].input) {
            result.replace(pos, replaceTable[i].input.size(),
                                replaceTable[i].xmlOutput);
            pos += replaceTable[i].xmlOutput.size() - 1;
            break;
         }
      }
      pos++;
   }
   return(result);
}


// ###### Remove brackets { ... } ###########################################
std::string& removeBrackets(std::string& string)
{
   while( (string.substr(0, 1) == "{") &&
       (string.substr(string.size() - 1) == "}") ) {
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
      if(string[s] != ' ') {
         break;
      }
   }
   for(e = length - 1; e >= 0; e--) {
      if(string[e] != ' ') {
         break;
      }
   }
   string = string.substr(s, length - s - (length - 1 - e) );

   // ====== Remove double whitespaces ======================================
   bool gotSpace = false;
   for(e = string.size() - 1; e >= 0; e--) {
      if(string[e] == ' ') {
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
