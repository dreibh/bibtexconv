# Created by: Thomas Dreibholz <dreibh@iem.uni-due.de>
# $FreeBSD: head/converters/bibtexconv/Makefile 428136 2016-12-08 17:01:31Z tijl $

PORTNAME=	bibtexconv
PORTVERSION=	1.1.17~rc0
CATEGORIES=	converters
MASTER_SITES=	https://www.uni-due.de/~be0001/bibtexconv/download/

MAINTAINER=	dreibh@iem.uni-due.de
COMMENT=	BibTeX Converter

LICENSE=	GPLv3+
LICENSE_FILE=	${WRKSRC}/COPYING

LIB_DEPENDS=	libcurl.so:ftp/curl

USES=		bison cmake python:3.4+ ssl tar:xz

CMAKE_ARGS+=    -DCMAKE_INSTALL_MANDIR=${PREFIX}/man

.include <bsd.port.mk>
