ifndef "$(BASE_DIR)"
   BASE_DIR = `pwd`
endif

SRC_ROOT=..
include $(SRC_ROOT)/Makefile.include

include config
include identity

###############################################
MK = $(MAKE) BASE_DIR=$(BASE_DIR)

###############################################
all:    lib

lib:
	cd src; $(MAKE) all
	cp src/libfthread.a ../../lib
	cp src/fthread.h ../include

bin:

depend:

validate:

info:
	@ echo "**** for cleaning: make clean"
	@ echo "**** for a complete installation: make install"
	@ echo "**** for desinstallation: make uninstall"
	@ echo "**** for testing installation: make tests"

###############################################
# Install
###############################################
install: 
	$(MK) -C src install

uninstall:
	$(MK) -C src uninstall

###############################################
# Clean
###############################################
clean:
	- $(MK) -C src clean
	- $(MK) -C examples clean
	- $(MK) -C test_dir clean
	rm -f *~
	rm ../../lib/libfthread.a
	rm ../include/fthread.h

###############################################
# Tests
###############################################
tests:
	$(MK) -C test_dir

###############################################
