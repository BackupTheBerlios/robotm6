SRC_ROOT=../..

## ===========================================================================
## Source files used to generate dependencies
## ===========================================================================
SRCS=motor.cpp lcd.cpp odometer.cpp sound.cpp

## ===========================================================================
## LIB_NAME and .o files for making libraries .a and .so
## ===========================================================================
## For standard directory you only need to set these values
## ===========================================================================
LIB_NAME=libbasedevices
OBJS=$(SRCS:.cpp=.o)

BIN_NAME=
BIN_OBJS=
BIN_LIBS=${IO_STATIC_LIB}
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
validate:

## ===========================================================================
## Specific rules
## ===========================================================================

## ===========================================================================
## DO NOT ADD ANYTHING BELOW THIS LINE -- IT WILL BE ERASED BY MAKE DEPEND
## ===========================================================================

# DO NOT DELETE THIS LINE -- make depend depends on it.