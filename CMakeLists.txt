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
# Copyright (C) 2010-2026 by Thomas Dreibholz
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

CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(bibtexconv LANGUAGES C CXX)

SET(BUILD_MAJOR "2")
SET(BUILD_MINOR "2")
SET(BUILD_PATCH "3")
SET(BUILD_VERSION ${BUILD_MAJOR}.${BUILD_MINOR}.${BUILD_PATCH})


#############################################################################
#### CMAKE INCLUDES                                                      ####
#############################################################################

INCLUDE(CheckCCompilerFlag)
INCLUDE(CheckCSourceCompiles)
INCLUDE(CheckCSourceRuns)
INCLUDE(CheckIncludeFile)
INCLUDE(CheckStructHasMember)
INCLUDE(GNUInstallDirs)


#############################################################################
#### PACKAGING                                                           ####
#############################################################################

SET(CPACK_SOURCE_GENERATOR "TXZ")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}-${BUILD_MAJOR}.${BUILD_MINOR}.${BUILD_PATCH}")
SET(CPACK_SOURCE_IGNORE_FILES "\\\\.git;\\\\.swp$;~$;\\\\.\\\\#;/\\\\#")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "^${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}[_-]")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "\\\\.make$|\\\\.log$")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "/CMakeCache\\\\.txt$")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "/(CMakeFiles|CMakeScripts|_CPack_Packages)/")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "/package-version\\\\.h$")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "/packaging\\\\.conf$")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "^${PROJECT_SOURCE_DIR}/(po.*/|src.*/|)Makefile$")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "^${PROJECT_SOURCE_DIR}/(po.*|src.*|[^/]*)\\\\.cmake$")
INCLUDE(CPack)

ADD_CUSTOM_TARGET(dist COMMAND ${CMAKE_MAKE_PROGRAM} clean package_source)


#############################################################################
#### UNINSTALL                                                           ####
#############################################################################

IF(NOT TARGET uninstall)
   CONFIGURE_FILE(
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
      "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
      IMMEDIATE @ONLY)

   ADD_CUSTOM_TARGET(uninstall
      COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
ENDIF()


#############################################################################
#### OS DEPENDENT                                                        ####
#############################################################################

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
   MESSAGE(STATUS "${CMAKE_SYSTEM_NAME} supported")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
   MESSAGE(STATUS "${CMAKE_SYSTEM_NAME} supported")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "NetBSD")
   MESSAGE(STATUS "${CMAKE_SYSTEM_NAME} supported")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "OpenBSD")
   MESSAGE(STATUS "${CMAKE_SYSTEM_NAME} supported")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
   MESSAGE(STATUS "${CMAKE_SYSTEM_NAME} supported")
ELSE()
   MESSAGE(FATAL_ERROR "${CMAKE_SYSTEM_NAME} not supported (yet?)")
ENDIF()


#############################################################################
#### REQUIREMENTS                                                        ####
#############################################################################

# ====== Threads ============================================================
FIND_PACKAGE(Threads REQUIRED)

# ====== Bison and Flex =====================================================
FIND_PACKAGE(BISON 3.0)
IF (NOT BISON_FOUND)
   MESSAGE(FATAL_ERROR "Cannot find Bison >= 3.0! Try:\n"
           " * Ubuntu/Debian: sudo apt install -y bison\n"
           " * Fedora:        sudo dnf install -y bison\n"
           " * SuSE:          sudo zypper install -y bison\n"
           " * Alpine:        sudo apk add bison\n"
           " * FreeBSD:       sudo pkg install -y bison\n"
           " * NetBSD:        sudo -y pkgin install bison\n"
           " * OpenBSD:       sudo pkg_add bison\n"
           " * Apple:         brew install -y bison && brew link --force bison")
ENDIF()

FIND_PACKAGE(FLEX)
IF (NOT FLEX_FOUND)
   MESSAGE(FATAL_ERROR "Cannot find Flex! Try:\n"
           " * Ubuntu/Debian: sudo apt install -y flex\n"
           " * Fedora:        sudo dnf install -y flex\n"
           " * SuSE:          sudo zypper install -y flex\n"
           " * Alpine:        sudo apk add flex\n"
           " * FreeBSD:       sudo pkg install -y flex\n"
           " * NetBSD:        sudo -y pkgin install bison\n"
           " * OpenBSD:       (should be installed by default)\n"
           " * Apple:         brew install flex")
ENDIF()

# ====== libcurl ============================================================
FIND_PACKAGE(CURL)
IF (CURL_FOUND)
   MESSAGE(STATUS "cURL found:")
   MESSAGE(STATUS " CURL_INCLUDE_DIRS: ${CURL_INCLUDE_DIRS}")
   MESSAGE(STATUS " CURL_LIBRARIES:    ${CURL_LIBRARIES}")
ELSE()
   MESSAGE(FATAL_ERROR "Cannot find cURL! Try:\n"
           " * Ubuntu/Debian: sudo apt install -y libcurl4-openssl-dev\n"
           " * Fedora:        sudo dnf install -y libcurl-devel\n"
           " * SuSE:          sudo zypper install -y libcurl-devel\n"
           " * Alpine:        sudo apk add curl-dev\n"
           " * FreeBSD:       sudo pkg install -y curl\n"
           " * NetBSD:        sudo -y pkgin install curl\n"
           " * OpenBSD:       sudo pkg_add curl\n"
           " * Apple:         brew install curl")
ENDIF()

# ====== libcrypto (for MD5 computation) ====================================
FIND_PACKAGE(OpenSSL)
IF (OpenSSL_FOUND)
   MESSAGE(STATUS "OpenSSL found:")
   MESSAGE(STATUS " OPENSSL_INCLUDE_DIR: ${OPENSSL_INCLUDE_DIR}")
   MESSAGE(STATUS " OPENSSL_LIBRARIES:   ${OPENSSL_LIBRARIES}")
ELSE()
   MESSAGE(FATAL_ERROR "Cannot find OpenSSL! Try:\n"
           " * Ubuntu/Debian: sudo apt install -y libssl-dev\n"
           " * Fedora:        sudo dnf install -y openssl-devel\n"
           " * SuSE:          sudo zypper install -y libopenssl-devel\n"
           " * Alpine:        sudo apk add openssl-dev\n"
           " * FreeBSD:       sudo pkg install -y openssl\n"
           " * NetBSD:        sudo -y pkgin install openssl\n"
           " * OpenBSD:       sudo pkg_add openssl\n"
           " * Apple:         brew install openssl")
ENDIF()


#############################################################################
#### SUBDIRECTORIES                                                      ####
#############################################################################

ADD_SUBDIRECTORY(src)
