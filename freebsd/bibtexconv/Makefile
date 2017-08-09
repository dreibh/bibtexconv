# New ports collection makefile for: bibtexconv
# Date created:                      07 June 2012
# Whom:                              Thomas Dreibholz <dreibh@iem.uni-due.de>
#
# $FreeBSD: ports/converters/bibtexconv/Makefile,v 1.1 2010/08/04 20:00:20 dreibh Exp $
#

PORTNAME=	bibtexconv
PORTVERSION=	1.0.0
CATEGORIES=	converters
MASTER_SITES=	https://www.uni-due.de/~be0001/bibtexconv/download/ \
		http://www.exp-math.uni-essen.de/~dreibh/bibtexconv/download/

MAINTAINER=	dreibh@iem.uni-due.de
COMMENT=	BibTeX Converter

LIB_DEPENDS+=   libcurl libcrypto

GNU_CONFIGURE=	yes
CONFIGURE_ARGS=	""
USE_GMAKE=	yes

LICENSE=	GPLv3
LICENSE_FILE=	${WRKSRC}/COPYING

PLIST_FILES=	bin/bibtexconv bin/bibtexconv-odt
MAN1=		bibtexconv.1 bibtexconv-odt.1

.include <bsd.port.mk>
