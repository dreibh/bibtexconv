/* $Id$
 *
 * BibTeX Convertor
 * Copyright (C) 2010 by Thomas Dreibholz
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

#ifndef STRINGHANDLING_H
#define STRINGHANDLING_H

#include <string>


std::string string2utf8(const std::string& string,
                        const std::string& nbsp = " ",
                        const bool         xmlStyle = false);

inline std::string string2xml(const std::string& string) {
   return(string2utf8(string, "&#160;", true));
}

std::string& removeBrackets(std::string& string);
std::string& trim(std::string& string);
std::string processBackslash(const std::string& string);

#endif
