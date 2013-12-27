# Generated automatically from Makefile.in by configure.
# Copyright (c) 1991-2013 Kawahara Lab., Kyoto University
# Copyright (c) 1997-2000 Information-technology Promotion Agency, Japan
# Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
# Copyright (c) 2005-2013 Julius project team, Nagoya Institute of Technology
# All rights reserved

#
# $Id: Makefile.in,v 1.11 2013/06/20 17:14:18 sumomo Exp $
#
SHELL=/bin/sh
.SUFFIXES:
.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

LIBSENT=../libsent
LIBJULIUS=../libjulius
CC=/usr/bin/gcc
CFLAGS=-g -O2
CPPFLAGS=-I. -I$(LIBJULIUS)/include -I$(LIBSENT)/include  `$(LIBSENT)/libsent-config --cflags` `$(LIBJULIUS)/libjulius-config --cflags`
LDFLAGS= -L$(LIBJULIUS) `$(LIBJULIUS)/libjulius-config --libs` -L$(LIBSENT) `$(LIBSENT)/libsent-config --libs` -liconv
GTK_CFLAGS=
GTK_LIBS=
RM=/bin/rm -f

## install
prefix=/usr/local
exec_prefix=${prefix}
bindir=${exec_prefix}/bin
libdir=${exec_prefix}/lib
includedir=${prefix}/include
mandir=${prefix}/man
INSTALL=/usr/bin/install -c
INSTALL_DATA=${INSTALL} -m 644
INSTALL_PROGRAM=${INSTALL}

############################################################

TARGET = julius.dSYM
INSTALLTARGET = julius.dSYM

OBJ = \
main.o \
recogloop.o \
module.o \
output_module.o \
output_stdout.o \
output_file.o \
record.o \
charconv.o charconv_iconv.o

############################################################

all: $(TARGET)

$(TARGET): $(LIBSENT)/libsent.a $(LIBJULIUS)/libjulius.a $(OBJ)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $(OBJ) $(LDFLAGS) $(GTK_LIBS)

############################################################

install: install.bin

install.bin: $(TARGET)
	${INSTALL} -d ${exec_prefix}/bin
	${INSTALL_PROGRAM} $(TARGET) ${bindir}/${INSTALLTARGET}

depend:
	makedepend -- $(CFLAGS) -- $(OBJSENT)

smallclean:
	$(RM) *.o

clean:
#	$(RM) *.o *.bak *~ core TAGS
	$(RM) $(OBJ)
	$(RM) *.bak *~ core TAGS
	$(RM) config.log config.cache
	$(RM) $(TARGET) $(INSTALLTARGET)
	$(RM) $(TARGET).exe $(INSTALLTARGET).exe

distclean:
#	$(RM) *.o *.bak *~ core TAGS
	$(RM) $(OBJ)
	$(RM) *.bak *~ core TAGS
	$(RM) config.log config.cache
	$(RM) $(TARGET) $(INSTALLTARGET)
	$(RM) $(TARGET).exe $(INSTALLTARGET).exe
	$(RM) config.status config.h
	$(RM) Makefile

############################################################
configure:
	autoconf
	$(RM) config.h.in
	autoheader
