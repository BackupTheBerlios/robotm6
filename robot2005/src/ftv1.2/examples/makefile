INCLDIR = ../include
LIBDIR  = ../lib

CC 	= gcc -Wall -O3 -D_REENTRANT -I $(INCLDIR)
LIBS	= -L $(LIBDIR) -lfthread -lpthread

############ C code to executable code #####################
.c :
	$(CC) $< $(LIBS)
	./a.out

####################
all:
	@echo "type: make xxx"

####################
clean:
	rm -f *~ ./a.out

####################
