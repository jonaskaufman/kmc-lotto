#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

######################################################################
AC_PREREQ([2.69])
AC_INIT([kmc-lotto], [0.1], [jlk@ucsb.edu])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux/m4])
AM_INIT_AUTOMAKE([subdir-objects])

######################################################################

######################################################################
# We'll be making shared libraries
LT_INIT

# Checks for programs.
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_17

######################################################################
# Checks for libraries.
AX_PTHREAD([],AC_MSG_WARN(pthread is required to run tests!))
AC_SEARCH_LIBS(dlopen, dl, [], AC_MSG_ERROR(dl library not found!))

######################################################################
AC_CONFIG_FILES([Makefile])

######################################################################
AC_OUTPUT
