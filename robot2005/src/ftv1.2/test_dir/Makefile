BASE_DIR=../..

include ../config

####################################################
CC	= $(C_COMPILER) -fno-strict-aliasing -O3 \
                        -D_REENTRANT

LIBS	= -lfthread -lpthread
#LIBS	= -posix4 -lfthread -lpthread # for solaris

####################################################
.c :
	@ echo "test of $<"
	@$(CC) -I$(INCL_DIR) -L$(LIB_DIR) $< $(LIBS)
	@extractresult.sh $< > EXPECTED
	- @./a.out | diff -w EXPECTED -

####################################################
all: \
aut01  aut04  aut07  aut10  aut13  aut16  aut19 \
aut02  aut05  aut08  aut11  aut14  aut17 \
aut03  aut06  aut09  aut12  aut15  aut18 \
test0   test08  test16  test24  test32  test40  test48 \
test01  test09  test17  test25  test33  test41  test49 \
test02  test10  test18  test26  test34  test42  test50 \
test03  test11  test19  test27  test35  test43  test51 \
test04  test12  test20  test28  test36  test44  test52 \
test05  test13  test21  test29  test37  test45  test53 \
test06  test14  test22  test30  test38  test46  test54 \
test07  test15  test23  test31  test39  test47  test55 \
select1  select2  select3  select4 \
creation1 double-sched \
error0   error03  error06  error09  error12  error15 \
error01  error04  error07  error10  error13  error16 \
error02  error05  error08  error11  error14


####################################################
clean:
	rm -f *~ a.out EXPECTED

####################################################