/* $Id$
 *
 * BibTeX Converter
 * Copyright (C) 2010-2011 by Thomas Dreibholz
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact: dreibh@iem.uni-due.de
 */

#ifndef PUBLICATIONSET_H
#define PUBLICATIONSET_H

#include <assert.h>
#include <string>

#include "node.h"
#include "stringhandling.h"


class PublicationSet
{
   public:
   PublicationSet(const size_t maxSize);
   ~PublicationSet();

   inline size_t size() const {
      return(entries);
   }
   inline size_t maxSize() const {
      return(maxEntries);
   }
   Node* get(const size_t index) const {
      assert(index < entries);
      return(publicationArray[index]);
   }

   bool add(Node* publication);
   void addAll(Node* publication);
   void sort(const std::string* sortKey,
             const bool*         sortAscending,
             const size_t        maxSortLevels);
   void clearAll();

   static std::string makeDownloadFileName(const char*        downloadDirectory,
                                           const std::string& anchor,
                                           const std::string& mimeString);

   static bool exportPublicationSetToBibTeX(PublicationSet* publicationSet,
                                            const char*     fileNamePrefix,
                                            const bool      separateFiles,
                                            const bool      skipNotesWithISBNandISSN,
                                            const bool      addNotesWithISBNandISSN,
                                            const bool      addUrlCommand);
   static bool exportPublicationSetToXML(PublicationSet* publicationSet,
                                         const char*     fileNamePrefix,
                                         const bool      separateFiles);
   static bool exportPublicationSetToCustom(PublicationSet*                 publicationSet,
                                            const std::string&              customPrintingHeader,
                                            const std::string&              customPrintingTrailer,
                                            const std::string&              printingTemplate,
                                            const std::vector<std::string>& monthNames,
                                            const std::string&              nbsp,
                                            const bool                      xmlStyle,
                                            const char*                     downloadDirectory,
                                            FILE*                           fh);

   private:
   static std::string applyTemplate(Node*                           publication,
                                    Node*                           prevPublication,
                                    Node*                           nextPublication,
                                    const std::string&              printingTemplate,
                                    const std::vector<std::string>& monthNames,
                                    const std::string&              nbsp,
                                    const bool                      xmlStyle,
                                    const char*                     downloadDirectory,
                                    FILE*                           fh);

   size_t maxEntries;
   size_t entries;
   Node** publicationArray;
};

#endif
