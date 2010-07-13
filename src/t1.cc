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
   { "<"  ,        "<",        "&lt;"    },
   { ">"  ,        ">",        "&gt;"    },
   { "\\\"" ,      "\"",       "&quot;"  },
   { "&"  ,        "&",        "&amp;"   },
   { "'"  ,        "'",        "&apos;"  },
   { "~",          " ",        "&#160;"  }   // &nbsp;
};


std::string string2xml(const std::string& string)
{
   std::string result(string);

   for(size_t i = 0; i < (sizeof(replaceTable) / sizeof(ReplaceTableEntry)); i++) {
      size_t pos;
      while( (pos = result.find(replaceTable[i].input)) != std::string::npos ) {
         printf("pos=%d  out=%s\n",pos,replaceTable[i].xmlOutput);
         result.replace(pos, strlen(replaceTable[i].xmlOutput),
                             replaceTable[i].xmlOutput);
      }
   }
   return(result);
}



int main(int argc, char** argv)
{
   std::string s = "T.~Dreibholz and A.~Jungmaier and M.~T{\\\"u}xen and {\\\"a}{\\\"o}{\\\"u}{\\\"A}{\\\"O}{\\\"U}ß";

//    for(size_t i =
//
//    size_t pos;
//    pos = s.find("{\"u}");
//    if(pos != std::string::npos) {
//       puts("R!");
//
//       s.replace(pos, 4, "ü");
//
//    }

   printf("S=<%s>\n", s.c_str());

   std::string r = string2xml(s);

   printf("R=<%s>\n", r.c_str());

   return 0;
}
