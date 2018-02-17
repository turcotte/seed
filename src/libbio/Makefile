##                           -*- Mode: Makefile -*- 
## Makefile --- 
## Author          : Truong Nguyen and Marcel Turcotte
## Created On      : Wed Jun  8 10:25:02 2005
## Last Modified By: Marcel Turcotte
## Last Modified On: Wed Jul 13 15:45:01 2005
##
## This copyrighted source code is freely distributed under the terms
## of the GNU General Public License. 
## See the files COPYRIGHT and LICENSE for details.

                                                                               

SHELL = /bin/sh

OBJECTS = seq.o

LIBS = -ldev
LIBDIR = -L../libdev
INCDIR = -I../libdev

.SUFFIXES:
.SUFFIXES: .c .o

.c.o: 
	$(CC) $(CFLAGS) $(INCDIR) -c $<

all: libbio.a

libbio.a: $(OBJECTS)
	$(AR) rcvs libbio.a $(OBJECTS)
	$(RANLIB) libbio.a
	chmod 644 libbio.a

tests: tests.o libbio.a
	$(CC) -o tests tests.o $(CFLAGS) $(LIBDIR) -L./ $(LIBS) -lbio -lm

check: tests
	time -p ./tests

clean:
	rm -f $(OBJECTS) libbio.a *~ tests.o tests
