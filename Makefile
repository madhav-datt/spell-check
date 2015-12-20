#
# Makefile
#
# Spellcheck program
#


# compiler to use
CC = gcc

# flags to pass compiler
CFLAGS = -ggdb3 -O0 -std=c99 -Wall -Werror -g

# name for executable
EXE = speller

# space-separated list of header files
HDRS = dictionary.h

# space-separated list of libraries, if any,
# each of which should be prefixed with -l
LIBS =

# space-separated list of source files
SRCS = speller.c dictionary.c

# automatically generated list of object files
OBJS = $(SRCS:.c=.o)


# default target
$(EXE): $(OBJS) $(HDRS) Makefile
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

# dependencies 
$(OBJS): $(HDRS) Makefile

# cleaning up the directory
clean:
	rm -f core $(EXE) *.o
