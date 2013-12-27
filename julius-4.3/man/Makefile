# Generated automatically from Makefile.in by configure.
# Copyright (c) 1991-2013 Kawahara Lab., Kyoto University
# Copyright (c) 2001-2005 Shikano Lab., Nara Institute of Science and Technology
# Copyright (c) 2005-2013 Julius project team, Nagoya Institute of Technology
# All rights reserved 
#
# $Id: Makefile.in,v 1.6 2013/06/20 17:14:27 sumomo Exp $
#
SHELL=/bin/sh
RM=/bin/rm -f
prefix=/usr/local
exec_prefix=${prefix}
INSTALL=/usr/bin/install -c

############################################################

all:
	@echo Nothing to build

install: install.man install.man.ja

install.man:
	${INSTALL} -d ${prefix}/man/man1
	for f in *.1; do \
	  ${INSTALL} -m 644 $$f ${prefix}/man/man1/$$f; \
	done

install.man.ja:
	${INSTALL} -d ${prefix}/man/ja/man1
	cd ja;
	for f in *.1; do \
	  ${INSTALL} -m 644 $$f ${prefix}/man/ja/man1/$$f; \
	done
	cd ..

############################################################

clean:
	@echo Nothing to do

distclean:
	$(RM) Makefile
