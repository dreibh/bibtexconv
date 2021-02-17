#!/bin/sed -f
#
# Copyright (C) 2018-2021 by Thomas Dreibholz
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
# Contact: dreibh@iem.uni-due.de

s/%1/%{custom-1}/g
s/%2/%{custom-2}/g
s/%3/%{custom-3}/g
s/%4/%{custom-4}/g
s/%5/%{custom-5}/g
s/%a/%{begin-author-loop}/g
s/%@/%{address}/g
s/%A/%{end-author-loop}/g
s/%B/%{booktitle}/g
s/%bD/%{begin-subdivision-day}/g
s/%bm/%{begin-subdivision-month}/g
s/%bM/%{begin-subdivision-month}/g
s/%bY/%{begin-subdivision-year}/g
s/%C/%{anchor}/g
s/%c/%{class}/g
s/%D/%{day}/g
s/%d/%{doi}/g
s/%#/%{download-file-name}/g
s/%eD/%{end-subdivision-day}/g
s/%E/%{edition}/g
s/%em/%{end-subdivision-month}/g
s/%eM/%{end-subdivision-month}/g
s/%eY/%{end-subdivision-year}/g
s/%F/%{author-family-name}/g
s/%f/%{is-first-author?}/g
s/%G/%{author-give-name}/g
s/%g/%{author-initials}/g
s/%H/%{how-published}/g
s/%I/%{isbn}/g
s/%i/%{issn}/g
s/%?/%{institution}/g
s/%J/%{journal}/g
s/%l/%{is-last-author?}/g
s/%L/%{label}/g
s/%M/%{month-name}/g
s/%m/%{month-number}/g
s/%n/%{is-not-first-author?}/g
s/%N/%{number}/g
s/%P/%{pages}/g
s/%$/%{publisher}/g
s/%q/%{urn}/g
s/%r/%{series}/g
s/%sB/%{url-size-bytes}/g
s/%sK/%{url-size-kib}/g
s/%S/%{school}/g
s/%T/%{title}/g
s/%t/%{type}/g
s/%U/%{url}/g
s/%V/%{volume}/g
s/%wD/%{within-subdivision-day}/g
s/%wm/%{within-subdivision-month}/g
s/%wM/%{within-subdivision-month}/g
s/%wY/%{within-subdivision-year}/g
s/%X/%{note}/g
s/%x/%{language}/g
s/%y/%{url-type}/g
s/%Y/%{year}/g
s/%Z/%{name}/g
s/%z/%{url-mime}/g
