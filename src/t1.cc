#include "mappings.h"

int main(int argc, char** argv)
{
   Mappings m;

   m.addMapping("author-urls", "authors.list", "Name", "URL");

   const MappingEntry* e = m.findMapping("author-urls");
   printf("=> %s\n", m.map(e, "Dreibholz, Thomas").c_str());
   printf("=> %s\n", m.map(e, "Test, Thomas").c_str());

   return 0;
}
