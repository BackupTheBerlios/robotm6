SRC_ROOT=..

## ===========================================================================
## LIB_NAME and .o files for making libraries .a and .so
## ===========================================================================
## For standard directory you only need to set these values
## ===========================================================================
LIB_NAME=liblog
OBJS=log.o

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
	${RM} -f ${BIN_SIMU_DIR}/logger ${BIN_DIR}/logger ${BIN_SIMU_DIR}/log2html ${BIN_SIMU_DIR}/logViewer
	${RM} -f  ${STATIC_LIB_DIR}/${LIB_NAME}.a    \
                  ${DYNAMIC_LIB_DIR}/${LIB_NAME}.so  \
                  $(STATIC_LIB_DIR)/${LIB_NAME}Basic.a

## ===========================================================================
## Main rules : ----- LIB -----
## ===========================================================================
lib: $(STATIC_LIB_DIR)/${LIB_NAME}.a ${DYNAMIC_LIB_DIR}/${LIB_NAME}.so 
lib: $(STATIC_LIB_DIR)/${LIB_NAME}Basic.a

## ===========================================================================
## Main rules : ----- BIN -----
## ===========================================================================
bin: ${BIN_DIR}/logger ${BIN_SIMU_DIR}/log2html ${BIN_SIMU_DIR}/logViewer
	cp ${BIN_DIR}/logger ${BIN_SIMU_DIR}/logger
## ===========================================================================
## Main rules : ----- VALIDATE -----
## ===========================================================================
validate: 

## ===========================================================================
## Specific rules
## ===========================================================================

${BIN_DIR}/logger: logger.o ${SRC_DIR}/include/log.h
	${CC} -o $@ logger.o ${DEFAULT_INC_DIR} -I/usr/include ${DEFAULT_CFLAGS} $(STATIC_LIB_DIR)/libtools.a -lz -l$(LIB_STDCPP)

${BIN_SIMU_DIR}/log2html: log2html.o ${SRC_DIR}/include/log.h
	${CC} -o $@ log2html.o ${DEFAULT_INC_DIR} -I/usr/include ${DEFAULT_CFLAGS} $(STATIC_LIB_DIR)/libtools.a -lz -l$(LIB_STDCPP)

${BIN_SIMU_DIR}/logViewer: logViewer.o ${SRC_DIR}/include/log.h
	${CC} -o $@ logViewer.o ${DEFAULT_INC_DIR} -I/usr/include ${DEFAULT_CFLAGS} ${STATIC_LIB_DIR}/libsoundPlayer.a ${STATIC_LIB_DIR}/libviewer3D.a ${STATIC_LIB_DIR}/liblogBasic.a ${STATIC_LIB_DIR}/libtools.a ${STATIC_LIB_DIR}/libfthread.a -lglut ${STATIC_LIB_DIR}/libviewer3D.a -lGLU -lGL -lm -lpthread -l$(LIB_STDCPP) -lz


$(STATIC_LIB_DIR)/${LIB_NAME}Basic.a: logBasic.o
	${AR} ${DEFAULT_AR_FLAGS} $@ logBasic.o

logBasic.o:  log.cpp log.o ${SRC_DIR}/include/log.h
	${CC} -o $@ -c ${DEFAULT_CFLAGS} log.cpp ${DEFAULT_INC_DIR} -DLOG_BASIC

## ===========================================================================
## DO NOT ADD ANYTHING BELOW THIS LINE -- IT WILL BE ERASED BY MAKE DEPEND
## ===========================================================================

# DO NOT DELETE THIS LINE -- make depend depends on it.
