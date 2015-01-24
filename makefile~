# MAKEFILE
# By: Jonathan Snowdon 0709496

#Compile Flags
CC     = gcc
CFLAGS = -Wall -g -c -pedantic -std=c99
LFLAGS = -Wall -g -pthread -std=c99
LIBS   = -lm

#Directory Flags
SDIR = ./src
IDIR = ./include
BDIR = ./bin

#Objects
OBJS       = a1.o
OBJS_DEBUG = a1_DEBUG.o

#Execution
a1: $(OBJS)
		@ $(CC) $(LFLAGS) $(OBJS) -o $(BDIR)/a1

a1.o: $(SDIR)/a1.c $(IDIR)/a1.h
		@ $(CC) $(CFLAGS) $(SDIR)/a1.c -o a1.o

#DEBUG Execution
a1_DEBUG: $(OBJS_DEBUG)
		$(CC) $(LFLAGS) $(OBJS_DEBUG) -o $(BDIR)/a1

a1_DEBUG.o: $(SDIR)/a1.c $(IDIR)/a1.h
		$(CC) $(CFLAGS) $(SDIR)/a1.c -o a1_DEBUG.o

#Make Targets
all: a1

run: a1
		@ $(BDIR)/a1

debug: a1_DEBUG
		$(BDIR)/a1

clean:
		@ rm $(BDIR)/a1 *.o
		@ rm $(BDIR)/file.bin
