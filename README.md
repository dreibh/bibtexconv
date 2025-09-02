<h1 align="center">
 BibTeXConv\
 <span style="font-size:75%">A BibTeX File Converter</span>\
 [<span style="font-size: 75%">https://www.nntb.no/~dreibh/bibtexconv</span>](https://www.nntb.no/~dreibh/bibtexconv/)
</h1>


# 💡 What is BibTeXConv?

BibTeXConv is a BibTeX file converter which allows to export BibTeX entries to other formats, including customly defined text output. Furthermore, it provides the possibility to check URLs (including MD5, size and MIME type computations) and to verify ISBN and ISSN numbers.

# 😀 Examples

Take a look into /usr/share/doc/bibtexconv/examples/ (or corresponding path of your system) for example export scripts. The export scripts contain the commands which are read by bibtexconv from standard input.

* Check URLs of all entries in [ExampleReferences.bib](src/ExampleReferences.bib), add MD5, size and MIME type items and write the results to UpdatedReferences.bib:

  <pre>
  bibtexconv ExampleReferences.bib --export-to-bibtex=UpdatedReferences.bib --check-urls --only-check-new-urls --non-interactive
  </pre>

* Use the export script [web-example1.export](src/web-example1.export) to export references from [ExampleReferences.bib](src/ExampleReferences.bib) to [MyPublications1.html](https://www.nntb.no/~dreibh/bibtexconv/MyPublications1.html) as XHTML 1.1. [ExampleReferences.bib](src/ExampleReferences.bib) references the script [get-author-url](src/get-author-url) and the list [authors.list](src/authors.list) to obtain the authors' website URLs:

  <pre>
  bibtexconv ExampleReferences.bib &lt;web-example1.export &gt;MyPublications1.html
  </pre>

  Note that using a script is slow, and may introduce a security issue when running export scripts from untrusted sources! The preferred way for mappings is to use mapping files, which is demonstrated by the next example.

* Use the export script [web-example2.export](src/web-example2.export) to export references from [ExampleReferences.bib](src/ExampleReferences.bib) to [MyPublications2.html](https://www.nntb.no/~dreibh/bibtexconv/MyPublications2.html) as XHTML 1.1. Unlike the example above, it reads [authors.list](src/authors.list) as a mapping file, and uses the fields *Name* and *URL* to map authors to URLs:

  <pre>
  bibtexconv ExampleReferences.bib --mapping=author-url:authors.list:Name:URL &lt;web-example2.export &gt;MyPublications2.html
  </pre>

  Mapping files have been introduced in BibTeXConv&nbsp;2.0.

* Use export script [text-example.export](src/text-example.export) to export references from [ExampleReferences.bib](src/ExampleReferences.bib) to [MyPublications.txt](https://www.nntb.no/~dreibh/bibtexconv/MyPublications.txt) as plain text:

  <pre>
  bibtexconv ExampleReferences.bib &lt;text-example.export &gt;MyPublications.txt
  </pre>

* Use export script [yaml-example.export](src/yaml-example.export) to export references from [ExampleReferences.bib](src/ExampleReferences.bib) to [MyPublications.yaml](https://www.nntb.no/~dreibh/bibtexconv/MyPublications.yaml) as YAML file according to [Debian Upstream MEtadata GAthered with YAml&nbsp;(UMEGAYA)](https://wiki.debian.org/UpstreamMetadata) format:

* Use export script [md-example.export](src/md-example.export) to export references from [ExampleReferences.bib](src/ExampleReferences.bib) to [MyPublications.md](https://www.nntb.no/~dreibh/bibtexconv/MyPublications.md) as Markdown file with [authors.list](src/authors.list) as a mapping file for the authors to URLs:

  <pre>
  bibtexconv ExampleReferences.bib --mapping=author-url:authors.list:Name:URL &lt;md-example.export &gt;MyPublications.md
  </pre>

* Convert all references in [ExampleReferences.bib](src/ExampleReferences.bib) to XML references to be includable in IETF Internet Drafts. For each reference, an own file is generated, named with the prefix "reference." (for example: reference.Globecom2010.xml for the reference Globecom2010):

  <pre>
  bibtexconv ExampleReferences.bib --export-to-separate-xmls=reference. --non-interactive
  </pre>

* Convert all references in [ExampleReferences.bib](src/ExampleReferences.bib) to BibTeX references. For each reference, an own file is generated, named with the prefix "" (here: no prefix; for example: Globecom2010.bib for the reference Globecom2010):
  <pre>
  bibtexconv ExampleReferences.bib --non-interactive --export-to-separate-bibtexs=
  </pre>

* Download all references in [ExampleReferences.bib](src/ExampleReferences.bib) providing an "url" entry to directory Downloads. If the corresponding file is already existing, a download is skipped. That is, the command can be run regularly to maintain an up-to-date publications directory. Updated references (including length, type and MD5 sum of the downloaded entries) are written to UpdatedReferences.bib:
  <pre>
  bibtexconv ExampleReferences.bib --export-to-bibtex=UpdatedReferences.bib --check-urls --store-downloads=Downloads --non-interactive
  </pre>

* Use export script [odt-example.export](src/odt-example.export) to export references from [ExampleReferences.bib](src/ExampleReferences.bib) to [MyPublications.odt](https://www.nntb.no/~dreibh/bibtexconv/MyPublications.odt) as [OpenDocument](https://www.adobe.com/uk/acrobat/resources/document-files/open-doc.html) Text (ODT), according to the template ODT file [ODT-Template.odt](src/ODT-Template.odt):

  <pre>
  bibtexconv-odt ODT-Template.odt MyPublications.odt ExampleReferences.bib odt-example.export
  </pre>

  ODT is the native format of [LibreOffice](https://www.libreoffice.org/)/[OpenOffice](https://www.openoffice.org/). However, LibreOffice/OpenOffice can also be used to convert it to Microsoft Word (DOCX) format, either via GUI or on the command-line to [MyPublications.docx](https://www.nntb.no/~dreibh/bibtexconv/MyPublications.docx):

  <pre>
  soffice --convert-to docx MyPublications.odt
  </pre>


# 📦 Binary Package Installation

Please use the issue tracker at [https://github.com/dreibh/bibtexconv/issues](https://github.com/dreibh/bibtexconv/issues) to report bugs and issues!

## Ubuntu Linux

For ready-to-install Ubuntu Linux packages of BibTeXConv, see [Launchpad PPA for Thomas Dreibholz](https://launchpad.net/~dreibh/+archive/ubuntu/ppa/+packages?field.name_filter=bibtexconv&field.status_filter=published&field.series_filter=)!

<pre>
sudo apt-add-repository -sy ppa:dreibh/ppa
sudo apt-get update
sudo apt-get install bibtexconv
</pre>

## Fedora Linux

For ready-to-install Fedora Linux packages of BibTeXConv, see [COPR PPA for Thomas Dreibholz](https://copr.fedorainfracloud.org/coprs/dreibh/ppa/package/bibtexconv/)!

<pre>
sudo dnf copr enable -y dreibh/ppa
sudo dnf install bibtexconv
</pre>

## FreeBSD

For ready-to-install FreeBSD packages of BibTeXConv, it is included in the ports collection, see [FreeBSD ports tree index of benchmarks/bibtexconv/](https://cgit.freebsd.org/ports/tree/converters/bibtexconv/)!

<pre>
pkg install bibtexconv
</pre>

Alternatively, to compile it from the ports sources:

<pre>
cd /usr/ports/converters/bibtexconv
make
make install
</pre>


# 💾 Build from Sources

BibTeXConv is released under the [GNU General Public Licence&nbsp;(GPL)](https://www.gnu.org/licenses/gpl-3.0.en.html#license-text).

Please use the issue tracker at [https://github.com/dreibh/bibtexconv/issues](https://github.com/dreibh/bibtexconv/issues) to report bugs and issues!

## Development Version

The Git repository of the BibTeXConv sources can be found at [https://github.com/dreibh/bibtexconv](https://github.com/dreibh/bibtexconv):

<pre>
git clone https://github.com/dreibh/bibtexconv
cd bibtexconv
cmake .
make
</pre>

Contributions:

* Issue tracker: [https://github.com/dreibh/bibtexconv/issues](https://github.com/dreibh/bibtexconv/issues).
  Please submit bug reports, issues, questions, etc. in the issue tracker!

* Pull Requests for BibTeXConv: [https://github.com/dreibh/bibtexconv/pulls](https://github.com/dreibh/bibtexconv/pulls).
  Your contributions to BibTeXConv are always welcome!

* CI build tests of BibTeXConv: [https://github.com/dreibh/bibtexconv/actions](https://github.com/dreibh/bibtexconv/actions).

* Coverity Scan analysis of BibTeXConv: [https://scan.coverity.com/projects/dreibh-bibtexconv](https://scan.coverity.com/projects/dreibh-bibtexconv).

## Release Versions

See [https://www.nntb.no/~dreibh/bibtexconv/#current-stable-release](https://www.nntb.no/~dreibh/bibtexconv/#current-stable-release) for release packages!


# 🔗 Useful Links

* [BibTeX](http://www.bibtex.org/)
* [CTAN - The Comprehensive TeX Archive Network](https://www.ctan.org/)
* [Academic Integrity and Ethics](https://web.archive.org/web/20190912152938/https://www.ittc.ku.edu/~jpgs/courses/lecture-academic-integrity-display.pdf)
