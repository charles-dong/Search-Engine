# Lines that start with "#" are comments
# Filename
# Description / Purpose
# Any specific warnings to build special files, should they apply

# Which compiler? This should be gcc
CC= gcc

# Any params you'll pass to gcc
# I have 2 sets of params - one for normal, one for a debugging version
CFLAGS=-Wall -pedantic -std=c99 -DPREFIX
DEBUG_FLAGS=$(CFLAGS) -ggdb -g3 -gdwarf-2 -DDEBUG 

# What are the relavent .c .h files
SOURCES=./crawler.c ./list.c 
TESTSOURCES=./test.c ./list.c

#library stuff
UTILDIR=../util/
UTILFLAG=-ltseutil
UTILLIB=$(UTILDIR)libtseutil.a
UTILC=$(UTILDIR)hash.c $(UTILDIR)html.c $(UTILDIR)file.c $(UTILDIR)dictionary.c
UTILH=$(UTILC:.c=.h)

# Here are the make commands
crawl:$(SOURCES) $(UTILDIR)header.h $(UTILLIB)
	$(CC) $(CFLAGS) -o crawl $(SOURCES) -L$(UTILDIR) $(UTILFLAG)

$(UTILLIB): $(UTILC) $(UTILH)
	cd $(UTILDIR); make;

crawlprint:$(SOURCES) $(UTILDIR)header.h $(UTILLIB)
	$(CC) $(CFLAGS) -DDEBUG -o crawl $(SOURCES) -L$(UTILDIR) $(UTILFLAG)

debug:$(SOURCES) $(UTILDIR)header.h $(UTILLIB)
	$(CC) $(DEBUG_FLAGS) -o debug $(SOURCES) -L$(UTILDIR) $(UTILFLAG)

test:$(TESTSOURCES) $(UTILDIR)header.h $(UTILLIB)
	$(CC) $(DEBUG_FLAGS) $(TESTSOURCES) -L$(UTILDIR) $(UTILFLAG)

#cleans data
d:
	rm -f ../data/*.html
	rm -f ../data/*
	rm -f logOutput 


#cleans everything but the data
leavedata:
	rm -f *.o
	rm -f *.gch
	rm -f *#
	rm -f *~
	rm -f *.swp
	rm -f a.out 
	rm -f debug 
	rm -f crawl 
	rm -rf a.out.dSYM
	rm -rf debug.dSYM
	rm -rf ../util/a.out.dSYM
	rm -f ../util/*.o 
	rm -f ../util/*.gch 
	rm -f ../util/*.swp
	rm -f ../util/a.out
	rm -f logOutput 

#cleans everything 
clean:
	rm -f *.o
	rm -f *.gch
	rm -f *#
	rm -f *~
	rm -f *.swp
	rm -f a.out 
	rm -f debug 
	rm -f crawl 
	rm -rf a.out.dSYM
	rm -rf debug.dSYM
	rm -rf ../util/a.out.dSYM
	rm -f ../util/*.o 
	rm -f ../util/*.gch 
	rm -f ../util/*.swp
	rm -f ../util/a.out
	rm -f logOutput 
	rm -f ../data/*.html
	rm -f ../data/*
	rm -rf ../data1
	rm -rf ../data0
	rm -rf ../data2
	rm -rf ../data3

cleanlogs:
	rm -f crawltestlog.*