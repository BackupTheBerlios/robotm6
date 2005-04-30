SRC_ROOT=.
## list of directories to compile and their orders

ifdef GUMSTIX
COMPILE_DIR_ORDER=log tools simulator move events io main 2005 devices #ftv1.2 
else
COMPILE_DIR_ORDER=log tools soundPlayer viewer3D simulator move events io main 2005 devices #ftv1.2 
endif
#modules

SRCS=*.cpp

all:

include $(SRC_ROOT)/Makefile.include

## ===========================================================================
## Complety recreate the project: Create all libs and binaries
## ===========================================================================
# HACK: should be able to do depend even for gumstix.
all: clean depend fast

## ===========================================================================
## Create all libs and binaries
## ===========================================================================
ifdef GUMSTIX
fast: timestamp lib bin
else
fast: timestamp lib bin validate 
endif

## ===========================================================================
## Create libraries. Must be done before creating binaries
## ===========================================================================
lib: default_lib_dirs

## ===========================================================================
## Create binaries
## ===========================================================================
bin: default_bin_dirs

## ===========================================================================
## Copy sbin binaries
## ===========================================================================
#copie les binaires utilises dans sbin
sbin: fast
	\cp /usr/bin/mpg123   ../sbin

## ===========================================================================
## Create validation binaries
## ===========================================================================
validate: default_validate_dirs

## ===========================================================================
## Clean all sub-directories
## ===========================================================================
clean : default_clean default_clean_dirs
	${RM} -f *~
	${RM} -f ../sbin/*~ ../bin/*~ ../binsimu/*~ ../validate/*~ ../so/*~ ../lib/*~ 
	${RM} -f ../movie/*~ ../screenshot/*~ ../doc/*~ ../sound/*~ ../mp3/*~
	${RM} -f include/*~
	${RM} -f include/driver/*~

## ===========================================================================
## Generate tar files of the source code
## ===========================================================================
tarsrc: clean
	cd ../doc; ${RM} -rf core core* *~ .gz .xv* *.bmp *.tga
	cd ../sbin; ${RM} -rf core core* *~ .gz .xv* *.bmp *.tga
	cd ../bin; ${RM} -rf core core* *~ .gz .xv* *.bmp *.tga
	cd ../binsimu; ${RM} -rf core core* *~ .gz .xv* *.bmp *.tga
	cd ../validate; ${RM} -rf core core* *~ 
	./maketar robot05 ../../ robot2005/src                        \
                  robot2005/bin robot2005/binsimu robot2005/validate  \
                  robot2005/README robot2005/*/README                 \
                  robot2005/sound/soundList.txt 

tar:   
	cd ../doc; ${RM} -rf core core* *~ .gz .xv* *.bmp *.tga
	cd ../sbin; ${RM} -rf core core* *~ .gz .xv* *.bmp *.tga
	cd ../bin; ${RM} -rf core core* *~ .gz .xv* *.bmp *.tga
	cd ../binsimu; ${RM} -rf core core* *~ .gz .xv* *.bmp *.tga
	cd ../validate; ${RM} -rf core core* *~ 
	./maketar robot05 ../../ robot2005/src                        \
                  robot2005/*/*.sh  \
                  robot2005/README robot2005/*/README                 \
                  robot2005/sound/soundList.txt                       \

tarall: clean doc
	./maketar robot05 ../../ robot2005

## ===========================================================================
## Count the number of lines of code
## ===========================================================================
line:
	wc -l */*.h */*.cpp */*/*.h */*/*.cpp */*/*/*.cpp > fichiers	
	cat fichiers
	wc -l fichiers
	${RM} -f fichiers

## ===========================================================================
## affiche la doc en html
## ===========================================================================
help:
	cat README
	netscape ../doc/index.html &

## ===========================================================================
## Generate the HTML doc 
## ===========================================================================
doc :
	\rm -rf $(SRC_ROOT)/../doc/html
	doxygen doxygenConfig

## ===========================================================================
## Check libs for undefied symbols
## ===========================================================================
check:
	undefinedSymbols "base_"

## ===========================================================================
## Upload the program into the robot
## ===========================================================================
so2: 
	scp -r ../gumstix/so/ root@gumb2:robot

so1:
	scp -r ../gumstix/so/ root@gumb:robot

sou2: 
	scp -r ../gumstix/so/ root@gumu2:robot

sou1:
	scp -r ../gumstix/so/ root@gumu:robot

install2: 
	scp -r ../gumstix/bin ../gumstix/so/ root@gumb2:robot

install1:
	scp -r ../gumstix/bin ../gumstix/so/ root@gumb:robot

installu2: 
	scp -r ../gumstix/bin ../gumstix/so/ root@gumu2:robot

installu1:
	scp -r ../gumstix/bin ../gumstix/so/ root@gumu:robot

cleanRobot2:
#efface les anciens logs et photos
	ssh root@gumb2 'rm -f robot/*.log.gz' 

## ===========================================================================
## Download the robot match data on the PC
## ===========================================================================
getData2:
# reccupere les donnees de match sur le gros PC
	scp root@gumb2:'robot/log/*.log.gz' ../log

getData1:
# reccupere les donnees de match sur le gros PC
	scp root@gumb:'robot/log/*.log.gz' ../log



## ===========================================================================
## Specific rules
## ===========================================================================
# cree un fichier contenant les informations de compilation
timestamp: 
	${RM} -f $(SRC_ROOT)/include/timestamp.h
	echo "/* Ne pas editer ce fichier, il est genere automatiquement */" >> timestamp.h
	echo "#ifndef TIMESTAMP_H" > timestamp.h	
	echo "#define TIMESTAMP_H" >> timestamp.h
	date '+#define __COMPILE_DATE__ "%d/%m/%y"' >> timestamp.h
	date '+#define __COMPILE_TIME__ "%H:%M:%S"' >> timestamp.h
	date '+#define __COMPILE_USER__ "${USER}"' >> timestamp.h
	date '+#define __COMPILE_DIR__ "${PWD}"' >> timestamp.h
	date '+#define __COMPILE_HOST__ "${HOST}"' >> timestamp.h
	echo "#endif" >> timestamp.h
	mv -f timestamp.h $(SRC_ROOT)/include/timestamp.h


valgrind:
	echo "valgrind check meamory issues"
	cd ../binsimu; valgrind --logfile="valgrind" --check_leak=yes --num_callers=16 ./robotMenu

# DO NOT DELETE
