SRC_ROOT=..

# when makke is run from robot2004/src, pwd = robot2004/src even in robot2004/src/viewer directory....
TEXTURE_DEFAULT_DIR=../texture
TEXTURE_DEFAULT_DIR_1=${PWD}/../texture
TEXTURE_DEFAULT_DIR_2=${PWD}/${SRC_ROOT}/../texture
## ===========================================================================
## LIB_NAME and .o files for making libraries .a and .so
## ===========================================================================
## For standard directory you only need to set these values
## ===========================================================================
LIB_NAME=
OBJS=

LIB_NAME=libviewer3D
OBJS=viewer3D.o viewerMap3D.o viewerMap2D.o viewerPlayer.o text.o texture.o button.o dummyXmuColor.o

VALIDATE_NAME=
VALIDATE_OBJS=
VALIDATE_LIBS=
VALIDATE_LIBS_LOCATION=
VALIDATE_FLAGS=

## ===========================================================================
## Source files used to generate dependencies
## ===========================================================================
SRCS=*.cpp *.c

## ===========================================================================
## Additionnal flags for this directory. See makefile.include for common flags
## ===========================================================================
CFLAGS=-DTEXTURE_DEFAULT_DIR=\"${TEXTURE_DEFAULT_DIR}\" -DTEXTURE_DEFAULT_DIR_1=\"${TEXTURE_DEFAULT_DIR_1}\" -DTEXTURE_DEFAULT_DIR_2=\"${TEXTURE_DEFAULT_DIR_2}\"
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
bin: OpenGlMain.o dummyXmuColor.o $(STATIC_LIB_DIR)/${LIB_NAME}.a
	${CC} -o $(BIN_DIR)/ViewerTest ${DEFAULT_INC_DIR} ${DEFAULT_CFLAGS} OpenGlMain.o dummyXmuColor.o $(STATIC_LIB_DIR)/${LIB_NAME}.a $(STATIC_LIB_DIR)/libtools.a $(STATIC_LIB_DIR)/liblogBasic.a -lglut -lGLU -lGL -lm -lpthread -l$(LIB_STDCPP) -lz

## ===========================================================================
## Main rules : ----- VALIDATE -----
## ===========================================================================
validate:  

## ===========================================================================
## Specific rules
## ===========================================================================

dummyXmuColor.o: dummy.c
	${CC} -o $@ -c dummy.c


## ===========================================================================
## DO NOT ADD ANYTHING BELOW THIS LINE -- IT WILL BE ERASED BY MAKE DEPEND
## ===========================================================================

# DO NOT DELETE THIS LINE -- make depend depends on it.
