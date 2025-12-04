// ==========================================================================
//                ____  _ _   _____   __  ______
//                | __ )(_) |_|_   _|__\ \/ / ___|___  _ ____   __
//                |  _ \| | '_ \| |/ _ \  / |   / _ \| '_ \ \ / /
//                | |_) | | |_) | |  __//  \ |__| (_) | | | \ V /
//                |____/|_|_.__/|_|\___/_/\_\____\___/|_| |_|\_/
//
//                          ---  BibTeX Converter  ---
//                   https://www.nntb.no/~dreibh/bibtexconv/
// ==========================================================================
//
// BibTeX Converter
// Copyright (C) 2010-2026 by Thomas Dreibholz
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Contact: thomas.dreibholz@gmail.com

#include "mappings.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// ###### Constructor #######################################################
Mappings::Mappings()
{
}


// ###### Destructor ########################################################
Mappings::~Mappings()
{
   std::map<const std::string, MappingEntry*>::iterator iterator = MappingSet.begin();
   while(iterator != MappingSet.end()) {
      delete iterator->second;
      iterator = MappingSet.erase(iterator);
   }
}


// ###### Split line into columns ###########################################
int Mappings::splitLine(const char**       columnArray,
                        const unsigned int maxColumns,
                        char*              line,
                        const unsigned int lineLength)
{
   assert(maxColumns >= 1);
   for(unsigned int i = 0; i < maxColumns; i++) {
      columnArray[i]  = nullptr;
   }

   bool         escaped = false;
   bool         quoted  = false;
   unsigned int column  = 0;
   for(unsigned int i = 0; i < lineLength; i++) {
      if(!escaped) {
         // ------ Special characters ---------------------------------------
         if(line[i] == '\\') {
            escaped = true;
            continue;
         }
         else if(line[i] == '"') {
            quoted = !quoted;
         }
         else if( (line[i] == '\n') || (line[i] == '\r') ) {
            if( (i > 0) && (line[i - 1] == '"') ) {   // Unquote
               line[i - 1] = 0x00;
            }
            line[i] = 0x00;
         }

         // ------ Begin of column ------------------------------------------
         if( (columnArray[column] == nullptr) && (!isDelimiter(line[i])) ) {
            if(line[i] == '"') {   // Unquote
               i++;
            }
            columnArray[column] = &line[i];
         }

         // ------ End of column --------------------------------------------
         else if(!quoted && isDelimiter(line[i])) {
            if( (i > 0) && (line[i - 1] == '"') ) {   // Unquote
               line[i - 1] = 0x00;
            }
            line[i] = 0x00;
            while(isDelimiter(line[i + 1])) {
               i++;
            }
            column++;
            if(column >= maxColumns) {
               break;
            }
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
bool Mappings::addMapping(const std::string& mappingName,
                          const std::string& mappingFileName,
                          const std::string& keyColumn,
                          const std::string& valueColumn)
{
   FILE* mappingFile;

   mappingFile = fopen(mappingFileName.c_str(), "r");
   if(mappingFile == nullptr) {
      fprintf(stderr, "ERROR: Unable to open mapping file %s: %s!\n",
               mappingFileName.c_str(), strerror(errno));
      return false;
   }

   char    buffer[16384];
   size_t  bufferSize = sizeof(buffer);
   char*   line       = buffer;
   ssize_t lineLength = getline((char**)&line, &bufferSize, mappingFile);
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
      for(unsigned int i = 0; i < columnsInFile; i++) {
         if( (keyColumnIndex == -1) && (strcmp(columnArray[i], keyColumn.c_str()) == 0) ) {
            keyColumnIndex = i;
         }
         if( (valueColumnIndex == -1) && (strcmp(columnArray[i], valueColumn.c_str()) == 0) ) {
            valueColumnIndex = i;
         }
      }
      if(keyColumnIndex < 0)  {
         fprintf(stderr, "ERROR: Key column %s not found in mapping file %s!\n",
                 keyColumn.c_str(), mappingFileName.c_str());
         fclose(mappingFile);
         return false;
      }
      if(valueColumnIndex < 0)  {
         fprintf(stderr, "ERROR: Value column %s not found in mapping file %s!\n",
                 valueColumn.c_str(), mappingFileName.c_str());
         fclose(mappingFile);
         return false;
      }

      // ====== Add new mapping =============================================
      MappingEntry* mappingEntry = new MappingEntry;
      assert(mappingEntry != nullptr);
      mappingEntry->Name = mappingName;
      MappingSet.insert(std::pair<std::string, MappingEntry*>(
         mappingEntry->Name, mappingEntry));

      // ====== Read data ===================================================
      const unsigned int columnsToProcess =
         1 + std::max(keyColumnIndex, valueColumnIndex);
      assert(columnsToProcess <= maxColumns);
      while( (lineLength = (getline((char**)&line, &bufferSize, mappingFile))) > 0 ) {
         if(line[0] != '#') {
            unsigned int columns =
               splitLine((const char**)&columnArray, maxColumns,
                        line, lineLength);
            if(columns == columnsToProcess) {
               const char* key   = columnArray[keyColumnIndex];
               const char* value = columnArray[valueColumnIndex];
               mappingEntry->Mappings.insert(
                  std::pair<std::string, std::string>(
                     std::string(key), std::string(value)));
            }
         }
         line = buffer;
      }
   }

   // ====== Handle read errors =============================================
   if( (lineLength < 0) && (!feof(mappingFile)) ) {
      fprintf(stderr, "ERROR: Unable to read from mapping file %s: %s\n",
              mappingFileName.c_str(), strerror(errno));
      fclose(mappingFile);
      return false;
   }

   fclose(mappingFile);
   return true;
}


// ###### Apply mapping #####################################################
const MappingEntry* Mappings::findMapping(const std::string& mappingName) const
{
   std::map<const std::string, MappingEntry*>::const_iterator found = MappingSet.find(mappingName);
   if(found != MappingSet.end()) {
      return found->second;
   }
   return nullptr;
}


// ###### Apply mapping #####################################################
bool Mappings::map(const MappingEntry* mappingEntry,
                   const std::string&  key,
                   std::string&        value) const
{
   assert(mappingEntry != nullptr);
   std::map<const std::string, const std::string>::const_iterator found =
      mappingEntry->Mappings.find(key);
   if(found != mappingEntry->Mappings.end()) {
      value = found->second;
      return true;
   }
   return false;
}
