CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(bibtexconv LANGUAGES C CXX)

SET(BUILD_MAJOR "1")
SET(BUILD_MINOR "3")
SET(BUILD_PATCH "7~rc0")
SET(BUILD_VERSION ${BUILD_MAJOR}.${BUILD_MINOR}.${BUILD_PATCH})


#############################################################################
#### INSTALLATION_DIRECTORIES                                            ####
#############################################################################

# See: https://cmake.org/cmake/help/v3.0/module/GNUInstallDirs.html
INCLUDE(GNUInstallDirs)


#############################################################################
#### PACKAGING                                                           ####
#############################################################################

SET(CPACK_SOURCE_GENERATOR "TXZ")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}-${BUILD_MAJOR}.${BUILD_MINOR}.${BUILD_PATCH}")
SET(CPACK_SOURCE_IGNORE_FILES "\\\\.git;\\\\.swp$;~$;\\\\.\\\\#;/\\\\#")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "^${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}[_-]")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "\\\\.cmake$|\\\\.make$|\\\\.log$")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "/CMakeCache\\\\.txt$")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "/(CMakeFiles|CMakeScripts|_CPack_Packages)/")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "/package-version\\\\.h$")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "/packaging\\\\.conf$")
LIST(APPEND CPACK_SOURCE_IGNORE_FILES "^${PROJECT_SOURCE_DIR}/(po.*/|src.*/|)Makefile$")
INCLUDE(CPack)

ADD_CUSTOM_TARGET(dist COMMAND ${CMAKE_MAKE_PROGRAM} clean package_source)


INCLUDE(CheckIncludeFile)


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
#### REQUIREMENTS                                                        ####
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
#### COMPILER FLAGS                                                      ####
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
#### SUBDIRECTORIES                                                      ####
#############################################################################

ADD_SUBDIRECTORY(src)
