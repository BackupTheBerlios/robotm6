SRC_ROOT=..

## ===========================================================================
## LIB_NAME and .o files for making libraries .a and .so
## ===========================================================================
## For standard directory you only need to set these values
## ===========================================================================
LIB_NAME=libsimulator
OBJS=simulatorClient.o

BIN_NAME=simulator
OBJS_SERVER=simulator.o simulatorServer.o simulatorRobot.o simulatorSkittle.o simulatorGrsBall.o


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
	${RM} -f  ${STATIC_LIB_DIR}/${LIB_NAME}.a        \
                  ${DYNAMIC_LIB_DIR}/${LIB_NAME}.so      \
                  ${BIN_DIR}/${BIN_NAME} \
                  ${BIN_DIR}/testSimulatorClient

## ===========================================================================
## Main rules : ----- LIB -----
## ===========================================================================
lib: ${STATIC_LIB_DIR}/${LIB_NAME}.a ${DYNAMIC_LIB_DIR}/${LIB_NAME}.so

## ===========================================================================
## Main rules : ----- BIN -----
## ===========================================================================
bin: $(OBJS_SERVER)
	${CC} -o ${BIN_DIR}/${BIN_NAME} ${DEFAULT_INC_DIR} ${DEFAULT_CFLAGS} \
           ${OBJS_SERVER} ${STATIC_LIB_DIR}/libviewer3D.a ${STATIC_LIB_DIR}/liblogBasic.a ${STATIC_LIB_DIR}/libtools.a ${STATIC_LIB_DIR}/libfthread.a -lglut ${STATIC_LIB_DIR}/libviewer3D.a -lGLU -lGL -lm -lpthread -l$(LIB_STDCPP) -lz
	${CC} -o ${BIN_DIR}/testSimulatorClient ${DEFAULT_INC_DIR} ${DEFAULT_CFLAGS} \
           testClient.cpp ${STATIC_LIB_DIR}/libsimulator.a ${STATIC_LIB_DIR}/liblogBasic.a ${STATIC_LIB_DIR}/libtools.a ${STATIC_LIB_DIR}/libfthread.a -lm -lpthread -l$(LIB_STDCPP) -lz


## ===========================================================================
## Main rules : ----- VALIDATE -----
## ===========================================================================
validate:

## ===========================================================================
## Specific rules
## ===========================================================================

## ===========================================================================
## DO NOT ADD ANYTHING BELOW THIS LINE -- IT WILL BE ERASED BY MAKE DEPEND
## ===========================================================================

# DO NOT DELETE THIS LINE -- make depend depends on it.
