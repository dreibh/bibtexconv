#!/bin/sed -f
#
# Copyright (C) 2018-2026 by Thomas Dreibholz
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Contact: thomas.dreibholz@gmail.com

s/%{author-give-name}/%{author-given-name}/g
s/%{how-published}/%{howpublished}/g
s/%{url-md5}/%{url.md5}/g
s/%{url-pagesize}/%{url.pagesize}/g
s/%{url-size-}/%{url.size.}/g
s/%{url-type}/%{url.type}/g
s/%{url-type}/%{url.type}/g
