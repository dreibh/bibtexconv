// $Id: bibtexconv.cc 1838 2014-04-12 19:02:25Z dreibh $
//
// Publication List Treeview
// Copyright (C) 2014 by Thomas Dreibholz
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
// Contact: dreibh@iem.uni-due.de
//


// ###### Prevent default behaviour #########################################
function preventDefault(event) {
   if (event.target.tagName != 'LI') return;
   event.preventDefault();
}


// ###### Handle click on list item #########################################
function handleClickOnListItem(event) {
   // ====== Only react on clicks on the list item, not to sub-elements =====
   if (event.target.tagName == 'A') {
      return;                    // Case #1: hyperlink => nothing to do here!
   }
   else if (event.target.tagName == 'LI') {
      listItem = event.target;   // Case #2: click on bullet point.
   }
   else {
      if (event.target.parentNode.tagName == 'LI') {
         listItem = event.target.parentNode;   // Case #3: parent is bullet point.
      }
   }
   
   // ====== Expand/collapse div-elemenets of this list item ================
   var divList = listItem.getElementsByTagName('div');
   for (var index = 0; index < divList.length; index++) {
      var div = divList[index]
      if(div.style.display == "block") {
         div.style.display = "none";    // collapse
         listItem.className = 'treeview-collapsed';
      }
      else {
         div.style.display = "block";   // expand
         listItem.className = 'treeview-expanded';
      }
   }
}


// ###### Add listener to all list items ####################################
function initializePublicationList() {
   var listItemList = document.getElementsByTagName('li');
   for (var index = 0; index < listItemList.length; index++) {
      var listItem = listItemList[index]
      listItem.addEventListener('mousedown', preventDefault, false);
      listItem.addEventListener('click', handleClickOnListItem, false);
      if(index == listItemList.length - 1) {
         // Prefetch the expanded bullet image!
         listItem.className = 'treeview-expanded';
      }
      listItem.className = 'treeview-collapsed';
   }
}


document.addEventListener('DOMContentLoaded', initializePublicationList, false);
