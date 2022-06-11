CC	= g++

DIR	= ../SAPPOROBDD
INCL    = $(DIR)/include
LIB	= $(DIR)/lib/BDD64.a

OPT 	= -O2 -DYY_ALWAYS_INTERACTIVE -I$(INCL) -DB_64

OBJ	= avoid.o

AVD     = avoid.exe

all:	$(AVD)

$(AVD): $(OBJ) $(LIB)
	$(CC) $(OPT) $(OBJ) $(LIB) $(XLIB) -o $(AVD)

avoid.o: avoid.cc 
	$(CC) $(OPT) -c avoid.cc	

clean:
	rm -f *.o
	rm -f *.exe
