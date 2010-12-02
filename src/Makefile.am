bin_PROGRAMS = bibtexconv   # t1

AM_YFLAGS = -d   # Tell bison to create grammar table!

bibtexconv_SOURCES = \
   bibtexconv.cc \
   unification.cc unification.h stringhandling.cc stringhandling.h \
   node.cc node.h \
   publicationset.h publicationset.cc \
   grammar.yy scanner.ll
bibtexconv_LDADD   =


# t1_SOURCES = t1.cc stringhandling.cc stringhandling.h
# t1_LDADD   =


dist_man_MANS = bibtexconv.8


CLEANFILES = grammar.cc grammar.h scanner.cc
