ifndef "$(BASE_DIR)"
   BASE_DIR = ../..
endif

include ../config

#############################################################
LIBS		= -lfthread -lpthread
#LIBS		= -lposix4 -lfthread -lpthread # for solaris

#############################################################
CFLAGS		= -Wall -O3 -D_REENTRANT

############ Basic libraries ################################
LIB_NAME	= fthread
LIBFT         	= lib$(LIB_NAME).a

############ C code to executable code ######################
.c.o :
	$(C_COMPILER) $(CFLAGS) -c $<

############ The h files ####################################
HFILES	      = fthread.h fthread_internal.h 	\
                event.h thread.h scheduler.h	\
                broadcast_order.h order.h	\
                trace.h verify.h itemlist.h	\
                mymalloc.h

############ The C files #####################################
CFILES	      = event.c thread.c scheduler.c 	\
                broadcast_order.c order.c	\
                automaton.c instruction.c 	\
                itemlist.c error.c		\
                mymalloc.c

############ The object files ################################
OBJS	      = event.o	thread.o scheduler.o 	\
                broadcast_order.o order.o 	\
                automaton.o instruction.o 	\
                itemlist.o error.o		\
                mymalloc.o

############ Make entries ####################################
all: lib

lib: $(HFILE) $(OBJS)
	$(AR) r $(LIBFT) $(OBJS)
	- $(RANLIB) $(LIBFT)

install:
	- cp $(LIBFT) $(LIB_DIR)
	- cp fthread.h $(INCL_DIR)

uninstall: 
	rm -f $(LIB_DIR)/$(LIBFT)
	rm -f $(INCL_DIR)/fthread.h

clean:
	- rm -f $(LIBFT)
	- rm -rf *.o *~ ./a.out

############ Auxiliary #######################################
TEST		= test.c

lines:
	wc -l $(CFILES) $(HFILES)

test: all
	$(C_COMPILER) $(CFLAGS) -I$(INCL_DIR) -L$(LIB_DIR) \
                   $(TEST) $(LIBS)
	./a.out

############ Dependencies ####################################
$(OBJS): $(HFILE)

############ end of makefile #################################

