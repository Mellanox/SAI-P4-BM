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
						sai.c \
						tun_if.c \
						saiacl.cpp \
						sailag.cpp \
						saisamplepacket.cpp \
						sai_adapter.cpp \
						saimcastfdb.cpp \
						saischeduler.cpp \
						sai_adapter_interface.cpp \
						saimirror.cpp \
						saischedulergroup.cpp \
						saibridge.cpp \
						saineighbor.cpp \
						sai_sniffer.cpp \
						saibuffer.cpp \
						sainexthop.cpp \
						saistp.cpp \
						saifdb.cpp \
						saipolicer.cpp \
						saiswitch.cpp \
						saihash.cpp \
						saiport.cpp \
						saitunnel.cpp \
						saihostif.cpp \
						saiqosmap.cpp \
						saiudf.cpp \
						saiipmc.cpp \
						saiqueue.cpp \
						saivirtualrouter.cpp \
						saiipmcgroup.cpp \
						sairoute.cpp \
						saivlan.cpp \
						sail2mc.cpp \
						sairouter_if.cpp \
						saiwred.cpp \
						sail2mcgroup.cpp \
						sairpfgroup.cpp
						
###############################################################################