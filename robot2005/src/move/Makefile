SRC_ROOT=..

## ===========================================================================
## LIB_NAME and .o files for making libraries .a and .so
## ===========================================================================
## For standard directory you only need to set these values
## ===========================================================================
LIB_NAME=libmove
OBJS=move.o movement.o robotPosition.o movementManager.o

LIB_NAME_SIMU=${LIB_NAME}Simu
OBJS_SIMU=moveSimu.o movement.o robotPosition.o movementManagerSimu.o

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
                  ${DYNAMIC_LIB_DIR}/${LIB_NAME}.so  
	${RM} -f  $(STATIC_LIB_DIR)/${LIB_NAME_REAL}.a   \
	${RM} -f  $(STATIC_LIB_DIR)/${LIB_NAME_SIMU}.a   \
                  ${DYNAMIC_LIB_DIR}/${LIB_NAME_SIMU}.so 

## ===========================================================================
## Main rules : ----- LIB -----
## ===========================================================================
lib: $(STATIC_LIB_DIR)/${LIB_NAME}.a ${DYNAMIC_LIB_DIR}/${LIB_NAME}.so 
lib: $(STATIC_LIB_DIR)/${LIB_NAME_SIMU}.a ${DYNAMIC_LIB_DIR}/${LIB_NAME_SIMU}.so



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
moveSimu.o: move.cpp move.o ${SRC_DIR}/include/move.h movement.h
	${CC} -o $@ -c move.cpp ${DEFAULT_CFLAGS} ${DEFAULT_INC_DIR} -DSIMULATED
movementManagerSimu.o: movementManager.cpp movementManager.o ${SRC_DIR}/include/move.h ${SRC_DIR}/include/robotPosition.h
	${CC} -o $@ -c movementManager.cpp ${DEFAULT_CFLAGS} ${DEFAULT_INC_DIR} -DSIMULATED


## ===========================================================================
## DO NOT ADD ANYTHING BELOW THIS LINE -- IT WILL BE ERASED BY MAKE DEPEND
## ===========================================================================

# DO NOT DELETE THIS LINE -- make depend depends on it.
