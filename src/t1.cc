#include <assert.h>
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

   private:
   static inline bool isDelimiter(const char c) {
      return ( (c == ' ') || (c == '\t') || (c == '\n') || (c == '\r') );
   }
   static int splitLine(const char**       columnArray,
                        const unsigned int maxColumns,
                        char*              line,
                        const unsigned int lineLength);
};


// ###### Constructor #######################################################
Mappings::Mappings()
{
}


// ###### Destructor ########################################################
Mappings::~Mappings()
{
}


// ###### Split line into columns ###########################################
int Mappings::splitLine(const char**       columnArray,
                        const unsigned int maxColumns,
                        char*              line,
                        const unsigned int lineLength)
{
   assert(maxColumns >= 1);
   bool         escaped = false;
   bool         quoted  = false;
   unsigned int column  = 0;
   columnArray[column]  = nullptr;
   printf("L=%s\n", line);
   for(unsigned int i = 0; i < lineLength; i++) {
      if(!escaped) {
         if(line[i] == '\\') {
            escaped = true;
         }
         else if(line[i] == '"') {
            quoted = !quoted;
         }
         else if( (line[i] == '\n') || (line[i] == '\r') ) {
            line[i] = 0x00;
         }

         if(!quoted) {
            // ------ Begin of column ---------------------------------------
            if( (columnArray[column] == nullptr) && (!isDelimiter(line[i])) ) {
               columnArray[column] = &line[i];
               printf("begin c%u  %d   S=%s\n", column, i, columnArray[column]);
            }
            // ------ End of column -----------------------------------------
            else if(isDelimiter(line[i])) {
               line[i] = 0x00;
               printf("end c%u  %d   E=%s.\n", column, i, columnArray[column]);
               while( (line[i + 1] == ' ') || (line[i + 1] == '\t') ) {
                  i++;
               }
               column++;
               if(column >= maxColumns) {
                  break;
               }
               columnArray[column] = nullptr;
            }
            else printf("--- c%u q=%d %d   chr=%c\n", column, quoted, i, line[i]);
         }
      }
      else {
         escaped = false;
      }
   }
   if(columnArray[column] != nullptr) {
      column++;
   }
   return column;
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
   ssize_t lineLength = getline((char**)&line, &bufferSize, mappingFile);
   printf("L=%d\n",(int)lineLength);
   if(lineLength >= 0) {
      // ====== Get columns names on first line =============================
      const unsigned int maxColumns = 16;
      const char*        columnArray[maxColumns];
      const unsigned int columnsInFile =
         splitLine((const char**)&columnArray, maxColumns,
                   line, lineLength);

      // ====== Identify key and value column ===============================
      int keyColumnIndex   = -1;
      int valueColumnIndex = -1;
      for(unsigned int i = 0;i<columnsInFile;i++) {
         printf("C<%u>=%s.\n", i, columnArray[i]);
         if( (keyColumnIndex == -1) && (strcmp(columnArray[i], keyColumn) == 0) ) {
            keyColumnIndex = i;
         }
         if( (valueColumnIndex == -1) && (strcmp(columnArray[i], valueColumn) == 0) ) {
            valueColumnIndex = i;
         }
      }
      if(keyColumnIndex < 0)  {
         fprintf(stderr, "ERROR: Key column %s not found in mapping file %s!\n",
                 keyColumn, mappingFileName);
         fclose(mappingFile);
         return false;
      }
      if(valueColumnIndex < 0)  {
         fprintf(stderr, "ERROR: Value column %s not found in mapping file %s!\n",
                 valueColumn, mappingFileName);
         fclose(mappingFile);
         return false;
      }

      // ====== Read data ===================================================
      const unsigned int columnsToProcess =
         1 + std::max(keyColumnIndex, valueColumnIndex);
      assert(columnsToProcess <= maxColumns);
      while( (lineLength = (getline((char**)&line, &bufferSize, mappingFile))) > 0 ) {
         if(line[0] != '#') {
            unsigned int columns =
               splitLine((const char**)&columnArray, maxColumns,
                        line, lineLength);
            printf("-> c=%d  %d\n",columns,columnsToProcess);
            if(columns == columnsToProcess) {
               const char* key   = columnArray[keyColumnIndex];
               const char* value = columnArray[valueColumnIndex];
               printf("OK <%s> <%s>\n", key,value);
            }
         }
         line = buffer;
      }
   }

   // ====== Handle read errors =============================================
   if( (lineLength < 0) && (errno != 0) ) {
      fprintf(stderr, "ERROR: Unable to read from mapping file %s: %s\n",
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
