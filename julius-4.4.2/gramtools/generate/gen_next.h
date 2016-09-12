/*
 * Copyright (c) 1991-2013 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2013 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#ifndef __NEXT__H__
#define __NEXT__H__

#include <sent/stddefs.h>
#include <sent/vocabulary.h>
#include <sent/dfa.h>
#include <sent/speech.h>

/* ported from julian/dfa_decode.c */
/* $B<!C18l8uJd(B */
typedef struct __nextword__ {
  WORD_ID id;			/* $BC18l(BID */
  int next_state;		/* $BA+0\8e$N(BDFA$B>uBVHV9f(B */
  boolean can_insert_sp;	/* $B2>@b$H$3$NC18l$N4V$K(Bsp$B$,F~$k2DG=@-$,$"$k>l9g(B TRUE */
} NEXTWORD;
/* $BItJ,J82>@b(B */
typedef struct __node__ {
  boolean endflag;              /* $BC5:w=*N;%U%i%0(B */
  WORD_ID seq[MAXSEQNUM];       /* $B2>@b$NC18l7ONs(B */
  short seqnum;                 /* $B2>@b$NC18l$N?t(B */
  int state;                    /* $B8=:_$N(BDFA$B>uBVHV9f(B */
} NODE;

NEXTWORD **nw_malloc();
void nw_free(NEXTWORD **nw);
int dfa_firstwords(NEXTWORD **nw);
int dfa_nextwords(NODE *hypo, NEXTWORD **nw);
int dfa_firstterms(NEXTWORD **nw);
int dfa_nextterms(NODE *hypo, NEXTWORD **nw);
boolean dfa_acceptable(NODE *hypo);


#endif /* __NEXT__H__ */
