CFLAGS=-m32 -g

all: raise

raise: raise.c elfparser.o
	$(CC) $(CFLAGS) -o ../bin/raise raise.c elfparser.o
	
elfparser.o: elfparser.c elfparser.h
	$(CC) $(CFLAGS) -c -o elfparser.o elfparser.c
	
clean:
	rm -rf ../bin/raise
