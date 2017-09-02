INC_DIRS = -I$(top_srcdir)/inc -I$(top_srcdir)/inc/sai -I$(top_srcdir)/@P4_BM_DIR@/thrift_src/gen-cpp
AM_CXXFLAGS = ${INC_DIRS} -Wall -O3 -std=c++11 -pthread -w @AM_CXXFLAGS@
AM_CFLAGS = ${INC_DIRS} @AM_CFLAGS@


lib_LTLIBRARIES  = libsai.la

# where to install the headers on the system
libsai_ladir = $(includedir)/sai

# the sources to add to the library and to add to the source distribution
libsai_la_SOURCES =    \
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

libsai_la_LIBADD = -lruntimestubs -lthrift -lpcap
						