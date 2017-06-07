SUBDIRS = test/ src/ test/sai_thrift_src test/sai_thrift_server lib/
.PHONY : all $(SUBDIRS)
all : $(SUBDIRS)

$(SUBDIRS) :
	$(MAKE) -C $@
