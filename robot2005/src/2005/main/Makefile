SRC_ROOT=../..

## ===========================================================================
## LIB_NAME and .o files for making libraries .a and .so
## ===========================================================================
## For standard directory you only need to set these values
## ===========================================================================
FILE=

BIN_NAME=
BIN_OBJS=
BIN_LIBS=${ALL_DYNAMIC_LIB}
BIN_LIBS_LOCATION=-L${DYNAMIC_LIB_DIR}
BIN_FLAGS=
BIN_SIMU_OBJS=
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
	${RM} -f  ${BIN_DIR}/attack       
	${RM} -f  ${BIN_SIMU_DIR}/attack
	${RM} -f  ${BIN_DIR}/defence      
	${RM} -f  ${BIN_SIMU_DIR}/defence

## ===========================================================================
## Main rules : ----- LIB -----
## ===========================================================================
lib:


## ===========================================================================
## Main rules : ----- BIN -----
## ===========================================================================
bin: ${BIN_DIR}/attack
bin: ${BIN_SIMU_DIR}/attack
bin: ${BIN_DIR}/defence
bin: ${BIN_SIMU_DIR}/defence



## ===========================================================================
## Main rules : ----- VALIDATE -----
## ===========================================================================
validate: 



## ===========================================================================
## Specific rules
## ===========================================================================

mainAttackSimu.o: mainAttack.cpp
	${CC} -o $@ -c mainAttack.cpp ${DEFAULT_INC_DIR} -DSIMULATED=true

mainDefenceSimu.o: mainDefence.cpp
	${CC} -o $@ -c mainDefence.cpp ${DEFAULT_INC_DIR} -DSIMULATED=true

${BIN_DIR}/attack: mainAttack.o
	${CC} -o $@ $^ $(DEFAULT_CFLAGS) ${INC_DIR} ${DEFAULT_INC_DIR} ${BIN_LIBS_LOCATION} ${BIN_LIBS} ${BIN_FLAGS}

${BIN_DIR}/defence: mainDefence.o
	${CC} -o $@ $^ $(DEFAULT_CFLAGS) ${INC_DIR} ${DEFAULT_INC_DIR} ${BIN_LIBS_LOCATION} ${BIN_LIBS} ${BIN_FLAGS}

${BIN_SIMU_DIR}/attack: mainAttackSimu.o
	${CC} -o $@ $^ $(DEFAULT_CFLAGS) ${INC_DIR} ${DEFAULT_INC_DIR} ${BIN_LIBS_LOCATION} ${BIN_LIBS} ${BIN_FLAGS}

${BIN_SIMU_DIR}/defence: mainDefenceSimu.o
	${CC} -o $@ $^ $(DEFAULT_CFLAGS) ${INC_DIR} ${DEFAULT_INC_DIR} ${BIN_LIBS_LOCATION} ${BIN_LIBS} ${BIN_FLAGS}

## ===========================================================================
## DO NOT ADD ANYTHING BELOW THIS LINE -- IT WILL BE ERASED BY MAKE DEPEND
## ===========================================================================

# DO NOT DELETE THIS LINE -- make depend depends on it.
