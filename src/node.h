/* $Id$
 *
 * BibTeX Converter
 * Copyright (C) 2010-2014 by Thomas Dreibholz
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

#include <string>
#include <vector>


extern void yyerror(const char* errorText);
extern int  yylex();


#define NODE_CUSTOM_ENTRIES 9

struct Node {
   struct Node*             prev;
   struct Node*             next;
   struct Node*             child;
   std::string              keyword;
   std::string              value;
   std::string              anchor;
   std::string              custom[NODE_CUSTOM_ENTRIES];
   std::vector<std::string> arguments;
   unsigned int             priority;
   int                      number;
};

void freeNode(struct Node* node);
void dumpNode(struct Node* node);

size_t countNodes(const Node* node);
size_t countChildNodes(const Node* node, const char* childKeyword);
Node* findNode(Node* node, const char* keyword);
Node* findChildNode(Node* node, const char* childKeyword);
Node* addOrUpdateChildNode(Node* node, const char* childKeyword, const char* value);

struct Node* makePublicationCollection(struct Node* node1, struct Node* node2);
struct Node* makePublication(const char* type, const char* label,
                             struct Node* publicationInfo);
struct Node* makePublicationInfo(struct Node* node1, struct Node* node2);
struct Node* makePublicationInfoItem(const char* keyword, const char* value);

#endif
