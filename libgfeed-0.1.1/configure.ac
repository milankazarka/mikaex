#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([libgfeed], [0.1.1], [azagli@inwind.it])
AC_CONFIG_SRCDIR([src/rss.c])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE
AC_LIBTOOL_WIN32_DLL

AC_CANONICAL_SYSTEM

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
GTK_DOC_CHECK

# Checks for libraries.
PKG_CHECK_MODULES([GFEED], [glib-2.0 >= 2.6.0
                            gobject-2.0 >= 2.6.0
                            libxml-2.0 >= 2.6.0])

AC_SUBST(GFEED_CFLAGS)
AC_SUBST(GFEED_LIBS)

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_STRFTIME

AC_CONFIG_FILES([
  libgfeed.pc
  Makefile
  src/Makefile
  test/Makefile
  docs/Makefile
  docs/reference/Makefile
  docs/reference/version.xml
])
AC_OUTPUT
