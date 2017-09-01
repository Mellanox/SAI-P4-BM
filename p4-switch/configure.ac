AC_INIT([p4-bmv2], [0.1], [yonatanp@mellanox.com])

# check if the source folder is correct
AC_CONFIG_SRCDIR([src/simple_switch.cpp])

# Checks for programs

# check for C++ preprocessor and compiler and the library compiler
AC_PROG_CXXCPP
AC_PROG_CXX
AM_PROG_AR
LT_INIT

AM_PATH_PYTHON([2.7],, [:])

AC_CONFIG_MACRO_DIRS([m4])

# add this to ARG macro
P4_BM_DIR="behavioral-model"
AC_SUBST(P4_BM_DIR)

# Thrift
AC_PATH_PROG([THRIFT], [thrift], [])

# automake initialisation and check for minimal automake API version 1.9
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

# use the C++ compiler for the following checks
AC_LANG([C++])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([string])
AC_CHECK_HEADERS([iostream])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# distribute additional compiler and linker flags
# --> set these variables instead of CXXFLAGS or LDFLAGS
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_LDFLAGS])

# files to generate via autotools (.am or .in source files)
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([p4-sai/Makefile])

# generate the final Makefile etc.
AC_CONFIG_SUBDIRS([behavioral-model])
AC_OUTPUT