##                           -*- Mode: Makefile -*- 
## Makefile --- 
## Author          : Truong Nguyen and Marcel Turcotte
## Created On      : Wed Jun  8 08:20:27 2005
## Last Modified By: turcotte
## Last Modified On: Thu Aug 16 08:25:56 2018
##
## This copyrighted source code is freely distributed under the terms
## of the GNU General Public License. 
## See the files COPYRIGHT and LICENSE for details.
                                                                               

SHELL = /bin/sh

LIBDEV_DIR = ./src/libdev

LIBVTREE_DIR = ./src/libvtree

LIBBIO_DIR = ./src/libbio

ALGORITHMS_DIR = ./src/algorithms

BIN_DIR = ../../bin

## Without RNALIB support

RNALIB =
RNALIB_INCLUDE =
RNALIB_LIB =
RNALIB_LIBS =

## With RNALIB support

# RNALIB = -DRNALIB
# RNALIB_INCLUDE = -I/usr/local/include
# RNALIB_LIB = -L /usr/local/lib/
# RNALIB_LIBS = -lRNA -lm

CC = gcc

## Memory usage
# Seed can display information about its memory usage, 
# add -DHAS_MALLINFO to CLFAGS
# add -lmalloc tp LDFLAGS
# However, this extremely slows down the execution time

CFLAGS = -O -std=c99

AR = ar
RANLIB = ranlib

MFLAGS = CC="$(CC)" CFLAGS="$(CFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)" BIN_DIR="$(BIN_DIR)"
RNALIB_FLAGS = RNALIB="$(RNALIB)" RNALIB_INCLUDE="$(RNALIB_INCLUDE)" RNALIB_LIB="$(RNALIB_LIB)" RNALIB_LIBS="$(RNALIB_LIBS)"

all: libdev libvtree libbio algorithms

libdev:
	cd $(LIBDEV_DIR) ; $(MAKE) $(MFLAGS) all

libvtree:
	cd $(LIBVTREE_DIR) ; $(MAKE) $(MFLAGS) all

libbio:
	cd $(LIBBIO_DIR) ; $(MAKE) $(MFLAGS) all

algorithms:
	cd $(ALGORITHMS_DIR) ; $(MAKE) $(MFLAGS) $(RNALIB_FLAGS) all

install:
	cd $(ALGORITHMS_DIR) ; mkdir -p $(BIN_DIR); $(MAKE) $(MFLAGS) $(RNALIB_FLAGS) install

check:
	cd $(LIBDEV_DIR) ; $(MAKE) $(MFLAGS) check
	cd $(LIBVTREE_DIR) ; $(MAKE) $(MFLAGS) check
	cd $(LIBBIO_DIR) ; $(MAKE) $(MFLAGS) check
	cd $(ALGORITHMS_DIR) ; $(MAKE) $(MFLAGS) check

clean:
	cd $(LIBDEV_DIR) ; $(MAKE) clean
	cd $(LIBVTREE_DIR) ; $(MAKE) clean
	cd $(LIBBIO_DIR) ; $(MAKE) clean
	cd $(ALGORITHMS_DIR) ; $(MAKE) clean
	rm -f *~

.SILENT:

