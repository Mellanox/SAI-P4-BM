SUBDIRS = test/ src/
# src/ test/sai_thrift_server test/unittest test/learn_app
.PHONY : all $(SUBDIRS)
all : $(SUBDIRS)

$(SUBDIRS) :
	$(MAKE) -C $@