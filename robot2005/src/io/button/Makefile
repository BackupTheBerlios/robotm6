SRC_ROOT=../..

## ===========================================================================
## LIB_NAME and .o files for making libraries .a and .so
## ===========================================================================
## For standard directory you only need to set these values
## ===========================================================================
FILE=buttons
LIB_NAME=lib${FILE}
OBJS=${FILE}.o

BIN_NAME=
BIN_OBJS=
BIN_LIBS=
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
## Source files used to generate dependencies
## ===========================================================================
SRCS=buttons.cpp

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
ifdef LIB_NAME
	${RM} -f  ${STATIC_LIB_DIR}/${LIB_NAME}.a        \
                  ${DYNAMIC_LIB_DIR}/${LIB_NAME}.so  
endif    
ifdef LIB_NAME_REAL
	${RM} -f  $(STATIC_LIB_DIR)/${LIB_NAME_REAL}.a   \
                  ${DYNAMIC_LIB_DIR}/${LIB_NAME_REAL}.so 
endif
ifdef LIB_NAME_SIMU
	${RM} -f  $(STATIC_LIB_DIR)/${LIB_NAME_SIMU}.a   \
                  ${DYNAMIC_LIB_DIR}/${LIB_NAME_SIMU}.so 
endif
ifdef VALIDATE_NAME
	${RM} -f  ${VALIDATE_DIR}/${VALIDATE_NAME}  
endif
ifdef BIN_NAME
	${RM} -f  ${BIN_DIR}/${BIN_NAME}                 
endif
ifdef BIN_SIMU_OBJS
	${RM} -f  ${BIN_SIMU_DIR}/${BIN_NAME} 
endif

## ===========================================================================
## Main rules : ----- LIB -----
## ===========================================================================
lib:
ifdef LIB_NAME
lib: $(STATIC_LIB_DIR)/${LIB_NAME}.a ${DYNAMIC_LIB_DIR}/${LIB_NAME}.so 
endif
ifdef LIB_NAME_REAL
lib: $(STATIC_LIB_DIR)/${LIB_NAME_REAL}.a ${DYNAMIC_LIB_DIR}/${LIB_NAME_REAL}.so 
endif
ifdef LIB_NAME_SIMU
lib: $(STATIC_LIB_DIR)/${LIB_NAME_SIMU}.a ${DYNAMIC_LIB_DIR}/${LIB_NAME_SIMU}.so
endif


## ===========================================================================
## Main rules : ----- BIN -----
## ===========================================================================
bin:
ifdef BIN_NAME
bin: ${BIN_DIR}/${BIN_NAME}
endif
ifdef BIN_SIMU_OBJS
bin: ${BIN_SIMU_DIR}/${BIN_NAME}
endif

## ===========================================================================
## Main rules : ----- VALIDATE -----
## ===========================================================================
validate:
ifdef VALIDATE_NAME
validate: ${VALIDATE_DIR}/${VALIDATE_NAME}
endif



## ===========================================================================
## Specific rules
## ===========================================================================



## ===========================================================================
## DO NOT ADD ANYTHING BELOW THIS LINE -- IT WILL BE ERASED BY MAKE DEPEND
## ===========================================================================

# DO NOT DELETE THIS LINE -- make depend depends on it.
