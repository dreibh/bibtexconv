INCLUDE_DIRECTORIES(.)

#############################################################################
# PROGRAMS
#############################################################################

BISON_TARGET(grammar grammar.yy ${CMAKE_CURRENT_BINARY_DIR}/grammar.cc)
FLEX_TARGET( scanner scanner.ll ${CMAKE_CURRENT_BINARY_DIR}/scanner.cc)
ADD_EXECUTABLE(bibtexconv bibtexconv.cc
   node.cc unification.cc unification.h stringhandling.cc publicationset.cc
   ${BISON_grammar_OUTPUTS} ${FLEX_scanner_OUTPUTS})
TARGET_LINK_LIBRARIES(bibtexconv ${CRYPTO_LIBRARIES} ${CURL_LIBRARY})
INSTALL(TARGETS bibtexconv RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
INSTALL(FILES bibtexconv.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)


INSTALL(FILES bibtexconv-odt ietf2bibtex
        DESTINATION ${CMAKE_INSTALL_BINDIR})
INSTALL(FILES bibtexconv-odt.1 ietf2bibtex.1
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)


#################################################
# EXAMPLES
#################################################

INSTALL(FILES ExampleReferences.bib ODT-Template.odt odt-example.export web-example.export text-example.export authors.list authors-fix.list get-author-url publication-list-treeview.js 
        DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/bibtexconv/examples)


#################################################
# SUBDIRECTORIES
#################################################

ADD_SUBDIRECTORY(Images)
