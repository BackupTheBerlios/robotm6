SRC_ROOT=../..

## ===========================================================================
## LIB_NAME and .o files for making libraries .a and .so
## ===========================================================================
## For standard directory you only need to set these values
## ===========================================================================
FILE=testIo

BIN_NAME=${FILE}
BIN_OBJS=${FILE}.o
BIN_LIBS=${ALL_DYNAMIC_LIB}
BIN_LIBS_LOCATION=-L${DYNAMIC_LIB_DIR}
BIN_FLAGS=
BIN_SIMU_OBJS=${FILE}Simu.o
BIN_SIMU_LIBS=${ALL_DYNAMIC_LIB} 
BIN_SIMU_LIBS_LOCATION=-L${DYNAMIC_LIB_DIR}
BIN_SIMU_FLAGS=

## ===========================================================================
## Source files used to generate dependencies
## ===========================================================================
SRCS=*.cpp

## ===========================================================================
## Additionnal flags for this directory. See makefile.include for common flags
## ===========================================================================
CFLAGS=
INC_DIR=
AR_FLAGS=
LD_FLAGS=

## ===========================================================================
## Include common makefile definitions
## ===========================================================================
include $(SRC_ROOT)/Makefile.include

## ===========================================================================
## Main rules : ----- ALL -----
## ===========================================================================
all: lib bin validate

## ===========================================================================
## Main rules : ----- CLEAN -----
## ===========================================================================
clean:	default_clean
	${RM} -f  ${BIN_DIR}/${BIN_NAME}                 
	${RM} -f  ${BIN_SIMU_DIR}/${BIN_NAME} 

## ===========================================================================
## Main rules : ----- LIB -----
## ===========================================================================
lib:


## ===========================================================================
## Main rules : ----- BIN -----
## ===========================================================================
bin: ${BIN_DIR}/${BIN_NAME}
bin: ${BIN_SIMU_DIR}/${BIN_NAME}


## ===========================================================================
## Main rules : ----- VALIDATE -----
## ===========================================================================
validate: 



## ===========================================================================
## Specific rules
## ===========================================================================

${FILE}Simu.o: ${FILE}.cpp
	${CC} -o $@ -c ${FILE}.cpp ${DEFAULT_INC_DIR} -DSIMULATED=true


## ===========================================================================
## DO NOT ADD ANYTHING BELOW THIS LINE -- IT WILL BE ERASED BY MAKE DEPEND
## ===========================================================================

# DO NOT DELETE THIS LINE -- make depend depends on it.