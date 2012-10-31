# Generated automatically from Makefile.in by configure.
# Copyright (c) 1991-2012 Kawahara Lab., Kyoto University
# Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
# Copyright (c) 2005-2012 Julius project team, Nagoya Institute of Technology
# All rights reserved

SHELL=/bin/sh
RM=/bin/rm -f
prefix=/usr/local
exec_prefix=${prefix}
INSTALL=/usr/bin/install -c

TARGET=mkdfa.pl
SUBDIR=mkfa-1.44-flex

all:
	(cd $(SUBDIR); $(MAKE))
	chmod +x $(TARGET)

install:
	(cd $(SUBDIR); $(MAKE) install)
	${INSTALL} -d ${exec_prefix}/bin
	${INSTALL} $(TARGET) ${exec_prefix}/bin

install.bin:
	(cd $(SUBDIR); $(MAKE) install.bin)
	${INSTALL} -d ${exec_prefix}/bin
	${INSTALL} $(TARGET) ${exec_prefix}/bin

clean:
	(cd $(SUBDIR); $(MAKE) clean)

distclean:
	if test -f $(SUBDIR)/Makefile; then \
	  (cd $(SUBDIR); $(MAKE) distclean); \
	fi
	$(RM) mkdfa.pl
	$(RM) Makefile
