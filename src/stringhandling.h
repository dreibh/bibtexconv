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

#ifndef STRINGHANDLING_H
#define STRINGHANDLING_H

#include <stdarg.h>
#include <string>
#include <vector>


const char* getXMLLanguageFromLaTeX(const char* language);

std::string string2utf8(const std::string& string,
                        const std::string& nbsp      = " ",
                        const std::string& lineBreak = "\n",
                        const bool         xmlStyle  = false);

inline std::string string2xml(const std::string& string) {
   return(string2utf8(string, "&#160;", "\n", true));
}

std::string& removeBrackets(std::string& string);
std::string& trim(std::string& string);
std::string extractToken(std::string& string, const std::string& delimiters);
void splitString(std::vector<std::string>& tokenVector,
                 const std::string&        input,
                 const std::string&        delimiter = std::string(":"));
std::string processBackslash(const std::string& string);
std::string laTeXtoURL(const std::string& str);
std::string urlToLaTeX(const std::string& str);
std::string labelToHTMLLabel(const std::string& string);
std::string labelToXMLLabel(const std::string& string);
std::string format(const char* fmt, ...);
void replaceAll(std::string&       string,
                const std::string& fromString,
                const std::string& toString);
bool hasPrefix(const std::string& string,
               const std::string& prefix,
               std::string&       rest);

#endif
