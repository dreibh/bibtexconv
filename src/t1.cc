#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>

#include "stringhandling.h"


int main(int argc, char** argv)
{
   std::string s = "category   \"test 2008\" \"äöü 2008\"";

   while(s != "") {
      std::string token = extractToken(trim(s), " ");
      printf("TOKEN=<%s>\n",token.c_str());
   }

   return 0;
}
