SRC_ROOT=../..

## ===========================================================================
## Source files used to generate dependencies
## ===========================================================================
ODOMETER_SRCS=odometer_04.cpp odometer_05.cpp
ifdef GUMSTIX
MOTOR_SRCS=motorOdom_05.cpp motor_05.cpp
else
MOTOR_SRCS=HCTLControl.cpp hct.cpp motorIsa_05.cpp motorOdom_05.cpp motor_05.cpp
endif
LCD_SRCS=lcd_03.cpp lcd_05.cpp
SOUND_SRCS=sound_05.cpp
SERVO_SRCS=servo_05.cpp
BUMPER_SRCS=bumper_05.cpp
ENV_SRCS=env_05.cpp
TESLA_SRCS=tesla_05.cpp
SKITTLE_DETECTOR_SRCS=skittleDetector_05.cpp
CRANE_SRCS=crane_05.cpp
ALIM_SRCS=alim_05.cpp
SRCS=$(ODOMETER_SRCS) $(MOTOR_SRCS) $(LCD_SRCS) $(SOUND_SRCS) $(SERVO_SRCS) $(BUMPER_SRCS) $(ENV_SRCS) $(ALIM_SRCS) $(TESLA_SRCS) $(SKITTLE_DETECTOR_SRCS) $(CRANE_SRCS)

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
		   ${BIN_DIR}/lcdPrint05      \
                   ${BIN_DIR}/getButton05     \
		   ${BIN_DIR}/testBumper      \
		   ${BIN_DIR}/testEnv         \
		   ${BIN_DIR}/testServo       \
		   ${BIN_DIR}/testLcd         \
                   ${BIN_DIR}/testCrane       \
		   ${BIN_DIR}/testSkittle     \
		   ${BIN_DIR}/testTesla       \
		   ${BIN_DIR}/testAlim        \
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
ifdef GUMSTIX
bin: ${BIN_DIR}/testServo ${BIN_DIR}/testBumper ${BIN_DIR}/testEnv ${BIN_DIR}/telecommand05 ${BIN_DIR}/lcdPrint05 ${BIN_DIR}/testLcd ${BIN_DIR}/getButton05 ${BIN_DIR}/testCrane ${BIN_DIR}/testTesla ${BIN_DIR}/testSkittle ${BIN_DIR}/testAlim
else
bin: ${BIN_DIR}/testServo ${BIN_DIR}/testBumper ${BIN_DIR}/testEnv ${BIN_DIR}/telecommandIsa ${BIN_DIR}/telecommand05 ${BIN_DIR}/lcdPrint05 ${BIN_DIR}/testLcd ${BIN_DIR}/getButton05 ${BIN_DIR}/testCrane ${BIN_DIR}/testTesla ${BIN_DIR}/testSkittle 
endif
## ===========================================================================
## Main rules : ----- VALIDATE -----
## ===========================================================================
validate:

## ===========================================================================
## Specific rules
## ===========================================================================
${BIN_DIR}/getButton05: bumper_05.cpp
	$(if ${QUIET},,echo " [$@]")
	${CC} -o $@ $^ ../base/bumper.o $(DEFAULT_CFLAGS) ${INC_DIR} ${DEFAULT_INC_DIR} -DGET_BUMPER_MAIN $(STATIC_LIB_DIR)/libevents.a $(IO_STATIC_LIB)

${BIN_DIR}/lcdPrint05: lcd_05.cpp
	$(if ${QUIET},,echo " [$@]")
	${CC} -o $@ $^ ../base/lcd.o $(DEFAULT_CFLAGS) ${INC_DIR} ${DEFAULT_INC_DIR} -DLCD_PRINT_MAIN $(IO_STATIC_LIB)

${BIN_DIR}/testLcd: lcd_05.cpp
	$(if ${QUIET},,echo " [$@]")
	${CC} -o $@ $^ ../base/lcd.o $(DEFAULT_CFLAGS) ${INC_DIR} ${DEFAULT_INC_DIR} -DTEST_MAIN $(IO_STATIC_LIB)

${BIN_DIR}/testServo: servo_05.cpp
	$(if ${QUIET},,echo " [$@]")
	${CC} -o $@ $^ ../base/servo.o $(DEFAULT_CFLAGS) ${INC_DIR} ${DEFAULT_INC_DIR} -DTEST_MAIN $(IO_STATIC_LIB)

${BIN_DIR}/testCrane: crane_05.cpp
	$(if ${QUIET},,echo " [$@]")
	${CC} -o $@ $^ ../base/crane.o $(DEFAULT_CFLAGS) ${INC_DIR} ${DEFAULT_INC_DIR} -DTEST_MAIN $(IO_STATIC_LIB)

${BIN_DIR}/testBumper: bumper_05.cpp
	$(if ${QUIET},,echo " [$@]")
	${CC} -o $@ $^ ../base/bumper.o $(DEFAULT_CFLAGS) ${INC_DIR} ${DEFAULT_INC_DIR} -DTEST_MAIN $(STATIC_LIB_DIR)/libevents.a $(IO_STATIC_LIB)

${BIN_DIR}/testAlim: alim_05.cpp
	$(if ${QUIET},,echo " [$@]")
	${CC} -o $@ $^ ../base/alim.o $(DEFAULT_CFLAGS) ${INC_DIR} ${DEFAULT_INC_DIR} -DTEST_MAIN $(STATIC_LIB_DIR)/libevents.a $(IO_STATIC_LIB)

${BIN_DIR}/testEnv: env_05.cpp
	$(if ${QUIET},,echo " [$@]")
	${CC} -o $@ $^ ../base/env.o $(DEFAULT_CFLAGS) ${INC_DIR} ${DEFAULT_INC_DIR} -DTEST_MAIN $(STATIC_LIB_DIR)/libevents.a $(IO_STATIC_LIB)

${BIN_DIR}/testTesla: tesla_05.cpp
	$(if ${QUIET},,echo " [$@]")
	${CC} -o $@ $^ ../base/tesla.o $(DEFAULT_CFLAGS) ${INC_DIR} ${DEFAULT_INC_DIR} -DTEST_MAIN $(STATIC_LIB_DIR)/libevents.a $(IO_STATIC_LIB)

${BIN_DIR}/testSkittle: skittleDetector_05.cpp
	$(if ${QUIET},,echo " [$@]")
	${CC} -o $@ $^ ../base/skittleDetector.o $(DEFAULT_CFLAGS) ${INC_DIR} ${DEFAULT_INC_DIR} -DTEST_MAIN $(STATIC_LIB_DIR)/libevents.a $(IO_STATIC_LIB)

${BIN_DIR}/telecommandIsa: motorIsa_05.cpp HCTLControl.o hct.o
	$(if ${QUIET},,echo " [$@]")
	${CC} -o $@ $^ ../base/motorTestMain.o $(DEFAULT_CFLAGS) ${INC_DIR} ${DEFAULT_INC_DIR} -DTEST_MAIN $(STATIC_LIB_DIR)/libevents.a $(IO_STATIC_LIB)

${BIN_DIR}/telecommand05: motorOdom_05.cpp
	$(if ${QUIET},,echo " [$@]")
	${CC} -o $@ $^ $(DEFAULT_CFLAGS) ${INC_DIR} ${DEFAULT_INC_DIR} -DTEST_MAIN $(IO_STATIC_LIB)
## ===========================================================================
## DO NOT ADD ANYTHING BELOW THIS LINE -- IT WILL BE ERASED BY MAKE DEPEND
## ===========================================================================

# DO NOT DELETE THIS LINE -- make depend depends on it.
