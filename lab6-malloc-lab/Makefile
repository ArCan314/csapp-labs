#
# Students' Makefile for the Malloc Lab
#
TEAM = bovik
VERSION = 1
HANDINDIR = /afs/cs.cmu.edu/academic/class/15213-f01/malloclab/handin

CC = gcc
# CFLAGS = -Wall -O0 -m32 -g -DDEBUG
# CFLAGS = -Wall -O0 -m32 -g
CFLAGS = -Wall -O3 -m32 -DNDEBUG -flto 
# CFLAGS = -Wall -O2 -g -pg -m32 -DNDEBUG

SHAREOBJS = mdriver.o memlib.o fsecs.o fcyc.o clock.o ftimer.o

OBJSNAIVE = mm-naive.o
OBJSFF1 = mm-first-fit.V1.o
OBJSFF1_1 = mm-first-fit.V1-1.o
OBJSFF1_2 = mm-first-fit.V1-2.o
OBJSNF = mm-next-fit.o
OBJSEFL = mm-explicit-free-list.o

mdriver: $(OBJSFF1) $(OBJSFF1_1) $(OBJSFF1_2) $(OBJSNAIVE) $(SHAREOBJS) $(OBJSNF) $(OBJSEFL)
	$(CC) $(CFLAGS) -o mdriver-firstfit1 $(SHAREOBJS) $(OBJSFF1) 
	$(CC) $(CFLAGS) -o mdriver-firstfit1.1 $(SHAREOBJS) $(OBJSFF1_1) 
	$(CC) $(CFLAGS) -o mdriver-firstfit1.2 $(SHAREOBJS) $(OBJSFF1_2) 
	$(CC) $(CFLAGS) -o mdriver-naive $(SHAREOBJS) $(OBJSNAIVE)
	$(CC) $(CFLAGS) -o mdriver-nextfit $(SHAREOBJS) $(OBJSNF)
	$(CC) $(CFLAGS) -o mdriver-efl $(SHAREOBJS) $(OBJSEFL)

mdriver.o: mdriver.c fsecs.h fcyc.h clock.h memlib.h config.h mm.h
memlib.o: memlib.c memlib.h
mm-naive.o: mm-first-fit.V1-2.c mm.h memlib.h
mm-first-fit.V1.o: mm-first-fit.V1.c mm.h memlib.h
mm-first-fit.V1-1.o: mm-first-fit.V1-1.c mm.h memlib.h
mm-first-fit.V1-2.o: mm-first-fit.V1-2.c mm.h memlib.h
mm-next-fit.o: mm-next-fit.c mm.h memlib.h
mm-explicit-free-list.o: mm-explicit-free-list.c mm.h memlib.h
fsecs.o: fsecs.c fsecs.h config.h
fcyc.o: fcyc.c fcyc.h
ftimer.o: ftimer.c ftimer.h config.h
clock.o: clock.c clock.h

handin:
	cp mm.c $(HANDINDIR)/$(TEAM)-$(VERSION)-mm.c

clean:
	rm -f *~ *.o mdriver-firstfit1 mdriver-firstfit1.1 mdriver-firstfit1.2 mdriver-naive mdriver-nextfit mdriver-elf


