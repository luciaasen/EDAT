CC = gcc
CFLAGS = -Wall -Wextra -pedantic -ansi
LDLIBS = -lodbc

EXE = odbc-test odbc-example1 odbc-example2 odbc-example3 odbc-example4 usuario oferta compra best_seller

all : $(EXE)

clean :
	rm -f *.o core $(EXE)

$(EXE) : % : %.o odbc.o
