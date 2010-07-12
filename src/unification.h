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

#ifndef UNIFICATION_H
#define UNIFICATION_H

#include "node.h"

#include <string>


void removeBrackets(std::string& str);
void trim(std::string& str);

void unifyAuthor(Node* node, Node* author);
void unifyBookTitle(Node* node, Node* booktitle);
void unifyISBN(Node* node, Node* isbn);
void unifyISSN(Node* node, Node* issn);
void unifyDate(Node* node, Node* year, Node* month, Node* day);
void unifyURL(Node* node, Node* url);
void unifyPages(Node* node, Node* pages);

#endif
