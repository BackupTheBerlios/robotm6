SRC_ROOT=../..

## ===========================================================================
## LIB_NAME and .o files for making libraries .a and .so
## ===========================================================================
## For standard directory you only need to set these values
## ===========================================================================
LIB_NAME=libmotor
OBJS=motor.o motorReal.o motorSimu.o HCTLControl.o hct.o

LIB_NAME_SIMU=
OBJS_SIMU=


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
	$(RM) -f ${BIN_DIR}/telecommand2 \
                 ${BIN_DIR}/testMotor    \
                 ${BIN_DIR}/telecommand  \
                 ${BIN_DIR}/testHctl
	${RM} -f  ${STATIC_LIB_DIR}/${LIB_NAME}.a        \
                  ${DYNAMIC_LIB_DIR}/${LIB_NAME}.so  

## ===========================================================================
## Main rules : ----- LIB -----
## ===========================================================================
lib: $(STATIC_LIB_DIR)/${LIB_NAME}.a ${DYNAMIC_LIB_DIR}/${LIB_NAME}.so 

## ===========================================================================
## Main rules : ----- BIN -----
## ===========================================================================
bin: ${BIN_DIR}/telecommand2 ${BIN_DIR}/testMotor ${BIN_DIR}/telecommand 
bin: ${BIN_DIR}/testHctl

## ===========================================================================
## Main rules : ----- VALIDATE -----
## ===========================================================================
validate:


## ===========================================================================
## Specific rules
## ===========================================================================

telecommand1.o: motorReal.cpp motorReal.o HCTLControl.h
	${CC} -o telecommand1.o -c motorReal.cpp ${DEFAULT_CFLAGS} ${DEFAULT_INC_DIR} -DTELECOMMAND_MAIN

telecommand2.o: motor.cpp motor.o 
	${CC} -o telecommand2.o -c motor.cpp ${DEFAULT_CFLAGS}  ${DEFAULT_INC_DIR} -DTELECOMMAND_MAIN

${BIN_DIR}/telecommand2: telecommand1.o telecommand2.o HCTLControl.o hct.o
	${CC} -o $@ ${DEFAULT_CFLAGS} telecommand1.o telecommand2.o HCTLControl.o hct.o ${SRC_ROOT}/../lib/liblog.a ${SRC_ROOT}/../lib/libtools.a -lm -lz -l$(LIB_STDCPP) -lpthread

${BIN_DIR}/telecommand: telecommand.o HCTLControl.o hct.o
	${CC} -o $@ $^ ${DEFAULT_CFLAGS} -l$(LIB_STDCPP)

${BIN_DIR}/testMotor: testMotor.o HCTLControl.o hct.o 
	${CC} -o  $@ $^ ${DEFAULT_CFLAGS} ${DEFAULT_INC_DIR} -l$(LIB_STDCPP)

${BIN_DIR}/testHctl: hct.cpp 
	${CC} -o  $@ hct.cpp -DTEST_MAIN ${DEFAULT_CFLAGS} ${DEFAULT_INC_DIR} -l$(LIB_STDCPP)

## ===========================================================================
## DO NOT ADD ANYTHING BELOW THIS LINE -- IT WILL BE ERASED BY MAKE DEPEND
## ===========================================================================

# DO NOT DELETE THIS LINE -- make depend depends on it.
