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

#ifndef NODE_H
#define NODE_H

struct Node {
   struct Node* next;
   struct Node* child;
   char*        string;
   char*        keyword;
   char*        value;
};

void freeNode(struct Node* node);
void dumpNode(struct Node* node);
struct Node* makePublicationCollection(struct Node* node1, struct Node* node2);
struct Node* makePublication(char* type, char* label, struct Node* publicationInfo);
struct Node* makePublicationInfo(struct Node* node1, struct Node* node2);
struct Node* makePublicationInfoItem(char* keyword, char* value);

#endif
