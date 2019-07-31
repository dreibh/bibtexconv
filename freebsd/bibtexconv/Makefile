# Created by: Thomas Dreibholz <dreibh@iem.uni-due.de>
# $FreeBSD: head/converters/bibtexconv/Makefile 428136 2016-12-08 17:01:31Z tijl $

PORTNAME=	bibtexconv
PORTVERSION=	1.1.15
CATEGORIES=	converters
MASTER_SITES=	https://www.uni-due.de/~be0001/bibtexconv/download/
EXTRACT_SUFX=	.tar.xz

MAINTAINER=	dreibh@iem.uni-due.de
COMMENT=	BibTeX Converter

LICENSE=	GPLv3+
LICENSE_FILE=	${WRKSRC}/COPYING

LIB_DEPENDS=	libcurl.so:ftp/curl

GNU_CONFIGURE=	yes
USES=		cmake ssl python:3.4+
CMAKE_ARGS+=    -DCMAKE_INSTALL_MANDIR=${PREFIX}/man

PLIST_FILES=	bin/bibtexconv bin/bibtexconv-odt bin/ietf2bibtex \
		man/man1/bibtexconv.1.gz man/man1/bibtexconv-odt.1.gz \
		man/man1/ietf2bibtex.1.gz

PORTEXAMPLES=	ExampleReferences.bib ODT-Template.odt odt-example.export \
		web-example.export text-example.export \
		Images/ListItem-Collapsed.dia Images/ListItem-Collapsed.svg \
		Images/ListItem-Expanded.dia Images/ListItem-Expanded.svg \
		Images/make-images

OPTIONS_DEFINE=	EXAMPLES

.include <bsd.port.mk>
