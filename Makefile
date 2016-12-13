# for C++ define  CC = g++
CC = g++
# -g    adds debugging information to the executable file
# -O    turns on optimization 
# -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -O -Wall

# typing 'make' will invoke the first target entry in the file 
# (in this case the default target entry)
all: tinymesh

# To create the executable file tinymesh we need the object files
# main.o, mesh.o, and cell.o:
tinymesh:  main.o  
	$(CC) $(CFLAGS) -o tinymesh main.o 

# create the object file mesh.o, we need the source
# files main.cpp, scanner.h, and counter.h:
main.o:  main.cpp  
	$(CC) $(CFLAGS) -c main.cpp vector.hpp

# To start over from scratch, type 'make clean'.  This
# removes the executable file, as well as old .o object
# files and *~ backup files:
clean: 
	$(RM) count *.o *~


