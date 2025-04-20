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
// Copyright (C) 2010-2025 by Thomas Dreibholz
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

#ifndef MAPPINGS_H
#define MAPPINGS_H

#include <map>
#include <string>


struct MappingEntry {
   std::string                                    Name;
   std::map<const std::string, const std::string> Mappings;
};

class Mappings
{
   public:
   Mappings();
   ~Mappings();

   bool addMapping(const std::string& mappingName,
                   const std::string& mappingFile,
                   const std::string& keyColumn,
                   const std::string& valueColumn);
   const MappingEntry* findMapping(const std::string& mappingName) const;
   bool map(const MappingEntry* mappingEntry,
            const std::string&  key,
            std::string&        value) const;

   private:
   std::map<const std::string, MappingEntry*> MappingSet;

   static inline bool isDelimiter(const char c) {
      return ( (c == ' ') || (c == '\t') || (c == '\n') || (c == '\r') );
   }
   static int splitLine(const char**       columnArray,
                        const unsigned int maxColumns,
                        char*              line,
                        const unsigned int lineLength);
};

#endif
