<h1 align="center">
 BibTeXConv<br />
 <span style="font-size: 75%">A BibTeX File Converter</span><br />
 <a href="https://www.nntb.no/~dreibh/bibtexconv/">
  <span style="font-size: 75%;">https://www.nntb.no/~dreibh/bibtexconv</span>
 </a>
</h1>


# 💡 What is BibTeXConv?

BibTeXConv is a BibTeX file converter which allows exporting BibTeX entries to other formats, including custom-defined text output. Furthermore, it provides the possibility to check URLs (including MD5, size and MIME type computations) and to verify ISBN and ISSN numbers.

# 😀 Examples

Take a look in `/usr/share/doc/bibtexconv/examples/` (or the corresponding path of your system) for example export scripts. The export scripts contain the commands which are read by bibtexconv from standard input.

* Check URLs of all entries in [ExampleReferences.bib](src/ExampleReferences.bib), add MD5, size and MIME type items and write the results to UpdatedReferences.bib:

  ```bash
  bibtexconv ExampleReferences.bib \
     --export-to-bibtex=UpdatedReferences.bib \
     --check-urls --only-check-new-urls --non-interactive
  ```

* Use the export script [web-example1.export](src/web-example1.export) to export references from [ExampleReferences.bib](src/ExampleReferences.bib) to [MyPublications1.html](https://www.nntb.no/~dreibh/bibtexconv/MyPublications1.html) as XHTML 1.1. [ExampleReferences.bib](src/ExampleReferences.bib) references the script [get-author-url](src/get-author-url) and the list [authors.list](src/authors.list) to obtain the authors' website URLs:

  ```bash
  bibtexconv ExampleReferences.bib \
     <web-example1.export >MyPublications1.html
  ```

  Note that using a script is slow, and may introduce a security issue when running export scripts from untrusted sources! The preferred way for mappings is to use mapping files, which is demonstrated by the next example.

* Use the export script [web-example2.export](src/web-example2.export) to export references from [ExampleReferences.bib](src/ExampleReferences.bib) to [MyPublications2.html](https://www.nntb.no/~dreibh/bibtexconv/MyPublications2.html) as XHTML 1.1. Unlike the example above, it reads [authors.list](src/authors.list) as a mapping file, and uses the fields *Name* and *URL* to map authors to URLs:

  ```bash
  bibtexconv ExampleReferences.bib \
     --mapping=author-url:authors.list:Name:URL \
     <web-example2.export >MyPublications2.html
  ```

  Mapping files have been introduced in BibTeXConv&nbsp;2.0.

* Use the export script [text-example.export](src/text-example.export) to export references from [ExampleReferences.bib](src/ExampleReferences.bib) to [MyPublications.txt](https://www.nntb.no/~dreibh/bibtexconv/MyPublications.txt) as plain text:

  ```bash
  bibtexconv ExampleReferences.bib \
     <text-example.export >MyPublications.txt
  ```

* Use the export script [yaml-example.export](src/yaml-example.export) to export references from [ExampleReferences.bib](src/ExampleReferences.bib) to [MyPublications.yaml](https://www.nntb.no/~dreibh/bibtexconv/MyPublications.yaml) as YAML file according to [Debian Upstream MEtadata GAthered with YAml&nbsp;(UMEGAYA)](https://wiki.debian.org/UpstreamMetadata) format:

  ```bash
  bibtexconv ExampleReferences.bib \
     <yaml-example.export >MyPublications.yaml
  ```

* Use the export script [md-example.export](src/md-example.export) to export references from [ExampleReferences.bib](src/ExampleReferences.bib) to [MyPublications.md](https://www.nntb.no/~dreibh/bibtexconv/MyPublications.md) as Markdown file with [authors.list](src/authors.list) as a mapping file for the authors to URLs:

  ```bash
  bibtexconv ExampleReferences.bib \
     --mapping=author-url:authors.list:Name:URL \
     <md-example.export >MyPublications.md
  ```

* Convert all references in [ExampleReferences.bib](src/ExampleReferences.bib) to XML references to be includable in IETF Internet Drafts. For each reference, a separate file is generated, named with the prefix "reference." (for example: reference.Globecom2010.xml for the reference Globecom2010):

  ```bash
  bibtexconv ExampleReferences.bib \
     --export-to-separate-xmls=reference. --non-interactive
  ```

* Convert all references in [ExampleReferences.bib](src/ExampleReferences.bib) to BibTeX references. For each reference, a separate file is generated, named with the prefix "" (here: no prefix; for example: Globecom2010.bib for the reference Globecom2010):

  ```bash
  bibtexconv ExampleReferences.bib \
     --export-to-separate-bibtexs= --non-interactive
  ```

* Download all references in [ExampleReferences.bib](src/ExampleReferences.bib) providing a `url` entry to the `Downloads` directory. If the corresponding file already exists, a download is skipped. That is, the command can be run regularly to maintain an up-to-date publications directory. Updated references (including length, type and MD5 sum of the downloaded entries) are written to UpdatedReferences.bib:

  ```bash
  bibtexconv ExampleReferences.bib \
     --export-to-bibtex=UpdatedReferences.bib \
     --check-urls --store-downloads=Downloads --non-interactive
  ```

* Use the export script [odt-example.export](src/odt-example.export) to export references from [ExampleReferences.bib](src/ExampleReferences.bib) to [MyPublications.odt](https://www.nntb.no/~dreibh/bibtexconv/MyPublications.odt) as [OpenDocument](https://www.adobe.com/uk/acrobat/resources/document-files/open-doc.html) Text (ODT), according to the template ODT file [ODT-Template.odt](src/ODT-Template.odt):

  ```bash
  bibtexconv-odt ODT-Template.odt MyPublications.odt \
     ExampleReferences.bib odt-example.export
  ```

  ODT is the native format of [LibreOffice](https://www.libreoffice.org/)/[OpenOffice](https://www.openoffice.org/). However, LibreOffice/OpenOffice can also be used to convert it to Microsoft Word (DOCX) format, either via the GUI or on the command line to [MyPublications.docx](https://www.nntb.no/~dreibh/bibtexconv/MyPublications.docx):

  ```bash
  soffice --convert-to docx MyPublications.odt
  ```

* Also take a look into the manual pages of BibTeXConv and BibTeXConv-ODT for further information and options:

  ```bash
  man bibtexconv
  man bibtexconv-odt
  ```


# 📦 Binary Package Installation

Please use the issue tracker at [https://github.com/dreibh/bibtexconv/issues](https://github.com/dreibh/bibtexconv/issues) to report bugs and issues!

## Ubuntu Linux

For ready-to-install [Ubuntu Linux](https://ubuntu.com/) packages of BibTeXConv, see the [Launchpad PPA for Thomas Dreibholz](https://launchpad.net/~dreibh/+archive/ubuntu/ppa/+packages?field.name_filter=bibtexconv&field.status_filter=published&field.series_filter=)!

```bash
sudo apt-add-repository -sy ppa:dreibh/ppa
sudo apt-get update
sudo apt-get install bibtexconv
```

## Debian Linux

For ready-to-install [Debian Linux](https://www.debian.org/) packages of BibTeXConv, see the [Open Build Service PPA for Thomas Dreibholz](https://build.opensuse.org/project/show/home:dreibh)!

Add the PPA repository:

```bash
. /etc/os-release
DISTRIBUTION="Debian_${VERSION_ID:-$([ "${VERSION_CODENAME:-}" = sid ] && echo Unstable || echo Testing)}"
URL="https://download.opensuse.org/repositories/home:/dreibh/${DISTRIBUTION}"
KEY="/etc/apt/keyrings/dreibh-obs.gpg"

curl -fsSL "${URL}/Release.key" | sudo gpg --batch --yes --dearmor -o "${KEY}"
printf "deb [signed-by=%s] %s/ /\ndeb-src [signed-by=%s] %s/ /\n" "${KEY}" "${URL}" "${KEY}" "${URL}" | \
   sudo tee /etc/apt/sources.list.d/obs-dreibh.list
sudo apt update
```

Then, install BibTeXConv:

```bash
sudo apt-get install bibtexconv
```

## Fedora Linux

For ready-to-install [Fedora Linux](https://fedoraproject.org/) packages of BibTeXConv, see the [COPR PPA for Thomas Dreibholz](https://copr.fedorainfracloud.org/coprs/dreibh/ppa/package/bibtexconv/)!

```bash
sudo dnf copr enable -y dreibh/ppa
sudo dnf install bibtexconv
```

## OpenSUSE Linux

For ready-to-install [OpenSUSE Linux](https://www.opensuse.org/) packages of BibTeXConv, see [Open Build Service PPA for Thomas Dreibholz](https://build.opensuse.org/project/show/home:dreibh)!

Add the PPA repository:

```bash
. /etc/os-release
[[ $VERSION_ID =~ ^[0-9]+\.[0-9]+$ ]] && DISTRIBUTION="${VERSION_ID}" || DISTRIBUTION="${NAME// /_}"
URL="https://download.opensuse.org/repositories/home:/dreibh/${DISTRIBUTION}"
rpm --import "${URL}/repodata/repomd.xml.key"
zypper addrepo -f "${URL}/" dreibh-obs
```

Then, install BibTeXConv:

```bash
sudo zypper install bibtexconv
```

## Alpine Linux

For ready-to-install [Alpine Linux](https://alpinelinux.org/) packages of BibTeXConv, see [Open Build Service PPA for Thomas Dreibholz](https://build.opensuse.org/project/show/home:dreibh)!

Add the PPA repository:

```bash
DISTRIBUTION="Alpine_Latest_community"
URL="https://download.opensuse.org/repositories/home:/dreibh"
wget -O \
   /etc/apk/keys/home:dreibh@build.opensuse.org-527a4e72.rsa.pub \
   "${URL}/${DISTRIBUTION}/x86_64/home:dreibh%40build.opensuse.org-527a4e72.rsa.pub"
if ! grep -q "^${URL}/${DISTRIBUTION}" /etc/apk/repositories ; then
   echo "${URL}/${DISTRIBUTION}" | sudo tee -a /etc/apk/repositories
fi
```

Then, install BibTeXConv:

```bash
sudo apk add bibtexconv
```

## FreeBSD

For ready-to-install [FreeBSD](https://www.freebsd.org/) packages of BibTeXConv, it is included in the ports collection; see [FreeBSD ports tree index of net/bibtexconv/](https://cgit.freebsd.org/ports/tree/net/bibtexconv/)!

```bash
sudo pkg install bibtexconv
```

Alternatively, to compile it from the ports sources:

```bash
cd /usr/ports/net/bibtexconv
make
sudo make install
```

## NetBSD

BibTeXConv supports [NetBSD](https://netbsd.org/). However, there is no NetBSD packaging yet. Just build from sources!

## OpenBSD

BibTeXConv supports [OpenBSD](https://www.openbsd.org/). However, there is no OpenBSD packaging yet. Just build from sources!

## Solaris (OpenIndiana)

BibTeXConv supports [Solaris (OpenIndiana)](https://www.openindiana.org/). However, there is no Solaris packaging yet. Just build from sources!

## GNU Hurd

BibTeXConv supports [GNU Hurd](https://www.gnu.org/software/hurd/) ([Debian GNU/Hurd](https://www.debian.org/ports/hurd/)). However, there is no Debian GNU/Hurd PPA on Open Build Service available yet. Just build from sources!

## Homebrew (Apple, Linux)

For the [Homebrew](https://brew.sh/) formula of BibTeXConv, see [Thomas Dreibholz's Homebrew Tap](https://github.com/dreibh/homebrew-tap)!

Add tap:

```bash
brew tap dreibh/tap
brew trust dreibh/tap
```

Then, install BibTeXConv:

```bash
brew install bibtexconv
```


# 💾 Build from Sources

BibTeXConv is released under the [GNU General Public License&nbsp;(GPL)](https://www.gnu.org/licenses/gpl-3.0.en.html#license-text).

Please use the issue tracker at [https://github.com/dreibh/bibtexconv/issues](https://github.com/dreibh/bibtexconv/issues) to report bugs and issues!

## Development Version

The Git repository of the BibTeXConv sources can be found at [https://github.com/dreibh/bibtexconv](https://github.com/dreibh/bibtexconv):

```bash
git clone https://github.com/dreibh/bibtexconv
cd bibtexconv
sudo ci/get-dependencies --install
cmake .
make
```

Optionally, for installation to the standard paths (usually under `/usr/local`):

```bash
sudo make install
```

Note: The script [`ci/get-dependencies`](https://github.com/dreibh/bibtexconv/blob/master/ci/get-dependencies) automatically installs the build dependencies under Debian/Ubuntu Linux, Fedora Linux, OpenSUSE Linux, Alpine Linux, FreeBSD, and Debian GNU/Hurd. For manual handling of the build dependencies, take a look at the packaging configuration files:

* [`debian/control`](https://github.com/dreibh/bibtexconv/blob/master/debian/control) (Debian/Ubuntu Linux, Debian GNU/Hurd),
* [`bibtexconv.spec`](https://github.com/dreibh/bibtexconv/blob/master/rpm/bibtexconv.spec) (Fedora Linux, OpenSUSE Linux),
* [`APKBUILD`](https://github.com/dreibh/bibtexconv/blob/master/packaging/APKBUILD) (Alpine Linux),
* [`Makefile`](https://github.com/dreibh/bibtexconv/blob/master/freebsd/bibtexconv/Makefile) (FreeBSD), and
* [`bibtexconv.rb`](https://github.com/dreibh/bibtexconv/blob/master/packaging/bibtexconv.rb) (Homebrew).

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
