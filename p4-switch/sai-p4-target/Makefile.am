simple_switch_thrift_py_files = \
gen-py/sswitch_runtime/constants.py \
gen-py/sswitch_runtime/__init__.py \
gen-py/sswitch_runtime/SimpleSwitch.py \
gen-py/sswitch_runtime/ttypes.py
simple_switch_thrift_files = \
gen-cpp/bm/simple_switch_constants.cpp \
gen-cpp/bm/simple_switch_constants.h \
gen-cpp/bm/SimpleSwitch.cpp \
gen-cpp/bm/SimpleSwitch.h \
gen-cpp/bm/simple_switch_types.cpp \
gen-cpp/bm/simple_switch_types.h

BUILT_SOURCES = $(simple_switch_thrift_files) $(simple_switch_thrift_py_files)

sswitchpydir = $(pythondir)/sswitch_runtime
nodist_sswitchpy_PYTHON = $(simple_switch_thrift_py_files)

python_PYTHON = sswitch_CLI.py

# See
# http://www.gnu.org/software/autoconf/manual/autoconf-2.69/html_node/Installation-Directory-Variables.html
edit = sed \
	-e 's|@pythondir[@]|$(pythondir)|g'

simple_switch_CLI: Makefile
	rm -f $@ $@.tmp
	$(edit) $(srcdir)/$@.in >$@.tmp
	chmod +x $@.tmp
	chmod a-w $@.tmp
	mv $@.tmp $@

simple_switch_CLI: simple_switch_CLI.in

EXTRA_DIST = \
simple_switch_CLI.in \
sswitch_CLI

AM_CXXFLAGS += -std=c++11 \
-I$(srcdir)/thrift/src/ \
-I$(builddir)/gen-cpp \
-I$(top_srcdir)/@P4_BM_DIR@/include \
-isystem$(top_srcdir)/@P4_BM_DIR@/third_party/jsoncpp/include \
-isystem$(top_srcdir)/@P4_BM_DIR@/third_party/spdlog \
-DBMLOG_DEBUG_ON -DBMLOG_TRACE_ON

libsimpleswitch_la_LIBADD = \
$(top_srcdir)/@P4_BM_DIR@/src/bm_sim/libbmsim.la \
$(top_srcdir)/@P4_BM_DIR@/src/bf_lpm_trie/libbflpmtrie.la \
$(top_srcdir)/@P4_BM_DIR@/src/BMI/libbmi.la \
$(top_srcdir)/@P4_BM_DIR@/third_party/jsoncpp/libjson.la \
-lboost_system -lboost_program_options -lthrift

noinst_LTLIBRARIES = libsimpleswitch.la

libsimpleswitch_la_SOURCES = \
simple_switch.cpp simple_switch.h primitives.cpp \
thrift/src/SimpleSwitch_server.cpp

bin_PROGRAMS = simple_switch

simple_switch_LDADD = \
$(top_srcdir)/@P4_BM_DIR@/src/bm_runtime/libbmruntime.la \
$(top_srcdir)/@P4_BM_DIR@/thrift_src/libruntimestubs.la \
libsimpleswitch.la \
libsimpleswitch_thrift.la \
-lthrift

simple_switch_SOURCES = main.cpp

lib_LTLIBRARIES = libsimpleswitch_thrift.la

nodist_libsimpleswitch_thrift_la_SOURCES = \
$(simple_switch_thrift_files)

# See http://www.gnu.org/software/automake/manual/html_node/Multiple-Outputs.html

THRIFT_IDL = $(srcdir)/thrift/simple_switch.thrift

EXTRA_DIST += $(THRIFT_IDL)

# We copy the generated Python code to srcdir/ if we have permissions (i.e. not
# for 'make distcheck'). This is to ensure we can run the CLI easily. Maybe I
# will think of a better solution later.
thrift_files.ts: $(THRIFT_IDL)
	@rm -f thrift_files.tmp
	@touch thrift_files.tmp
	@mkdir -p $(builddir)/gen-cpp/bm
	$(THRIFT) -out $(builddir)/gen-cpp/bm --gen cpp -r $(THRIFT_IDL)
	$(THRIFT) -o $(builddir) --gen py -r $(THRIFT_IDL)
	if mkdir $(srcdir)/sswitch_runtime.test 2>/dev/null; then \
	  rm -rf $(srcdir)/sswitch_runtime/; \
	  cp -r $(builddir)/gen-py/sswitch_runtime/ $(srcdir)/; \
	  rm -rf $(srcdir)/sswitch_runtime.test; else :; \
        fi
	@mv -f thrift_files.tmp $@

$(BUILT_SOURCES): thrift_files.ts
## Recover from the removal of $@
	@if test -f $@; then :; else \
	  trap 'rm -rf thrift_files.lock thrift_files.ts' 1 2 13 15; \
## mkdir is a portable test-and-set
	if mkdir thrift_files.lock 2>/dev/null; then \
## This code is being executed by the first process.
	  rm -f thrift_files.ts; \
	  $(MAKE) $(AM_MAKEFLAGS) thrift_files.ts; \
	  result=$$?; rm -rf thrift_files.lock; exit $$result; \
	else \
## This code is being executed by the follower processes.
## Wait until the first process is done.
	  while test -d thrift_files.lock; do sleep 1; done; \
## Succeed if and only if the first process succeeded.
	    test -f thrift_files.ts; \
	  fi; \
	fi

nodist_pkginclude_HEADERS = \
gen-cpp/bm/simple_switch_constants.h \
gen-cpp/bm/SimpleSwitch.h \
gen-cpp/bm/simple_switch_types.h

bin_SCRIPTS = run_bm.sh \
			  veth_setup.sh \
			  veth_teardown.sh \
			  simple_switch_CLI

P4_JSON_FILES = sai_bridge.json sai_router.json
dist_pkgdata_DATA = ${P4_JSON_FILES}
sai_bridge.json: p4src/bridge/sai_bridge.p4
	p4c-bmv2 --p4-v1.1 --json sai_bridge.json p4src/bridge/sai_bridge.p4
sai_router.json: p4src/router/sai_router.p4
	p4c-bmv2 --p4-v1.1 --json sai_router.json p4src/router/sai_router.p4

CLEANFILES = $(BUILT_SOURCES) \
thrift_files.ts \
gen-cpp/bm/SimpleSwitch_server.skeleton.cpp
# \ $(bin_SCRIPTS)
# \ ${P4_JSON_FILES}

# I used to do a rm on gen-cpp, but it was removing a .deps directory, instead I
# am adding the skeleton file to CLEANFILES
clean-local:
	rm -rf gen-py
