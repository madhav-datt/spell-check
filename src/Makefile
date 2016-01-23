#
# Makefile for spellcheck program
# Typing 'make' or 'make spellcheck' will create the executable file.
#
# Copyright (C)   2016    Madhav Datt
# http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
#

# compiler to use
#
CC = gcc

# flags t0 pass compiler
#
CFLAGS  = -ggdb3 -O0 -std=c99 -Wall -Werror -g

# target executable name
#
default: spellcheck

# for executable file spellcheck 
# object files are speller.o, autocorrect.o:
#
spellcheck:  speller.o autocorrect.o 
	$(CC) $(CFLAGS) -o spellcheck speller.o autocorrect.o

# for object file speller.o
# source files speller.c, autocorrect.h:
#
speller.o:  speller.c ../autocorrect-lib/autocorrect.h 
	$(CC) $(CFLAGS) -c speller.c

# for object file autocorrect.o
# source files autocorrect.c and autocorrect.h:
#
autocorrect.o:  ../autocorrect-lib/autocorrect.c ../autocorrect-lib/autocorrect.h 
	$(CC) $(CFLAGS) -c ../autocorrect-lib/autocorrect.c

# remove old *.o object files and *~ backup files:
#
clean: 
	$(RM) spellcheck *.o *~
