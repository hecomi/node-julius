/*
 * Copyright (c) 1991-2013 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2013 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#include <sent/stddefs.h>
#include <sent/vocabulary.h>
#include <sent/dfa.h>
#include <sent/speech.h>
#include "nextword.h"

extern WORD_INFO *winfo;
extern DFA_INFO *dfa;
extern char **termname;

/* $B%U%i%0C#(B */
extern boolean no_term_file;
extern boolean verbose_flag;
extern boolean term_mode;

/* $B<!$K@\B32DG=$J%+%F%4%j$HA+0\@h>uBV$N=89g$rJV$9(B */
int
next_terms(int stateid, int *termbuf, int *nextstatebuf)
{
  DFA_ARC *arc, *arc2;
  int cate, ns;
  int cnum;

  cnum = 0;
  for (arc = dfa->st[stateid].arc; arc; arc = arc->next) {
    cate = arc->label;
    ns = arc->to_state;
    if (dfa->is_sp[cate]) {
      for (arc2 = dfa->st[ns].arc; arc2; arc2 = arc2->next) {
	termbuf[cnum] = arc2->label;
	nextstatebuf[cnum] = arc2->to_state;
	cnum++;
      }
    } else {			/* not noise */
      termbuf[cnum] = cate;
      nextstatebuf[cnum] = ns;
      cnum++;
    }
  }
  return cnum;
}     

/* $B%+%F%4%jHV9f$N=EJ#$rHr$1$k(B */
int
compaction_int(int *a, int num)
{
  int i,j,d;

  d = 0;
  for(i=0;i<num;i++) {
    for (j=0;j<d;j++) {
      if (a[i] == a[j]) {
	break;
      }
    }
    if (j == d) {
      a[d++] = a[i];
    }
  }
  return d;
}
