ifndef "$(BASE_DIR)"
   BASE_DIR = `pwd`
endif

include config
include identity

###############################################
MK = $(MAKE) BASE_DIR=$(BASE_DIR)

###############################################
all:
	$(MK) -C src

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

###############################################
# Tests
###############################################
tests:
	$(MK) -C test_dir

###############################################
