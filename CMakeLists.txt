CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
PROJECT(bibtexconv LANGUAGES C CXX)

SET(BUILD_MAJOR "1")
SET(BUILD_MINOR "3")
SET(BUILD_PATCH "2")
SET(BUILD_VERSION ${BUILD_MAJOR}.${BUILD_MINOR}.${BUILD_PATCH})


#############################################################################
# INSTALLATION_DIRECTORIES
#############################################################################

# See: https://cmake.org/cmake/help/v3.0/module/GNUInstallDirs.html
INCLUDE(GNUInstallDirs)


#############################################################################
# PACKAGING
#############################################################################

SET(CPACK_SOURCE_GENERATOR "TXZ")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME
   "${CMAKE_PROJECT_NAME}-${BUILD_MAJOR}.${BUILD_MINOR}.${BUILD_PATCH}")
SET(CPACK_SOURCE_IGNORE_FILES
   "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}-*;${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}_*;/build/;/.git/;/config.log;/CMakeFiles/;CMakeCache.txt$;.cmake$;.tar.gz$;/_CPack_Packages;/Makefile$;~$;/build-deb;/clean-deb;/filter-empty-entries;/make-symbols;/make-ppa;/make-deb;/debian.conf;/make-rpm;/rpm.conf;${CPACK_SOURCE_IGNORE_FILES}")
INCLUDE(CPack)

ADD_CUSTOM_TARGET(dist COMMAND ${CMAKE_MAKE_PROGRAM} clean package_source)


INCLUDE(CheckIncludeFile)


#############################################################################
# REQUIREMENTS
#############################################################################

# ###### Threads ############################################################
FIND_PACKAGE(Threads REQUIRED)

# ###### Bison and Flex #####################################################
FIND_PACKAGE(BISON)
FIND_PACKAGE(FLEX)

# ###### libcurl ############################################################
FIND_PACKAGE(CURL REQUIRED)

# ###### libcrypto (MD5 computation) ########################################
FIND_PACKAGE(OpenSSL REQUIRED)


#############################################################################
# COMPILER FLAGS
#############################################################################

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall -DUSE_UTF8 -D_FORTIFY_SOURCE=2 -O2 -Wformat -Wformat-security")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -DUSE_UTF8 -D_FORTIFY_SOURCE=2 -O2 -Wformat -Wformat-security")

IF (CMAKE_BUILD_TYPE MATCHES DEBUG)
    MESSAGE(STATUS "Building in DEBUG mode")
    # _FORTIFY_SOURCE requires compiling with optimization
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O")
ENDIF()


#############################################################################
# SUBDIRECTORIES
#############################################################################

ADD_SUBDIRECTORY(src)
