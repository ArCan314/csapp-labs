#
# Student makefile for Cache Lab
# Note: requires a 64-bit x86-64 system 
#
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -m64# -DDEBUG
OPTFLAG = -O0

all: csim test-trans tracegen
	# Generate a handin tar file each time you compile
	-tar -cvf ${USER}-handin.tar  csim.c trans.c 

csim: csim.c cachelab.c cachelab.h cache.o
	$(CC) $(CFLAGS) $(OPTFLAG) -o csim csim.c cachelab.c cache.o -lm 

test-trans: test-trans.c trans.o cachelab.c cachelab.h cache.c
	$(CC) $(CFLAGS) -o test-trans test-trans.c cachelab.c cache.o trans.o 

tracegen: tracegen.c trans.o cachelab.c cache.o
	$(CC) $(CFLAGS) -o tracegen tracegen.c trans.o cachelab.c cache.o

trans.o: trans.c
	$(CC) $(CFLAGS) $(OPTFLAG) -c trans.c

cache.o: cache.c
	$(CC) $(CFLAGS) $(OPTFLAG) -c cache.c

#
# Clean the src dirctory
#
clean:
	rm -rf *.o
	rm -f *.tar
	rm -f csim
	rm -f test-trans tracegen
	rm -f trace.all trace.f*
	rm -f .csim_results .marker
