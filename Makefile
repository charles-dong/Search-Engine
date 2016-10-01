# Which compiler? 
CC= gcc

# params for gcc
CFLAGS=-Wall -pg -pedantic -std=c99
CFLAGS1=-Wall
DEBUG_FLAGS=$(CFLAGS) -ggdb -g3 -gdwarf-2 -DDEBUG 

# relevant .c .h files
OBJS3=./queryEngine.o
SOURCES3=./queryEngine/queryEngine.c

OBJSTEST=./testQuery.o
SOURCESTEST=./queryEngine/testQuery.c

SOURCES=./util/hash.c ./util/html.c ./util/file.c ./util/dictionary.c

#library stuff
UTILDIR=./util/
UTILFLAG=-ltseutil
UTILLIB=$(UTILDIR)libtseutil.a
UTILC=$(UTILDIR)hash.c $(UTILDIR)html.c $(UTILDIR)file.c $(UTILDIR)dictionary.c
UTILH=$(UTILC:.c=.h)

#make files
query:
	cd $(UTILDIR); make; cd ..;
	$(CC) $(CFLAGS) -o query $(SOURCES3) -L$(UTILDIR) $(UTILFLAG)

query2:
	cd $(UTILDIR); make; cd ..;
	$(CC) $(CFLAGS) -ggdb -o query $(SOURCES3) -L$(UTILDIR) $(UTILFLAG)

index:
	cd indexer; make index; ./index ../data3/ index.dat; make leavedat;

test:
	cd $(UTILDIR); make; cd ..;
	$(CC) $(DEBUG_FLAGS) $(SOURCESTEST) -L$(UTILDIR) $(UTILFLAG)

debug:
	cd $(UTILDIR); make; cd ../indexer/;
	$(CC) $(DEBUG_FLAGS) $(SOURCES3) $(SOURCES)


#cleans everything 
clean:
	rm -f *.o
	rm -f *.gch
	rm -f *.out 
	rm -f debug
	rm -f index
	rm -f query
	rm -f *.dat
	rm -f logOutput
	rm -rf index.dSYM
	rm -rf a.out.dSYM
	rm -f ../util/*.gch
	rm -f ../util/*.o
	rm -f test
	rm -rf query.dSYM

cleanlogs:
	rm -f querytestlog.*


cleanall:
	cd crawler; make clean; cd ..;
	cd indexer; make clean; cd ..;
	cd util; make clean; make cleanlib; cd ..;
	make clean;

leavedata:
	cd crawler; make leavedata; cd ..;
	cd indexer; make leavedat; cd ..;
	cd util; make clean; make cleanlib; cd ..;
	make clean;
