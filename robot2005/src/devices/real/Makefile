SRC_ROOT=../..

## ===========================================================================
## Source files used to generate dependencies
## ===========================================================================
ODOMETER_SRCS=odometer_04.cpp odometer_05.cpp
MOTOR_SRCS=HCTLControl.cpp hct.cpp motorIsa_05.cpp motorOdom_05.cpp motor_05.cpp
LCD_SRCS=lcd_03.cpp lcd_05.cpp
SOUND_SRCS=sound_05.cpp
SERVO_SRCS=servo_05.cpp
BUMPER_SRCS=bumper_05.cpp
ENV_SERCS=env_05.cpp
ALIM_SRCS=alim_05.cpp
SRCS=$(ODOMETER_SRCS) $(MOTOR_SRCS) $(LCD_SRCS) $(SOUND_SRCS) $(SERVO_SRCS) $(BUMPER_SRCS) $(ENV_SERCS) $(ALIM_SRCS) 

## ===========================================================================
## LIB_NAME and .o files for making libraries .a and .so
## ===========================================================================
## For standard directory you only need to set these values
## ===========================================================================
LIB_NAME=librealdevices
OBJS=$(SRCS:.cpp=.o)

BINS=
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
                  ${DYNAMIC_LIB_DIR}/${LIB_NAME}.so      \
		   ${BIN_DIR}/testBumper      \
		   ${BIN_DIR}/testEnv         \
                   ${BIN_DIR}/telecommandIsa  \
                   ${BIN_DIR}/telecommand05
#	${RM} -f  ${BIN_DIR}/${BIN_NAME}                 


## ===========================================================================
## Main rules : ----- LIB -----
## ===========================================================================
lib: $(STATIC_LIB_DIR)/${LIB_NAME}.a ${DYNAMIC_LIB_DIR}/${LIB_NAME}.so 

## ===========================================================================
## Main rules : ----- BIN -----
## ===========================================================================
#bin: ${BIN_DIR}/${BIN_NAME}
bin: ${BIN_DIR}/testBumper ${BIN_DIR}/testEnv ${BIN_DIR}/telecommandIsa ${BIN_DIR}/telecommand05

## ===========================================================================
## Main rules : ----- VALIDATE -----
## ===========================================================================
validate:

## ===========================================================================
## Specific rules
## ===========================================================================
${BIN_DIR}/testBumper: bumper_05.cpp
	${CC} -o $@ $^ ../base/bumper.o $(DEFAULT_CFLAGS) ${INC_DIR} ${DEFAULT_INC_DIR} -DTEST_MAIN $(STATIC_LIB_DIR)/libevents.a $(IO_STATIC_LIB)

${BIN_DIR}/testEnv: env_05.cpp
	${CC} -o $@ $^ ../base/env.o $(DEFAULT_CFLAGS) ${INC_DIR} ${DEFAULT_INC_DIR} -DTEST_MAIN $(STATIC_LIB_DIR)/libevents.a $(IO_STATIC_LIB)

${BIN_DIR}/telecommandIsa: motorIsa_05.cpp HCTLControl.o hct.o
	${CC} -o $@ $^ ../base/motor.o $(DEFAULT_CFLAGS) ${INC_DIR} ${DEFAULT_INC_DIR} -DTEST_MAIN $(STATIC_LIB_DIR)/libevents.a $(IO_STATIC_LIB)

${BIN_DIR}/telecommand05: motorOdom_05.cpp
	${CC} -o $@ $^ $(DEFAULT_CFLAGS) ${INC_DIR} ${DEFAULT_INC_DIR} -DTEST_MAIN $(IO_STATIC_LIB)
## ===========================================================================
## DO NOT ADD ANYTHING BELOW THIS LINE -- IT WILL BE ERASED BY MAKE DEPEND
## ===========================================================================

# DO NOT DELETE THIS LINE -- make depend depends on it.
