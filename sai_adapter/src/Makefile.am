# additional include paths necessary to compile the C++ library

AM_CXXFLAGS = -I$(top_srcdir)/inc -I$(top_srcdir)/inc/sai -Wall -O3 -std=c++11 -pthread -w @AM_CXXFLAGS@
AM_CFLAGS = -I$(top_srcdir)/inc -I$(top_srcdir)/inc/sai @AM_CFLAGS@
###############################################################################
# THE LIBRARIES TO BUILD
###############################################################################

# the library names to build (note we are building static libs only)
lib_LIBRARIES = libp4sai.a

# where to install the headers on the system
libp4sai_adir = $(includedir)/p4sai

# the sources to add to the library and to add to the source distribution
libp4sai_a_SOURCES =    \
						sai_adapter_interface.cpp \
						sai.c \
						saihostif.cpp \
						saineighbor.cpp \
						saiport.cpp \
						sairouter_if.cpp \
						saiswitch.cpp \
						saivlan.cpp \
						sai_adapter.cpp \
						saibridge.cpp \
						saifdb.cpp \
						sailag.cpp \
						sainexthop.cpp \
						sairoute.cpp \
						sai_sniffer.cpp \
						saivirtualrouter.cpp \
						tun_if.c
						
###############################################################################