Name: bibtexconv
Version: 2.0.3
Release: 1
Summary: BibTeX converter
Group: Applications/Databases
License: GPL-3.0-or-later
URL: https://www.nntb.no/~dreibh/bibtexconv/
Source: https://www.nntb.no/~dreibh/bibtexconv/download/%{name}-%{version}.tar.xz

AutoReqProv: on
BuildRequires: cmake
BuildRequires: gcc
BuildRequires: gcc-c++
BuildRequires: bison
BuildRequires: flex
BuildRequires: openssl-devel
BuildRequires: libcurl-devel
Requires: file
Requires: libcurl
Requires: openssl-libs
Requires: poppler-utils
Requires: zip
BuildRoot: %{_tmppath}/%{name}-%{version}-build

%description
BibTeXConv is a BibTeX file converter which allows one to export BibTeX entries to other formats, including customly defined text output. Furthermore, it provides the possibility to check URLs (including MD5, size and MIME type computations) and to verify ISBN and ISSN numbers.

%prep
%setup -q

%build
%cmake -DCMAKE_INSTALL_PREFIX=/usr .
%cmake_build

%install
%cmake_install

%files
%{_bindir}/bibtexconv
%{_bindir}/bibtexconv-odt
%{_datadir}/bash-completion/completions/bibtexconv
%{_datadir}/bash-completion/completions/bibtexconv-odt
%{_mandir}/man1/bibtexconv.1.gz
%{_mandir}/man1/bibtexconv-odt.1.gz
%{_datadir}/doc/bibtexconv/examples/authors.list
%{_datadir}/doc/bibtexconv/examples/get-author-url
%{_datadir}/doc/bibtexconv/examples/publication-list-treeview.js
%{_datadir}/doc/bibtexconv/examples/ExampleReferences.bib
%{_datadir}/doc/bibtexconv/examples/ODT-Template.odt
%{_datadir}/doc/bibtexconv/examples/RSerPool.bib
%{_datadir}/doc/bibtexconv/examples/odt-example.export
%{_datadir}/doc/bibtexconv/examples/text-example.export
%{_datadir}/doc/bibtexconv/examples/web-example1.export
%{_datadir}/doc/bibtexconv/examples/web-example2.export
%{_datadir}/doc/bibtexconv/examples/web-rserpool.export
%{_datadir}/doc/bibtexconv/examples/yaml-example.export
%{_datadir}/doc/bibtexconv/examples/Images/ListItem-Collapsed.dia
%{_datadir}/doc/bibtexconv/examples/Images/ListItem-Collapsed.svg
%{_datadir}/doc/bibtexconv/examples/Images/ListItem-Expanded.dia
%{_datadir}/doc/bibtexconv/examples/Images/ListItem-Expanded.svg
%{_datadir}/doc/bibtexconv/examples/Images/make-images


%package ietf2bibtex
Summary: Create BibTeX entry for IETF document (RFC or Internet Draft)
Requires: %{name} = %{version}-%{release}

%description ietf2bibtex
ietf2bibtex creates a BibTeX entry for an IETF document (i.e. RFC or Internet
Draft). The necessary information is automatically obtained from online XML
references and the actual document.

%files ietf2bibtex
%{_bindir}/ietf2bibtex
%{_datadir}/bash-completion/completions/ietf2bibtex
%{_datadir}/doc/bibtexconv/examples/authors-fix.list
%{_mandir}/man1/ietf2bibtex.1.gz


%changelog
* Wed Jun 18 2025 Thomas Dreibholz <thomas.dreibholz@gmail.com> - 2.0.3
- New upstream release.
* Mon May 12 2025 Thomas Dreibholz <thomas.dreibholz@gmail.com> - 2.0.2
- New upstream release.
* Tue Apr 29 2025 Thomas Dreibholz <thomas.dreibholz@gmail.com> - 2.0.1
- New upstream release.
* Sun Apr 27 2025 Thomas Dreibholz <thomas.dreibholz@gmail.com> - 2.0.0
- New upstream release.
* Mon Apr 21 2025 Thomas Dreibholz <thomas.dreibholz@gmail.com> - 1.4.5
- New upstream release.
* Mon Mar 17 2025 Thomas Dreibholz <thomas.dreibholz@gmail.com> - 1.4.4
- New upstream release.
* Tue Mar 04 2025 Thomas Dreibholz <thomas.dreibholz@gmail.com> - 1.4.3
- New upstream release.
* Sun Mar 02 2025 Thomas Dreibholz <thomas.dreibholz@gmail.com> - 1.4.2
- New upstream release.
* Sun Mar 02 2025 Thomas Dreibholz <thomas.dreibholz@gmail.com> - 1.4.1
- New upstream release.
* Fri Dec 13 2024 Thomas Dreibholz <thomas.dreibholz@gmail.com> - 1.4.0
- New upstream release.
* Wed Dec 06 2023 Thomas Dreibholz <thomas.dreibholz@gmail.com> - 1.3.6
- New upstream release.
* Thu Oct 05 2023 Thomas Dreibholz <thomas.dreibholz@gmail.com> - 1.3.5
- New upstream release.
* Sun Jan 22 2023 Thomas Dreibholz <thomas.dreibholz@gmail.com> - 1.3.4
- New upstream release.
* Sun Sep 11 2022 Thomas Dreibholz <dreibh@iem.uni-due.de> - 1.3.3
- New upstream release.
* Mon Nov 29 2021 Thomas Dreibholz <dreibh@iem.uni-due.de> - 1.3.2
- New upstream release.
* Sun Nov 07 2021 Thomas Dreibholz <dreibh@iem.uni-due.de> - 1.3.1
- New upstream release.
* Tue Sep 07 2021 Thomas Dreibholz <dreibh@iem.uni-due.de> - 1.3.0
- New upstream release.
* Thu Apr 29 2021 Thomas Dreibholz <dreibh@iem.uni-due.de> - 1.2.0
- New upstream release.
* Sat Mar 06 2021 Thomas Dreibholz <dreibh@iem.uni-due.de> - 1.1.22
- New upstream release.
* Fri Nov 13 2020 Thomas Dreibholz <dreibh@iem.uni-due.de> - 1.1.21
- New upstream release.
* Fri Apr 24 2020 Thomas Dreibholz <dreibh@iem.uni-due.de> - 1.1.20
- New upstream release.
* Fri Feb 07 2020 Thomas Dreibholz <dreibh@iem.uni-due.de> - 1.1.19
- New upstream release.
* Wed Sep 11 2019 Thomas Dreibholz <dreibh@iem.uni-due.de> - 1.1.18
- New upstream release.
* Fri Aug 23 2019 Thomas Dreibholz <dreibh@iem.uni-due.de> - 1.1.17
- New upstream release.
* Wed Aug 07 2019 Thomas Dreibholz <dreibh@iem.uni-due.de> - 1.1.16
- New upstream release.
* Fri Jul 26 2019 Thomas Dreibholz <dreibh@iem.uni-due.de> - 1.1.15
- New upstream release.
* Tue May 21 2019 Thomas Dreibholz <dreibh@iem.uni-due.de> - 1.1.14
- New upstream release.
* Wed Nov 22 2017 Thomas Dreibholz <dreibh@iem.uni-due.de> - 1.0.0
- Initial RPM release
