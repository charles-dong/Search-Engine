
# Which compiler? 
CC= gcc

# params for gcc
CFLAGS=-Wall -pg -std=c99

#files
CFILES=./dictionary.c ./file.c ./hash.c ./html.c


libtseutil.a:	
	make cleanlib
	$(CC) $(CFLAGS) -c $(CFILES)
	ar -cvr libtseutil.a *.o
	make clean

clean:
	rm -f *.o

cleanlib:
	rm -f libtseutil.a
