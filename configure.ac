AC_INIT([BibTeXConv], [1.1.7~rc1], [dreib@iem.uni-due.de])

AM_INIT_AUTOMAKE
AC_PREREQ(2.60)

AC_CONFIG_HEADERS([config.h:config.in])
AC_CONFIG_SRCDIR([src/bibtexconv.cc])

# LT_INIT
# AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_YACC

# ###### colorgcc ###########################################################
AC_ARG_ENABLE([colorgcc],
              [  --enable-colorgcc       enable colorgcc support ][default=no]],
              enable_colorgcc=$enableval, enable_colorgcc=no)
if test "${enable_colorgcc}" = "yes"; then
   AC_CHECK_PROG([COLORGCC], [colorgcc], [yes], [no])
   if test "$COLORGCC" == "yes"
   then
      AC_MSG_NOTICE([Compiling using colorgcc])
      CC="colorgcc"
      CXX="colorgcc -lstdc++"
   fi
fi

# ###### Maintainer mode ####################################################
AC_ARG_ENABLE([maintainer-mode],
[  --enable-maintainer-mode            enable maintainer mode ]
[default=yes]],enable_maintainer_mode=$enableval,enable_maintainer_mode=no)
if test "$enable_maintainer_mode" = "yes"; then
   CFLAGS="$CFLAGS -Wall -Wunreachable-code -g3"
   CXXFLAGS="$CXXFLAGS -Wall -Wunreachable-code -g3"
else
   CFLAGS="$CFLAGS -Wall -Wunreachable-code"
   CXXFLAGS="$CXXFLAGS -Wall -Wunreachable-code"
fi


# ###### Operating system checks ############################################
case $host_os in
freebsd*)
   CFLAGS="$CFLAGS -I/usr/local/include"
   CXXFLAGS="$CXXFLAGS -I/usr/local/include"
   LDFLAGS="$LDFLAGS -L/usr/local/lib"
esac


# ###### libcurl ############################################################
AC_CHECK_LIB(curl, curl_easy_perform,
               [ CURL_LIBS=-lcurl ],
               [ AC_ERROR([libcurl not found!]) ],)
AC_SUBST(CURL_LIBS)


# ###### libssl (MD5 computation) ###########################################
AC_CHECK_LIB(crypto, MD5_Init,
               [ CRYPTO_LIBS=-lcrypto ],
               [ AC_ERROR([libcrypto not found!]) ],)
AC_SUBST(CRYPTO_LIBS)


# ###### Compiler flags #####################################################
CFLAGS="$CFLAGS -DUSE_UTF8"
CXXFLAGS="$CXXFLAGS -DUSE_UTF8"


# ###### Makefiles ##########################################################
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
