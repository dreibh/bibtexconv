# BibTeXConv

A BibTeX file converter

<https://www.nntb.no/~dreibh/bibtexconv/>

## Description

BibTeXConv is a BibTeX file converter which allows to export BibTeX entries to other formats, including customly defined text output. Furthermore, it provides the possibility to check URLs (including MD5, size and MIME type computations) and to verify ISBN and ISSN numbers.

## Examples

Have a look into /usr/share/doc/bibtexconv/examples/ (or corresponding path of your system) for example export scripts. The export scripts contain the commands which are read by bibtexconv from standard input.

- Check URLs of all entries in [ExampleReferences.bib](src/ExampleReferences.bib), add MD5, size and MIME type items and write the results to UpdatedReferences.bib:
  ```
  bibtexconv ExampleReferences.bib -export-to-bibtex=UpdatedReferences.bib -check-urls -only-check-new-urls -non-interactive
  ```
- Use the export script [web-example.export](src/web-example.export) to export references from [ExampleReferences.bib](src/ExampleReferences.bib) to MyPublications.html as XHTML 1.1. [ExampleReferences.bib](src/ExampleReferences.bib) references the script [get-author-url](src/get-author-url) and the list [authors.list](src/authors.list) to obtain the authors' website URLs.
  ```
  bibtexconv ExampleReferences.bib <web-example.export >MyPublications.html
  ```
- Use export script [text-example.export](src/text-example.export) to export references from [ExampleReferences.bib](src/ExampleReferences.bib) to MyPublications.txt as plain text:
  ```
  bibtexconv ExampleReferences.bib <text-example.export >MyPublications.txt
  ```
- Convert all references in [ExampleReferences.bib](src/ExampleReferences.bib) to XML references to be includable in IETF Internet Drafts. For each reference, an own file is generated, named with the prefix "reference." (for example: reference.Globecom2010.xml for the reference Globecom2010):
  ```
  bibtexconv ExampleReferences.bib -export-to-separate-xmls=reference. -non-interactive
  ```
- Convert all references in [ExampleReferences.bib](src/ExampleReferences.bib) to BibTeX references. For each reference, an own file is generated, named with the prefix "" (here: no prefix; for example: Globecom2010.bib for the reference Globecom2010):
  ```
  bibtexconv ExampleReferences.bib -non-interactive -export-to-separate-bibtexs=
  ```
- Download all references in [ExampleReferences.bib](src/ExampleReferences.bib) providing an "url" entry to directory Downloads. If the corresponding file is already existing, a download is skipped. That is, the command can be run regularly to maintain an up-to-date publications directory. Updated references (including length, type and MD5 sum of the downloaded entries) are written to UpdatedReferences.bib:
  ```
  bibtexconv ExampleReferences.bib -export-to-bibtex=UpdatedReferences.bib -check-urls -store-downloads=Downloads -non-interactive
  ```
- Use export script [odt-example.export](src/odt-example.export) to export references from [ExampleReferences.bib](src/ExampleReferences.bib) to MyPublications.odt as [OpenDocument](https://www.adobe.com/uk/acrobat/resources/document-files/open-doc.html) Text (ODT), according to the template ODT file [ODT-Template.odt](src/ODT-Template.odt):
  ```
  bibtexconv-odt ODT-Template.odt MyPublications.odt ExampleReferences.bib odt-example.export
  ```
  ODT is the native format of [LibreOffice](https://www.libreoffice.org/)/[OpenOffice](https://www.openoffice.org/). However, LibreOffice/OpenOffice can also be used to convert it to Microsoft Word (DOCX) format, either via GUI or on the command-line to MyPublications.docx:
  ```
  soffice --convert-to docx MyPublications.odt
  ```

## Binary Package Installation

Please use the issue tracker at [https://github.com/dreibh/bibtexconv/issues](https://github.com/dreibh/bibtexconv/issues) to report bugs and issues!

### Ubuntu Linux

For ready-to-install Ubuntu Linux packages of BibTeXConv, see [Launchpad PPA for Thomas Dreibholz](https://launchpad.net/~dreibh/+archive/ubuntu/ppa/+packages?field.name_filter=bibtexconv&field.status_filter=published&field.series_filter=)!

```
sudo apt-add-repository -sy ppa:dreibh/ppa
sudo apt-get update
sudo apt-get install bibtexconv
```

### Fedora Linux

For ready-to-install Fedora Linux packages of BibTeXConv, see [COPR PPA for Thomas Dreibholz](https://copr.fedorainfracloud.org/coprs/dreibh/ppa/package/bibtexconv/)!

```
sudo dnf copr enable -y dreibh/ppa
sudo dnf install bibtexconv
```

### FreeBSD

For ready-to-install FreeBSD packages of BibTeXConv, it is included in the ports collection, see [FreeBSD ports tree index of converters/bibtexconv/](https://cgit.freebsd.org/ports/tree/converters/bibtexconv/)!

```
pkg install bibtexconv
```

Alternatively, to compile it from the ports sources:

```
cd /usr/ports/converters/bibtexconv
make
make install
```

## Sources Download

BibTeXConv is released under the GNU General Public Licence (GPL).

Please use the issue tracker at [https://github.com/dreibh/bibtexconv/issues](https://github.com/dreibh/bibtexconv/issues) to report bugs and issues!

### Development Version

The Git repository of the BibTeXConv sources can be found at [https://github.com/dreibh/bibtexconv](https://github.com/dreibh/bibtexconv):

```
git clone https://github.com/dreibh/bibtexconv
cd bibtexconv
cmake .
make
```

Contributions:

- Issue tracker: [https://github.com/dreibh/bibtexconv/issues](https://github.com/dreibh/bibtexconv/issues).
  Please submit bug reports, issues, questions, etc. in the issue tracker!

- Pull Requests for BibTeXConv: [https://github.com/dreibh/bibtexconv/pulls](https://github.com/dreibh/bibtexconv/pulls).
  Your contributions to BibTeXConv are always welcome!

- CI build tests of BibTeXConv: [https://github.com/dreibh/bibtexconv/actions](https://github.com/dreibh/bibtexconv/actions).

- Coverity Scan analysis of BibTeXConv: [https://scan.coverity.com/projects/dreibh-bibtexconv](https://scan.coverity.com/projects/dreibh-bibtexconv).

### Current Stable Release

See [https://www.nntb.no/~dreibh/bibtexconv/#Download](https://www.nntb.no/~dreibh/bibtexconv/#Download)!
