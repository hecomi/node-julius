/**
 * @file   ngram_decode.c
 * 
 * <JA>
 * @brief  N-gram��Ψ�˴�Ť���ñ��ͽ¬����2�ѥ��� 
 *
 * Julius ��N-gram���Ѥ��������å��ǥ����ǥ���(��2�ѥ�)�ˤ����ơ�
 * ������³������ñ��ν������ꤹ��. 
 * 
 * Ϳ����줿Ÿ��������λ�ü�ե졼���ͽ¬����ñ��ȥ�ꥹ���
 * ����ͽ¬�ե졼����դ˽�ü��¸�ߤ���ñ��ν����
 * ����N-gram�и���Ψ�ȤȤ���֤�. 
 *
 * Julius �Ǥ� ngram_firstwords(), ngram_nextwords(), ngram_acceptable() ��
 * ���줾����2�ѥ��Υᥤ��ؿ� wchmm_fbs() ����ƤӽФ����. �ʤ���
 * Julian �ǤϤ����δؿ�������� dfa_decode.c �δؿ����Ѥ�����. 
 * </JA>
 * 
 * <EN>
 * @brief N-gram based word prediction for the 2nd pass.
 *
 * These functions returns next word candidates in the 2nd recognition
 * pass of Julius, i.e. N-gram based stack decoding.
 *
 * Given a partial sentence hypothesis, it first estimate the beginning frame
 * of the hypothesis based on the word trellis.  Then the words in the word
 * trellis around the estimated frame are extracted from the word trellis.
 * They will be returned with their N-gram probabilities.
 *
 * In Julius, ngram_firstwords(), ngram_nextwords() and ngram_acceptable()
 * are called from main search function wchmm_fbs().  In Julian,
 * corresponding functions in dfa_decode.c will be used instead.
 * </EN>
 * 
 * @author Akinobu Lee
 * @date   Fri Jul  8 14:57:51 2005
 *
 * $Revision: 1.8 $
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
 * ��ñ����䥽������ qsort ������Хå��ؿ�. 
 * 
 * @param a [in] ����1
 * @param b [in] ����2
 * 
 * @return a��ñ��ID > b��ñ��ID �ʤ�1, �դʤ� -1, Ʊ���ʤ� 0 ���֤�. 
 * </JA>
 * <EN>
 * qsort callback function to sort next word candidates by their word ID.
 * 
 * @param a [in] element 1
 * @param b [in] element 2
 * 
 * @return 1 if word id of a > that of b, -1 if negative, 0 if equal.
 * </EN>
 */
static int
compare_nw(NEXTWORD **a, NEXTWORD **b)
{
  if ((*a)->id > (*b)->id) return 1;
  if ((*a)->id < (*b)->id) return -1;
  return 0;
}

/** 
 * <JA>
 * ��ñ�����ꥹ���⤫��ñ��򸡺�����. 
 * 
 * @param nw [in] ��ñ�����ꥹ��
 * @param w [in] ��������ñ���ID
 * @param num [in] ��ñ�����ꥹ�Ȥ�Ĺ��
 * 
 * @return ���Ĥ��ä���礽�μ�ñ����乽¤�ΤؤΥݥ��󥿡����Ĥ���ʤ����
 * NULL ���֤�. 
 * </JA>
 * <EN>
 * Find a word from list of next word candidates.
 * 
 * @param nw [in] list of next word candidates
 * @param w [in] word id to search for
 * @param num [in] length of @a nw
 * 
 * @return the pointer to the NEXTWORD data if found, or NULL if not found.
 * </EN>
 */
/* find next word candiate whose id 'w' */
static NEXTWORD *
search_nw(NEXTWORD **nw, WORD_ID w, int num)
{
  int left,right,mid;
  NEXTWORD *tmp;

  if (num == 0) return NULL;
  left = 0;
  right = num - 1;
  while (left < right) {
    mid = (left + right) / 2;
    if ((nw[mid])->id < w) {
      left = mid + 1;
    } else {
      right = mid;
    }
  }
  tmp = nw[left];
  if (tmp->id == w) {
    return tmp;
  } else {
    return NULL;
  }
}

/** 
 * <EN>
 * Compute backward N-gram score from forward N-gram.
 * </EN>
 * <JA>
 * ������� N-gram �������������� N-gram ���黻�Ф���. 
 * </JA>
 * 
 * @param ngram [in] N-gram data structure
 * @param w [in] word sequence
 * @param wlen [in] length of @a w
 * 
 * @return the backward probability of the word w[0].
 * 
 */
static LOGPROB
ngram_forw2back(NGRAM_INFO *ngram, WORD_ID *w, int wlen)
{
  int i;
  LOGPROB p1, p2;

  p1 = 0.0;
  for(i = 1; i < ngram->n; i++) {
    if (i >= wlen) break;
    p1 += ngram_prob(ngram, i, &(w[1]));
  }
  p2 = 0.0;
  for(i = 0; i < ngram->n; i++) {
    if (i >= wlen) break;
    p2 += ngram_prob(ngram, i+1, w);
  }

  return(p2 - p1);
}

/** 
 * <JA>
 * @brief  ñ��ȥ�ꥹ���鼡ñ��������Ф���. 
 *
 * ñ��ȥ�ꥹ��λ��ꤷ���ե졼���˽�ü��¸�ߤ���ȥ�ꥹñ��
 * �Υꥹ�Ȥ���Ф��������μ�ñ��Ȥ��Ƥ� N-gram ��³��Ψ��׻�����. 
 * ���Υꥹ�Ȥ�ñ�����¤�Τ��ɲä����֤�. 
 * 
 * @param r [in] ǧ���������󥹥���
 * @param nw [i/o] ��ñ�����ꥹ�ȡ���з�̤� @a oldnum �ʹߤ��ɲä�����
 * @param oldnum [in] @a nw �ˤ��Ǥ˳�Ǽ����Ƥ��뼡ñ��ο�
 * @param hypo [in] Ÿ������ʸ����
 * @param t [in] ����ե졼��
 * 
 * @return ��Хꥹ�Ȥ��ɲä������Ȥ� @a nw �˴ޤޤ�뼡ñ������. 
 * </JA>
 * <EN>
 * @brief  Extract next word candidates from word trellis.
 *
 * This function extracts the list of trellis words whose word end
 * has survived in the word trellis at the specified frame.
 * The N-gram probabilities of them are then computed and added to
 * the current next word candidates data.
 * 
 * @param r [in] recognition process instance
 * @param nw [in] list of next word candidates (new words will be appended at @a oldnum)
 * @param oldnum [in] number of words already stored in @a nw
 * @param hypo [in] the source sentence hypothesis
 * @param t [in] specified frame
 * 
 * @return the total number of words currently stored in the @a nw.
 * </EN>
 */
static int
pick_backtrellis_words(RecogProcess *r, NEXTWORD **nw, int oldnum, NODE *hypo, short t)
{
  int i;
  WORD_ID w;
  LOGPROB rawscore;
#ifdef WPAIR
  int w_old = WORD_INVALID;
#endif
  int num;
  int cnnum;		///< Num of found non-transparent words (<=2)
  int last_trans;		///< Num of skipped transparent words
  StackDecode *dwrk;

  BACKTRELLIS *bt;
  WORD_INFO *winfo;
  NGRAM_INFO *ngram;
  LOGPROB lm_weight2, lm_penalty2, lm_penalty_trans;

  num = oldnum;
  bt = r->backtrellis;
  winfo = r->lm->winfo;
  ngram = r->lm->ngram;
  lm_weight2 = r->config->lmp.lm_weight2;
  lm_penalty2 = r->config->lmp.lm_penalty2;
  lm_penalty_trans = r->config->lmp.lm_penalty_trans;
  dwrk = &(r->pass2);

  /* set word contexts to cnword[] from 1 considering transparent words */
  if (ngram) {
    cnnum = 0;
    last_trans = 0;
    for(i=hypo->seqnum-1;i>=0;i--) {
      if (! winfo->is_transparent[hypo->seq[i]]) {
	dwrk->cnword[cnnum+1] = hypo->seq[i];
	cnnum++;
	if (cnnum >= ngram->n - 1) break;
      } else {
	last_trans++;
      }
    }
    if (ngram->dir == DIR_RL) {
      for(i=0;i<cnnum;i++) {
	dwrk->cnwordrev[cnnum-1-i] = dwrk->cnword[i+1];
      }
    }
    /* use ngram id */
    if (ngram->dir == DIR_RL) {
      for(i=0;i<cnnum;i++) dwrk->cnwordrev[i] = winfo->wton[dwrk->cnwordrev[i]];
    } else {
      for(i=0;i<cnnum;i++) dwrk->cnword[i+1] = winfo->wton[dwrk->cnword[i+1]];
    }
  }

  /* lookup survived words in backtrellis on time frame 't' */
  for (i=0;i<bt->num[t];i++) {
    w = (bt->rw[t][i])->wid;
#ifdef WORD_GRAPH
    /* only words on the word graphs are expanded */
    if (!(bt->rw[t][i])->within_wordgraph) continue;
#endif /* not WORD_GRAPH */
#ifdef WPAIR
    /* some word have same word ID with different previous word, so
       only one will be opened (best word will be selected later
       by next_word() */
    if (w == w_old) continue;	/* backtrellis is sorted by word ID */
    else w_old = w;
#endif /* WPAIR */
    /* skip if already exist */
    if (search_nw(nw, w, oldnum) != NULL) continue;

    /* compute LM probability of the word */
    if (ngram) {
      /* compute N-gram probability */
      if (ngram->dir == DIR_RL) {
	/* just compute N-gram prob of the word candidate */
	dwrk->cnwordrev[cnnum] = winfo->wton[w];
	rawscore = ngram_prob(ngram, cnnum + 1, dwrk->cnwordrev);
      } else {
	dwrk->cnword[0] = winfo->wton[w];
	rawscore = ngram_forw2back(ngram, dwrk->cnword, cnnum + 1);
      }
#ifdef CLASS_NGRAM
      rawscore += winfo->cprob[w];
#endif
    }
    if (r->lmvar == LM_NGRAM_USER) {
      /* call user-defined function */
      /* be careful that the word context is ordered in backward direction */
      rawscore = (*(r->lm->lmfunc.lmprob))(winfo, hypo->seq, hypo->seqnum, w, rawscore);
    }

    nw[num]->tre   = bt->rw[t][i];
    nw[num]->id    = w;
    nw[num]->lscore = rawscore * lm_weight2 + lm_penalty2;
    if (winfo->is_transparent[w]) {
      /*nw[num]->lscore -= (LOGPROB)last_trans * TRANS_RENZOKU_PENALTY;*/
      if (winfo->is_transparent[hypo->seq[hypo->seqnum-1]]) {
	nw[num]->lscore += lm_penalty_trans;
      }
    }
    
    /* j_printf("%d: %s added\n", num, winfo->wname[nw[num]->id]); */
    num++;
  }

  return num;
}

/** 
 * <JA>
 * @brief  ñ��ȥ�ꥹ���鼡ñ�콸�����ꤹ��. 
 *
 * ����ե졼������� lookup_range ʬ�˽�ü������ȥ�ꥹ���ñ��򽸤ᡤ
 * ��ñ�칽¤�Τ��ۤ���. Ʊ��ñ�줬�嵭���ϰ����ʣ�������硤
 * ����ե졼��ˤ�äȤ�ᤤ�ȥ�ꥹ���ñ�줬���򤵤��. 
 * 
 * @param r [in] ǧ���������󥹥���
 * @param nw [out] ��ñ�콸����Ǽ���빽¤�ΤؤΥݥ���
 * @param hypo [in] Ÿ��������ʬʸ����
 * @param tm [in] ñ���õ���濴�Ȥʤ����ե졼��
 * @param t_end [in] ñ���õ���ե졼��α�ü
 * 
 * @return @a nw �˳�Ǽ���줿��ñ�����ο����֤�. 
 * </JA>
 * <EN>
 * @brief  Determine next word candidates from the word trellis.
 *
 * This function builds a list of next word candidates by looking up
 * the word trellis at specified frame, with lookup_range frame margin.
 * If the same words exists in the near frames, only the one nearest to the
 * specified frame will be chosen.
 * 
 * @param r [in] recognition process instance
 * @param nw [out] pointer to hold the extracted words as list of next word candidates
 * @param hypo [in] partial sentence hypothesis from which the words will be expanded
 * @param tm [in] center time frame to look up the words
 * @param t_end [in] right frame boundary for the lookup.
 * 
 * @return the number of next words candidates stored in @a nw.
 * </EN>
 */
static int
get_backtrellis_words(RecogProcess *r, NEXTWORD **nw, NODE *hypo, short tm, short t_end)
{
  int num = 0;
  int t, t_step;
  int oldnum=0;

  BACKTRELLIS *bt;
  int lookup_range;

  if (tm < 0) return(0);

  bt = r->backtrellis;
  lookup_range = r->config->pass2.lookup_range;

#ifdef PREFER_CENTER_ON_TRELLIS_LOOKUP
  /* fix for 3.2 (01/10/18 by ri) */
  /* before and after (one near center frame has high priority) */
  for (t_step = 0; t_step < lookup_range; t_step++) {
    /* before or center */
    t = tm - t_step;
    if (t < 0 || t > bt->framelen - 1 || t >= t_end) continue;
    num = pick_backtrellis_words(r, nw, oldnum, hypo, t);
    if (num > oldnum) {
      qsort(nw, num, sizeof(NEXTWORD *),
	    (int (*)(const void *,const void *))compare_nw);
      oldnum = num;
    }
    if (t_step == 0) continue;	/* center */
    /* after */
    t = tm + t_step;
    if (t < 0 || t > bt->framelen - 1 || t >= t_end) continue;
    num = pick_backtrellis_words(r, nw, oldnum, hypo, t);
    if (num > oldnum) {
      qsort(nw, num, sizeof(NEXTWORD *),
	    (int (*)(const void *,const void *))compare_nw);
      oldnum = num;
    }
  }

#else

  /* before the center frame */
  for(t = tm; t >= tm - lookup_range; t--) {
    if (t < 0) break;
    num = pick_backtrellis_words(r, nw, oldnum, hypo, t);
    if (num > oldnum) {
      qsort(nw, num, sizeof(NEXTWORD *),
	    (int (*)(const void *,const void *))compare_nw);
      oldnum = num;
    }
  }
  /* after the center frame */
  for(t = tm + 1; t < tm + lookup_range; t++) {
    if (t > bt->framelen - 1) break;
    if (t >= t_end) break;
    num = pick_backtrellis_words(r, nw, oldnum, hypo, t);
    if (num > oldnum) {
      qsort(nw, num, sizeof(NEXTWORD *),
	    (int (*)(const void *,const void *))compare_nw);
      oldnum = num;
    }
  }
#endif

  return num;
}

/** 
 * <JA>
 * @brief  ��Ÿ��ñ������. 
 * 
 * ����ˤ��Ÿ���оݤȤʤ�ʤ�ñ���ꥹ�Ȥ���õ��. 
 * 
 * @param nw [i/o] ��ñ�콸��ʽ������Ÿ���Ǥ��ʤ�ñ�줬�õ����
 * @param hypo [in] Ÿ��������ʬʸ����
 * @param num [in] @a nw �˸��߳�Ǽ����Ƥ���ñ���
 * @param winfo [in] ñ�켭��
 * 
 * @return ������ nw �˴ޤޤ�뼡ñ���
 * </JA>
 * <EN>
 * @brief  Remove non-expansion word from list.
 * 
 * Remove words in the nextword list which should not be expanded.
 * 
 * @param nw [i/o] list of next word candidates (will be shrinked by removing some words)
 * @param hypo [in] partial sentence hypothesis from which the words will be expanded
 * @param num [in] current number of next words in @a nw
 * @param winfo [in] word dictionary
 * 
 * @return the new number of words in @a nw
 * </EN>
 */
static int
limit_nw(NEXTWORD **nw, NODE *hypo, int num, WORD_INFO *winfo)
{
  int src,dst;
  int newnum;

  /* <s>����ϲ���Ÿ�����ʤ� */
  /* no hypothesis will be generated after "<s>" */
  if (hypo->seq[hypo->seqnum-1] == winfo->head_silwid) {
    return(0);
  }

  dst = 0;
  for (src=0; src<num; src++) {
    if (nw[src]->id == winfo->tail_silwid) {
      /* </s> ��Ÿ�����ʤ� */
      /* do not expand </s> (it only appears at start) */
      continue;
    }
#ifdef FIX_35_INHIBIT_SAME_WORD_EXPANSION
    /* ľ��ñ���Ʊ���ȥ�ꥹñ���Ÿ�����ʤ� */
    /* inhibit expanding the exactly the same trellis word twice */
    if (nw[src]->tre == hypo->tre) continue;
#endif
    
    if (src != dst) memcpy(nw[dst], nw[src], sizeof(NEXTWORD));
    dst++;
  }
  newnum = dst;

  return newnum;
}
	

/** 
 * <JA>
 * @brief  ���ñ�첾�⽸������. 
 *
 * N-gram�١�����õ���Ǥϡ���������ñ��������̵��ñ��˸��ꤵ��Ƥ���. 
 * �����������硼�ȥݡ����������ơ��������ϡ���1�ѥ��Ǻǽ��ե졼��˽�ü��
 * �Ĥä�ñ���������ٺ����ñ��Ȥʤ�. 
 * 
 * @param nw [out] ��ñ�����ꥹ�ȡ�����줿���ñ�첾����Ǽ�����
 * @param peseqlen [in] ���ϥե졼��Ĺ
 * @param maxnw [in] @a nw �˳�Ǽ�Ǥ���ñ��κ����
 * @param r [in] ǧ���������󥹥���
 * 
 * @return @a nw �˳�Ǽ���줿ñ���������֤�. 
 * </JA>
 * <EN>
 * @brief  Get initial word hypotheses at the beginning.
 *
 * on N-gram based recogntion, the initial hypothesis is fixed to the tail
 * silence word.  Exception is that, in short-pause segmentation mode, the
 * initial hypothesis will be chosen from survived words on the last input
 * frame in the first pass.
 * 
 * @param nw [out] pointer to hold the initial word candidates
 * @param peseqlen [in] input frame length
 * @param maxnw [in] maximum number of words that can be stored in @a nw
 * @param r [in] recognition process instance
 * 
 * @return the number of words extracted and stored to @a nw.
 * </EN>
 *
 * @callgraph
 * @callergraph
 */
int
ngram_firstwords(NEXTWORD **nw, int peseqlen, int maxnw, RecogProcess *r)
{

  if (r->config->successive.enabled) {
    /* in sp segment mode  */
    if (r->sp_break_2_begin_word != WORD_INVALID) {
      /* �������� �ǽ��ե졼��˻Ĥä�ñ��ȥ�ꥹ��κ���ñ�� */
      /* the initial hypothesis is the best word survived on the last frame of
	 the segment */
      nw[0]->id = r->sp_break_2_begin_word;
    } else {
      /* �ǽ���������: �������� ñ���������̵��ñ��(=winfo->tail_silwid) */
      /* we are in the last of sentence: initial hypothesis is word-end silence word */
      nw[0]->id = r->lm->winfo->tail_silwid;
    }
  } else {
    /* initial hypothesis should be word-end silence word */
    nw[0]->id = r->lm->winfo->tail_silwid;
  }

  nw[0]->lscore = uni_prob(r->wchmm->ngram, r->wchmm->winfo->wton[nw[0]->id]);
#ifdef CLASS_NGRAM
  nw[0]->lscore += r->wchmm->winfo->cprob[nw[0]->id];
#endif
  nw[0]->lscore *= r->config->lmp.lm_weight2;
#ifndef FIX_PENALTY
  nw[0]->lscore += r->config->lmp.lm_penalty2;
#endif

  return 1;			/* number of words = 1 */
}

/** 
 * <JA>
 * @brief ��ñ�첾�⽸����֤�. 
 *
 * Ϳ����줿��ʬʸ���⤫�顤������³������ñ��ν�����֤�. �ºݤˤϡ�
 * ��1�ѥ��η�̤Ǥ���ȥ�ꥹñ�콸�� bt ��ǡ�Ÿ��������ʬʸ����κǽ�ñ���
 * �ʿ��ꤵ�줿�˻�ü�ե졼�� hypo->estimated_next_t �������¸�ߤ���
 * ñ�콸����Ф��������� N-gram ��³��Ψ��׻������֤�. 
 * ���Ф��줿��ñ�첾��ϡ����餫���� maxnm ��Ĺ������
 * �ΰ褬���ݤ���Ƥ��� nw �˳�Ǽ�����. 
 * 
 * @param hypo [in] Ÿ������ʸ����
 * @param nw [out] ��ñ�����ꥹ�Ȥ��Ǽ�����ΰ�ؤΥݥ���
 * @param maxnw [in] @a nw �κ���Ĺ
 * @param r [in] ǧ���������󥹥���
 * 
 * @return ��Ф��� nw �˳�Ǽ���줿��ñ�첾��ο����֤�. 
 * </JA>
 * <EN>
 * @brief  Return the list of next word candidate.
 *
 * Given a partial sentence hypothesis "hypo", it returns the list of
 * next word candidates.  Actually, it extracts from word trellis the
 * list of words whose word-end node has survived near the estimated
 * beginning-of-word frame of last word "hypo->estimated_next_t", and store
 * them to "nw" with their N-gram probabilities. 
 * 
 * @param hypo [in] source partial sentence hypothesis
 * @param nw [out] pointer to store the list of next word candidates (should be already allocated)
 * @param maxnw [in] maximum number of words that can be stored to @a nw
 * @param r [in] recognition process instance
 * 
 * @return the number of extracted next word candidates in @a nw.
 * </EN>
 * @callgraph
 * @callergraph
 */
int
ngram_nextwords(NODE *hypo, NEXTWORD **nw, int maxnw, RecogProcess *r)
{
  int num, num2;

  if (hypo->seqnum == 0) {
    j_internal_error("ngram_nextwords: hypo contains no word\n");
  }

  /* ����ο��꽪ü����ˤ����� backtrellis��˻ĤäƤ���ñ������� */
  /* get survived words on backtrellis at the estimated end frame */
  num = get_backtrellis_words(r, nw, hypo, hypo->estimated_next_t, hypo->bestt);

  /* Ÿ���Ǥ��ʤ�ñ�������å����Ƴ��� */
  /* exclude unallowed words */
  num2 = limit_nw(nw, hypo, num, r->lm->winfo);

  if (debug2_flag) jlog("DEBUG: ngram_decode: %d-%d=%d unfolded\n",num, num-num2,num2);

  return(num2);
}

/** 
 * <JA>
 * @brief  ����Ƚ��
 * 
 * Ϳ����줿��ʬʸ���⤬��ʸ�ʤ��ʤ��õ����λ�ˤȤ���
 * ������ǽ�Ǥ��뤫�ɤ������֤�. N-gram �Ǥ�ʸƬ���б�����̵��ñ��
 * (silhead) �Ǥ���м�������. 
 * 
 * @param hypo [in] ��ʬʸ����
 * @param r [in] ǧ���������󥹥���
 * 
 * @return ʸ�Ȥ��Ƽ�����ǽ�Ǥ���� TRUE���Բ�ǽ�ʤ� FALSE ���֤�. 
 * </JA>
 * <EN>
 * @brief  Acceptance check.
 * 
 * Return whether the given partial hypothesis is acceptable as a sentence
 * and can be treated as a final search candidate.  In N-gram mode, it checks
 * whether the last word is the beginning-of-sentence silence (silhead).
 * 
 * @param hypo [in] partial sentence hypothesis to be examined
 * @param r [in] recognition process instance
 * 
 * @return TRUE if acceptable as a sentence, or FALSE if not.
 * </EN>
 * @callgraph
 * @callergraph
 */
boolean
ngram_acceptable(NODE *hypo, RecogProcess *r)
{

  if (r->config->successive.enabled) {
    /* �Ǹ�β��⤬�裱�ѥ����ಾ��κǽ��ñ��Ȱ��פ��ʤ���Фʤ�ʤ� */
    /* the last word should be equal to the first word on the best hypothesis on 1st pass */
    if (hypo->seq[hypo->seqnum-1] == r->sp_break_2_end_word) {
      return TRUE;
    }
  } else {
    /* �Ǹ�β��⤬ʸƬ̵��ñ��Ǥʤ���Фʤ�ʤ� */
    /* the last word should be head silence word */
    if (hypo->seq[hypo->seqnum-1] == r->lm->winfo->head_silwid) {
      return TRUE;
    }
  }
  return FALSE;
}

/* end of file */
