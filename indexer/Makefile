# Which compiler? 
CC= gcc

# params for gcc
EXEC=index
CFLAGS=-Wall -pg -pedantic -std=c99
CFLAGS1=-Wall
DEBUG_FLAGS=$(CFLAGS) -ggdb -g3 -gdwarf-2 -DDEBUG 

# relevant .c .h files
OBJS =./file.o ./html.o ./hash.o ./dictionary.o
SOURCES=../util/dictionary.c ../util/file.c ../util/hash.c ../util/html.c

OBJS2=./buildIndex.o
SOURCES2=./buildIndex.c

#library stuff
UTILDIR=../util/
UTILFLAG=-ltseutil
UTILLIB=$(UTILDIR)libtseutil.a
UTILC=$(UTILDIR)hash.c $(UTILDIR)html.c $(UTILDIR)file.c $(UTILDIR)dictionary.c
UTILH=$(UTILC:.c=.h)

#make files
index:
	cd $(UTILDIR); make; cd ../indexer/;
	$(CC) $(CFLAGS1) -c $(SOURCES2) 
	$(CC) $(CFLAGS) -o index $(OBJS2) -L$(UTILDIR) $(UTILFLAG)

debug:
	cd $(UTILDIR); make; cd ../indexer/;
	$(CC) $(CFLAGS1) -c $(SOURCES2) 
	$(CC) $(DEBUG_FLAGS) -o debug $(OBJS2) -L$(UTILDIR) $(UTILFLAG)


#cleans everything 
clean:
	rm -f *.o
	rm -f *.gch
	rm -f *.out 
	rm -f debug
	rm -f index
	rm -f *.dat
	rm -f logOutput
	rm -rf index.dSYM
	rm -f ../util/*.gch
	rm -f ../util/*.o

leavedat:
	rm -f *.o
	rm -f *.gch
	rm -f *.out 
	rm -f debug
	rm -f index
	rm -f logOutput
	rm -rf index.dSYM
	rm -f ../util/*.gch
	rm -f ../util/*.o

cleanlogs:
	rm -f indextestlog.*
