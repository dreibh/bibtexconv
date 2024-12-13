# ==========================================================================
#                ____  _ _   _____   __  ______
#                | __ )(_) |_|_   _|__\ \/ / ___|___  _ ____   __
#                |  _ \| | '_ \| |/ _ \  / |   / _ \| '_ \ \ / /
#                | |_) | | |_) | |  __//  \ |__| (_) | | | \ V /
#                |____/|_|_.__/|_|\___/_/\_\____\___/|_| |_|\_/
#
#                          ---  BibTeX Converter  ---
#                   https://www.nntb.no/~dreibh/bibtexconv/
# ==========================================================================
#
# BibTeX Converter
# Copyright (C) 2010-2025 by Thomas Dreibholz
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Contact: thomas.dreibholz@gmail.com


SET(CMAKE_INCLUDE_CURRENT_DIR ON)

INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR} ${CURL_INCLUDE_DIRS})


#############################################################################
#### PROGRAMS                                                            ####
#############################################################################

BISON_TARGET(grammar grammar.yy ${CMAKE_CURRENT_BINARY_DIR}/grammar.cc)
FLEX_TARGET( scanner scanner.ll ${CMAKE_CURRENT_BINARY_DIR}/scanner.cc)
ADD_EXECUTABLE(bibtexconv
   bibtexconv.cc
   node.cc
   publicationset.cc
   stringhandling.cc
   unification.cc
   unification.h
   ${BISON_grammar_OUTPUTS}
   ${FLEX_scanner_OUTPUTS}
)
TARGET_LINK_LIBRARIES(bibtexconv ${OPENSSL_CRYPTO_LIBRARY} ${CURL_LIBRARIES})
INSTALL(TARGETS     bibtexconv RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
INSTALL(FILES       bibtexconv.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
INSTALL(FILES       bibtexconv.bash-completion
        DESTINATION ${CMAKE_INSTALL_DATADIR}/bash-completion/completions
        RENAME      bibtexconv)


INSTALL(PROGRAMS    bibtexconv-odt
        DESTINATION ${CMAKE_INSTALL_BINDIR})
INSTALL(FILES       bibtexconv-odt.1
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
INSTALL(FILES       bibtexconv-odt.bash-completion
        DESTINATION ${CMAKE_INSTALL_DATADIR}/bash-completion/completions
        RENAME      bibtexconv-odt)


INSTALL(PROGRAMS    ietf2bibtex
        DESTINATION ${CMAKE_INSTALL_BINDIR})
INSTALL(FILES       ietf2bibtex.1
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
INSTALL(FILES       ietf2bibtex.bash-completion
        DESTINATION ${CMAKE_INSTALL_DATADIR}/bash-completion/completions
        RENAME      ietf2bibtex)


#############################################################################
#### EXAMPLES                                                            ####
#############################################################################

INSTALL(FILES authors-fix.list
              authors.list
              ExampleReferences.bib
              odt-example.export
              ODT-Template.odt
              publication-list-treeview.js
              text-example.export
              web-example.export
              yaml-example.export
        DESTINATION ${CMAKE_INSTALL_DOCDIR}/examples)
INSTALL(PROGRAMS get-author-url
        DESTINATION ${CMAKE_INSTALL_DOCDIR}/examples)


#############################################################################
#### SUBDIRECTORIES                                                      ####
#############################################################################

ADD_SUBDIRECTORY(Images)
