SRC_ROOT=../..

## ===========================================================================
## LIB_NAME and .o files for making libraries .a and .so
## ===========================================================================
## For standard directory you only need to set these values
## ===========================================================================
LIB_NAME=libuart
OBJS=uart.o uartBuffer.o uartManager.o ioManager.o

BIN_NAME=uartScan
BIN_OBJS=uartScannerMain.o uart.o uartBuffer.o ioManager.o
BIN_LIBS=${SRC_ROOT}/../lib/libtools.a ${SRC_ROOT}/../lib/liblog.a -lm -lz -l$(LIB_STDCPP) -lpthread
BIN_LIBS_LOCATION=
BIN_FLAGS=
BIN_SIMU_OBJS=
BIN_SIMU_LIBS=
BIN_SIMU_LIBS_LOCATION=$(BIN_LIBS_LOCATION)
BIN_SIMU_FLAGS=-DSIMULATED

VALIDATE_NAME=
VALIDATE_OBJS=
VALIDATE_LIBS=
VALIDATE_LIBS_LOCATION=
VALIDATE_FLAGS=

## ===========================================================================
## Source files used to generate dependencies
## ===========================================================================
SRCS=uart.cpp uartBuffer.cpp uartManager.cpp

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
	${RM} -f  ${STATIC_LIB_DIR}/${LIB_NAME}.a        \
                  ${DYNAMIC_LIB_DIR}/${LIB_NAME}.so  
	${RM} -f  ${BIN_DIR}/${BIN_NAME}                 


## ===========================================================================
## Main rules : ----- LIB -----
## ===========================================================================
lib: $(STATIC_LIB_DIR)/${LIB_NAME}.a ${DYNAMIC_LIB_DIR}/${LIB_NAME}.so 

## ===========================================================================
## Main rules : ----- BIN -----
## ===========================================================================
bin: ${BIN_DIR}/${BIN_NAME}

## ===========================================================================
## Main rules : ----- VALIDATE -----
## ===========================================================================
validate:

## ===========================================================================
## Specific rules
## ===========================================================================

uartScannerMain.o: uartManager.cpp ${SRC_DIR}/include/uart.h
	${CC} -o $@ -c uartManager.cpp ${DEFAULT_CFLAGS} -DUART_SCANNER_MAIN  ${DEFAULT_INC_DIR} 

## ===========================================================================
## DO NOT ADD ANYTHING BELOW THIS LINE -- IT WILL BE ERASED BY MAKE DEPEND
## ===========================================================================

# DO NOT DELETE THIS LINE -- make depend depends on it.
