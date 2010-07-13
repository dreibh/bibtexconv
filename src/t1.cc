#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>

#include "stringhandling.h"


int main(int argc, char** argv)
{
   std::string s = "T.~Dreibholz and A.~Jungmaier and M.~T{\\\"u}xen and T{´e}st Caf{\'e} and {\\\"a}{\\\"o}{\\\"u}{\\\"A}{\\\"O}{\\\"U}ß";

   printf("STR=<%s>\n", s.c_str());
   printf("XML=<%s>\n", string2xml(s).c_str());
   printf("UTF=<%s>\n", string2utf8(s).c_str());

   return 0;
}
