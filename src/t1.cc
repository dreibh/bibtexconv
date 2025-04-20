#include "mappings.h"

#include <assert.h>
#include <vector>


// ###### Split string into token by delimiter ##############################
void splitString(const std::string&        input,
                 const std::string&        delimiter,
                 std::vector<std::string>& tokenVector)
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


int main(int argc, char** argv)
{
   Mappings m;

   std::string arg("author-urls:authors.list:Name:URL");
   std::vector<std::string> v;
   splitString(arg, ":", v);
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
   assert(e != NULL);
   printf("=> %s\n", m.map(e, "Dreibholz, Thomas").c_str());
   printf("=> %s\n", m.map(e, "Test, Thomas").c_str());

   return 0;
}
