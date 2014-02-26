bin_PROGRAMS = bibtexconv   # t1

AM_YFLAGS = -d   # Tell bison to create grammar table!

bibtexconv_SOURCES = \
   bibtexconv.cc \
   unification.cc unification.h stringhandling.cc stringhandling.h \
   node.cc node.h \
   publicationset.h publicationset.cc \
   grammar.hh grammar.yy scanner.ll
bibtexconv_LDADD = $(CURL_LIBS) $(CRYPTO_LIBS) -lm


%.cc:	%.yy
	bison -d --defines=$(patsubst %.yy,%.hh,$<) --output=$(patsubst %.yy,%.cc,$<) $<


examplesdir        = $(docdir)/examples
dist_examples_DATA = ExampleReferences.bib ODT-Template.odt odt-example.export web-example.export text-example.export authors.list get-author-url

dist_bin_SCRIPTS = bibtexconv-odt

dist_man_MANS = bibtexconv.1 bibtexconv-odt.1

EXTRA_DIST = converter.sed convert-to-new-format

CLEANFILES = grammar.cc grammar.hh scanner.cc


# t1_SOURCES = t1.cc stringhandling.cc stringhandling.h
# t1_LDADD   =
