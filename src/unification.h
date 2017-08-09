/* $Id$
 *
 * BibTeX Converter
 * Copyright (C) 2010-2017 by Thomas Dreibholz
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


void unifyAuthor(Node* publication, Node* author);
void unifyBookTitle(Node* publication, Node* booktitle);
void unifyISBN(Node* publication, Node* isbn);
void unifyISSN(Node* publication, Node* issn);
void unifyDate(Node* publication, Node* year, Node* month, Node* day);
void unifyURL(Node* publication, Node* url);
void unifyPages(Node* publication, Node* pages);
void unifyNumPages(Node* publication, Node* numpages);

#endif
