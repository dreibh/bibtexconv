#include "mappings.h"
#include "stringhandling.h"

#include <assert.h>


int main(int argc, char** argv)
{
   Mappings m;

   std::string arg("author-urls:authors.list:Name:URL");
   std::vector<std::string> v;
   splitString(v, arg, ":");
   // for(unsigned int i = 0; i < v.size(); i++) {
   //    printf("%u: %s\n", i, v[i].c_str());
   // }
   if(v.size() == 4) {
      m.addMapping(v[0], v[1], v[2], v[3]);
   }
   else {
      // fprintf(stderr, "ERROR: Bad mapping specification %s!\n", argv[i]);
      exit(1);
   }

   const MappingEntry* e = m.findMapping("author-urls");
   assert(e != nullptr);
   std::string value;

   value = "DEFAULT";
   m.map(e, "Dreibholz, Thomas", value);
   printf("=> %s\n", value.c_str());
   value = "DEFAULT";
   m.map(e, "Test, Thomas", value);
   printf("=> %s\n", value.c_str());

   return 0;
}
