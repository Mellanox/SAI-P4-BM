AC_INIT([p4-sai-adapter], [0.1], [yonatanp@mellanox.com])

# check if the source folder is correct
AC_CONFIG_SRCDIR([src/sai.c])

# Checks for programs

# check for C++ preprocessor and compiler and the library compiler
AC_PROG_CXXCPP
AC_PROG_CXX
AC_PROG_RANLIB
AM_PROG_AR

#   # Check for thrift    - Why doesn't this work?
#   AC_PATH_PROG([THRIFT], [thrift], [])
#   AC_SUBST([THRIFT_LIB], ["-lthrift"])
#   AC_CHECK_HEADER([thrift/Thrift.h], [], [AC_MSG_ERROR([Thrift headers not found. Install Thrift from http://thrift.apache.org/docs/install/])])

# Create thrift output for sai_thrift_server (needed for ptf)
echo "Generating thrift output file for sai_thrift_server"
thrift --gen cpp --gen py -o test/sai_thrift_src/ test/sai_thrift_src/switch_sai.thrift 

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
AC_CONFIG_FILES([inc/Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([test/Makefile])
AC_CONFIG_FILES([test/sai_thrift_server/Makefile])
AC_CONFIG_FILES([test/sai_thrift_src/Makefile])

# generate the final Makefile etc.
AC_OUTPUT