Name: bibtexconv
Version: 1.1.12~b1
Release: 1
Summary: BibTeX converter
Group: Applications/Databases
License: GPLv3
URL: https://www.uni-due.de/~be0001/bibtexconv/
Source: https://www.uni-due.de/~be0001/bibtexconv/download/%{name}-%{version}.tar.gz

AutoReqProv: on
BuildRequires: cmake
BuildRequires: bison
BuildRequires: flex
BuildRequires: openssl-devel
BuildRequires: libcurl-devel
Requires: file
Requires: zip
Requires: libcurl
Requires: openssl-libs
BuildRoot: %{_tmppath}/%{name}-%{version}-build

%description
BibTeXConv is a BibTeX file converter which allows one to export BibTeX entries to other formats, including customly defined text output. Furthermore, it provides the possibility to check URLs (including MD5, size and MIME type computations) and to verify ISBN and ISSN numbers.

%prep
%setup -q

%build
%cmake -DCMAKE_INSTALL_PREFIX=/usr .
make %{?_smp_mflags}

%install
make DESTDIR=%{buildroot} install

%files
%{_bindir}/bibtexconv
%{_bindir}/bibtexconv-odt
%{_bindir}/ietf2bibtex
%{_datadir}/man/man1/bibtexconv.1.gz
%{_datadir}/man/man1/bibtexconv-odt.1.gz
%{_datadir}/man/man1/ietf2bibtex.1.gz
%{_datadir}/doc/bibtexconv/examples/authors.list
%{_datadir}/doc/bibtexconv/examples/authors-fix.list
%{_datadir}/doc/bibtexconv/examples/get-author-url
%{_datadir}/doc/bibtexconv/examples/publication-list-treeview.js
%{_datadir}/doc/bibtexconv/examples/ExampleReferences.bib
%{_datadir}/doc/bibtexconv/examples/ODT-Template.odt
%{_datadir}/doc/bibtexconv/examples/odt-example.export
%{_datadir}/doc/bibtexconv/examples/text-example.export
%{_datadir}/doc/bibtexconv/examples/web-example.export
%{_datadir}/doc/bibtexconv/examples/publication-list-treeview.js
%{_datadir}/doc/bibtexconv/examples/Images/ListItem-Collapsed.dia
%{_datadir}/doc/bibtexconv/examples/Images/ListItem-Collapsed.svg
%{_datadir}/doc/bibtexconv/examples/Images/ListItem-Expanded.dia
%{_datadir}/doc/bibtexconv/examples/Images/ListItem-Expanded.svg
%{_datadir}/doc/bibtexconv/examples/Images/make-images

%doc

%changelog
* Wed Nov 22 2017 Thomas Dreibholz <dreibh@iem.uni-due.de> - 1.0.0
- Initial RPM release
