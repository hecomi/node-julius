/**
 * @file   dfa_decode.c
 * 
 * <JA>
 * @brief  ����ʸˡ�˴�Ť���ñ��ͽ¬���裲�ѥ���
 *
 * Ϳ����줿������Ф��ơ�DFA ʸˡ����³��ǽ�ʼ�ñ��ν������ꤹ��. 
 * �������ºݤˤ�, Ÿ���������ͽ¬������ü�ե졼����դ�ñ��ȥ�ꥹ
 * ��˻ĤäƤ���ñ��Τߤ�Ÿ�������. 
 *
 * ʸˡ����Ǥϥ��硼�ȥݡ�����ñ��Ȥ��Ƶ��Ҥ������Υ��硼�ȥݡ���ñ���
 * �и����֤�ʸˡ�ǻ��ꤹ��. ���������ºݤ����ϤǤϤ������ꤷ�����֤�
 * ɬ������ݡ���������ʤ����ᡤñ��Ÿ���ˤ����Ƥϡ�
 * ��ñ�콸��˥��硼�ȥݡ�����������ϡ�����ˤ��μ���ñ�콸��ޤǸ���
 * ��ñ�콸��˴ޤ��. �ºݤˤ����˥��硼�ȥݡ�������������뤫�ɤ����ϡ�
 * search_bestfirst_main.c ��ξ�ԤΥ���������Ӥ���Ƚ�Ǥ���. 
 *
 * ʸˡ���Ѥ���ǧ���������󥹥��󥹤Ǥϡ�dfa_firstwords(), dfa_nextwords(),
 * dfa_acceptable(), dfa_eosscore() ����2�ѥ��Υᥤ��ؿ� wchmm_fbs() ����
 * ���Ѥ����. �ʤ� N-gram ���Ѥ���ǧ���������󥹥��󥹤Ǥϡ�
 * ����� ngram_decode.c ��δؿ����Ȥ���. 
 * </JA>
 * 
 * <EN>
 * @brief  Grammar-based word prediction (2nd pass)
 *
 * Given a part-of-sentence hypothesis, these function determine a set of next
 * words allowed to be connected by the grammar.  Actually, only words in the
 * word trellis, which exist around the estimated word-end frame will be
 * expanded.
 *
 * When using DFA grammar, the possible (short) pause insertion point
 * should be explicitly specified in grammar, by defining "short-pause
 * word" in vocabulary and write its appearance in grammar.  Since a
 * short pause will not always appear on the specified point, Julius
 * will consider the skipping of such short pause word for the next
 * word prediction in these functions.  Whether short pause was
 * actually inserted or not in the user input will be determined by
 * score in search_bestfirst_main.c.
 *
 * In recognition process instance with DFA grammar, dfa_firstwords(),
 * dfa_nextwords(), dfa_acceptable() and dfa_eosscore() will be called
 * from main search function wchmm_fbs().  When using N-gram, on the
 * other hand, the corresponding functions in ngram_decode.c will be
 * used instead.  </EN>
 * 
 * @author Akinobu LEE
 * @date   Mon Mar  7 15:31:00 2005
 *
 * $Revision: 1.7 $
 * 
 */
/*
 * Copyright (c) 1991-2013 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2013 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#include <julius/julius.h>

/** 
 * <JA>
 * ʸˡ�ˤ������äơ�ʸƬ����³������ñ���ǽ��ͽ¬ñ�췲�Ȥ����֤�. 
 * 
 * @param nw [out] ��ñ�콸��γ�Ǽ��ؤΥݥ���
 * @param peseqlen [in] ���ϥե졼��Ĺ
 * @param maxnw [in] @a nw �ε���������Ĺ
 * @param r [in] ǧ���ץ������󥹥���
 * 
 * @return ͽ¬���줿ñ��� (���������顼���� -1 ���֤�)
 * </JA>
 * <EN>
 * Return initial word set from grammar.
 * 
 * @param nw [out] pointer to hold the resulting next word set
 * @param peseqlen [in] input frame length
 * @param maxnw [in] maximum number of words that can be set in @a nw
 * @param r [in] recognition process instance
 * 
 * @return the number of predicted words, or -1 on error.
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
int
dfa_firstwords(NEXTWORD **nw, int peseqlen, int maxnw, RecogProcess *r)
{
  DFA_INFO *dfa;
  DFA_ARC *arc;
  MULTIGRAM *m;
  int s, sb, se;
  int cate, iw, ns;
  int num = 0;

  dfa = r->lm->dfa;

  for (m = r->lm->grammars; m; m = m->next) {
    if (m->active) {
      sb = m->state_begin;
      se = sb + m->dfa->state_num;
      for(s=sb;s<se;s++) {
	if ((dfa->st[s].status & INITIAL_S) != 0) { /* from initial state */
	  for (arc = dfa->st[s].arc; arc; arc = arc->next) {	/* for all arc */
	    cate = arc->label;	/* category ID */
	    ns = arc->to_state;	/* next DFA state ID */
	    /* all words within the category is expanded */
	    for (iw=0;iw<dfa->term.wnum[cate];iw++) {
	      nw[num]->id = dfa->term.tw[cate][iw]; /* word ID */
	      nw[num]->next_state = ns; /* next state */
	      nw[num]->can_insert_sp = FALSE; /* short pause should not inserted before this word */
#ifdef FIX_PENALTY
	      nw[num]->lscore = 0.0;
#else
	      nw[num]->lscore = r->config->lmp.penalty2;
#ifdef CLASS_NGRAM
	      /* add per-word penalty */
	      nw[num]->lscore += r->wchmm->winfo->cprob[nw[num]->id];
#endif
#endif
	      num++;
	      if (num >= maxnw) return -1; /* buffer overflow */
	    }
	  }
	}
      }
    }
  }

  return num;
}

/** 
 * <JA>
 * ��ʬʸ������Ф��ơ�ʸˡ�˽��äƼ�����³������ñ�췲���֤�. 
 * 
 * @param hypo [in] Ÿ��������ʬʸ����
 * @param nw [out] ��ñ�콸��γ�Ǽ��ؤΥݥ���
 * @param maxnw [in] @a nw �ε���������Ĺ
 * @param r [in] ǧ���ץ������󥹥���
 * 
 * @return ͽ¬���줿ñ��� (���������顼���� -1 ���֤�)
 * </JA>
 * <EN>
 * Given a part-of-sentence hypothesis, returns the next word set defined
 * by DFA grammar.
 * 
 * @param hypo [in] the source part-of-sentene hypothesis
 * @param nw [out] pointer to hold the resulting next word set
 * @param maxnw [in] maximum number of words that can be set in @a nw
 * @param r [in] recognition process instance
 * 
 * @return the number of predicted words, or -1 on error.
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
int
dfa_nextwords(NODE *hypo, NEXTWORD **nw, int maxnw, RecogProcess *r)
{
  DFA_INFO *dfa;
  DFA_ARC *arc, *arc2;
  int iw,cate,ns,cate2,ns2;
  int num = 0;

  dfa = r->lm->dfa;

  /* hypo->state: current DFA state ID */
  for (arc = dfa->st[hypo->state].arc; arc; arc = arc->next) {/* for all arc */
    cate = arc->label;
    ns = arc->to_state;
    if (dfa->is_sp[cate]) {	/* short pause */
      /* expand one more next (not expand the short pause word itself) */
      for (arc2 = dfa->st[ns].arc; arc2; arc2 = arc2->next) {
	cate2 = arc2->label;
	ns2 = arc2->to_state;
	for (iw=0;iw<dfa->term.wnum[cate2];iw++) {
	  nw[num]->id = dfa->term.tw[cate2][iw];
	  nw[num]->next_state = ns2;
	  nw[num]->can_insert_sp = TRUE;
	  nw[num]->lscore = r->config->lmp.penalty2;
#ifdef CLASS_NGRAM
	  /* add per-word penalty */
	  nw[num]->lscore += r->wchmm->winfo->cprob[nw[num]->id];
#endif
	  num++;
	  if (num >= maxnw) return -1; /* buffer overflow */
	}
      }
    } else {			/* not short pause */
      /* all words within the category is expanded */
      for (iw=0;iw<dfa->term.wnum[cate];iw++) {
	nw[num]->id = dfa->term.tw[cate][iw];
	nw[num]->next_state = ns;
	nw[num]->can_insert_sp = FALSE;
	nw[num]->lscore = r->config->lmp.penalty2;
#ifdef CLASS_NGRAM
	/* add per-word penalty */
	nw[num]->lscore += r->wchmm->winfo->cprob[nw[num]->id];
#endif
	num++;
	if (num >= maxnw) return -1; /* buffer overflow */
      }
    }
  }
  return num;
}


/** 
 * <JA>
 * ��ʬʸ���⤬ʸˡ��ʸ�Ȥ��ƺǽ�(������ǽ)���֤ˤ��뤫�ɤ������֤�. 
 * 
 * @param hypo [in] ��ʬʸ����
 * @param r [in] ǧ���ץ������󥹥���
 * 
 * @return ������ǽ���֤ˤ���Ȥ� TRUE �����Բ�ǽ�ʤȤ� FALSE
 * </JA>
 * <EN>
 * Return whether the hypothesis is currently on final state
 * 
 * @param hypo [in] sentence hypothesis
 * @param r [in] recognition process instance
 * 
 * @return TRUE when on final state, or FALSE if not acceptable.
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
boolean
dfa_acceptable(NODE *hypo, RecogProcess *r)
{
  if (r->lm->dfa->st[hypo->state].status & ACCEPT_S) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/* patch by kashima */
/** 
 * <JA>
 * ��ñ����䤬���ο��ꤵ�줿��³ͽ¬���������ñ��ȥ�ꥹ���
 * ���뤫�ɤ���������å������⤷����Ф��Υȥ�ꥹñ��ؤΥݥ��󥿤򥻥å�
 * ����. �ʤ��������³���Ϥ��ȤǷ�ޤ�Τǡ������ǤϺ�Ŭ�ʥȥ�ꥹñ��
 * �Ǥʤ��Ƥ褤. 
 * 
 * @param nword [i/o] ��ñ����� (�б�����ȥ�ꥹñ��ؤΥݥ��󥿤�
 * ���åȤ����)
 * @param hypo [in] Ÿ��������
 * @param r [in] ǧ���ץ������󥹥���
 * 
 * @return ñ��ȥ�ꥹ���ͽ¬�����ն�˼�ñ�줬¸�ߤ���� TRUE��¸��
 * ���ʤ���� FALSE ���֤�. 
 * </JA>
 * <EN>
 * Check if the given nextword exists in the word trellis around the
 * estimated connection time.  If exist, set the pointer to the corresponding
 * trellis word to the nextword.  Since the best connection time will be
 * re-computed later, it need not to be an optimal one.
 * 
 * @param nword [i/o] next word candidate (pointer to the found trellis word
 * will be set)
 * @param hypo [in] source part-of-sentence hypothesis
 * @param r [in] recognition process instance
 * 
 * @return TRUE if the nextword exists on the word trellis around the estimated
 * connection point, or FALSE if not exist.
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
boolean
dfa_look_around(NEXTWORD *nword, NODE *hypo, RecogProcess *r)
{
  int t,tm;
  int i;
  WORD_ID w;
  BACKTRELLIS *bt;
  int lookup_range;

  bt = r->backtrellis;
  lookup_range = r->config->pass2.lookup_range;
  
  tm = hypo->estimated_next_t;	/* estimated connection time */

  /* look aound [tm-lookup_range..tm+lookup_range] frame */
  /* near the center is better:
     1. the first half (backward)   2. the second half (forward) */
  /* 1. backward */
  for(t = tm; t >= tm - lookup_range; t--) {
    if (t < 0) break;
     for (i=0;i<bt->num[t];i++) {
       w = (bt->rw[t][i])->wid;
       if(w == nword->id){	/* found */
         nword->tre = bt->rw[t][i];
         return TRUE;
       }
     }
  }
  /* 2. forward */
  for(t = tm + 1; t < tm + lookup_range; t++) {
    if (t > bt->framelen - 1) break;
    if (t >= hypo->bestt) break;
    for (i=0;i<bt->num[t];i++) {
      w = (bt->rw[t][i])->wid;
      if(w == nword->id){	/* found */
        nword->tre = bt->rw[t][i];
        return TRUE;
      }
    }
  }

  return FALSE;			/* not found */
}

/* end of file */
