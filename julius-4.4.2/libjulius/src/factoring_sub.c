/**
 * @file   factoring_sub.c
 * 
 * <JA>
 * @brief  ���쥹������factoring�׻�����1�ѥ���
 *
 * ���Υե�����ˤϡ��裱�ѥ��ˤ����Ƹ��쥹������ factoring ��Ԥ������
 * �ؿ����ޤޤ�Ƥ��ޤ�. �ڹ�¤�������ǤΥ��֥ĥ꡼���ñ��ꥹ��
 * (successor list) �ι��ۡ������ǧ����θ��쥹�����׻��롼����
 * �ޤޤ�ޤ�. 
 *
 * successor list �ϡ��ڹ�¤������γƥΡ��ɤ˳���դ����롤
 * ���ΥΡ��ɤ�ͭ����ñ��Υꥹ�ȤǤ�. �ڹ�¤������ˤ����ơ�
 * ����ʬ�μ��ΥΡ��ɤ����Υꥹ�Ȥ��ݻ����ޤ�. �ºݤˤϥꥹ�Ȥ��Ѳ�����
 * ��ꡤ���ʤ���ڹ�¤������λޤ�ʬ�����˳���դ����ޤ�. 
 * �㤨�С��ʲ��Τ褦���ڹ�¤������ξ�硤�����ν񤤤Ƥ���Ρ��ɤ�
 * successor list ������դ����ޤ�. 
 * <pre>
 *
 *        2-o-o - o-o-o - o-o-o          word "A" 
 *       /
 *  1-o-o
 *       \       4-o-o                   word "B"
 *        \     /   
 *         3-o-o - 5-o-o - 7-o-o         word "C"
 *              \        \ 
 *               \        8-o-o          word "D"
 *                6-o-o                  word "E"
 * </pre>
 *
 * �� successor list �Ϥ��Υ��֥ĥ꡼�˴ޤޤ��ñ��Υꥹ�ȤǤ�. 
 * ������Ǥϰʲ��Τ褦�ˤʤ�ޤ�. 
 *
 * <pre>
 *   node  | successor list (wchmm->state[node].sc)
 *   =======================
 *     1   | A B C D E
 *     2   | A
 *     3   |   B C D E
 *     4   |   B
 *     5   |     C D
 *     6   |         E
 *     7   |     C
 *     8   |       D
 * </pre>
 *
 * ���� successor list �˴ޤޤ��ñ�줬���Ĥˤʤä��Ȥ������λ�����
 * ñ�줬���ꤹ��. �嵭�ξ�硤ñ�� "A" �ϥΡ��� 2 �ΰ��֤Ǥ��Ǥ�
 * ���θ�³ñ��Ȥ��� "A" �ʳ�̵���Τǡ������ǳ��ꤹ��. 
 * ���ʤ����ñ�� A �����Τʸ��쥹�����ϡ�ñ�콪ü���Ԥ����Ρ��� 2 �Ƿ�ޤ�. 
 *
 * �裱�ѥ��ˤ����� factoring �η׻��ϡ��ºݤˤ� beam.c �ǹԤʤ���. 
 * 2-gram factoring�ξ�硤���Ρ��ɤ� successor list ��¸�ߤ����,
 * ���� successor list ��ñ��� 2-gram �κ����ͤ���, ���¤��Ƥ��Ƥ���
 * factoring �ͤ򹹿�����. successor list ��ñ�줬1�ĤΥΡ��ɤǤϡ�
 * ������2-gram����ưŪ�˳�����Ƥ���. 
 * 1-gram factoring�ξ�硤���Ρ��ɤ� successor list ��¸�ߤ����硤
 * ���� successor list ��ñ��� 1-gram �κ����ͤ��ᡤ���¤��Ƥ��Ƥ���
 * factoring �ͤ򹹿�����. successor list ��ñ�줬���ĤΥΡ��ɤǡ��Ϥ����
 * 2-gram ��׻�����. 
 *
 * �ºݤǤ� 1-gram factoring �Ǥϳ� successor list �ˤ����� factoring ��
 * ��ñ����������¸�ʤΤǡ�successor list ���ۻ������Ƥ��餫����׻�����
 * ����. ���ʤ�������󥸥�ư�����ڹ�¤��������۸塤successor list
 * ���ۤ����顤ñ���2�İʾ�ޤ� successor list �ˤĤ��ƤϤ��� 1-gram ��
 * �����ͤ�׻����ơ�����򤽤ΥΡ��ɤ� fscore ���Ф˳�Ǽ���Ƥ���������
 * successor list �� free ���Ƥ��ޤ��Ф褤. ñ�줬���ĤΤߤ� successor list
 * �ˤĤ��ƤϤ���ñ��ID��Ĥ��Ƥ�����õ�����˥ѥ�����������ã������
 * ���Τ�2-gram��׻�������ɤ�. 
 *
 * DFAʸˡ���ѻ��ϡ��ǥե���ȤǤϸ�������(���ƥ���������)��
 * ���ƥ���ñ�̤��ڤ��ۤ��뤳�Ȥ���Ū��ɽ������. ���Τ��ᡤ
 * ������ factoring �������Ѥ����ʤ�. ��������
 * CATEGORY_TREE �� undefined �Ǥ���С�����Ū factoring ���Ѥ�����������
 * Ŭ�Ѥ�Ԥ����Ȥ��ǽ�Ǥ���. 
 * ���ʤ�������Ρ��ɤ� successor list ��¸�ߤ����,
 * ���� successor list ��γ�ñ���ľ��ñ���ñ���������Ĵ��,
 * ���Τ�����ĤǤ���³��ǽ��ñ�줬����С��������ܤ��������Ĥ�
 * �ʤ�������ܤ����ʤ�. ���ε�ǽ�ϵ��ѻ��ͤΤ���˻Ĥ���Ƥ���ΤߤǤ���. 
 * </JA>
 * 
 * <EN>
 * @brief  LM factoring on 1st pass.
 * </EN>
 *
 * This file contains functions to do language score factoring on the 1st
 * pass.  They build a successor lists which holds the successive words in
 * each sub tree on the tree lexicon, and also provide a factored LM
 * probability on each nodes on the tree lexicon.
 *
 * The "successor list" will be assigned for each lexicon tree node to
 * represent a list of words that exist in the sub-tree and share the node.
 * Actually they will be assigned to the branch node.
 * Below is the example of successor lists on a tree lexicon, in which
 * the lists is assigned to the numbered nodes.
 * 
 * <pre>
 *         2-o-o - o-o-o - o-o-o          word "A" 
 *        /
 *   1-o-o
 *        \       4-o-o                   word "B"
 *         \     /   
 *          3-o-o - 5-o-o - 7-o-o         word "C"
 *           \            \ 
 *            \            8-o-o          word "D"
 *             6-o-o                      word "E"
 * </pre>
 *
 * The contents of the successor lists are the following:
 *
 * <pre>
 *  node  | successor list (wchmm->state[node].sc)
 *  =======================
 *    1   | A B C D E
 *    2   | A
 *    3   |   B C D E
 *    4   |   B
 *    5   |     C D
 *    6   |         E
 *    7   |     C
 *    8   |       D
 * </pre>
 *
 * When the 1st pass proceeds, if the next going node has a successor list,
 * all the word 2-gram scores in the successor list on the next node
 * will be computed, and the propagating LM value in the token on
 * the current node will be replaced by the maximum value of the scores
 * when copied to the next node.  Appearently, if the successor list has
 * only one word, it means that the word can be determined on that point,
 * and the precise 2-gram value will be assigned as is.
 *
 * When using 1-gram factoring, the computation will be slightly different.
 * Since the factoring value (maximum value of 1-gram scores on each successor
 * list) is independent of the word context, they can be computed statically
 * before the search.  Thus, for all the successor lists that have more than
 * two words, the maximum 1-gram value is computed and stored to
 * "fscore" member in tree lexicon, and the successor lists will be freed.
 * The successor lists with only one word should still remain in the
 * tree lexicon, to compute the precise 2-gram scores for the words.
 *
 *
 * When using DFA grammar, Julian builds separated lexicon trees for every
 * word categories, to statically express the catergory-pair constraint.
 * Thus these factoring scheme is not used by default.
 * However you can still force Julian to use the grammar-based
 * deterministic factoring scheme by undefining CATEGORY_TREE.
 * If CATEGORY_TREE is undefined, the word connection constraint will be
 * performed based on the successor list at the middle of tree lexicon.
 * This enables single tree search on Julian.  This function is left
 * only for technical reference.
 * 
 * @author Akinobu LEE
 * @date   Mon Mar  7 23:20:26 2005
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

/*----------------------------------------------------------------------*/

/** 
 * <JA>
 * �ڹ�¤�����������Ρ��ɤ� successor list ���ۤ���ᥤ��ؿ�
 * 
 * @param wchmm [i/o] �ڹ�¤������
 * </JA>
 * <EN>
 * Main function to build whole successor list to lexicon tree.
 * 
 * @param wchmm [i/o] tree lexicon
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
void
make_successor_list(WCHMM_INFO *wchmm)
{
  int node;
  WORD_ID w;
  int i, j;
  int s;
  WORD_ID *scnumlist;
  WORD_ID *sclen;
  int scnum, new_scnum;
  int *scidmap;
  boolean *freemark;

  jlog("STAT: make successor lists for factoring\n");

  /* 1. initialize */
  /* initialize node->sclist index on wchmm tree */
  for (node=0;node<wchmm->n;node++) wchmm->state[node].scid = 0;

  /* parse the tree to assign unique scid and get the maximum size of
     successor list */
  scnum = 1;
  for (w=0;w<wchmm->winfo->num;w++) {
    for (i=0;i<wchmm->winfo->wlen[w];i++) {
      if (wchmm->state[wchmm->offset[w][i]].scid == 0) {
	wchmm->state[wchmm->offset[w][i]].scid = scnum;
	scnum++;
      }
    }
    if (wchmm->state[wchmm->wordend[w]].scid == 0) {
      wchmm->state[wchmm->wordend[w]].scid = scnum;
      scnum++;
    }
  }
  if (debug2_flag) {
    jlog("DEBUG: initial successor list size = %d\n", scnum);
  }

  /* 2. count number of each successor */
  sclen = (WORD_ID *)mymalloc(sizeof(WORD_ID) * scnum);
  for (i=1;i<scnum;i++) sclen[i] = 0;
  for (w=0;w<wchmm->winfo->num;w++) {
    for (i=0;i<wchmm->winfo->wlen[w];i++) {
      sclen[wchmm->state[wchmm->offset[w][i]].scid]++;
    }
    sclen[wchmm->state[wchmm->wordend[w]].scid]++;
  }

  /* 3. delete bogus successor lists */
  freemark = (boolean *)mymalloc(sizeof(boolean) * scnum);
  for (i=1;i<scnum;i++) freemark[i] = FALSE;
  for (w=0;w<wchmm->winfo->num;w++) {
    node = wchmm->wordend[w];	/* begin from the word end node */
    i = wchmm->winfo->wlen[w]-1;
    while (i >= 0) {		/* for each phoneme start node */
      if (node == wchmm->offset[w][i]) {
	/* word with only 1 state: skip */
	i--;
	continue;
      }
      if (wchmm->state[node].scid == 0) break; /* already parsed */
      if (sclen[wchmm->state[node].scid] == sclen[wchmm->state[wchmm->offset[w][i]].scid]) {
	freemark[wchmm->state[node].scid] = TRUE;	/* mark the node */
	wchmm->state[node].scid = 0;
      }
      node = wchmm->offset[w][i];
      i--;
    }
  }
  /* build compaction map */
  scidmap = (int *)mymalloc(sizeof(int) * scnum);
  scidmap[0] = 0;
  j = 1;
  for (i=1;i<scnum;i++) {
    if (freemark[i]) {
      scidmap[i] = 0;
    } else {
      scidmap[i] = j;
      j++;
    }
  }
  new_scnum = j;
  if (debug2_flag) {
    jlog("DEBUG: compacted successor list size = %d\n", new_scnum);
  }

  /* 4. rewrite scid and do compaction for new sclen */
  for (node=0;node<wchmm->n;node++) {
    if (wchmm->state[node].scid > 0) {
      wchmm->state[node].scid = scidmap[wchmm->state[node].scid];
    }
  }
  wchmm->sclen = (WORD_ID *)mybmalloc2(sizeof(WORD_ID) * new_scnum, &(wchmm->malloc_root));
  for (i=1;i<scnum;i++) {
    if (scidmap[i] != 0) wchmm->sclen[scidmap[i]] = sclen[i];
  }
  wchmm->scnum = new_scnum;

  free(scidmap);
  free(freemark);
  free(sclen);

  /* 5. now index completed, make word list for each list */
  wchmm->sclist = (WORD_ID **)mybmalloc2(sizeof(WORD_ID *) * wchmm->scnum, &(wchmm->malloc_root));
  scnumlist = (WORD_ID *)mymalloc(sizeof(WORD_ID) * wchmm->scnum);
  for(i=1;i<wchmm->scnum;i++) {
    wchmm->sclist[i] = (WORD_ID *)mybmalloc2(sizeof(WORD_ID) * wchmm->sclen[i], &(wchmm->malloc_root));
    scnumlist[i] = 0;
  }
  {
    int scid;
    for (w=0;w<wchmm->winfo->num;w++) {
      for (i=0;i<wchmm->winfo->wlen[w];i++) {
	scid = wchmm->state[wchmm->offset[w][i]].scid;
	if (scid != 0) {
	  wchmm->sclist[scid][scnumlist[scid]] = w;
	  scnumlist[scid]++;
	  if (scnumlist[scid] > wchmm->sclen[scid]) {
	    jlog("hogohohoho\n");
	    exit(1);
	  }
	}
      }
      /* at word end */
      scid = wchmm->state[wchmm->wordend[w]].scid;
      if (scid != 0) {
	wchmm->sclist[scid][scnumlist[scid]] = w;
	scnumlist[scid]++;
	  if (scnumlist[scid] > wchmm->sclen[scid]) {
	    jlog("hogohohoho\n");
	    exit(1);
	  }
      }
    }
  }
  free(scnumlist);

  jlog("STAT: done\n");
}

#ifdef UNIGRAM_FACTORING

/** 
 * <JA>
 * �ڹ�¤�����������Ρ��ɤ� successor list ���ۤ���ᥤ��ؿ�(unigram factoring ��
 * 
 * @param wchmm [i/o] �ڹ�¤������
 * </JA>
 * <EN>
 * Main function to build whole successor list to lexicon tree for unigram factoring
 * 
 * @param wchmm [i/o] tree lexicon
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
void
make_successor_list_unigram_factoring(WCHMM_INFO *wchmm)
{

#ifndef FAST_FACTOR1_SUCCESSOR_LIST

  /* old way */
  make_successor_list(wchmm);
  calc_all_unigram_factoring_values(wchmm);

#else  /* ~FAST_FACTOR1_SUCCESSOR_LIST */

  /* new way */

  int node, node2;
  WORD_ID w, w2;
  int i, j, n, f;
  int s;
  LOGPROB tmpprob;

  jlog("STAT: make successor lists for unigram factoring\n");

  /* 1. initialize */
  /* initialize node->sclist index on wchmm tree */
  for (node=0;node<wchmm->n;node++) wchmm->state[node].scid = 0;

  /* in unigram factoring, number of successor = vocabulary size */
  wchmm->scnum = wchmm->winfo->num + 1;
  if (debug2_flag) {
    jlog("DEBUG: successor list size = %d\n", wchmm->scnum);
  }

  /* allocate successor list for 1-gram factoring */
  wchmm->scword = (WORD_ID *)mybmalloc2(sizeof(WORD_ID) * wchmm->scnum, &(wchmm->malloc_root));

  /* 2. make successor list, and count needed fscore num */
  f = 1;
  s = 1;
  for (w=0;w<wchmm->winfo->num;w++) {
    for (i=0;i<wchmm->winfo->wlen[w] + 1;i++) {
      if (i < wchmm->winfo->wlen[w]) {
	node = wchmm->offset[w][i];
      } else {
	node = wchmm->wordend[w];
      }
      if (wchmm->state[node].scid == 0) { /* not assigned */
	/* new node found, assign new and exit here */
	wchmm->state[node].scid = s;
	wchmm->scword[s] = w;
	s++;
	if (s > wchmm->scnum) {
	  jlog("InternalError: make_successor_list_unigram_factoring: scid num exceeded?\n");
	  return;
	}
	break;
      } else if (wchmm->state[node].scid > 0) {
	/* that node has successor */
	/* move it to the current first isolated node in that word */
	w2 = wchmm->scword[wchmm->state[node].scid];
	for(j=i+1;j<wchmm->winfo->wlen[w2] + 1;j++) {
	  if (j < wchmm->winfo->wlen[w2]) {
	    node2 = wchmm->offset[w2][j];
	  } else {
	    node2 = wchmm->wordend[w2];
	  }
	  if (wchmm->state[node2].scid == 0) { /* not assigned */
	    /* move successor to there */
	    wchmm->state[node2].scid = wchmm->state[node].scid;
	    break;
	  }
	}
	if (j >= wchmm->winfo->wlen[w2] + 1) {
	  /* not found? */
	  jlog("InternalError: make_successor_list_unigram_factoring: no isolated word for %d\n", w2);
	  return;
	}
	/* make current node as fscore node */
	n = f++;
	wchmm->state[node].scid = -n;
	/* not compute unigram factoring value yet */
      }

    }
  }

  /* 2. allocate fscore buffer */
  wchmm->fsnum = f;
  wchmm->fscore = (LOGPROB *)mymalloc(sizeof(LOGPROB) * wchmm->fsnum);
  for(n=0;n<wchmm->fsnum;n++) wchmm->fscore[n] = LOG_ZERO;

  /* 3. parse again to assign fscore values */
  for (w=0;w<wchmm->winfo->num;w++) {
    for (i=0;i<wchmm->winfo->wlen[w] + 1;i++) {
      if (i < wchmm->winfo->wlen[w]) {
	node = wchmm->offset[w][i];
      } else {
	node = wchmm->wordend[w];
      }
      if (wchmm->state[node].scid < 0) {
	/* update max */
	if (wchmm->ngram) {
	  tmpprob = uni_prob(wchmm->ngram, wchmm->winfo->wton[w])
#ifdef CLASS_NGRAM
	    + wchmm->winfo->cprob[w]
#endif
	    ;
	} else {
	  tmpprob = LOG_ZERO;
	}
	if (wchmm->lmvar == LM_NGRAM_USER) {
	  tmpprob = (*(wchmm->uni_prob_user))(wchmm->winfo, w, tmpprob);
	}
	n = - wchmm->state[node].scid;
	if (wchmm->fscore[n] < tmpprob) {
	  wchmm->fscore[n] = tmpprob;
	}
      }

    }
  }

#endif  /* ~FAST_FACTOR1_SUCCESSOR_LIST */

  jlog("STAT: done\n");
}

#endif /* UNIGRAM_FACTORING */


/** 
 * <JA>
 * ���ۤ��줿 factoring ����� multipath �Ѥ�Ĵ������. factoring �����,
 * ��ǥ����Τ򥹥��åפ������ܤ�������Ϥ�����β��Ǥإ��ԡ�����. 
 * �ޤ���(���Ϥ�����ʤ�)ʸƬʸˡ�Ρ��ɤ�ñ����Ƭ�Ρ��ɤ��饳�ԡ�����. 
 * 
 * @param wchmm [in] �ڹ�¤������
 * </JA>
 * <EN>
 * Adjust factoring data in tree lexicon for multipath transition handling.
 * 
 * @param wchmm [in] tree lexicon
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
void
adjust_sc_index(WCHMM_INFO *wchmm)
{
  WORD_ID w;
  int i,j,k;
  HMM_Logical *ltmp;
  int ltmp_state_num;
  int ato;
  LOGPROB prob;
  int node, scid;
  A_CELL2 *ac;
  
  /* duplicate scid for HMMs with more than one arc from initial state */
  for(w=0;w<wchmm->winfo->num;w++) {
    for(k=0;k<wchmm->winfo->wlen[w];k++) {
      node = wchmm->offset[w][k];
      scid = wchmm->state[node].scid;
      if (scid == 0) continue;
      ltmp = wchmm->winfo->wseq[w][k];
      ltmp_state_num = hmm_logical_state_num(ltmp);
      if ((hmm_logical_trans(ltmp))->a[0][ltmp_state_num-1] != LOG_ZERO) {
	j = k + 1;
	if (j == wchmm->winfo->wlen[w]) {
	  if (wchmm->state[wchmm->wordend[w]].scid == 0) {
	    jlog("STAT: word %d: factoring node copied for skip phone\n", w);
	    wchmm->state[wchmm->wordend[w]].scid = scid;
	  }
	} else {
	  if (wchmm->state[wchmm->offset[w][j]].scid == 0) {
	    jlog("STAT: word %d: factoring node copied for skip phone\n", w);
	    wchmm->state[wchmm->offset[w][j]].scid = scid;
	  }
	}
      }
      for(ato=1;ato<ltmp_state_num;ato++) {
	prob = (hmm_logical_trans(ltmp))->a[0][ato];
	if (prob != LOG_ZERO) {
	  wchmm->state[node+ato-1].scid = scid;
	}
      }
    }
  }

  /* move scid and fscore on the head state to the head grammar state */
  for(i=0;i<wchmm->startnum;i++) {
    node = wchmm->startnode[i];
    if (wchmm->state[node].out.state != NULL) {
      j_internal_error("adjust_sc_index: outprob exist in word-head node??\n");
    }
    if (wchmm->next_a[node] != LOG_ZERO) {
      if (wchmm->state[node+1].scid != 0) {
	if (wchmm->state[node].scid != 0 && wchmm->state[node].scid != wchmm->state[node+1].scid) {
	  j_internal_error("adjust_sc_index: different successor list within word-head phone?\n");
	}
	wchmm->state[node].scid = wchmm->state[node+1].scid;
	wchmm->state[node+1].scid = 0;
      }
    }
    for(ac=wchmm->ac[node];ac;ac=ac->next) {
      for(k=0;k<ac->n;k++) {
	if (wchmm->state[ac->arc[k]].scid != 0) {
	  if (wchmm->state[node].scid != 0 && wchmm->state[node].scid != wchmm->state[ac->arc[k]].scid) {
	    j_internal_error("adjust_sc_index: different successor list within word-head phone?\n");
	  }
	  wchmm->state[node].scid = wchmm->state[ac->arc[k]].scid;
	  wchmm->state[ac->arc[k]].scid = 0;
	}
      }
    }
  }
}


/* -------------------------------------------------------------------- */
/* factoring computation */

/** 
 * <JA>
 * �ڹ�¤�������Ѥ� factoring ����å����������դ����ƽ��������. 
 * ���δؿ��ϥץ���೫�ϻ��˰��٤����ƤФ��. 
 * 
 * @param wchmm [i/o] �ڹ�¤������
 * </JA>
 * <EN>
 * Initialize factoring cache for a tree lexicon, allocating memory for
 * cache.  This should be called only once on start up.
 * 
 * @param wchmm [i/o] tree lexicon
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
void
max_successor_cache_init(WCHMM_INFO *wchmm)
{
  int i;
  LM_PROB_CACHE *l;
  WORD_ID wnum;

  /* for word-internal */
  l = &(wchmm->lmcache);

  l->probcache = (LOGPROB *) mymalloc(sizeof(LOGPROB) * wchmm->scnum);
  l->lastwcache = (WORD_ID *) mymalloc(sizeof(WORD_ID) * wchmm->scnum);
  for (i=0;i<wchmm->scnum;i++) {
    l->lastwcache[i] = WORD_INVALID;
  }
  /* for cross-word */
  if (wchmm->ngram) {
    wnum = wchmm->ngram->max_word_num;
  } else {
    wnum = wchmm->winfo->num;
  }
#ifdef HASH_CACHE_IW
  l->iw_cache_num = wnum * jconf.search.pass1.iw_cache_rate / 100;
  if (l->iw_cache_num < 10) l->iw_cache_num = 10;
#else
  l->iw_cache_num = wnum;
#endif /* HASH_CACHE_IW */
  l->iw_sc_cache = (LOGPROB **)mymalloc(sizeof(LOGPROB *) * l->iw_cache_num);
  for (i=0;i<l->iw_cache_num;i++) {
    l->iw_sc_cache[i] = NULL;
  }
#ifdef HASH_CACHE_IW
  l->iw_lw_cache = (WORD_ID *)mymalloc(sizeof(WORD_ID) * l->iw_cache_num);
  for (i=0;i<l->iw_cache_num;i++) {
    l->iw_lw_cache[i] = WORD_INVALID;
  }
#endif
}

/** 
 * <JA>
 * ñ��֤� factoring cache �Υ����ΰ���������. 
 * 
 * @param wchmm [i/o] �ڹ�¤������
 * </JA>
 * <EN>
 * Free cross-word factoring cache.
 * 
 * @param wchmm [i/o] tree lexicon
 * </EN>
 */
static void
max_successor_prob_iw_free(WCHMM_INFO *wchmm)
{
  int i;
  LM_PROB_CACHE *l;
  l = &(wchmm->lmcache);
  for (i=0;i<l->iw_cache_num;i++) {
    if (l->iw_sc_cache[i] != NULL) free(l->iw_sc_cache[i]);
    l->iw_sc_cache[i] = NULL;
  }
}

/** 
 * <JA>
 * factoring �� cache �Υ����ΰ�����Ʋ�������. 
 * 
 * @param wchmm [i/o] �ڹ�¤������
 * </JA>
 * <EN>
 * Free all memory for  factoring cache.
 * 
 * @param wchmm [i/o] tree lexicon
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
void
max_successor_cache_free(WCHMM_INFO *wchmm)
{
  free(wchmm->lmcache.probcache);
  free(wchmm->lmcache.lastwcache);
  max_successor_prob_iw_free(wchmm);
  free(wchmm->lmcache.iw_sc_cache);
#ifdef HASH_CACHE_IW
  free(wchmm->lmcache.iw_lw_cache);
#endif
}

#ifdef UNIGRAM_FACTORING

/** 
 * <JA>
 * @brief  ñ����Ƭ�Ρ��ɤΤ���Factoring �ˤ����ƥ���å��夬ɬ�פʥΡ��ɤ�
 * �ꥹ�Ȥ��������. 
 *
 * 1-gram factoring �ϡ��ޥΡ��ɤˤ�����ľ��ñ��˰�¸���ʤ�������
 * (unigram�κ�����)��Ϳ����. ���Τ��ᡤñ��֤� factoring �׻��ˤ����ơ�
 * �ڹ�¤��������ʣ����ñ��Ƕ�ͭ����Ƥ���ñ����Ƭ�Ρ��ɤˤĤ��Ƥϡ�
 * �����ͤ�ľ��ñ��ˤ�餺�����ͤǤ��ꡤǧ������ñ��֥���å�����ݻ�
 * ����ɬ�פϤʤ�. 
 * 
 * ���δؿ��Ǥϡ�ñ����Ƭ�Ρ��ɤΥꥹ�Ȥ��餽�Τ褦�� factoring ����å��夬
 * ���פʥΡ��ɤ�������ơ�1-gram factoring ����ñ��֥���å��夬ɬ�פ�
 * ñ����Ƭ�Ρ��ɡʡ�¾��ñ��ȶ�ͭ����Ƥ��ʤ���Ω����ñ����Ƭ�Ρ��ɡˤ�
 * �ꥹ�Ȥ��������wchmm->start2isolate ����� wchmm->isolatenum �˳�Ǽ����. 
 * 
 * @param wchmm [i/o] �ڹ�¤������
 * </JA>
 * <EN>
 * @brief  Make a list of word head nodes on which cross-word factoring cache
 * is needed.
 *
 * On 1-gram factoring, the branch nodes on tree lexicon has a fixed
 * factoring value (maximum 1-gram score of all sub-tree words).  Thus, when
 * computing cross-word factoring at word head nodes on inter-word
 * transition, such 1-gram factoring nodes on word head, shared by several
 * words, need not be cached in inter-word factoring cache.
 *
 * This function make a list of word-head nodes which requires inter-word
 * factoring caching (i.e. isolated word head nodes, does not shared by other
 * words) from the existing list of word head nodes, and set it to
 * wchmm->start2isolate and wchmm->isolatenum.
 * 
 * @param wchmm [i/o] tree lexicon
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
void
make_iwcache_index(WCHMM_INFO *wchmm)
{
  int i, node, num;

  wchmm->start2isolate = (int *)mymalloc(sizeof(int) * wchmm->startnum);
  num = 0;
  for(i=0;i<wchmm->startnum;i++) {
    node = wchmm->startnode[i];
    if (wchmm->state[node].scid >= 0) {	/* not a factoring node (isolated node, has no 1-gram factoring value) */
      wchmm->start2isolate[i] = num;
      num++;
    } else {			/* factoring node (shared) */
      wchmm->start2isolate[i] = -1;
    }
  }
  wchmm->isolatenum = num;
}

#ifndef FAST_FACTOR1_SUCCESSOR_LIST

/** 
 * <JA>
 * @brief  �ڹ�¤�������� 1-gram factoring �ͤ�׻����Ƴ�Ǽ����. 
 *
 * 1-gram factoring �Ǥ�ñ��֤Ƕ�ͭ����Ƥ���ޥΡ��ɤǤ� 1-gram �κ�����
 * ��Ϳ����. ñ������ˤ��ʤ����ᡤ�����ͤ�ǧ����������
 * �׻����Ƥ������Ȥ��Ǥ���. ���δؿ����ڹ�¤������
 * ���ΤˤĤ��ơ���ͭ����Ƥ����successor list �ˣ��İʾ��ñ�����ĥΡ��ɡ�
 * �Ρ��ɤ� 1-gram factoring �ͤ�׻����Ƴ�Ǽ����. 1-gram factoring�ͤ�
 * �׻���ϡ����ΥΡ��ɤ� successor list �Ϥ�Ϥ����פǤ��뤿�ᡤ������
 * �������. 
 *
 * �ºݤˤϡ�factoring �ͤ� wchmm->fscore �˽缡��¸���졤�Ρ��ɤ�
 * scid �ˤ�����¸�ͤؤΥ���ǥå���(1-)������ͤ���Ǽ�����. ���פˤʤä�
 * successor list �ϡ��ºݤˤ� compaction_successor ��ǡ��б�����Ρ��ɤ�
 * scid ����ˤʤäƤ��� successor list �������뤳�ȤǹԤʤ���. 
 * 
 * @param wchmm [i/o] �ڹ�¤������
 * </JA>
 * <EN>
 * @brief  Calculate all the 1-gram factoring values on tree lexicon.
 *
 * On 1-gram factoring, the shared nodes on branch has fixed factoring score
 * from 1-gram values, independent of the word context on recognition.  So
 * the values are fixed for all recognition and can be calculated before
 * search.  This function stores all the neede 1-gram factoring value by
 * traversing tree lexicon with successor lists and compute maximum 1-gram
 * for each successor lists that has more than two words (=shared).
 * Since a successor list is no more neede after the 1-gram value is computed,
 * they will be freed.
 *
 * Actually, computed factoring scores will be stored in wchmm->fscore
 * sequencially, and the index value, starting from 1,
 * to the fscore list is stored in scid of each nodes as a negative value.
 * The free will be performed in compaction_successor() by checking if a
 * successor's corresponding scid on tree lexicon has negative value.
 * 
 * @param wchmm [i/o] tree lexicon
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
void
calc_all_unigram_factoring_values(WCHMM_INFO *wchmm)
{
  S_CELL *sc, *sctmp;
  LOGPROB tmpprob, maxprob;
  int i, n;

  /* count needed number of 1-gram factoring nodes */
  n = 0;
  for (i=1;i<wchmm->scnum;i++) {
    sc = wchmm->sclist[i];
    if (sc == NULL) {
      j_internal_error("call_all_unigram_factoring_values: sclist has no sc?\n");
    }
    if (sc->next != NULL) {
      /* more than two words, so compute maximum 1-gram probability */
      n++;
    }
  }
  wchmm->fsnum = n + 1;
  /* allocate area */
  wchmm->fscore = (LOGPROB *)mymalloc(sizeof(LOGPROB) * wchmm->fsnum);
  /* assign values */
  n = 1;
  for (i=1;i<wchmm->scnum;i++) {
    sc = wchmm->sclist[i];
    if (sc->next != NULL) {
      maxprob = LOG_ZERO;
      for (sctmp = sc; sctmp; sctmp = sctmp->next) {
	if (wchmm->ngram) {
	  tmpprob = uni_prob(wchmm->ngram, wchmm->winfo->wton[sctmp->word])
#ifdef CLASS_NGRAM
	    + wchmm->winfo->cprob[sctmp->word] 
#endif
	    ;
	} else {
	  tmpprob = LOG_ZERO;
	}
	if (wchmm->lmvar == LM_NGRAM_USER) {
	  tmpprob = (*(wchmm->uni_prob_user))(wchmm->winfo, sctmp->word, tmpprob);
	}
	if (maxprob < tmpprob) maxprob = tmpprob;
      }
      wchmm->fscore[n] = maxprob;
      free_successor(wchmm, i);
      wchmm->state[wchmm->sclist2node[i]].scid = - n;
      n++;
    }
  }
  /* garbage collection of factored sclist */
  compaction_successor(wchmm);
}

#endif

#else  /* ~UNIGRAM_FACTORING */

/** 
 * <JA>
 * �ڹ�¤�������Τ���Ρ��ɤˤĤ��ơ�Ϳ����줿ñ��������Ф���2-gram
 * ��������׻�����. 
 * 
 * @param wchmm [in] �ڹ�¤������
 * @param lastword [in] ľ��ñ��
 * @param node [in] @a wchmm ��ΥΡ����ֹ�
 * 
 * @return 2-gram ��Ψ. 
 * </JA>
 * <EN>
 * Compute 2-gram factoring value for the node and return the probability.
 * 
 * @param wchmm [in] tree lexicon
 * @param lastword [in] the last context word
 * @param node [in] node ID on @a wchmm
 * 
 * @return the log probability of 2-gram on that node.
 * </EN>
 * 
 */
static LOGPROB
calc_successor_prob(WCHMM_INFO *wchmm, WORD_ID lastword, int node)
{
  LOGPROB tmpprob, maxprob;
  WORD_ID lw, w;
  int i;
  int scid;

  maxprob = LOG_ZERO;
  if (wchmm->ngram) {
    lw = wchmm->winfo->wton[lastword];
  }

  scid = wchmm->state[node].scid;

  for (i = 0; i < wchmm->sclen[scid]; i++) {
    w = wchmm->sclist[scid][i];
    if (wchmm->ngram) {
      tmpprob = (*(wchmm->ngram->bigram_prob))(wchmm->ngram, lw , wchmm->winfo->wton[w])
#ifdef CLASS_NGRAM
	+ wchmm->winfo->cprob[w]
#endif
	;
    } else {
      tmpprob = LOG_ZERO;
    }
    if (wchmm->lmvar == LM_NGRAM_USER) {
      tmpprob = (*(wchmm->bi_prob_user))(wchmm->winfo, lastword, w, tmpprob);
    }
    if (maxprob < tmpprob) maxprob = tmpprob;
  }

  return(maxprob);
}

#endif  /* ~UNIGRAM_FACTORING */

/** 
 * <JA>
 * @brief  ñ����Τ���Ρ��ɤˤĤ��� factoring �ͤ�׻�����. 
 *
 * 1-gram factoring �Ǹ���factoring�ͤ�������Ϥ����ͤ�¨�¤��֤����. 
 * ¾�ξ��ϡ����ΥΡ��ɤΥ��֥ĥ꡼���ñ��� 2-gram��Ψ�ʤκ����͡ˤ�
 * �׻������. 
 *
 * ñ���� factoring ����å��夬��θ�����. ���ʤ���ƥΡ��ɤˤĤ���
 * ľ��ñ�줬���󥢥��������줿�Ȥ���Ʊ���Ǥ���С�
 * ������ͤ��֤��졤�����Ǥʤ�����ͤ�׻���������å��夬���������. 
 * 
 * @param wchmm [in] �ڹ�¤������
 * @param lastword [in] ľ��ñ���ID
 * @param node [in] �Ρ����ֹ�
 * 
 * @return �����ǥ륹����
 * </JA>
 * <EN>
 * @brief  compute factoring LM score for the given word-internal node.
 *
 * If it is a shared branch node and 1-gram factoring is used, the
 * constant factoring value which has already been assigned before search
 * will be returned immediately.  Else, the maximum 2-gram probability
 * of corresponding successor words are computed.
 *
 * The word-internal factoring cache is consulted within this function.
 * If the given last word is the same as the last call on that node,
 * the last computed value will be returned, else the maximum value
 * will be computed update the cache with the last word and value.
 * 
 * @param wchmm [in] tree lexicon
 * @param lastword [in] word ID of last context word
 * @param node [in] node ID
 * 
 * @return the LM factoring score.
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
LOGPROB
max_successor_prob(WCHMM_INFO *wchmm, WORD_ID lastword, int node)
{
  LOGPROB maxprob;
  WORD_ID last_nword, w;
  int scid;
  LM_PROB_CACHE *l;

  l = &(wchmm->lmcache);

  if (lastword != WORD_INVALID) { /* return nothing if no previous word */
    if (wchmm->ngram) {
      last_nword = wchmm->winfo->wton[lastword];
    } else {
      last_nword = lastword;
    }
    scid = wchmm->state[node].scid;
#ifdef UNIGRAM_FACTORING
    if (scid < 0) {
      /* return 1-gram factoring value already calced */
      return(wchmm->fscore[(- scid)]);
    } else {
      /* this node has only one successor */
      /* return precise 2-gram score */
      if (last_nword != l->lastwcache[scid]) {
	/* calc and cache */
	w = wchmm->scword[scid];
	if (wchmm->ngram) {
	  maxprob = (*(wchmm->ngram->bigram_prob))(wchmm->ngram, last_nword, wchmm->winfo->wton[w])
#ifdef CLASS_NGRAM
	    + wchmm->winfo->cprob[w]
#endif
	    ;
	} else {
	  maxprob = LOG_ZERO;
	}
	if (wchmm->lmvar == LM_NGRAM_USER) {
	  maxprob = (*(wchmm->bi_prob_user))(wchmm->winfo, lastword, w, maxprob);
	}
	l->lastwcache[scid] = last_nword;
	l->probcache[scid] = maxprob;
	return(maxprob);
      } else {
	/* return cached */
	return (l->probcache[scid]);
      }
    }
#else  /* UNIGRAM_FACTORING */
    /* 2-gram */
    if (last_nword != l->lastwcache[scid]) {
      maxprob = calc_successor_prob(wchmm, lastword, node);
      /* store to cache */
      l->lastwcache[scid] = last_nword;
      l->probcache[scid] = maxprob;
      return(maxprob);
    } else {
      return (l->probcache[scid]);
    }
#endif /* UNIGRAM_FACTORING */
  } else {
    return(0.0);
#if 0
    maxprob = LOG_ZERO;
    for (sc=wchmm->state[node].sc;sc;sc=sc->next) {
      tmpprob = uni_prob(wchmm->ngram, sc->word);
      if (maxprob < tmpprob) maxprob = tmpprob;
    }
    return(maxprob);
#endif
  }

}

/** 
 * <JA>
 * @brief  ñ��֤� factoring �ͤΥꥹ�Ȥ��֤�. 
 *
 * Ϳ����줿ľ��ñ����Ф��ơ�factoring�ͤ�׻����٤����Ƥ�ñ����Ƭ�ؤ�
 * factoring �ͤ�׻��������Υꥹ�Ȥ��֤�. ����factoring�ͤ�
 * ľ��ñ�줴�Ȥ˥ꥹ��ñ�̤ǥ���å��夵���. ���ʤ��������ľ��ñ�줬
 * ����ޤǤ˰��٤Ǥ�ľ��ñ��Ȥ��ƽи����Ƥ�����硤���Υꥹ�Ȥ򤽤Τޤ�
 * �֤�. 
 * 
 * @param wchmm [in] �ڹ�¤������
 * @param lastword [in] ľ��ñ��
 * 
 * @return ��ñ����Ƭ�Ρ��ɤؤ� factoring �������Υꥹ��
 * </JA>
 * <EN>
 * @brief  Compute cross-word facgtoring values for word head nodes and return
 * the list.
 *
 * Given a last word, this function compute the factoring LM scores for all
 * the word head node to which the context-dependent (not 1-gram) factoring
 * values should be computed.  The resulting list of factoring values are
 * cached within this function per the last word.
 * 
 * @param wchmm [in] tree lexicon
 * @param lastword [in] last word
 * 
 * @return the list of factoring LM scores for all the needed word-head nodes.
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
LOGPROB *
max_successor_prob_iw(WCHMM_INFO *wchmm, WORD_ID lastword)
{
  int i, j, x, node;
  int last_nword;
  WORD_ID w;
  LM_PROB_CACHE *l;
  LOGPROB p;

  l = &(wchmm->lmcache);

  if (wchmm->ngram) {
    last_nword = wchmm->winfo->wton[lastword];
  } else {
    last_nword = lastword;
  }

#ifdef HASH_CACHE_IW
  x = last_nword % l->iw_cache_num;
  if (l->iw_lw_cache[x] == last_nword) { /* cache hit */
    return(l->iw_sc_cache[x]);
  }
#else  /* full cache */
  if (l->iw_sc_cache[last_nword] != NULL) { /* cache hit */
    return(l->iw_sc_cache[last_nword]);
  }
  x = last_nword;
  /* cache mis-hit, calc probs and cache them as new */
#endif
  /* allocate cache memory */
  if (l->iw_sc_cache[x] == NULL) {
#ifdef UNIGRAM_FACTORING
    l->iw_sc_cache[x] = (LOGPROB *)mymalloc(sizeof(LOGPROB)*wchmm->isolatenum);
#else
    l->iw_sc_cache[x] = (LOGPROB *)mymalloc(sizeof(LOGPROB)*wchmm->startnum);
#endif
    if (l->iw_sc_cache[x] == NULL) { /* malloc failed */
      /* clear existing cache, and retry */
      max_successor_prob_iw_free(wchmm);
      jlog("STAT: inter-word LM cache (%dMB) rehashed\n",
	       (l->iw_cache_num * 
#ifdef UNIGRAM_FACTORING
		wchmm->isolatenum
#else
		wchmm->startnum
#endif
		) / 1000 * sizeof(LOGPROB) / 1000);
#ifdef UNIGRAM_FACTORING
      l->iw_sc_cache[x] = (LOGPROB *)mymalloc(sizeof(LOGPROB)*wchmm->isolatenum);
#else
      l->iw_sc_cache[x] = (LOGPROB *)mymalloc(sizeof(LOGPROB)*wchmm->startnum);
#endif
      if (l->iw_sc_cache[x] == NULL) { /* malloc failed again? */
	j_internal_error("max_successor_prob_iw: cannot malloc\n");
      }
    }
  }

  /* calc prob for all startid */
#ifdef UNIGRAM_FACTORING
  for (j=0;j<wchmm->startnum;j++) {
    i = wchmm->start2isolate[j];
    if (i == -1) continue;
    node = wchmm->startnode[j];
    if (wchmm->state[node].scid <= 0) {
      /* should not happen!!! below is just for debugging */
      j_internal_error("max_successor_prob_iw: isolated (not shared) tree root node has unigram factoring value??\n");
    } else {
      w = wchmm->scword[wchmm->state[node].scid];
      if (wchmm->ngram) {
	p = (*(wchmm->ngram->bigram_prob))(wchmm->ngram, last_nword, wchmm->winfo->wton[w])
#ifdef CLASS_NGRAM
	  + wchmm->winfo->cprob[w]
#endif
	  ;
      } else {
	p = LOG_ZERO;
      }
      if (wchmm->lmvar == LM_NGRAM_USER) {
	p = (*(wchmm->bi_prob_user))(wchmm->winfo, lastword, w, p);
      }
      l->iw_sc_cache[x][i] = p;
    }
  }
#else  /* ~UNIGRAM_FACTORING */
  for (i=0;i<wchmm->startnum;i++) {
    node = wchmm->startnode[i];
    l->iw_sc_cache[x][i] = calc_successor_prob(wchmm, lastword, node);
  }
#endif
#ifdef HASH_CACHE_IW
  l->iw_lw_cache[x] = last_nword;
#endif

  return(l->iw_sc_cache[x]);
}

/** 
 * <JA>
 * @brief  ʸˡ�ˤ��ñ�������Ū factoring
 *
 * Julian �ˤ����� CATEGORY_TREE ���������Ƥ���Ȥ��ʥǥե���ȡˡ�
 * �ڹ�¤������ϥ��ƥ���ñ�̡ʤ��ʤ����ʸ����ε���ñ�̡ˤǹ��ۤ���뤿�ᡤ
 * ��1�ѥ��Ǥθ����ǥ�Ǥ��륫�ƥ����������ñ��λϽ�ü��Ŭ�ѤǤ���. 
 * 
 * ���� CATEGORY_TREE ���������Ƥ��ʤ���硤�ڹ�¤�������
 * �������Τ�ñ����ڤ�����뤿�ᡤ���ƥ���������� N-gram (Julius) ��
 * Ʊ�ͤ�ñ����� factoring ��Ʊ�ͤε�����Ŭ�Ѥ����ɬ�פ�����. 
 *
 * ���δؿ��� CATEGORY_TREE ���������Ƥ��ʤ��Ȥ��ˡ��嵭�� factoring
 * �ʷ���Ū factoring �ȸƤФ��ˤ�Ԥʤ�������󶡤���Ƥ���. 
 * 
 * @param wchmm [in] �ڹ�¤������
 * @param lastword [in] ľ��ñ��
 * @param node [in] �Ρ����ֹ�
 *
 * @return ���ƥ�������夽�λޤؤ����ܤ��������� TRUE, �Բ�ǽ�Ǥ���� FALSE
 * </JA>
 * <EN>
 * @brief  Deterministic factoring for grammar-based recognition (Julian)
 *
 * If CATEGORY_TREE is defined (this is default) on Julian, the tree lexicon
 * will be organized per category and the category-pair constraint used
 * in the 1st pass can be applied statically at cross-word transition.
 *
 * If the CATEGORY_TREE is not defined, a single tree lexicon will be
 * constucted for a whole dictionary.  In this case, the category-pair
 * constraint should be applied dynamically in the word-internal transition,
 * like the factoring scheme with N-gram (Julius).
 *
 * This function provides such word-internal factoring for grammar-based
 * recognition (called deterministic factoring) when CATEGORY_TREE is
 * undefined in Julian.
 * 
 * @param wchmm [in] tree lexicon
 * @param lastword [in] last word
 * @param node [in] node ID to check the constraint
 * 
 * @return TRUE if the transition to the branch is allowed on the category-pair
 * constraint, or FALSE if not allowed.
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
boolean
can_succeed(WCHMM_INFO *wchmm, WORD_ID lastword, int node)
{
  int lc;
  int i;
  int s;

  /* return TRUE if at least one subtree word can connect */

  s = wchmm->state[node].scid;

  if (lastword == WORD_INVALID) { /* case at beginning-of-word */
    for (i = 0; i < wchmm->sclen[s]; i++) {
      if (dfa_cp_begin(wchmm->dfa, wchmm->sclist[s][i]) == TRUE) return(TRUE);
    }
    return(FALSE);
  } else {
    lc = wchmm->winfo->wton[lastword];
    for (i = 0; i < wchmm->sclen[s]; i++) {
      if (dfa_cp(wchmm->dfa, lc, wchmm->sclist[s][i]) == TRUE) return(TRUE);
    }
    return(FALSE);
  }
}

/* end of file */
