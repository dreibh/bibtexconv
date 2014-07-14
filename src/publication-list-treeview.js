// $Id$
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
      else {
         return;   // Case #4: click elsewhere -> do nothing!
      }
   }
   
   // ====== Expand/collapse div-elemenets of this list item ================
   var expand = 0;   // What to do here?
   if(listItem.className == 'treeview-collapsed') {
      expand = 1;
      listItem.className = 'treeview-expanded';
   }
   else if(listItem.className == 'treeview-expanded') {
      expand = -1;
      listItem.className = 'treeview-collapsed';
   }
   
   var divList = listItem.getElementsByTagName('div');
   for (var index = 0; index < divList.length; index++) {
      var div = divList[index]
      if (expand == -1) {
         div.style.display = "none";    // collapse
      }
      else if (expand == 1) {
         div.style.display = "block";   // expand
      }
   }
}


// ###### Add listener to all list items ####################################
function initializePublicationList() {
   var listItemList = document.getElementsByTagName('li');
   var prefetched   = false;
   for (var index = 0; index < listItemList.length; index++) {
      var listItem = listItemList[index]
      if( (listItem.className == 'treeview-collapsed') ||
          (listItem.className == 'treeview-expanded') ) {
         listItem.addEventListener('mousedown', preventDefault, false);
         listItem.addEventListener('click', handleClickOnListItem, false);

         if(!prefetched) {
            // Prefetch the expanded bullet image!
            listItem.className = 'treeview-expanded';
            listItem.className = 'treeview-collapsed';
            prefetched = true;
         }
      }
   }
}


document.addEventListener('DOMContentLoaded', initializePublicationList, false);
