# Makefile for Writing Make Files Example

# *****************************************************
# Variables to control Makefile operation

CC = gcc
CFLAGS = -Wall -g -Wextra
LDFLAGS=-lpthread
RM=/bin/rm

# ****************************************************
# Targets needed to bring the executable up to date

main: main.o vm.o cpu.o mmu.o io.o ram.o isa.o debug.o
	$(CC) $(CFLAGS) -o dumbsel main.o vm.o cpu.o mmu.o io.o ram.o isa.o debug.o $(LDFLAGS)

# The main.o target can be written more simply


#main.o: main.c cpu.h

main.o: main.c
	$(CC) $(CFLAGS) -c main.c $(LDFLAGS)

vm.o: vm.h

cpu.o: cpu.h

mmu.o: mmu.h

io.o: io.h

ram.o: ram.h

isa.o: isa.h

debug.o: debug.h

clean:
	$(RM) dumbsel *.o

