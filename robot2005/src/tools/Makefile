SRC_ROOT=..

## ===========================================================================
## LIB_NAME and .o files for making libraries .a and .so
## ===========================================================================
## For standard directory you only need to set these values
## ===========================================================================
LIB_NAME=libtools
OBJS=robotCommon.o geometry2D.o mthread.o robotFThread.o robotString.o \
     robotTimer.o shm.o network.o serialPort.o image.o

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
	${RM} -f  ${VALIDATE_DIR}/geometry2D ${VALIDATE_DIR}/serialPort
	${RM} -f  ${STATIC_LIB_DIR}/${LIB_NAME}.a        \
                  ${DYNAMIC_LIB_DIR}/${LIB_NAME}.so  

## ===========================================================================
## Main rules : ----- LIB -----
## ===========================================================================
lib: $(STATIC_LIB_DIR)/${LIB_NAME}.a ${DYNAMIC_LIB_DIR}/${LIB_NAME}.so 

## ===========================================================================
## Main rules : ----- BIN -----
## ===========================================================================
bin: 

## ===========================================================================
## Main rules : ----- VALIDATE -----
## ===========================================================================
validate: ${VALIDATE_DIR}/geometry2D ${VALIDATE_DIR}/serialPort
## ===========================================================================
## Specific rules
## ===========================================================================

${VALIDATE_DIR}/geometry2D: geometry2D.cpp geometry2D.o ${SRC_DIR}/include/geometry2D.h
	${CC} -o $@  geometry2D.cpp ${DEFAULT_CFLAGS} ${DEFAULT_INC_DIR} -DTEST_MAIN -lm -lstdc++-3-libc6.2-2-2.10.0

${VALIDATE_DIR}/serialPort: serialPort.cpp serialPort.o robotCommon.o ${SRC_DIR}/include/serialPort.h
	${CC} -o $@ ${DEFAULT_CFLAGS} serialPort.cpp robotCommon.o ${DEFAULT_INC_DIR} ${STATIC_LIB_DIR}/liblogBasic.a -DSERIAL_MAIN -lm -lstdc++-3-libc6.2-2-2.10.0

## ===========================================================================
## DO NOT ADD ANYTHING BELOW THIS LINE -- IT WILL BE ERASED BY MAKE DEPEND
## ===========================================================================

# DO NOT DELETE THIS LINE -- make depend depends on it.
