# BibTeXConv

A BibTeX file converter

<https://www.uni-due.de/~be0001/bibtexconv/>

## Description

BibTeXConv is a BibTeX file converter which allows to export BibTeX entries to other formats, including customly defined text output. Furthermore, it provides the possibility to check URLs (including MD5, size and MIME type computations) and to verify ISBN and ISSN numbers.

##Examples

Have a look into /usr/share/doc/bibtexconv/examples/ (or corresponding path of your system) for example export scripts. The export scripts contain the commands which are read by bibtexconv from standard input.

- Check URLs of all entries in /usr/share/doc/bibtexconv/examples/ExampleReferences.bib, add MD5, size and MIME type items and write the results to UpdatedReferences.bib:

    bibtexconv /usr/share/doc/bibtexconv/examples/ExampleReferences.bib -export-to-bibtex=UpdatedReferences.bib -check-urls -only-check-new-urls -non-interactive

- Use the export script /usr/share/doc/bibtexconv/examples/web-example.export to export references from /usr/share/doc/bibtexconv/examples/ExampleReferences.bib to MyPublications.html as XHTML 1.1.

    bibtexconv /usr/share/doc/bibtexconv/examples/ExampleReferences.bib </usr/share/doc/bibtexconv/examples/web-example.export >MyPublications.html

- Use export script /usr/share/doc/bibtexconv/examples/text-example.export to export references from /usr/share/doc/bibtexconv/examples/ExampleReferences.bib to MyPublications.txt as plain text:

    bibtexconv /usr/share/doc/bibtexconv/examples/ExampleReferences.bib </usr/share/doc/bibtexconv/examples/text-example.export >MyPublications.txt

- Convert all references in /usr/share/doc/bibtexconv/examples/ExampleReferences.bib to XML references to be includable in IETF Internet Drafts. For each reference, an own file is generated, named with the prefix "reference." (for example: reference.Globecom2010.xml for the reference Globecom2010):

    bibtexconv /usr/share/doc/bibtexconv/examples/ExampleReferences.bib -non-interactive -export-to-separate-xmls=reference.

- Convert all references in /usr/share/doc/bibtexconv/examples/ExampleReferences.bib to BibTeX references. For each reference, an own file is generated, named with the prefix "" (here: no prefix; for example: Globecom2010.bib for the reference Globecom2010):

    bibtexconv /usr/share/doc/bibtexconv/examples/ExampleReferences.bib -non-interactive -export-to-separate-bibtexs=

- Download all references in /usr/share/doc/bibtexconv/examples/ExampleReferences.bib providing an "url" entry to directory Downloads. If the corresponding file is already existing, a download is skipped. That is, the command can be run regularly to maintain an up-to-date publications directory. Updated references (including length, type and MD5 sum of the downloaded entries) are written to UpdatedReferences.bib:

    bibtexconv /usr/share/doc/bibtexconv/examples/ExampleReferences.bib -export-to-bibtex=UpdatedReferences.bib -check-urls -store-downloads=Downloads -non-interactive

- Use export script /usr/share/doc/bibtexconv/examples/odt-example.export to export references from /usr/share/doc/bibtexconv/examples/ExampleReferences.bib to MyPublications.odt as OpenDocument Text (ODT), according to the template ODT file /usr/share/doc/bibtexconv/examples/ODT-Template.odt:

    bibtexconv-odt /usr/share/doc/bibtexconv/examples/ODT-Template.odt MyPublications.odt /usr/share/doc/bibtexconv/examples/ExampleReferences.bib /usr/share/doc/bibtexconv/examples/odt-example.export

## Binary Package Installation

Please use the issue tracker at https://github.com/dreibh/bibtexconv/issues to report bugs and issues!

### Ubuntu Linux

For ready-to-install Ubuntu Linux packages of BibTeXConv, see Launchpad PPA for Thomas Dreibholz!

```
sudo apt-add-repository -sy ppa:dreibh/ppa
sudo apt-get update
sudo apt-get install bibtexconv
```

### Fedora Linux

For ready-to-install Fedora Linux packages of BibTeXConv, see COPR PPA for Thomas Dreibholz!

```
sudo dnf copr enable -y dreibh/ppa
sudo dnf install bibtexconv
```

### FreeBSD

For ready-to-install FreeBSD packages of BibTeXConv, it is included in the ports collection, see Index of /head/converters/bibtexconv/!

    pkg install subnetcalc

Alternatively, to compile it from the ports sources:

```
cd /usr/ports/converters/bibtexconv
make
make install
```

## Sources Download

BibTeXConv is released under the GNU General Public Licence (GPL).

Please use the issue tracker at https://github.com/dreibh/bibtexconv/issues to report bugs and issues!

### Development Version

The Git repository of the BibTeXConv sources can be found at https://github.com/dreibh/bibtexconv:

- Issue tracker: https://github.com/dreibh/bibtexconv/issues.
  Please submit bug reports, issues, questions, etc. in the issue tracker!

- Pull Requests for BibTeXConv: https://github.com/dreibh/bibtexconv/pulls.
  Your contributions to BibTeXConv are always welcome!

- Travis CI automated build tests of BibTeXConv: https://travis-ci.org/dreibh/bibtexconv.

- Coverity Scan analysis of BibTeXConv: https://scan.coverity.com/projects/dreibh-bibtexconv.

### Current Stable Release

The tarball has been signed with my GnuPG key DF605BB0760F2D65. Its authenticity and integrity can be verified by:

gpg --keyserver hkp://keyserver.ubuntu.com --recv-keys DF605BB0760F2D65
gpg --verify bibtexconv-VERSION.tar.xz.asc bibtexconv-VERSION.tar.xz

- [bibtexconv-1.1.18.tar.xz (Tar/XZ file)](https://www.uni-due.de/~be0001/bibtexconv/download/bibtexconv-1.1.18.tar.xz)
- [bibtexconv-1.1.18.tar.xz.asc (Signature)](https://www.uni-due.de/~be0001/bibtexconv/download/bibtexconv-1.1.18.tar.xz.asc)

## Requirements

    Linux or FreeBSD (other OS should work too), a C++ compiler, Flex/Yacc, libcurl, libcrypto.
