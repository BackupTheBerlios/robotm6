# Generated automatically from Makefile.in by configure.
# Makefile  - amp  audio mpeg player v0.7
# tomislav uzelac  1996,1997


#DEBUG = -DDEBUG

# ---- Linux/i686 ----

CC = gcc-3.4
DEFINES = -DNO_BYTE_SWAPPING -DOS_Linux -DARCH_i686 
CFLAGS = -O6 -ffast-math -fomit-frame-pointer  ${DEFINES} ${DEBUG} 
LDFLAGS = 

#
# uncomment both of these if you want profiling support
#
#PG = -pg
#LDFLAGS+= -pg

#
# you should probably leave these untouched ####################################
#

OBJ = dump.o formats.o getbits.o huffman.o layer3.o getdata.o misc2.o transform.o audio.o \
      args.o buffer.o audioIO.o position.o layer2.o rtbuf.o guicontrol.o util.o getopt.o getopt1.o

LIBS = -lm -lpthread

all: amp ../../../binsimu/soundPlayer

../../../binsimu/soundPlayer: amp
	cp amp ../../../binsimu/soundPlayer

.PHONY: clean cleandist dep 
clean:
	rm -f *.o *~ amp core ../../../binsimu/soundPlayer

cleandist: clean
	rm -f *~ config.status config.cache config.log config.h Makefile .depend

distclean: cleandist

amp: $(OBJ) 
	$(CC) -o amp $(OBJ) $(LDFLAGS) $(LIBS) 

.c.o:
	$(CC) -c $(CFLAGS) $(GGDB) $(PG) $< 

.c.s:
	$(CC) -S $(CFLAGS) $(GGDB) $(PG) $< 

guicontrol.o: ../../include/soundList.h

depend:
	$(CC) -MM ${DEFINES} ${DEBUG} *.c > .depend

-include .depend
