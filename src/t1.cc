#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>


struct ReplaceTableEntry
{
   const char* input;
   const char* utf8Output;
   const char* xmlOutput;
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
         if(result.substr(pos, strlen(replaceTable[i].input)) == replaceTable[i].input) {
            result.replace(pos, strlen(replaceTable[i].input),
                                replaceTable[i].utf8Output);
            pos += strlen(replaceTable[i].utf8Output) - 1;
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
         if(result.substr(pos, strlen(replaceTable[i].input)) == replaceTable[i].input) {
            result.replace(pos, strlen(replaceTable[i].input),
                                replaceTable[i].xmlOutput);
            pos += strlen(replaceTable[i].xmlOutput) - 1;
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




int main(int argc, char** argv)
{
   std::string s = "T.~Dreibholz and A.~Jungmaier and M.~T{\\\"u}xen and T{´e}st Caf{\'e} and {\\\"a}{\\\"o}{\\\"u}{\\\"A}{\\\"O}{\\\"U}ß";

   printf("STR=<%s>\n", s.c_str());
   printf("XML=<%s>\n", string2xml(s).c_str());
   printf("UTF=<%s>\n", string2utf8(s).c_str());

   return 0;
}
