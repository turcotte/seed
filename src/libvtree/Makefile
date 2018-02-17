##                           -*- Mode: Makefile -*- 
## Makefile --- 
## Author          : Truong Nguyen and Marcel Turcotte
## Created On      : Wed Jun  8 08:25:30 2005
## Last Modified By: Marcel Turcotte
## Last Modified On: Wed Jul 13 15:48:34 2005
##
## This copyrighted source code is freely distributed under the terms
## of the GNU General Public License. 
## See the files COPYRIGHT and LICENSE for details.

                                                                               

SHELL = /bin/sh

OBJECTS = construct.o access.o repeats.o lce.o debug.o

LIBS = -lvtree -ldev
LIBDIR = -L../libdev -L./
INCDIR = -I../libdev -I./

.SUFFIXES:
.SUFFIXES: .c .o

.c.o: 
	$(CC) $(CFLAGS) $(INCDIR) -c $<

all: libvtree.a

libvtree.a: $(OBJECTS)
	$(AR) rcvs libvtree.a $(OBJECTS)
	$(RANLIB) libvtree.a
	chmod 644 libvtree.a

tests: tests.o libvtree.a
	$(CC) -o tests tests.o $(LIBDIR) $(CFLAGS) $(LIBS) -lm

check: tests
	time -p ./tests

clean:
	rm -f $(OBJECTS) tests.o libvtree.a *~ tests
