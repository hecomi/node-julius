# Generated automatically from Makefile.in by configure.
# Copyright (c) 1991-2012 Kawahara Lab., Kyoto University
# Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
# Copyright (c) 2005-2012 Julius project team, Nagoya Institute of Technology
# All rights reserved
#
# $Id: Makefile.in,v 1.7 2012/07/27 08:44:57 sumomo Exp $
#
SHELL=/bin/sh
.SUFFIXES:
.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

LIBSENT=../libsent
CC=gcc
CFLAGS=-g -O2
CPPFLAGS=-I$(LIBSENT)/include    `$(LIBSENT)/libsent-config --cflags`
LDFLAGS= -L$(LIBSENT) `$(LIBSENT)/libsent-config --libs`
RM=/bin/rm -f
prefix=/usr/local
exec_prefix=${prefix}
INSTALL=/usr/bin/install -c

OBJ=mkbingram.o
TARGET=mkbingram.dSYM

all: $(TARGET)

$(TARGET): $(OBJ) $(LIBSENT)/libsent.a
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $(OBJ) $(LDFLAGS)

install: install.bin

install.bin: $(TARGET)
	${INSTALL} -d ${exec_prefix}/bin
	${INSTALL} $(TARGET) ${exec_prefix}/bin

clean:
	$(RM) $(OBJ)
	$(RM) *~ core
	$(RM) $(TARGET) $(TARGET).exe

distclean:
	$(RM) $(OBJ)
	$(RM) *~ core
	$(RM) $(TARGET) $(TARGET).exe
	$(RM) Makefile
