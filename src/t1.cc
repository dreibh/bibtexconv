#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>

#include "stringhandling.h"


int main(int argc, char** argv)
{
   std::string s = "category   year/descending   month/descending";

   while(s != "") {
      std::string token = extractToken(trim(s), " ");
      printf("TOKEN=<%s>\n",token.c_str());
   }

   return 0;
}
