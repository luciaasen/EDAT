all: testtr

testtr:	testtr.o table.o type.o index.o record.o
	gcc -o testtr.o table.o record.o type.o index.o
	
record.o: record.c record.h type.h
	gcc -c -std=c99 record.c

table.o: table.c table.h record.h type.h index.h
	gcc -c -std=c99 table.c

testtr.o: testtr.c table.h record.h type.h
	gcc -c -std=c99 testtr.c

type.o: type.c type.h
	gcc -c -std=c99 type.c

index.o: index.c index.h
	gcc -c -std=c99 index.c
	
clean:
	rm *.o
