#include <stdio.h>
#include <stdlib.h>
#include <string>


struct ReplaceTable
{
   const char* input;
   const char* output;
};

ReplaceTable[] = {
   { "{\"u}", "ü" }
};



int main(int argc, char** argv)
{
   std::string s = "T.~Dreibholz and A.~Jungmaier and M.~T{\"u}xen";

   for(size_t i =

   size_t pos;
   pos = s.find("{\"u}");
   if(pos != std::string::npos) {
      puts("R!");

      s.replace(pos, 4, "ü");

   }

   printf("S=<%s>\n", s.c_str());

   return 0;
}
