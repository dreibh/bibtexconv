#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <map>
#include <vector>


class Mappings
{
   public:
   Mappings();
   ~Mappings();

   bool addMapping(const char* mappingName,
                   const char* mappingFile,
                   const char* keyColumn,
                   const char* valueColumn);
};


// ###### Constructor #######################################################
Mappings::Mappings()
{
}


// ###### Destructor ########################################################
Mappings::~Mappings()
{
}


// ###### Add mapping #######################################################
bool Mappings::addMapping(const char* mappingName,
                          const char* mappingFileName,
                          const char* keyColumn,
                          const char* valueColumn)
{
   FILE* mappingFile;

   mappingFile = fopen(mappingFileName, "r");
   if(mappingFile == nullptr) {
      fprintf(stderr, "ERROR: Unable to open mapping file %s: %s!\n",
               mappingFileName, strerror(errno));
      return false;
   }

   char    buffer[65536];
   size_t  bufferSize = sizeof(buffer);
   char*   line       = buffer;
   ssize_t lineLength;
   while( (lineLength = (getline((char**)&line, &bufferSize, mappingFile))) > 0 ) {

      line = buffer;
   }
   if( (lineLength < 0) && (errno != 0) ) {
      fprintf(stderr, "ERROR: Unable to read from insert file %s: %s\n",
              mappingFileName, strerror(errno));
      fclose(mappingFile);
      return false;
   }

   fclose(mappingFile);
   return true;
}


int main(int argc, char** argv)
{
   Mappings m;

   m.addMapping("author-urls", "authors.list", "Name", "URL");

   return 0;
}
