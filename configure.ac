#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([com], [0.01], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([include/Makefile.am])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_INSTALL

# Install path
PREFIX="/home"
prefix=$PREFIX

# Compile flags
CFLAGS_DEBUG="\
    -g \
    "
CFLAGS_OPTIMIZE="\
    -O2 \
    "
CFLAGS_WARNING="\
    -Werror \
    -Wall \
    -Wextra \
    "
CFLAGS_INCLUDE="\
    -I${PREFIX}/include \
    "
CFLAGS="\
    ${CFLAGS_DEBUG} \
    ${CFLAGS_OPTIMIZE} \
    ${CFLAGS_WARNING} \
    ${CFLAGS_INCLUDE} \
     -std=c11 \
     "
LDFLAGS="\
    -L${PREFIX}/lib \
    "
LIBS="\
    "

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 include/Makefile])

AC_OUTPUT
