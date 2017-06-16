SUBDIRS = test/ src/ test/sai_thrift_src test/sai_thrift_server 
.PHONY : all $(SUBDIRS)
all : $(SUBDIRS)

$(SUBDIRS) :
	$(MAKE) -C $@
