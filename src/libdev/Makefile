##                           -*- Mode: Makefile -*- 
## Makefile --- 
## Author          : Truong Nguyen and Marcel Turcotte
## Created On      : Wed Jun  8 08:15:09 2005
## Last Modified By: Marcel Turcotte
## Last Modified On: Mon Aug  8 18:03:37 2005
##
## This copyrighted source code is freely distributed under the terms
## of the GNU General Public License. 
## See the files COPYRIGHT and LICENSE for details.

                                                                               

SHELL = /bin/sh

OBJECTS = libdev.o vector.o ivector.o bitset.o list.o

LIBS = -ldev
LIBDIR = -L./
INCDIR = -I./

.SUFFIXES:
.SUFFIXES: .c .o

.c.o: 
	$(CC) $(CFLAGS) $(INCDIR) -c $<

all: libdev.a

libdev.a: $(OBJECTS)
	$(AR) rcvs libdev.a $(OBJECTS)
	$(RANLIB) libdev.a
	chmod 644 libdev.a

tests: tests.o libdev.a
	$(CC) -o tests tests.o $(LIBDIR) $(CFLAGS) $(LIBS)

check: tests
	time -p ./tests

clean:
	rm -f $(OBJECTS) tests.o tests *~ libdev.a

