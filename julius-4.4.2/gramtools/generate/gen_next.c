/*
 * Copyright (c) 1991-2013 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2013 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

/* next_word functions */

#include "common.h"
#include "gen_next.h"

extern WORD_INFO *winfo;
extern DFA_INFO *dfa;

NEXTWORD **
nw_malloc()
{
  NEXTWORD **nw;
  NEXTWORD *nwtmp;
  int i;
  int maxnw;

  maxnw = winfo->num * 2;	/* NOISE$B$rHt$P$9J,(B */
  /* $BO"B3NN0h$rG[Ns$K3d$jEv$F$k(B */
  nw = (NEXTWORD **)malloc(maxnw * sizeof(NEXTWORD *));
  nwtmp = (NEXTWORD *)malloc(maxnw * sizeof(NEXTWORD));
  nw[0] = nwtmp;
  for (i=1;i<maxnw; i++) {
    nw[i] = &(nwtmp[i]);
  }
  return nw;
}

/* $BM=B,<!C18l3JG<NN0h$N2rJ|(B */
void
nw_free(NEXTWORD **nw)
{
  free(nw[0]);
  free(nw);
}



/* $B=i4|>uBV$+$iA+0\$7$&$kC18l=89g$rJV$9(B */
/* $BJV$jCM(B: $BC18l?t(B*/
/* NOISE: $B$3$3$K$OMh$J$$;EMM(B */
int
dfa_firstwords(NEXTWORD **nw)
{
  DFA_ARC *arc;
  int i, cate, iw, ns;
  int num = 0;

  for (i=0;i<dfa->state_num;i++) {
    if ((dfa->st[i].status & INITIAL_S) != 0) { /* $B=i4|>uBV$+$i(B */
      for (arc = dfa->st[i].arc; arc; arc = arc->next) {	/* $BA4$F$NA+0\(B */
	cate = arc->label;
	ns = arc->to_state;
	/* $BA+0\$KBP1~$9$k%+%F%4%jFb$NA4C18l$rE83+(B */
	for (iw=0;iw<dfa->term.wnum[cate];iw++) {
	  nw[num]->id = dfa->term.tw[cate][iw];
	  nw[num]->next_state = ns;
	  nw[num]->can_insert_sp = FALSE;
	  num++;
	}
      }
    }
  }

  return num;
}
int
dfa_firstterms(NEXTWORD **nw)
{
  DFA_ARC *arc;
  int i, cate, ns;
  int num = 0;

  for (i=0;i<dfa->state_num;i++) {
    if ((dfa->st[i].status & INITIAL_S) != 0) { /* $B=i4|>uBV$+$i(B */
      for (arc = dfa->st[i].arc; arc; arc = arc->next) {	/* $BA4$F$NA+0\(B */
	cate = arc->label;
	ns = arc->to_state;
	/* $BA+0\$KBP1~$9$k%+%F%4%jFb$N(B1$BC18l$rE83+(B */
	if (dfa->term.wnum[cate] == 0) continue;
	nw[num]->id = dfa->term.tw[cate][0];
	nw[num]->next_state = ns;
	nw[num]->can_insert_sp = FALSE;
	num++;
      }
    }
  }

  return num;
}

/* $B<!$K@\B3$7F@$kC18l72$rJV$9(B */
/* $BJV$jCM(B:$BC18l?t(B */
/* NOISE: $B@h$^$G8+$F!$(Bcan_insert_sp=TRUE$B$GJV$9(B */
int
dfa_nextwords(NODE *hypo, NEXTWORD **nw)
{
  DFA_ARC *arc, *arc2;
  int iw,cate,ns,cate2,ns2;
  int num = 0;

  for (arc = dfa->st[hypo->state].arc; arc; arc = arc->next) {
    cate = arc->label;
    ns = arc->to_state;
    if (dfa->is_sp[cate]) {
      /* $B@h$^$G8+$k!#<+J,$OE83+$7$J$$(B */
      for (arc2 = dfa->st[ns].arc; arc2; arc2 = arc2->next) {
	cate2 = arc2->label;
	ns2 = arc2->to_state;
	for (iw=0;iw<dfa->term.wnum[cate2];iw++) {
	  nw[num]->id = dfa->term.tw[cate2][iw];
	  nw[num]->next_state = ns2;
	  nw[num]->can_insert_sp = TRUE;
	  num++;
	}
      }
    } else {
      /* $BA+0\$KBP1~$9$k%+%F%4%jFb$NA4C18l$rE83+(B */
      for (iw=0;iw<dfa->term.wnum[cate];iw++) {
	nw[num]->id = dfa->term.tw[cate][iw];
	nw[num]->next_state = ns;
	nw[num]->can_insert_sp = FALSE;
	num++;
      }
    }
  }
  return num;
}
int
dfa_nextterms(NODE *hypo, NEXTWORD **nw)
{
  DFA_ARC *arc, *arc2;
  int cate,ns,cate2,ns2;
  int num = 0;

  for (arc = dfa->st[hypo->state].arc; arc; arc = arc->next) {
    cate = arc->label;
    ns = arc->to_state;
    if (dfa->is_sp[cate]) {
      /* $B@h$^$G8+$k!#<+J,$OE83+$7$J$$(B */
      for (arc2 = dfa->st[ns].arc; arc2; arc2 = arc2->next) {
	cate2 = arc2->label;
	ns2 = arc2->to_state;
	if (dfa->term.wnum[cate2] == 0) continue;
	nw[num]->id = dfa->term.tw[cate2][0];
	nw[num]->next_state = ns2;
	nw[num]->can_insert_sp = TRUE;
	num++;
      }
    } else {
      /* $BA+0\$KBP1~$9$k%+%F%4%jFb$NA4C18l$rE83+(B */
      if (dfa->term.wnum[cate] == 0) continue;
      nw[num]->id = dfa->term.tw[cate][0];
      nw[num]->next_state = ns;
      nw[num]->can_insert_sp = FALSE;
      num++;
    }
  }
  return num;
}

/* $B2>@b$,J8$H$7$F<uM}2DG=$G$"$k$+$I$&$+$rJV$9(B */
/* NOISE: $B$3$3$K$O$3$J$$;EMM(B */
boolean
dfa_acceptable(NODE *hypo)
{
  /* $B<uM}>uBV$J$i(B */
  if (dfa->st[hypo->state].status & ACCEPT_S) {
    return TRUE;
  } else {
    return FALSE;
  }
}
