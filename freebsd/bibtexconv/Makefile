# New ports collection makefile for: bibtexconv
# Date created:                      08 August 2010
# Whom:                              Thomas Dreibholz <dreibh@iem.uni-due.de>
#
# $FreeBSD: ports/net/bibtexconv/Makefile,v 1.1 2010/08/04 20:00:20 ohauer Exp $
#

PORTNAME=	bibtexconv
PORTVERSION=	0.3.0
CATEGORIES=	net
MASTER_SITES=	http://www.iem.uni-due.de/~dreibh/bibtexconv/download/ \
		http://www.exp-math.uni-essen.de/~dreibh/bibtexconv/download/

MAINTAINER=	dreibh@iem.uni-due.de
COMMENT=	BibTeX Convertor

# LIB_DEPENDS+=

GNU_CONFIGURE=	yes
CONFIGURE_ARGS=	""
USE_GMAKE=	yes

LICENSE=	GPLv3
LICENSE_FILE=	${WRKSRC}/COPYING

PLIST_FILES=	bin/bibtexconv
MAN8=		bibtexconv.8

.include <bsd.port.mk>
