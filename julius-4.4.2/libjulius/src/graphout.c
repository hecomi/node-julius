/**
 * @file   graphout.c
 * 
 * <JA>
 * @brief  ñ���ƥ���������.
 * </JA>
 * 
 * <EN>
 * @brief  Output word lattice.
 * </EN>
 * 
 * @author Akinobu LEE
 * @date   Thu Mar 17 12:46:31 2005
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

/// Define if you want debugging output for graph generation
#undef GDEBUG

/// Define if you want much more debugging output for graph generation
#undef GDEBUG2

#if defined(GDEBUG) || defined(GDEBUG2)
static WCHMM_INFO *wchmm_local;	///< Local copy, just for debug
#endif

/** 
 * <JA>
 * ����ս��Ϥ���������. ���ߤϥǥХå��ѽ����Τ�. 
 * 
 * @param wchmm [in] �ڹ�¤������
 * </JA>
 * <EN>
 * Initialize data for graphout.
 * 
 * @param wchmm [in] tree lexicon
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
void
wordgraph_init(WCHMM_INFO *wchmm)
{
#if defined(GDEBUG) || defined(GDEBUG2)
  wchmm_local = wchmm;
#endif
}


/**************************************************************/
/* allocation and free of a WordGraph instance */

/** 
 * <JA>
 * �����ñ��򿷤��������������Υݥ��󥿤��֤�. 
 * 
 * @param wid [in] ñ��ID
 * @param headphone [in] ñ����Ƭ�β���
 * @param tailphone [in] ñ����ü�β���
 * @param leftframe [in] ��ü����(�ե졼��)
 * @param rightframe [in] ��ü����(�ե졼��)
 * @param fscore_head [in] ��ü�Ǥ���ʬʸ������ (g + h)
 * @param fscore_tail [in] ��ü�Ǥ���ʬʸ������ (g + h)
 * @param gscore_head [in] ��Ƭ�Ǥ�������ü�����Viterbi������ (g)
 * @param gscore_tail [in] �����Ǥ�������ü�����Viterbi������ (g)
 * @param lscore [in] ñ��θ��쥹���� (Julian �Ǥ��ͤ˰�̣�ʤ�)
 * @param cm [in] ñ��ο����٥����� (õ������ưŪ�˷׻����줿���)
 * 
 * @return �������������줿�����ñ��ؤΥݥ���
 * </JA>
 * <EN>
 * Allocate a new graph word and return a new pointer to it.
 * 
 * @param wid [in] word ID
 * @param headphone [in] phoneme on head of word
 * @param tailphone [in] phoneme on tail of word
 * @param leftframe [in] beginning time in frames
 * @param rightframe [in] end time in frames
 * @param fscore_head [in] sentence score on search at word head (g + h)
 * @param fscore_tail [in] sentence score on search at word tail (g + h)
 * @param gscore_head [in] Viterbi score accumulated from input end at word head (g)
 * @param gscore_tail [in] Viterbi score accumulated from input end at word tail (g)
 * @param lscore [in] language score of the word (bogus in Julian)
 * @param cm [in] word confidence score (computed on search time)
 * 
 * @return pointer to the newly created graph word.
 * </EN>
 */
static WordGraph *
wordgraph_new(WORD_ID wid, HMM_Logical *headphone, HMM_Logical *tailphone, int leftframe, int rightframe, LOGPROB fscore_head, LOGPROB fscore_tail, LOGPROB gscore_head, LOGPROB gscore_tail, LOGPROB lscore, LOGPROB cm)
{
  WordGraph *new;

  new = (WordGraph *)mymalloc(sizeof(WordGraph));
  new->wid = wid;
  new->lefttime = leftframe;
  new->righttime = rightframe;
  new->fscore_head = fscore_head;
  new->fscore_tail = fscore_tail;
  new->gscore_head = gscore_head;
  new->gscore_tail = gscore_tail;
  new->lscore_tmp = lscore;		/* n-gram only */
#ifdef CM_SEARCH
  new->cmscore = cm;
#endif
  new->forward_score = new->backward_score = 0.0;
  if (rightframe - leftframe + 1 != 0) {
    //new->amavg = (gscore_head - gscore_tail - lscore) / (float)(rightframe - leftframe + 1);
    new->amavg = (gscore_head - gscore_tail) / (float)(rightframe - leftframe + 1);
  }
  new->headphone = headphone;
  new->tailphone = tailphone;
  new->leftwordmaxnum = FANOUTSTEP;
  new->leftword = (WordGraph **)mymalloc(sizeof(WordGraph *) * new->leftwordmaxnum);
  new->left_lscore = (LOGPROB *)mymalloc(sizeof(LOGPROB) * new->leftwordmaxnum);
  new->leftwordnum = 0;
  new->rightwordmaxnum = FANOUTSTEP;
  new->rightword = (WordGraph **)mymalloc(sizeof(WordGraph *) * new->rightwordmaxnum);
  new->right_lscore = (LOGPROB *)mymalloc(sizeof(LOGPROB) * new->rightwordmaxnum);
  new->rightwordnum = 0;

  new->mark = FALSE;
#ifdef GRAPHOUT_DYNAMIC
  new->purged = FALSE;
#endif
  new->next = NULL;
  new->saved = FALSE;

  new->graph_cm = 0.0;

#ifdef GDEBUG
 {
   int i;
   WordGraph *w;
   jlog("DEBUG: NEW: \"%s\"[%d..%d]\n", wchmm_local->winfo->woutput[new->wid], new->lefttime, new->righttime);
   for(i=0;i<new->leftwordnum;i++) {
     w = new->leftword[i];
     jlog("DEBUG: \t left%d:  \"%15s\"[%d..%d]\n", i, wchmm_local->winfo->woutput[w->wid], w->lefttime, w->righttime);
   }
   for(i=0;i<new->rightwordnum;i++) {
     w = new->rightword[i];
     jlog("DEBUG: \tright%d:  \"%15s\"[%d..%d]\n", i, wchmm_local->winfo->woutput[w->wid], w->lefttime, w->righttime);
   }
   jlog("DEBUG: \headphone: %s\n", new->headphone->name);
   jlog("DEBUG: \tailphone: %s\n", new->tailphone->name);
 }
#endif

  return(new);
}

/** 
 * <JA>
 * ���륰���ñ��Υ����ΰ���������. 
 * 
 * @param wg [in] �����ñ��
 * </JA>
 * <EN>
 * Free a graph word.
 * 
 * @param wg [in] graph word to be freed.
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
void
wordgraph_free(WordGraph *wg)
{
  free(wg->rightword);
  free(wg->right_lscore);
  free(wg->leftword);
  free(wg->left_lscore);
  free(wg);
}

/**************************************************************/
/* Handling contexts */

/** 
 * <JA>
 * ���륰���ñ��κ�����ƥ����Ȥ˿����ʥ����ñ����ɲä���. 
 * 
 * @param wg [i/o] �ɲ���Υ����ñ��
 * @param left [in] @a wg �κ�����ƥ����ȤȤ����ɲä���륰���ñ��
 * @param lscore [in] ��³���쥹����
 * </JA>
 * <EN>
 * Add a graph word as a new left context.
 * 
 * @param wg [i/o] word graph to which the @a left word will be added as left context.
 * @param left [in] word graph which will be added to the @a wg as left context.
 * @param lscore [in] word connection score
 * </EN>
 */
static void
wordgraph_add_leftword(WordGraph *wg, WordGraph *left, LOGPROB lscore)
{
  if (wg == NULL) return;
  if (left == NULL) return;
  if (wg->leftwordnum >= wg->leftwordmaxnum) {
    /* expand */
    wg->leftwordmaxnum += FANOUTSTEP;
    wg->leftword = (WordGraph **)myrealloc(wg->leftword, sizeof(WordGraph *) * wg->leftwordmaxnum);
    wg->left_lscore = (LOGPROB *)myrealloc(wg->left_lscore, sizeof(LOGPROB) * wg->leftwordmaxnum);
  }
  wg->leftword[wg->leftwordnum] = left;
  wg->left_lscore[wg->leftwordnum] = lscore;
  wg->leftwordnum++;
#ifdef GDEBUG
  jlog("DEBUG: addleft: \"%s\"[%d..%d] added as %dth left of \"%s\"[%d..%d]\n", wchmm_local->winfo->woutput[left->wid], left->lefttime, left->righttime, wg->leftwordnum, wchmm_local->winfo->woutput[wg->wid], wg->lefttime, wg->righttime);
#endif
}

/** 
 * <JA>
 * ���륰���ñ��α�����ƥ����Ȥ˿����ʥ����ñ����ɲä���. 
 * 
 * @param wg [i/o] �ɲ���Υ����ñ��
 * @param right [in] @a wg �α�����ƥ����ȤȤ����ɲä���륰���ñ��
 * @param lscore [in] ��³���쥹����
 * </JA>
 * <EN>
 * Add a graph word as a new right context.
 * 
 * @param wg [i/o] word graph to which the @a right word will be added as
 * right context.
 * @param right [in] word graph which will be added to the @a wg as right
 * context.
 * @param lscore [in] word connection score
 * </EN>
 */
static void
wordgraph_add_rightword(WordGraph *wg, WordGraph *right, LOGPROB lscore)
{
  if (wg == NULL) return;
  if (right == NULL) return;
  if (wg->rightwordnum >= wg->rightwordmaxnum) {
    /* expand */
    wg->rightwordmaxnum += FANOUTSTEP;
    wg->rightword = (WordGraph **)myrealloc(wg->rightword, sizeof(WordGraph *) * wg->rightwordmaxnum);
    wg->right_lscore = (LOGPROB *)myrealloc(wg->right_lscore, sizeof(LOGPROB) * wg->rightwordmaxnum);
  }
  wg->rightword[wg->rightwordnum] = right;
  wg->right_lscore[wg->rightwordnum] = lscore;
  wg->rightwordnum++;
#ifdef GDEBUG
  jlog("DEBUG: addright: \"%s\"[%d..%d] added as %dth right of \"%s\"[%d..%d]\n", wchmm_local->winfo->woutput[right->wid], right->lefttime, right->righttime, wg->rightwordnum, wchmm_local->winfo->woutput[wg->wid], wg->lefttime, wg->righttime);
#endif
}

/** 
 * <JA>
 * ������ƥ����Ȥ˻��ꤷ�������ñ�줬���ˤ��뤫�ɤ��������å�����
 * �ʤ�����ɲä���. 
 * 
 * @param wg [i/o] Ĵ�٤륰���ñ��
 * @param left [in] ���Υ����ñ�줬 @a wg �κ�����ƥ����Ȥˤ��뤫�����å�����
 * @param lscore [in] ��³���쥹����
 * 
 * @return Ʊ�������ñ�줬������ƥ����Ȥ�¸�ߤ����������ɲä������� TRUE,
 * ������ƥ����ȤȤ���Ʊ�������ñ�줬���Ǥ�¸�ߤ��Ƥ����ɲä��ʤ��ä�����
 * FALSE���֤�. 
 * </JA>
 * <EN>
 * Check for the left context if the specified graph already exists, and
 * add it if not yet.
 * 
 * @param wg [i/o] graph word whose left context will be checked 
 * @param left [in] graph word to be checked as left context of @a wg
 * @param lscore [in] word connection score
 * 
 * @return TRUE if not exist yet and has been added, or FALSE if already
 * exist and thus not added.
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
boolean
wordgraph_check_and_add_leftword(WordGraph *wg, WordGraph *left, LOGPROB lscore)
{
  int i;

  if (wg == NULL) return FALSE;
  if (left == NULL) return FALSE;
  for(i=0;i<wg->leftwordnum;i++) {
    if (wg->leftword[i] == left) {
      break;
    }
  }
  if (i >= wg->leftwordnum) { /* no leftword matched */
    wordgraph_add_leftword(wg, left, lscore);
    return TRUE;
  } else if (wg->left_lscore[i] < lscore) {
    /* for same word connection, keep maximum LM score */
    if (debug2_flag) jlog("DEBUG: check_and_add_leftword: update left\n");
    wg->left_lscore[i] = lscore;
  }
  return FALSE;
}

/** 
 * <JA>
 * ������ƥ����Ȥ˻��ꤷ�������ñ�줬���ˤ��뤫�ɤ��������å�����
 * �ʤ�����ɲä���. 
 * 
 * @param wg [i/o] Ĵ�٤륰���ñ��
 * @param right [in] ���Υ����ñ�줬 @a wg �α�����ƥ����Ȥˤ��뤫�����å�����
 * @param lscore [in] ��³���쥹����
 * 
 * @return Ʊ�������ñ�줬������ƥ����Ȥ�¸�ߤ����������ɲä������� TRUE,
 * ������ƥ����ȤȤ���Ʊ�������ñ�줬���Ǥ�¸�ߤ��Ƥ����ɲä��ʤ��ä�����
 * FALSE���֤�. 
 * </JA>
 * <EN>
 * Check for the right context if the specified graph already exists, and
 * add it if not yet.
 * 
 * @param wg [i/o] graph word whose right context will be checked 
 * @param right [in] graph word to be checked as right context of @a wg
 * @param lscore [in] word connection score
 * 
 * @return TRUE if not exist yet and has been added, or FALSE if already
 * exist and thus not added.
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
boolean
wordgraph_check_and_add_rightword(WordGraph *wg, WordGraph *right, LOGPROB lscore)
{
  int i;

  if (wg == NULL) return FALSE;
  if (right == NULL) return FALSE;
  for(i=0;i<wg->rightwordnum;i++) {
    if (wg->rightword[i] == right) {
      break;
    }
  }
  if (i >= wg->rightwordnum) { /* no rightword matched */
    wordgraph_add_rightword(wg, right, lscore);
    return TRUE;
  } else if (wg->right_lscore[i] < lscore) {
    /* for same word connection, keep maximum LM score */
    if (debug2_flag) jlog("DEBUG: check_and_add_rightword: update right\n");
    wg->right_lscore[i] = lscore;
  }
  return FALSE;
}

/** 
 * <JA>
 * Ʊ�쥰���ñ��Υޡ�������,ñ�쥰��դΥ���ƥ����Ȥ������̤�ñ�쥰��դ�
 * �ɲä���. 
 * 
 * @param dst [i/o] �ɲ���Υ����ñ��
 * @param src [in] �ɲø��Υ����ñ��
 * 
 * @return 1�ĤǤ⿷�����ɲä����� TRUE, 1�Ĥ��ɲä���ʤ���� FALSE ���֤�. 
 * </JA>
 * <EN>
 * Add all the context words to other for merging the same two graph words.
 * 
 * @param dst [i/o] destination graph word
 * @param src [in] source graph word
 * 
 * @return TRUE if at least one context word has been newly added, or FALSE if
 * context on @a dst has not been updated.
 * </EN>
 */
static boolean
merge_contexts(WordGraph *dst, WordGraph *src)
{
  int s, d;
  WordGraph *adding;
  boolean ret;

#ifdef GDEBUG
  jlog("DEBUG: merge_contexts: merging context of \"%s\"[%d..%d] to \"%s\"[%d..%d]...\n",
	 wchmm_local->winfo->woutput[src->wid], src->lefttime, src->righttime,
	 wchmm_local->winfo->woutput[dst->wid], dst->lefttime, dst->righttime);
#endif

  ret = FALSE;
  
  /* left context */
  for(s=0;s<src->leftwordnum;s++) {
    adding = src->leftword[s];
    if (adding->mark) continue;
    /* direct link between dst and src will disapper to avoid unneccesary loop */
    if (adding == dst) {
#ifdef GDEBUG
      jlog("DEBUG: merge_contexts: skipping direct link (dst) -> (src)\n");
#endif
      continue;
    }
    for(d=0;d<dst->leftwordnum;d++) {
      if (dst->leftword[d]->mark) continue;
      if (dst->leftword[d] == adding) {
	break;
      }
    }
    if (d >= dst->leftwordnum) { /* no leftword matched */
      wordgraph_add_leftword(dst, adding, src->left_lscore[s]);
#ifdef GDEBUG
      jlog("DEBUG: merge_contexts: added \"%s\"[%d..%d] as a new left context\n",
	     wchmm_local->winfo->woutput[adding->wid], adding->lefttime, adding->righttime);
#endif
      ret = TRUE;
    } else if (dst->left_lscore[d] < src->left_lscore[s]) {
      jlog("DEBUG: merge_context: update left\n");
      dst->left_lscore[d] = src->left_lscore[s];
    }
#ifdef GDEBUG
    else {
      jlog("DEBUG: merge_contexts: \"%s\"[%d..%d] already exist\n",
	     wchmm_local->winfo->woutput[adding->wid], adding->lefttime, adding->righttime);
    }
#endif
  }

  /* right context */
  for(s=0;s<src->rightwordnum;s++) {
    adding = src->rightword[s];
    if (adding->mark) continue;
    /* direct link between dst and src will disapper to avoid unneccesary loop */
    if (adding == dst) {
#ifdef GDEBUG
      jlog("DEBUG: merge_contexts: skipping direct link (src) -> (dst)\n");
#endif
      continue;
    }
    for(d=0;d<dst->rightwordnum;d++) {
      if (dst->rightword[d]->mark) continue;
      if (dst->rightword[d] == adding) {
	break;
      }
    }
    if (d >= dst->rightwordnum) { /* no rightword matched */
      wordgraph_add_rightword(dst, adding, src->right_lscore[s]);
#ifdef GDEBUG
      jlog("DEBUG: merge_contexts: added \"%s\"[%d..%d] as a new right context\n",
	     wchmm_local->winfo->woutput[adding->wid], adding->lefttime, adding->righttime);
#endif
      ret = TRUE;
    } else if (dst->right_lscore[d] < src->right_lscore[s]) {
      jlog("DEBUG: merge_context: update right\n");
      dst->right_lscore[d] = src->right_lscore[s];
    }
#ifdef GDEBUG
    else {
      jlog("DEBUG: merge_contexts: \"%s\"[%d..%d] already exist\n",
	     wchmm_local->winfo->woutput[adding->wid], adding->lefttime, adding->righttime);
    }
#endif
  }
  
  return(ret);
}

/** 
 * <JA>
 * ������ƥ����Ⱦ�Τ��륰���ñ����̤Υ����ñ����֤�������. 
 * 
 * @param wg [i/o] ����оݤΥ����ñ��
 * @param from [in] �֤��������Ȥʤ뺸����ƥ����Ⱦ�Υ����ñ��
 * @param to [in] �֤�������Υ����ñ��
 * @param lscore [in] ��³���쥹����
 * </JA>
 * <EN>
 * Substitute a word at left context of a graph word to another.
 * 
 * @param wg [i/o] target graph word.
 * @param from [in] left context word to be substituted
 * @param to [in] substitution destination.
 * @param lscore [in] word connection score
 * </EN>
 */
static void
swap_leftword(WordGraph *wg, WordGraph *from, WordGraph *to, LOGPROB lscore)
{
  int i;
  
#ifdef GDEBUG
  jlog("DEBUG: swapleft: replacing left of \"%s\"[%d..%d] from \"%s\"[%d..%d] to \"%s\"[%d..%d]...\n",
	 wchmm_local->winfo->woutput[wg->wid], wg->lefttime, wg->righttime,
	 wchmm_local->winfo->woutput[from->wid], from->lefttime, from->righttime,
	 wchmm_local->winfo->woutput[to->wid], to->lefttime, to->righttime);
#endif
  
  for(i=0;i<wg->leftwordnum;i++) {
    if (wg->leftword[i] == from) {
      wg->leftword[i] = to;
      wg->left_lscore[i] = lscore;
    }
  }
}

/** 
 * <JA>
 * ������ƥ����Ⱦ�Τ��륰���ñ����̤Υ����ñ����֤�������. 
 * 
 * @param wg [i/o] ����оݤΥ����ñ��
 * @param from [in] �֤��������Ȥʤ뱦����ƥ����Ⱦ�Υ����ñ��
 * @param to [in] �֤�������Υ����ñ��
 * @param lscore [in] ��³���쥹����
 * </JA>
 * <EN>
 * Substitute a word at right context of a graph word to another.
 * 
 * @param wg [i/o] target graph word.
 * @param from [in] right context word to be substituted
 * @param to [in] substitution destination.
 * @param lscore [in] word connection score
 * </EN>
 */
static void
swap_rightword(WordGraph *wg, WordGraph *from, WordGraph *to, LOGPROB lscore)
{
  int i;
  
#ifdef GDEBUG
  jlog("DEBUG: swapright: replacing right of \"%s\"[%d..%d] from \"%s\"[%d..%d] to \"%s\"[%d..%d]...\n",
	 wchmm_local->winfo->woutput[wg->wid], wg->lefttime, wg->righttime,
	 wchmm_local->winfo->woutput[from->wid], from->lefttime, from->righttime,
	 wchmm_local->winfo->woutput[to->wid], to->lefttime, to->righttime);
#endif

  for(i=0;i<wg->rightwordnum;i++) {
    if (wg->rightword[i] == from) {
      wg->rightword[i] = to;
      wg->right_lscore[i] = lscore;
    }
  }
}

/** 
 * <JA>
 * ������ƥ����ȥꥹ����ν�ʣ������
 * 
 * @param wg [i/o] ����оݤΥ����ñ��
 * </JA>
 * <EN>
 * Delete duplicate entries in left context list of a graph word.
 * 
 * @param wg [i/o] target graph word
 * </EN>
 */
static void
uniq_leftword(WordGraph *wg)
{
  int i, j, dst;
  boolean ok;

  dst = 0;
  for(i=0;i<wg->leftwordnum;i++) {
    ok = TRUE;
    for(j=0;j<dst;j++) {
      if (wg->leftword[i] == wg->leftword[j]) {
	ok = FALSE;
	break;
      }
    }
    if (ok == TRUE) {
      wg->leftword[dst] = wg->leftword[i];
      wg->left_lscore[dst] = wg->left_lscore[i];
      dst++;
    }
  }
  wg->leftwordnum = dst;
}

/** 
 * <JA>
 * ������ƥ����ȥꥹ����ν�ʣ������
 * 
 * @param wg [i/o] ����оݤΥ����ñ��
 * </JA>
 * <EN>
 * Delete duplicate entries in right context list of a graph word.
 * 
 * @param wg [i/o] target graph word
 * </EN>
 */
static void
uniq_rightword(WordGraph *wg)
{
  int i, j, dst;
  boolean ok;

  dst = 0;
  for(i=0;i<wg->rightwordnum;i++) {
    ok = TRUE;
    for(j=0;j<dst;j++) {
      if (wg->rightword[i] == wg->rightword[j]) {
	ok = FALSE;
	break;
      }
    }
    if (ok == TRUE) {
      wg->rightword[dst] = wg->rightword[i];
      wg->right_lscore[dst] = wg->right_lscore[i];
      dst++;
    }
  }
  wg->rightwordnum = dst;
}

/** 
 * <JA>
 * �����Υ����ñ��Υ���ƥ����ȥꥹ�Ȥ��餽�Υ����ñ�켫�Ȥ�õ��. 
 * 
 * @param wg [in] ����оݤΥ����ñ��
 * </JA>
 * <EN>
 * Remove the specified word graph from contexts of all left and right words.
 * 
 * @param wg [in] target graph word
 * </EN>
 */
static void
wordgraph_remove_context(WordGraph *wg)
{
  WordGraph *w;
  int i,j,k;

  if (wg == NULL) return;

  for(i=0;i<wg->leftwordnum;i++) {
    w = wg->leftword[i];
    k=0;
    for(j=0;j<w->rightwordnum;j++) {
      if (w->rightword[j] != wg) {
	if (j != k) {
	  w->rightword[k] = w->rightword[j];
	  w->right_lscore[k] = w->right_lscore[j];
	}
	k++;
      }
    }
    w->rightwordnum = k;
  }
  for(i=0;i<wg->rightwordnum;i++) {
    w = wg->rightword[i];
    k=0;
    for(j=0;j<w->leftwordnum;j++) {
      if (w->leftword[j] != wg) {
	if (j != k) {
	  w->leftword[k] = w->leftword[j];
	  w->left_lscore[k] = w->left_lscore[j];
	}
	k++;
      }
    }
    w->leftwordnum = k;
#ifdef GDEBUG2
    if (w->leftwordnum == 0) {
      jlog("DEBUG: leftword becomes 0 by remove_context\n");
      put_wordgraph(jlog_get_fp(), w, wchmm_local->winfo);
      jlog("DEBUG: by deleting its left context:\n");
      put_wordgraph(jlog_get_fp(), wg, wchmm_local->winfo);
    }
#endif
  }
}

/** 
 * <JA>
 * �����ñ��κ����Υ���ƥ����Ȥ��󥯤���. 
 * 
 * @param wg [in] ����оݤΥ����ñ��
 * </JA>
 * <EN>
 * link all words at the context of the graph word.
 * 
 * @param wg [in] target graph word
 * </EN>
 */
static void
wordgraph_link_context(WordGraph *wg)
{
  int i,j;
  WordGraph *left, *right;
  
  if (wg == NULL) return;
  for(i=0;i<wg->leftwordnum;i++) {
    left = wg->leftword[i];
    if (left->mark) continue;
    if (left == wg) continue;
    for(j=0;j<wg->rightwordnum;j++) {
      right = wg->rightword[j];
      if (right->mark) continue;
      if (right == wg) continue;
      if (left == right) continue;
      wordgraph_check_and_add_leftword(right, left, wg->left_lscore[i]);
      wordgraph_check_and_add_rightword(left, right, wg->right_lscore[j]);
    }
  }
}

/**************************************************************/
/* Operations for organizing WordGraph set */

/** 
 * <JA>
 * ñ�쥰�����κ���ޡ������դ���ñ���������. 
 * 
 * @param rootp [i/o] ñ�쥰��դΥ롼�ȥΡ��ɤؤΥݥ���
 * 
 * @return ������줿ñ��ο�
 * </JA>
 * <EN>
 * Actually erase the marked words in word graph.
 * 
 * @param rootp [i/o] pointer to root node of a word graph
 * 
 * @return the number of erased words.
 * </EN>
 */
static int
wordgraph_exec_erase(WordGraph **rootp)
{
  WordGraph *wg, *we, *wtmp;
  int count;

  if (*rootp == NULL) return(0);
  
  wg = *rootp;
  count = 0;
  while (wg != NULL) {
    we = wg->next;
    while(we != NULL && we->mark == TRUE) {
      wtmp = we->next;
      wordgraph_free(we); count++;
      we = wtmp;
    }
    wg->next = we;
    wg = we;
  }
  if ((*rootp)->mark == TRUE) {
    wtmp = (*rootp)->next;
    wordgraph_free(*rootp); count++;
    *rootp = wtmp;
  }

  return(count);
}

/** 
 * <JA>
 * ����ե������� qsort ������Хå�
 * 
 * @param x [in] ���ǣ�
 * @param y [in] ���ǣ�
 * 
 * @return x > y �ʤ� 1, x < y �ʤ� -1, x = y �ʤ� 0 ���֤�. 
 * </JA>
 * <EN>
 * qsort callback for word sorting.
 * 
 * @param x [in] element 1
 * @param y [in] element 2
 * 
 * @return 1 if x>y, -1 if x<y, 0 if x = y.
 * </EN>
 */
static int
compare_lefttime(WordGraph **x, WordGraph **y)
{
  if ((*x)->lefttime > (*y)->lefttime) return 1;
  else if ((*x)->lefttime < (*y)->lefttime) return -1;
  else {
    if ((*x)->righttime > (*y)->righttime) return 1;
    else if ((*x)->righttime < (*y)->righttime) return -1;
    else {
      if ((*x)->fscore_head < (*y)->fscore_head) return 1;
      else if ((*x)->fscore_head > (*y)->fscore_head) return -1;
      else return 0;
    }
  }
}

/** 
 * <JA>
 * ñ�쥰��������ñ��򳫻ϻ��ֽ�˥����Ȥ����̤��ֹ��Ĥ���. 
 * 
 * @param rootp [i/o] ñ�쥰��դΥ롼�ȥΡ��ɤؤΥݥ��󥿳�Ǽ���
 * @param r [i/o] ǧ���������󥹥���
 * </JA>
 * <EN>
 * Sort words by left time and annotate sequencial id for them in a word graph.
 * 
 * @param rootp [i/o] address of pointer to root node of a word graph
 * @param r [i/o] recognition process instance
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
int
wordgraph_sort_and_annotate_id(WordGraph **rootp, RecogProcess *r)
{
  WordGraph *wg;
  int cnt;
  WordGraph **wlist;
  int i;
  WordGraph *wo;

  /* count total number of words in the graph */
  cnt = 0;
  for(wg=*rootp;wg;wg=wg->next) cnt++;
  if (cnt == 0) return 0;
  /* sort them by lefttime */
  wlist = (WordGraph **)mymalloc(sizeof(WordGraph *) * cnt);
  i = 0;
  for(wg=*rootp;wg;wg=wg->next) {
    wlist[i++] = wg;
  }
  qsort(wlist, cnt, sizeof(WordGraph *), (int (*)(const void *, const void *))compare_lefttime);

  /* annotated id and re-order the link by the id */
  wo = NULL;
  for(i=cnt-1;i>=0;i--) {
    wg = wlist[i];
    wg->id = i;
    wg->next = wo;
    wo = wg;
  }
  *rootp = wo;
  free(wlist);

  return cnt;
}

/** 
 * <JA>
 * ñ�쥰��������ñ������Ʋ�������. 
 * 
 * @param rootp [i/o] ñ�쥰��դΥ롼�ȥΡ��ɤؤΥݥ���
 * </JA>
 * <EN>
 * Free all the words in a word graph.
 * 
 * @param rootp [i/o] pointer to root node of a word graph
 * </EN>
 *
 * @callgraph
 * @callergraph
 *
 */
void
wordgraph_clean(WordGraph **rootp)
{
  WordGraph *wg, *wtmp;

  wg = *rootp;
  while(wg != NULL) {
    wtmp = wg->next;
    wordgraph_free(wg);
    wg = wtmp;
  }
  *rootp = NULL;

}

/*********************************************************************/
/* Post-processing of generated word arcs after search has been done */

/** 
 * <JA>
 * ñ�쥰��տ������åȤΤ���� qsort �ѥ�����Хå�. fscore_head ��
 * �߽�˥����Ȥ���. 
 * 
 * @param x [in] ���ǣ�
 * @param y [in] ���ǣ�
 * 
 * @return qsort �˽स���֤���
 * </JA>
 * <EN>
 * Callback function for qsort to do word graph depth cutting. Graph
 * words will be sorted downward based on fscore_head.
 * 
 * @param x [in] element 1
 * @param y [in] element 2
 * 
 * @return values for qsort
 * </EN>
 */
static int
compare_beam(WordGraph **x, WordGraph **y)
{
  if ((*x)->fscore_head < (*y)->fscore_head) return 1;
  else if ((*x)->fscore_head > (*y)->fscore_head) return -1;
  else return 0;
}

/** 
 * <JA>
 * @brief  ����ո�������Σ������ñ�쥰��դ����. 
 * 
 * õ������������줿ñ����佸�礫�顤��ü����Ϥޤ�ѥ����̵��leafñ���
 * ������뤳�Ȥǽ��ñ�쥰��դ���Ф���. 
 * 
 * @param rootp [i/o] ñ�쥰��դΥ롼�ȥΡ��ɤؤΥݥ���
 * @param r [in] ǧ���������󥹥���
 * </JA>
 * <EN>
 * @brief  Post-processing step 1: Extract initial word graph.
 * 
 * Extract initial word graph from generated word arcs while search, by
 * purging leaf nodes and arcs that are not on the path from edge to edge.
 * 
 * @param rootp [i/o] pointer to root node of a word graph
 * @param r [in] recognition process instance
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
void
wordgraph_purge_leaf_nodes(WordGraph **rootp, RecogProcess *r)
{
  WordGraph *wg;
  int i, dst;
  boolean changed;
  int count, erased, del_left, del_right;

  /* count whole */
  count = 0;
  for(wg=*rootp;wg;wg=wg->next) count++;
  if (verbose_flag) jlog("STAT: graphout: %d initial word arcs generated\n", count);
  if (count == 0) return;
  
  if (verbose_flag) jlog("STAT: graphout: step 1: purge leaf nodes\n");

  /* mark words to be erased */
  del_left = del_right = 0;
  do {
    changed = FALSE;
    for(wg=*rootp;wg;wg=wg->next) {
      if (wg->mark == TRUE) continue;
      /* mark if wg has no left context, or all leftwords are marked */
      if (wg->lefttime != 0) {
	for(i=0;i<wg->leftwordnum;i++) {
	  if (wg->leftword[i]->mark == FALSE) break;
	}
	if (i >= wg->leftwordnum) {
	  wg->mark = TRUE;
	  changed = TRUE;
	  del_left++;
	  continue;
	}
      }
      /* mark if wg has no right context, or all rightwords are marked */
      if (wg->righttime != r->peseqlen - 1) {
	for(i=0;i<wg->rightwordnum;i++) {
	  if (wg->rightword[i]->mark == FALSE) break;
	}
	if (i >= wg->rightwordnum) {
	  wg->mark = TRUE;
	  changed = TRUE;
	  del_right++;
	  continue;
	}
      }
    }
  } while (changed == TRUE);

  if (verbose_flag) jlog("STAT: graphout: %d leaf words found (left_blank=%d, right_blank=%d)\n", del_left + del_right, del_left, del_right);

  /* do compaction of left/rightwords */
    for(wg=*rootp;wg;wg=wg->next) {
      if (wg->mark) continue;
      dst = 0;
      for(i=0;i<wg->leftwordnum;i++) {
	if (wg->leftword[i]->mark == FALSE) {
	  if (dst != i) {
	    wg->leftword[dst] = wg->leftword[i];
	    wg->left_lscore[dst] = wg->left_lscore[i];
	  }
	  dst++;
	}
      }
      wg->leftwordnum = dst;
    }
    for(wg=*rootp;wg;wg=wg->next) {
      if (wg->mark) continue;
      dst = 0;
      for(i=0;i<wg->rightwordnum;i++) {
	if (wg->rightword[i]->mark == FALSE) {
	  if (dst != i) {
	    wg->rightword[dst] = wg->rightword[i];
	    wg->right_lscore[dst] = wg->right_lscore[i];
	  }
	  dst++;
	}
      }
      wg->rightwordnum = dst;
    }

  /* execute erase of marked words */
  erased = wordgraph_exec_erase(rootp);
  if (verbose_flag) jlog("STAT: graphout: %d words purged, %d words left in lattice\n", erased, count - erased);

}

/** 
 * <JA>
 * @brief  ����ո�������Σ�. ��������դο����ˤ��ñ�����Υ��å�
 * 
 * GRAPHOUT_DEPTHCUT �����������դο����ˤ��ñ�����Υ��åȤ�Ԥ�. 
 * 
 * @param rootp [i/o] ñ�쥰��դΥ롼�ȥΡ��ɤؤΥݥ���
 * @param r [in] ǧ���������󥹥���
 * </JA>
 * <EN>
 * @brief  Post-processing step 1.5: word graph depth cutting
 * 
 * If GRAPHOUT_DEPTHCUT is defined, perform word graph depth cutting.
 * 
 * @param rootp [i/o] pointer to root node of a word graph
 * @param r [in] recognition process instance
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
void
wordgraph_depth_cut(WordGraph **rootp, RecogProcess *r)
{
#ifdef GRAPHOUT_DEPTHCUT

  WordGraph *wg;
  int i, dst;
  boolean changed;
  int count, erased, del_left, del_right;
  WordGraph **wlist;
  boolean f;
  int *wc;
  int t;
  int pruned;


  if (r->config->graph.graphout_cut_depth < 0) return;

  if (verbose_flag) jlog("STAT: graphout: step 1.5: cut less likely hypothesis by depth of %d\n", r->config->graph.graphout_cut_depth);

  /* count whole */
  count = 0;
  for(wg=*rootp;wg;wg=wg->next) count++;
  if (count == 0) return;
  
  /* prepare buffer to count words per frame */
  wc = (int *)mymalloc(sizeof(int) * r->peseqlen);
  for (t=0;t<r->peseqlen;t++) wc[t] = 0;
  /* sort words by fscore_head */
  wlist = (WordGraph **)mymalloc(sizeof(WordGraph *) * count);
  i = 0;
  for(wg=*rootp;wg;wg=wg->next) {
    wlist[i++] = wg;
  }
  qsort(wlist, count, sizeof(WordGraph *), (int (*)(const void *, const void *))compare_beam);
  /* count words per frame, and unlink/mark them if below beam width */
  pruned = 0;
  for (i=0;i<count;i++) {
    wg = wlist[i];
    f = TRUE;
    for (t=wg->lefttime;t<=wg->righttime;t++) {
      wc[t]++;
      if (wc[t] <= r->config->graph.graphout_cut_depth) f = FALSE;
    }
    if (f) {
      //wordgraph_remove_context(wg);
      wg->mark = TRUE;
      pruned++;
    }
  }
#ifdef GDEBUG2
  jlog("DEBUG: GRAPH DEPTH STATISTICS: NUMBER OF WORDS PER FRAME\n");
  for(t=0;t<r->peseqlen;t++) {
    if (wc[t] > r->config->graph.graphout_cut_depth) {
      jlog("*");
    } else {
      jlog(" ");
    }
    jlog("%4d: %d\n", t, wc[t]);
  }
#endif
  if (verbose_flag) jlog("STAT: graphout: %d words out of %d are going to be pruned by depth cutting\n", pruned, count);
  free(wlist);
  free(wc);

  /* mark words to be erased */
  del_left = del_right = 0;
  do {
    changed = FALSE;
    for(wg=*rootp;wg;wg=wg->next) {
      if (wg->mark == TRUE) continue;
      /* mark if wg has no left context, or all leftwords are marked */
      if (wg->lefttime != 0) {
	for(i=0;i<wg->leftwordnum;i++) {
	  if (wg->leftword[i]->mark == FALSE) break;
	}
	if (i >= wg->leftwordnum) {
	  wg->mark = TRUE;
	  changed = TRUE;
	  del_left++;
	  continue;
	}
      }
      /* mark if wg has no right context, or all rightwords are marked */
      if (wg->righttime != r->peseqlen - 1) {
	for(i=0;i<wg->rightwordnum;i++) {
	  if (wg->rightword[i]->mark == FALSE) break;
	}
	if (i >= wg->rightwordnum) {
	  wg->mark = TRUE;
	  changed = TRUE;
	  del_right++;
	  continue;
	}
      }
    }
  } while (changed == TRUE);

  if (verbose_flag) jlog("STAT: graphout: %d new leaves found (left_blank=%d, right_blank=%d)\n", del_left + del_right, del_left, del_right);

  /* do compaction of left/rightwords */
    for(wg=*rootp;wg;wg=wg->next) {
      if (wg->mark) continue;
      dst = 0;
      for(i=0;i<wg->leftwordnum;i++) {
	if (wg->leftword[i]->mark == FALSE) {
	  if (dst != i) {
	    wg->leftword[dst] = wg->leftword[i];
	    wg->left_lscore[dst] = wg->left_lscore[i];
	  }
	  dst++;
	}
      }
      wg->leftwordnum = dst;
    }
    for(wg=*rootp;wg;wg=wg->next) {
      if (wg->mark) continue;
      dst = 0;
      for(i=0;i<wg->rightwordnum;i++) {
	if (wg->rightword[i]->mark == FALSE) {
	  if (dst != i) {
	    wg->rightword[dst] = wg->rightword[i];
	    wg->right_lscore[dst] = wg->right_lscore[i];
	  }
	  dst++;
	}
      }
      wg->rightwordnum = dst;
    }

  /* execute erase of marked words */
  erased = wordgraph_exec_erase(rootp);
  if (verbose_flag) jlog("STAT: graphout: total %d words purged, %d words left in lattice\n", erased, count - erased);

#else  /* ~GRAPHOUT_DEPTHCUT */

  if (verbose_flag) jlog("STAT: graphout: step 1.5: graph depth cutting has been disabled, skipped\n");

#endif

}

/** 
 * <JA>
 * ñ��֤ζ�������Τ���������¹Ԥ���. ��������ñ�������å���,
 * ��³ñ��֤Ƕ������־���ˤ��줬����Ȥ��ϡ����Τ����������. 
 * ʣ���Υ���ƥ����ȴ֤ǰۤʤ붭������¸�ߤ������,�����
 * ���ԡ����Ƥ��줾��˹�碌��. �ޤ����饤����Ȥ�������ñ�������. 
 * 
 * @param rootp [i/o] �����ñ��ꥹ�ȤΥ롼�ȥݥ���
 * @param mov_num_ret [out] �������֤�ư����ñ������Ǽ�����ѿ��ؤΥݥ���
 * @param dup_num_ret [out] ���ԡ����줿ñ������Ǽ�����ѿ��ؤΥݥ���
 * @param del_num_ret [out] ������줿ñ������Ǽ�����ѿ��ؤΥݥ���
 * @param mod_num_ret [out] �ѹ����줿ñ������Ǽ�����ѿ��ؤΥݥ���
 * @param count [in] ����վ��ñ���
 * @param maxfnum
 * @param peseqlen
 * @param lmtype
 * @param p_framelist
 * @param p_framescorelist
 * 
 * @return ��������ñ�줬���İʾ��ѹ������� TRUE���ѹ��ʤ��Ǥ���� FALSE
 * ���֤�. 
 * </JA>
 * <EN>
 * Execute adjustment of word boundaries.  It looks through the graph to
 * check correspondence of word boundary information among context, and if
 * there is a gap, the beginning frame of right word will be moved to the
 * end frame of left word.  If several alignment is found among contexts,
 * the word will be duplicated and each will be fit to each context.  Also,
 * words with invalid alignment will be eliminated.
 * 
 * @param rootp [in] root pointer to the list of graph words
 * @param mov_num_ret [out] pointer to hold resulted number of moved words
 * @param dup_num_ret [out] pointer to hold resulted number of duplicated words
 * @param del_num_ret [out] pointer to hold resulted number of eliminated words
 * @param mod_num_ret [out] pointer to hold resulted number of modified words
 * @param count [in] number of words in graph
 * @param maxfnum
 * @param peseqlen
 * @param lmtype
 * @param p_framelist
 * @param p_framescorelist
 * 
 * @return TRUE if any word has been changed, or FALSE if no word has been altered.
 * </EN>
 */
static boolean
wordgraph_adjust_boundary_sub(WordGraph **rootp, int *mov_num_ret, int *dup_num_ret, int *del_num_ret, int *mod_num_ret, int count, int *maxfnum, int peseqlen, int lmtype, int **p_framelist, LOGPROB **p_framescorelist)
{
  WordGraph *wg, *left, *new;
  int i, j, k;
  int fnum;
  int mov_num, dup_num, del_num, mod_num;
  boolean changed = FALSE;
  int *framelist;
  LOGPROB *framescorelist;

  mov_num = dup_num = del_num = mod_num = 0;

  framelist = *p_framelist;
  framescorelist = *p_framescorelist;

  /* maximum number of left context words does not exceed total word num */
  /* allocate temporal work area.  these are permanent buffer that will
     be kept between recognition sessions. */
  if (*maxfnum == 0) {
    /* when this is called for the first time, allocate buffer */
    *maxfnum = count;
    framelist = (int *)mymalloc(sizeof(int) * (*maxfnum));
    framescorelist = (LOGPROB *)mymalloc(sizeof(LOGPROB) * (*maxfnum));
#ifdef GDEBUG
    jlog("DEBUG: Notice: maxfnum starts at %d\n", *maxfnum);
#endif
  } else if (*maxfnum < count) {
    /* for later call, expand buffer if necessary */
    free(framescorelist);
    free(framelist);
    *maxfnum = count;
    framelist = (int *)mymalloc(sizeof(int) * (*maxfnum));
    framescorelist = (LOGPROB *)mymalloc(sizeof(LOGPROB) * (*maxfnum));
#ifdef GDEBUG
    jlog("DEBUG: Notice: maxfnum expanded by count (%d)\n", *maxfnum);
#endif
  }

#ifdef GDEBUG2
  jlog("DEBUG: ***CHECK LOOP BEGIN***\n");
#endif
  for(wg=*rootp;wg;wg=wg->next) {
    if (wg->mark) continue;	/* already marked */
#ifdef GDEBUG2
    jlog("DEBUG:   [%d..%d] \"%s\"\n", wg->lefttime, wg->righttime, wchmm_local->winfo->woutput[wg->wid]);
#endif
    if (wg->leftwordnum == 0) {	/* no leftword */
      if (wg->lefttime != 0) {
	/* some fraction found by former elimination: remove this */
#ifdef GDEBUG2
	jlog("DEBUG:   -> no leftword at middle of lattice, eliminate this\n");
#endif
	wordgraph_remove_context(wg);
	wg->mark = TRUE;
	del_num++;
	changed = TRUE;
      }
      /* if has no leftword, adjustment of this word is not needed */
      continue;
    }
    if (wg->rightwordnum == 0) {	/* no rightword */
      if (wg->righttime != peseqlen-1) {
	/* some fraction found by former elimination: remove this */
#ifdef GDEBUG2
	jlog("DEBUG:   -> no rightword at middle of lattice, eliminate this\n");
#endif
	wordgraph_remove_context(wg);
	wg->mark = TRUE;
	del_num++;
	changed = TRUE;
	continue;
      }
      /* if on right edge, continue adjusting */
    }
    /* correct lefttime variation to framelist[] and framescorelist[] */
    fnum = 0;
    /* check for buffer overrun */
    if (wg->leftwordnum > (*maxfnum)) {
      /* expand buffer if necessary */
      free(framescorelist);
      free(framelist);
      *maxfnum = wg->leftwordnum;
      framelist = (int *)mymalloc(sizeof(int) * (*maxfnum));
      framescorelist = (LOGPROB *)mymalloc(sizeof(LOGPROB) * (*maxfnum));
#ifdef GDEBUG
      jlog("DEBUG: Notice: wg->leftwordnum exceeds maxfnum (%d > %d), expanded\n", wg->leftwordnum, *maxfnum);
#endif
    }
    for(i=0;i<wg->leftwordnum;i++) {
      left = wg->leftword[i];
      if (left->mark) continue;
      for(j=0;j<fnum;j++) {
	if (framelist[j] == left->righttime + 1) break;
      }
      if (j >= fnum) {
	framelist[fnum] = left->righttime + 1;
	/* the tail gscore contains the language score of the word,
	   so the head gscore of its right context should consider this */
	framescorelist[fnum] = left->gscore_tail - wg->left_lscore[i];
	fnum++;
      }
    }
#ifdef GDEBUG2
    jlog("DEBUG:   possible boundary of left words:");
    if (fnum == 0) {
      jlog(" (not exist)\n");
    } else {
      for(j=0;j<fnum;j++) jlog(" %d", framelist[j]);
      jlog("\n");
    }
#endif
    if (fnum == 0) continue;	/* no left context */
    /* one candidate: just move the original (or not move) */
    if (fnum == 1) {
      if (wg->lefttime != framelist[0]) {
#ifdef GDEBUG2
	jlog("DEBUG:     !moving as [%d..%d]", framelist[0], wg->righttime);
#endif
	/* check the time correctness: if the lefttime is larger than
	   righttime, this graph word has been completely overridden by
	   the left word (i.e. the aligned frames are absorbed by
	   re-alignment.  In this case this word should be removed.
	*/
	if (framelist[0] > wg->righttime) {
#ifdef GDEBUG2
	  jlog(" : eliminated");
#endif
	  wordgraph_link_context(wg);
	  wordgraph_remove_context(wg);
	  wg->mark = TRUE;
	  del_num++;
	} else {
#ifdef GDEBUG2
	  jlog(" : ok");
#endif
	  /* adjust time and score */
	  wg->lefttime = framelist[0];
	  wg->gscore_head = framescorelist[0];
	  mov_num++;
	}
#ifdef GDEBUG2
	jlog("\n");
#endif
	changed = TRUE;
      } else if (wg->gscore_head != framescorelist[0]) {
	/* adjust only score */
#ifdef GDEBUG2
	jlog("DEBUG:     !ghead score changed: %f -> %f\n", wg->gscore_head, framescorelist[0]);
#endif
	wg->gscore_head = framescorelist[0];
	mod_num++;
	changed = TRUE;
      }
    }
    if (fnum > 1) {
      /* multiple candidate: make copy for each (fnum)*/
      for(j=0;j<fnum;j++) {
	/* duplicate */
	dup_num++;
#ifdef GDEBUG2
	jlog("DEBUG:     !duping as [%d..%d]", framelist[j], wg->righttime);
#endif
	
	if (framelist[j] > wg->righttime) {
	  /* bogus link: link leftwords and rightwords, and delete this */
#ifdef GDEBUG2
	  jlog(" : eliminated");
#endif
	  for(i=0;i<wg->leftwordnum;i++) {
	    left = wg->leftword[i];
	    if (left->mark) continue;
	    if (left->righttime + 1 == framelist[j]) {
	      for(k=0;k<wg->rightwordnum;k++) {
		if ((wg->rightword[k])->mark) continue;
		if (wg->rightword[k] == left) continue;
		wordgraph_check_and_add_leftword(wg->rightword[k], left, wg->left_lscore[i]);
		wordgraph_check_and_add_rightword(left, wg->rightword[k], wg->right_lscore[k]);
	      }
	    }
	  }
	  del_num++;
	  
	} else {
	  /* really duplicate */
#ifdef GDEBUG2
	  jlog(" : ok");
#endif
	  new = wordgraph_new(wg->wid, wg->headphone, wg->tailphone, framelist[j], wg->righttime, wg->fscore_head, wg->fscore_tail, framescorelist[j], wg->gscore_tail, wg->lscore_tmp
#ifdef CM_SEARCH
			      , wg->cmscore
#else
			      , LOG_ZERO
#endif
			      );
	  /* copy corresponding link */
	  for(i=0;i<wg->leftwordnum;i++) {
	    if ((wg->leftword[i])->mark) continue;
	    if ((wg->leftword[i])->righttime + 1 == framelist[j]) {
	      wordgraph_add_leftword(new, wg->leftword[i], wg->left_lscore[i]);
	      wordgraph_add_rightword(wg->leftword[i], new, wg->left_lscore[i]);
	    }
	  }
	  for(i=0;i<wg->rightwordnum;i++) {
	    if ((wg->rightword[i])->mark) continue;
	    wordgraph_add_rightword(new, wg->rightword[i], wg->right_lscore[i]);
	    wordgraph_add_leftword(wg->rightword[i], new, wg->right_lscore[i]);
	  }
	  new->saved = TRUE;
	  new->next = *rootp;
	  *rootp = new;
	}

#ifdef GDEBUG2
	jlog("\n");
#endif
      }
      
      /* remove the original */
#ifdef GDEBUG2
      jlog("DEBUG:     !delete original [%d..%d]\n", wg->lefttime, wg->righttime);
#endif
      wordgraph_remove_context(wg);
      wg->mark = TRUE;
      dup_num--;
      
      changed = TRUE;
    }
  }

  *mov_num_ret = mov_num;
  *dup_num_ret = dup_num;
  *del_num_ret = del_num;
  *mod_num_ret = mod_num;

  *p_framelist = framelist;
  *p_framescorelist = framescorelist;

#ifdef GDEBUG2
  if (changed) {
    jlog("DEBUG: *** some graph has been altered, check loop continues\n");
  } else {
    jlog("DEBUG: *** graph not changed at last loop, check ends here\n");
  }
#endif

  return (changed);
}

/** 
 * <JA>
 * �������˶�������䥹����������Ʊ���ñ�줬�����礽����ޡ�������. 
 * 
 * @param rootp [i/o] �����ñ��ꥹ�ȤΥ롼�ȥݥ���
 * @param rest_ret [out] �ޡ�����Υ�������ñ������֤��ݥ���
 * @param merged_ret [out] �ޡ������줿ñ������֤��ݥ���
 * </JA>
 * <EN>
 * Merge duplicated words with exactly the same scores and alignments.
 * 
 * @param rootp [i/o] root pointer to the list of graph words
 * @param rest_ret [out] pointer to hold resulted number of words left in graph
 * @param merged_ret [out] pointer to hold resuled number of merged words
 * </EN>
 */
static void
wordgraph_compaction_thesame_sub(WordGraph **rootp, int *rest_ret, int *merged_ret)
{
  WordGraph *wg, *we;
  int i, count, erased, merged;

  count = 0;
  merged = 0;
  for(wg=*rootp;wg;wg=wg->next) {
    count++;
    if (wg->mark == TRUE) continue;
    for(we=wg->next;we;we=we->next) {
      if (we->mark == TRUE) continue;
      /* find the word with exactly the same time and score */
      if (wg->wid == we->wid &&
	  wg->headphone == we->headphone &&
	  wg->tailphone == we->tailphone &&
	  wg->lefttime == we->lefttime &&
	  wg->righttime == we->righttime &&
	  wg->fscore_head == we->fscore_head &&
	  wg->fscore_tail == we->fscore_tail) {
	/* merge contexts */
	merge_contexts(wg, we);
	/* swap contexts of left / right contexts */
	for(i=0;i<we->leftwordnum;i++) {
	  if (we->leftword[i]->mark) continue;
	  //if (we->leftword[i] == wg) continue;
	  swap_rightword(we->leftword[i], we, wg, we->left_lscore[i]);
	}
	for(i=0;i<we->rightwordnum;i++) {
	  if (we->rightword[i]->mark) continue;
	  //if (we->rightword[i] == wg) continue;
	  swap_leftword(we->rightword[i], we, wg, we->right_lscore[i]);
	}
	we->mark = TRUE;
	merged++;
      }
    }
  }

  erased = wordgraph_exec_erase(rootp);

  for(wg=*rootp;wg;wg=wg->next) {
    uniq_leftword(wg);
    uniq_rightword(wg);
  }

  *rest_ret = count - erased;
  *merged_ret = merged;
}

/** 
 * <JA>
 * @brief  ����ո�������Σ���ñ�춭�������Ĵ��. 
 * 
 * GRAPHOUT_PRECISE_BOUNDARY ���������³ñ��˰�¸�������Τ�ñ�춭��
 * �����뤿��ˡ�õ����ˤ����ơ������ñ��������������Ȥ˼���Ÿ������
 * ����Ū��ñ�춭�����ư������. ���Τ��ᡤ�����ñ��Τ�ġʰ�ư���Ρ�
 * ��������Ȥ��б����Ȥ�ʤ��ʤ�Τǡ�õ����λ��˳�ñ��������ñ���
 * ������ñ�춭�������¤����뤳�Ȥ���������Ȥ�. 
 *
 * ñ�춭���Τ����ñ��֤����¤��뤿�ᡤ���٤Ƥ�ñ�춭����ư���ʤ��ʤ�ޤ�
 * Ĵ���������֤����. ����ʥ���դǤ�û��ñ���ʨ�������ǽ����������ʤ�
 * ��礬���뤬�����ξ�� GRAPHOUT_LIMIT_BOUNDARY_LOOP ����ꤹ�뤳�Ȥǡ�
 * �����֤����ξ�¤� graphout_limit_boundary_loop_num �����¤Ǥ���. 
 * 
 * @param rootp [i/o] ñ�쥰��դΥ롼�ȥΡ��ɤؤΥݥ���
 * @param r [i/o] ǧ���������󥹥���
 * </JA>
 * <EN>
 * @brief  Post-processing step 2: Adjust word boundaries.
 * 
 * When GRAPHOUT_PRECISE_BOUNDARY is defined, the word boundaries will be
 * moved depending on the later word expansion to get context-dependent
 * precise boundaries.  So the precise boundary, modified after generation
 * while search, should be propagated to the context words in the post
 * processing.
 *
 * Since the affect of word boundaries may propagate to the context words,
 * the adjustment procedure has to be executed iteratively until all the
 * boundaries are fixated.  However, when graph is large, the oscillation of
 * short words will results in very long loop.  By defining
 * GRAPHOUT_LIMIT_BOUNDARY_LOOP, the number of the adjustment loop can be
 * up to the number specified by graphout_limit_bounrady_loop_num.
 * 
 * @param rootp [i/o] pointer to root node of a word graph
 * @param r [i/o] recognition process instance
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
void
wordgraph_adjust_boundary(WordGraph **rootp, RecogProcess *r)
{
#ifdef GRAPHOUT_PRECISE_BOUNDARY
  WordGraph *wg;
  int mov_num, dup_num, del_num, mod_num;
  int count, merged;
  boolean flag;
  int loopcount;
  int maxfnum;
  int *framelist;		///< frame list for adjust_boundary_sub
  LOGPROB *framescorelist;	///< frame score list for adjust_boundary_sub

  loopcount = 0;

  if (verbose_flag) jlog("STAT: graphout: step 2: adjust boundaries\n");
  mov_num = dup_num = del_num = 0;

  /* count number of all words */
  count = 0;
  for(wg=*rootp;wg;wg=wg->next) count++;
  maxfnum = 0;

  do {
    /* do adjust */
    flag = wordgraph_adjust_boundary_sub(rootp, &mov_num, &dup_num, &del_num, &mod_num, count, &maxfnum, r->peseqlen, r->lmtype, &framelist, &framescorelist);
    /* do compaction */
    wordgraph_compaction_thesame_sub(rootp, &count, &merged);
    if (verbose_flag) jlog("STAT: graphout: #%d: %d moved, %d duplicated, %d purged, %d modified, %d idential, %d left\n", loopcount + 1, mov_num, dup_num, del_num, mod_num, merged, count);
#ifdef GRAPHOUT_LIMIT_BOUNDARY_LOOP
    if (++loopcount >= r->config->graph.graphout_limit_boundary_loop_num) {
      if (verbose_flag) jlog("STAT: graphout: loop count reached %d, terminate loop now\n", r->config->graph.graphout_limit_boundary_loop_num);
      break;
    }
#endif
  } while (flag);

  /* free work area allocated in adjust_boundary_sub */
  if (maxfnum > 0) {
    free(framescorelist);
    free(framelist);
  }

  /* execute erase of marked words */
  wordgraph_exec_erase(rootp);

#else

  if (verbose_flag) jlog("STAT: graphout: step 2: SKIP (adjusting boundaries)\n");

#endif /* GRAPHOUT_PRECISE_BOUNDARY */

}

 
/** 
 * <JA>
 * @brief  ����ո�������Σ���ñ���«�͡ʴ���Ʊ���
 * 
 * ñ�춭���������ʬʸ���⥹�����������˰��פ���Ʊ��ñ��ɤ������Ĥ�«�ͤ�. 
 * 
 * @param rootp [i/o] ñ�쥰��դΥ롼�ȥΡ��ɤؤΥݥ���
 * </JA>
 * <EN>
 * @brief  Post-processing step 3: Bundle words (exactly the same ones)
 * 
 * This function bundles same words which have exactly the same
 * boundaries and partial sentence scores.
 * 
 * @param rootp [i/o] pointer to root node of a word graph
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
void
wordgraph_compaction_thesame(WordGraph **rootp)
{
  int rest, erased;

  if (verbose_flag) jlog("STAT: graphout: step 3: merge idential hypotheses (same score, boundary, context)\n");
  wordgraph_compaction_thesame_sub(rootp, &rest, &erased);
  if (verbose_flag) jlog("STAT: graphout: %d words merged, %d words left in lattice\n", erased, rest);
}

/** 
 * <JA>
 * @brief  ����ո�������Σ���ñ���«�͡ʶ��Ʊ���
 * 
 * ñ�춭�����郎���פ���Ʊ��ñ��ɤ������Ĥ�«�ͤ�. ��������
 * Ʊ��Ǥʤ��Ƥ�«�ͤ���. ���ξ�硤��ʬʸ���������Ǥ�⤤���䤬
 * �Ĥ�. graph_merge_neighbor_range �� �� �ξ��ϼ¹Ԥ���ʤ�. 

 * 
 * @param rootp [i/o] ñ�쥰��դΥ롼�ȥΡ��ɤؤΥݥ���
 * @param r [i/o] ǧ���������󥹥���
 * </JA>
 * <EN>
 * @brief  Post-processing step 4: Bundle words (same boundaries)
 * 
 * This function bundles the same words which have exactly the same
 * boundaries, allowing having different scores.  The word with
 * the best partial sentence score will be adopted.  This function
 * will not take effect when graph_merge_neightbor_range is lower than 0.
 * 
 * @param rootp [i/o] pointer to root node of a word graph
 * @param r [i/o] recognition process instance
 * </EN>
 *
 * @callgraph
 * @callergraph
 *
 */
void
wordgraph_compaction_exacttime(WordGraph **rootp, RecogProcess *r)
{
  WordGraph *wg, *we;
  int i, count, erased;

  if (r->config->graph.graph_merge_neighbor_range < 0) {
    if (verbose_flag) jlog("STAT: graphout: step 4: SKIP (merge the same words with same boundary to the most likely one\n");
    return;
  }

  if (verbose_flag) jlog("STAT: graphout: step 4: merge same words with same boundary to the most likely one\n");

  count = 0;
  for(wg=*rootp;wg;wg=wg->next) {
    count++;
    if (wg->mark == TRUE) continue;
    for(we=wg->next;we;we=we->next) {
      if (we->mark == TRUE) continue;
      /* find same words at same position */
      if (wg->wid == we->wid &&
	  wg->lefttime == we->lefttime &&
	  wg->righttime == we->righttime) {
	/* merge contexts */
	merge_contexts(wg, we);
	/* swap contexts of left / right contexts */
	for(i=0;i<we->leftwordnum;i++) {
	  swap_rightword(we->leftword[i], we, wg, we->left_lscore[i]);
	}
	for(i=0;i<we->rightwordnum;i++) {
	  swap_leftword(we->rightword[i], we, wg, we->right_lscore[i]);
	}
	/* keep the max score */
	if (wg->fscore_head < we->fscore_head) {
	  wg->headphone = we->headphone;
	  wg->tailphone = we->tailphone;
	  wg->fscore_head = we->fscore_head;
	  wg->fscore_tail = we->fscore_tail;
	  wg->gscore_head = we->gscore_head;
	  wg->gscore_tail = we->gscore_tail;
	  wg->lscore_tmp = we->lscore_tmp;
#ifdef CM_SEARCH
	  wg->cmscore = we->cmscore;
#endif
	  wg->amavg = we->amavg;
	}
	we->mark = TRUE;
      }
    }
  }
  erased = wordgraph_exec_erase(rootp);
  if (verbose_flag) jlog("STAT: graphout: %d words merged, %d words left in lattice\n", erased, count-erased);

  for(wg=*rootp;wg;wg=wg->next) {
    uniq_leftword(wg);
    uniq_rightword(wg);
  }
}

/** 
 * <JA>
 * @brief  ����ո�������Σ���ñ���«�͡ʶ�˵��֡�
 * 
 * ����ñ�춭����������Ʊ��ñ��ɤ������Ĥ�«�ͤ�. �������������
 * graph_merge_neighbor_range ��Ϳ�������줬 0 ����Ǥ�����ϼ¹Ԥ���ʤ�. 
 * 
 * @param rootp [i/o] ñ�쥰��դΥ롼�ȥΡ��ɤؤΥݥ���
 * @param r [i/o] ǧ���������󥹥���
 * </JA>
 * <EN>
 * @brief  Post-processing step 5: Bundle words (neighbor words)
 * 
 * This function bundles the same words which appears at similar place.
 * If the difference of both the left boundary and right right boundary
 * is under graph_merge_neighbor_range, it will be bundled.
 * If its value is lower than or equal to 0, this function does not take
 * effect.
 * 
 * @param rootp [i/o] pointer to root node of a word graph
 * @param r [i/o] recognition process instance
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
void
wordgraph_compaction_neighbor(WordGraph **rootp, RecogProcess *r)
{
  WordGraph *wg, *we;
  int i, count, erased;

  if (r->config->graph.graph_merge_neighbor_range <= 0) {
    if (verbose_flag) jlog("STAT: graphout: step 5: SKIP (merge the same words around)\n");
    return;
  }

  if (verbose_flag) jlog("STAT: graphout: step 5: merge same words around, with %d frame margin\n", r->config->graph.graph_merge_neighbor_range);

  count = 0;
  for(wg=*rootp;wg;wg=wg->next) {
    count++;
    if (wg->mark == TRUE) continue;
    for(we=wg->next;we;we=we->next) {
      if (we->mark == TRUE) continue;
      if (wg->wid == we->wid &&
	  abs(wg->lefttime - we->lefttime) <= r->config->graph.graph_merge_neighbor_range &&
	  abs(wg->righttime - we->righttime) <= r->config->graph.graph_merge_neighbor_range) {
	/* merge contexts */
	merge_contexts(wg, we);
	/* swap contexts of left / right contexts */
	for(i=0;i<we->leftwordnum;i++) {
	  swap_rightword(we->leftword[i], we, wg, we->left_lscore[i]);
	}
	for(i=0;i<we->rightwordnum;i++) {
	  swap_leftword(we->rightword[i], we, wg, we->right_lscore[i]);
	}
	/* keep the max score */
	if (wg->fscore_head < we->fscore_head) {
	  wg->headphone = we->headphone;
	  wg->tailphone = we->tailphone;
	  wg->fscore_head = we->fscore_head;
	  wg->fscore_tail = we->fscore_tail;
	  wg->gscore_head = we->gscore_head;
	  wg->gscore_tail = we->gscore_tail;
	  wg->lscore_tmp = we->lscore_tmp;
#ifdef CM_SEARCH
	  wg->cmscore = we->cmscore;
#endif
	  wg->amavg = we->amavg;
	}
	we->mark = TRUE;
      }
    }
  }
  erased = wordgraph_exec_erase(rootp);
  if (verbose_flag) jlog("STAT: graphout: %d words merged, %d words left in lattice\n", erased, count-erased);

  for(wg=*rootp;wg;wg=wg->next) {
    uniq_leftword(wg);
    uniq_rightword(wg);
  }
 
}

/**************************************************************/
/* generation of graph word candidates while search */

/** 
 * <JA>
 * ������ñ�쥰��ո�������������֤�. ���λ����ǤϤޤ�ñ�쥰�����ˤ�
 * ��Ͽ����Ƥ��ʤ�. 
 * 
 * @param wid [in] ñ��ID
 * @param wid_left [in] word ID of left context for determining head phone 
 * @param wid_right [in] word ID of right context for determining tail phone 
 * @param leftframe [in] ��ü����(�ե졼��)
 * @param rightframe [in] ��ü����(�ե졼��)
 * @param fscore_head [in] ��ü�Ǥ���ʬʸ������ (g + h)
 * @param fscore_tail [in] ��ü�Ǥ���ʬʸ������ (g + h)
 * @param gscore_head [in] ��Ƭ�Ǥ�������ü�����Viterbi������ (g)
 * @param gscore_tail [in] �����Ǥ�������ü�����Viterbi������ (g)
 * @param lscore [in] ���쥹����
 * @param cm [in] ������
 * @param r [in] ǧ���������󥹥���
 * 
 * @return �������������줿�����ñ�����ؤΥݥ���
 * </JA>
 * <EN>
 * Return a newly allocated graph word candidates.  The resulting word
 * is not registered to the word graph yet.
 * 
 * @param wid [in] word ID
 * @param wid_left [in] word ID of left context for determining head phone 
 * @param wid_right [in] word ID of right context for determining tail phone 
 * @param leftframe [in] beginning time in frames
 * @param rightframe [in] end time in frames
 * @param fscore_head [in] sentence score on search at word head (g + h)
 * @param fscore_tail [in] sentence score on search at word tail (g + h)
 * @param gscore_head [in] Viterbi score accumulated from input end at word head (g)
 * @param gscore_tail [in] Viterbi score accumulated from input end at word tail (g)
 * @param lscore [in] language score
 * @param cm [in] confidence score
 * @param r [in] recognition process instance
 * 
 * @return pointer to the newly created graph word candidate.
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
WordGraph *
wordgraph_assign(WORD_ID wid, WORD_ID wid_left, WORD_ID wid_right, int leftframe, int rightframe, LOGPROB fscore_head, LOGPROB fscore_tail, LOGPROB gscore_head, LOGPROB gscore_tail, LOGPROB lscore, LOGPROB cm, RecogProcess *r)
{
  WordGraph *newarc;
  HMM_Logical *l, *ret, *head, *tail;
  WORD_INFO *winfo;

  winfo = r->lm->winfo;

  /* find context dependent phones at head and tail */
  l = winfo->wseq[wid][winfo->wlen[wid]-1];
  if (wid_right != WORD_INVALID) {
    ret = get_right_context_HMM(l, winfo->wseq[wid_right][0]->name, r->am->hmminfo);
    if (ret != NULL) l = ret;
  }
  if (winfo->wlen[wid] > 1) {
    tail = l;
    l = winfo->wseq[wid][0];
  }
  if (wid_left != WORD_INVALID) {
    ret = get_left_context_HMM(l, winfo->wseq[wid_left][winfo->wlen[wid_left]-1]->name, r->am->hmminfo);
    if (ret != NULL) l = ret;
  }
  head = l;
  if (winfo->wlen[wid] <= 1) {
    tail = l;
  }

  /* generate a new graph word hypothesis */
  newarc = wordgraph_new(wid, head, tail, leftframe, rightframe, fscore_head, fscore_tail, gscore_head, gscore_tail, lscore, cm);
  //jlog("DEBUG:     [%d..%d] %d\n", leftframe, rightframe, wid);
  return newarc;
}

/** 
 * <JA>
 * �����ñ������ñ�쥰��դΰ����Ȥ��Ƴ��ꤹ��. ���ꤵ�줿�����ñ��ˤ�
 * saved �� TRUE �����åȤ����. 
 * 
 * @param wg [i/o] ��Ͽ���륰���ñ�����
 * @param right [i/o] @a wg �α�����ƥ����ȤȤʤ�ñ��
 * @param root [i/o] ����Ѥ�ñ�쥰��դΥ롼�ȥΡ��ɤؤΥݥ���
 * </JA>
 * <EN>
 * Register a graph word candidate to the word graph as a member.
 * The registered word will have the saved member to be set to TRUE.
 * 
 * @param wg [i/o] graph word candidate to be registered
 * @param right [i/o] right context graph word
 * @param root [i/o] pointer to root node of already registered word graph
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
void
wordgraph_save(WordGraph *wg, WordGraph *right, WordGraph **root)
{
  if (wg != NULL) {
    wg->next = *root;
    *root = wg;
    wg->saved = TRUE;
    wordgraph_add_leftword(right, wg, wg->lscore_tmp);
    wordgraph_add_rightword(wg, right, wg->lscore_tmp);
  }
}

#ifdef GRAPHOUT_DYNAMIC

/** 
 * <JA>
 * ����ñ�쥰��ո���ˤĤ��ơ����˳��ꤷ�������ñ�����Ʊ�����֤�
 * Ʊ��ñ�줬���뤫�ɤ�����Ĵ�٤�. �⤷����С�ñ�쥰��ո����
 * ����ƥ����Ȥ򤽤γ���Ѥߥ����ñ��˥ޡ�������. 
 *
 * GRAPHOUT_SEARCH������ϡ�����ˤ�����õ������ߤ��٤����ɤ�����Ƚ�ꤹ��. 
 * ���ʤ������ñ�첾�⤬���Υ����ñ��κ�����ƥ����ȤȤ��ƴ��˳��ꤷ��
 * �����ñ����ˤ���С�����ʾ��Ÿ�������פ�õ������ߤ��٤���Ƚ�ꤹ��. 
 * 
 * @param now [i/o] ñ�쥰��ո���
 * @param root [i/o] ����Ѥ�ñ�쥰��դΥ롼�ȥΡ��ɤؤΥݥ���
 * @param next_wid [in] ��ñ�첾��
 * @param merged_p [out] õ������ߤ��٤��ʤ� TRUE��³�Ԥ��Ƥ褱���
 * FALSE ����Ǽ����� (GRAPHOUT_SEARCH �����)
 * @param jconf [in] õ��������ѥ�᡼��
 * 
 * @return Ʊ�����֤�Ʊ��ñ�줬���ä���硤�ޡ����������
 * ����Ѥߥ����ñ��ؤΥݥ��󥿤��֤�. �⤷�ʤ��ä���硤NULL ���֤�. 
 * </JA>
 * <EN>
 * Check if a graph word with the same word ID and same position as the
 * given graph word candidate exists in the already registered word graph.
 * If such graph word is found, the word contexts of the given word
 * graph candidate will be merged to the found graph word in the registered
 * word graph.
 *
 * When GRAPHOUT_SEARCH is defined, whether to terminate the search at here
 * will be determined here.  That is, if the next word in search already
 * exists in the list of left context words of the merged graph word,
 * it is determined that the next path has already been expanded and thus
 * there is no need to proceed more on this hypothesis.
 * 
 * @param now [i/o] graph word candidate
 * @param root [i/o] pointer to root node of already registered word graph
 * @param next_wid [in] next word on search
 * @param merged_p [out] will be set to TRUE if search should be terminated,
 * or FALSE if search should be proceeded (when GRAPHOUT_SEARCH defined)
 * @param jconf [in] configuration parameters for this search
 * 
 * @return the pointer to the already registered graph word when the same
 * word was found on the same position, or NULL if such word not found in
 * already registered word graph.
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
WordGraph *
wordgraph_check_merge(WordGraph *now, WordGraph **root, WORD_ID next_wid, boolean *merged_p, JCONF_SEARCH *jconf)
{
  WordGraph *wg;
  int i;
#ifdef GDEBUG
  WordGraph *w;
#endif

#ifdef GRAPHOUT_SEARCH
  *merged_p = FALSE;
#endif

  if (now == NULL) return(NULL);

#ifdef GDEBUG
  jlog("DEBUG: check_merge: checking \"%s\"[%d..%d]\n", wchmm_local->winfo->woutput[now->wid], now->lefttime, now->righttime);
  for(i=0;i<now->leftwordnum;i++) {
    w = now->leftword[i];
    jlog("DEBUG: \t left%d:  \"%15s\"[%d..%d]\n", i, wchmm_local->winfo->woutput[w->wid], w->lefttime, w->righttime);
  }
  for(i=0;i<now->rightwordnum;i++) {
    w = now->rightword[i];
    jlog("DEBUG: \tright%d:  \"%15s\"[%d..%d]\n", i, wchmm_local->winfo->woutput[w->wid], w->lefttime, w->righttime);
  }
#endif

  for(wg=*root;wg;wg=wg->next) {
    if (wg == now) continue;
#ifdef GRAPHOUT_DYNAMIC
    /* skip already merged word */
    if (wg->purged) continue;
#endif
    if (jconf->graph.graph_merge_neighbor_range < 0) {
      /* when no merging, words with different triphone context at word edge
	 should be differenciated */
      if (wg->headphone != now->headphone || wg->tailphone != now->tailphone) {
	continue;
      }
    }
    if (wg->wid == now->wid
	&& wg->lefttime == now->lefttime
	&& wg->righttime == now->righttime) {
      /* same word on the same position is found in current word graph */
#ifdef GDEBUG
      jlog("DEBUG: check_merge: same word found: \"%s\"[%d..%d]\n", wchmm_local->winfo->woutput[wg->wid], wg->lefttime, wg->righttime);
      for(i=0;i<wg->leftwordnum;i++) {
	w = wg->leftword[i];
	jlog("DEBUG: \t left%d:  \"%15s\"[%d..%d]\n", i, wchmm_local->winfo->woutput[w->wid], w->lefttime, w->righttime);
      }
      for(i=0;i<wg->rightwordnum;i++) {
	w = wg->rightword[i];
	jlog("DEBUG: \tright%d:  \"%15s\"[%d..%d]\n", i, wchmm_local->winfo->woutput[w->wid], w->lefttime, w->righttime);
      }
#endif
      /* merge contexts */
      merge_contexts(wg, now);
      /* swap contexts of left / right contexts */
      for(i=0;i<now->leftwordnum;i++) {
	swap_rightword(now->leftword[i], now, wg, now->left_lscore[i]);
	uniq_rightword(now->leftword[i]);
      }
      for(i=0;i<now->rightwordnum;i++) {
	swap_leftword(now->rightword[i], now, wg, now->right_lscore[i]);
	uniq_leftword(now->rightword[i]);
      }
#ifdef GRAPHOUT_SEARCH
      /* if the left and right contexts of now are already included in wg,
	 and wg already has left node of next word,
	 it means that
	 the current word and the last word context is
	 already included in the existing word graph.
	 So, in the case this partial path should be abandoned.
      */
      for(i=0;i<wg->leftwordnum;i++) {
	if (wg->leftword[i]->wid == next_wid) break;
      }
      if (i < wg->leftwordnum) {
	*merged_p = TRUE;
      }
#endif /* GRAPHOUT_SEARCH */
#ifdef GRAPHOUT_OVERWRITE
      /*  if current hypothesis score is higher than saved,
	  overwrite the scores and not terminate */
      if (
#ifdef GRAPHOUT_OVERWRITE_GSCORE
	  //wg->gscore_head < now->gscore_head
	  wg->amavg < now->amavg;
#else
	  wg->fscore_head < now->fscore_head
#endif
	  ) {
	  wg->headphone = now->headphone;
	  wg->tailphone = now->tailphone;
	  wg->fscore_head = now->fscore_head;
	  wg->fscore_tail = now->fscore_tail;
	  wg->gscore_head = now->gscore_head;
	  wg->gscore_tail = now->gscore_tail;
	  wg->lscore_tmp = now->lscore_tmp;
#ifdef CM_SEARCH
	  wg->cmscore = now->cmscore;
#endif
	  wg->amavg = now->amavg;
#ifdef GRAPHOUT_SEARCH
	  *merged_p = FALSE;
#endif
      }
#endif /* GRAPHOUT_OVERWRITE */
      /* the merged word should be discarded for later merging from
	 another word, so disable this */
      now->purged = TRUE;
      
      /* return the found one */
      return wg;
    }
  }
  /* if the same word not found, return NULL */
  return NULL;
}
#endif /* GRAPHOUT_DYNAMIC */

/**************************************************************/
/* misc. functions */

/** 
 * <JA>
 * �����ñ��ξ����ƥ����Ȥǽ��Ϥ���. ���Ƥϰʲ��ΤȤ��ꡧ
 * <pre>
 *   ID: left=������ƥ����Ȥ�ID[,ID,...] right=������ƥ�����ID[,ID,..]
 *   [��ü�ե졼��..��ü�ե졼��]
 *   wid=ñ��ID
 *   name="ñ��̾"
 *   lname="N-gram ñ��̾�����뤤�ϥ��ƥ����ֹ� (Julian)"
 *   f=õ����κ�ü�Ǥ���ʬʸ������(g(n) + h(n+1)) n=����ñ��
 *   f_prev=õ����α�ü�Ǥ���ʬʸ������(g(n-1) + h(n)) n=����ñ��
 *   g_head=��ü�Ǥ�����Viterbi������ g(n)
 *   g_prev=��ü�Ǥ�����Viterbi������ g(n-1) + LM(n)
 *   lscore=���쥹���� LM(n)   (Julius �ξ��Τ�)
 *   AMavg=�ե졼��ʿ�Ѳ�������
 *   cmscore=ñ�쿮����
 * </pre>
 *
 * @param fp [in] ������Υե�����ݥ���
 * @param wg [in] ���Ϥ��륰���ñ��
 * @param winfo [in] ñ�켭��
 * </JA>
 * <EN>
 * Output information of a graph word in text in the format below:
 * (n means the word)
 *
 * <pre>
 *   ID: left=left_context_ID[,ID,...] right=right_context_ID[,ID,...]
 *   [left_edge_frame...right_edge_frame]
 *   wid=word_id
 *   name="word string"
 *   lname="N-gram word string (Julius) or category number (Julian)"
 *   f="partial sentence score at left edge (g(n) + h(n+1)) on search time"
 *   f_prev="partial sentence score at right edge (g(n-1) + h(n)) on search time"
 *   g_head="accumulated viterbi score at left edge (g(n))"
 *   g_prev="accumulated viterbi score at right edge (g(n-1) + LM(n)"
 *   lscore="language score LM(n)  (Julius only)"
 *   AMavg="average acoustic likelihood per frame"
 *   cmscore="confidence score"
 * </pre>
 * @param fp [in] file pointer to which output should go
 * @param wg [in] graph word to output
 * @param winfo [in] word dictionary
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
void
put_wordgraph(FILE *fp, WordGraph *wg, WORD_INFO *winfo)
{
  int i;
  if (fp == NULL) return;
  if (wg == NULL) {
    fprintf(fp, "(NULL)\n");
  } else {
    fprintf(fp, "%d:", wg->id);
    fprintf(fp, " [%d..%d]", wg->lefttime, wg->righttime);
    for(i=0;i<wg->leftwordnum;i++) {
      fprintf(fp, (i == 0) ? " left=%d" : ",%d", wg->leftword[i]->id);
    }
    for(i=0;i<wg->rightwordnum;i++) {
      fprintf(fp, (i == 0) ? " right=%d" : ",%d", wg->rightword[i]->id);
    }
    for(i=0;i<wg->leftwordnum;i++) {
      fprintf(fp, (i == 0) ? " left_lscore=%f" : ",%f", wg->left_lscore[i]);
    }
    for(i=0;i<wg->rightwordnum;i++) {
      fprintf(fp, (i == 0) ? " right_lscore=%f" : ",%f", wg->right_lscore[i]);
    }
    fprintf(fp, " lscore_tmp=%f", wg->lscore_tmp);

    fprintf(fp, " wid=%d name=\"%s\" lname=\"%s\" f=%f f_prev=%f g_head=%f g_prev=%f", wg->wid, winfo->woutput[wg->wid], winfo->wname[wg->wid], wg->fscore_head, wg->fscore_tail, wg->gscore_head, wg->gscore_tail);
    fprintf(fp, " forward_score=%f backword_score=%f", wg->forward_score, wg->backward_score);
    if (wg->righttime - wg->lefttime + 1 != 0) {
      fprintf(fp, " AMavg=%f", wg->amavg);
    }
#ifdef CM_SEARCH
    fprintf(fp, " cmscore=%f", wg->cmscore);
#endif
    fprintf(fp, " graphcm=%f", wg->graph_cm);
    fprintf(fp, " headphone=%s", wg->headphone->name);
    fprintf(fp, " tailphone=%s", wg->tailphone->name);
    fprintf(fp, "\n");
  }
}

/** 
 * <JA>
 * �������줿ñ�쥰��������ñ���ƥ����Ƚ��Ϥ���. 
 * 
 * @param fp [in] ������Υե�����ݥ���
 * @param root [in] ñ�쥰��դΥ롼�ȥΡ��ɤؤΥݥ���
 * @param winfo [in] ñ�켭��
 * </JA>
 * <EN>
 * Output text information of all the words in word graph.
 * 
 * @param fp [in] file pointer to which output should go
 * @param root [in] pointer to root node of a word graph
 * @param winfo [in] word dictionary
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
void
wordgraph_dump(FILE *fp, WordGraph *root, WORD_INFO *winfo)
{
  WordGraph *wg;
  fprintf(fp, "--- begin wordgraph data ---\n");
  for(wg=root;wg;wg=wg->next) {
    put_wordgraph(fp, wg, winfo);
  }
  fprintf(fp, "--- end wordgraph data ---\n");
}

/** 
 * <JA>
 * �ǥХå��ѡ�ñ�쥰��դ�������������å�����. 
 * 
 * @param rootp [in] ñ�쥰��դΥ롼�ȥΡ��ɤؤΥݥ���
 * @param r [i/o] ǧ���������󥹥���
 * </JA>
 * <EN>
 * For debug: Check the coherence in word graph.
 * 
 * @param rootp [in] pointer to root node of a word graph
 * @param r [i/o] recognition process instance
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
void
wordgraph_check_coherence(WordGraph *rootp, RecogProcess *r)
{
  WordGraph *wg, *wl, *wr;
  int nl, nr;
  WORD_INFO *winfo;

  winfo = r->lm->winfo;

  for(wg=rootp;wg;wg=wg->next) {
    /* check ID overflow */
    if (wg->id < 0 || wg->id >= r->graph_totalwordnum) {
      jlog("ERROR: invalid graph word id \"%d\" (should be [0..%d])\n", wg->id, r->graph_totalwordnum-1);
      put_wordgraph(jlog_get_fp(), wg, winfo);
      continue;
    }
    /* check link */
    for(nl=0;nl<wg->leftwordnum;nl++){
      wl = wg->leftword[nl];
      if (wl->id < 0 || wl->id >= r->graph_totalwordnum) {
	jlog("ERROR: invalid graph word id \"%d\" (should be [0..%d]) in left context\n", wl->id, r->graph_totalwordnum-1);
	put_wordgraph(jlog_get_fp(), wg, winfo);
	continue;
      }
      for(nr=0;nr<wl->rightwordnum;nr++){
	if (wl->rightword[nr] == wg) break;
      }
      if (nr >= wl->rightwordnum) {
	jlog("ERROR: on graph, reverse link not found in left context\n");
	put_wordgraph(jlog_get_fp(), wg, winfo);
	put_wordgraph(jlog_get_fp(), wl, winfo);
	continue;
      }
    }
    for(nr=0;nr<wg->rightwordnum;nr++){
      wr = wg->rightword[nr];
      if (wr->id < 0 || wr->id >= r->graph_totalwordnum) {
	jlog("ERROR: invalid graph word id \"%d\" (should be [0..%d]) in right context\n", wr->id, r->graph_totalwordnum-1);
	put_wordgraph(jlog_get_fp(), wg, winfo);
	continue;
      }
      for(nl=0;nl<wr->leftwordnum;nl++){
	if (wr->leftword[nl] == wg) break;
      }
      if (nl >= wr->leftwordnum) {
	jlog("ERROR: on graph, reverse link not found in left context\n");
	put_wordgraph(jlog_get_fp(), wg, winfo);
	put_wordgraph(jlog_get_fp(), wr, winfo);
	continue;
      }
    }
  }
}


/* lattice-based posterior probability computation by forward-backward
   algorithm */
/** 
 * <EN>
 * qsort callback function to order words from right to left.
 * </EN>
 * <JA>
 * ñ��򱦤��麸���¤٤뤿��� qsort ������Хå��ؿ�
 * </JA>
 * 
 * @param x [in] 1st element
 * @param y [in] 2nd element
 * 
 * @return value required by qsort
 * 
 */
static int
compare_forward(WordGraph **x, WordGraph **y)
{
  if ((*x)->righttime < (*y)->righttime) return 1;
  else if ((*x)->righttime > (*y)->righttime) return -1;
  else return 0;
}

/** 
 * <EN>
 * qsort callback function to order words from left to right.
 * </EN>
 * <JA>
 * ñ��򺸤��鱦���¤٤뤿��� qsort ������Хå��ؿ�
 * </JA>
 * 
 * @param x [in] 1st element
 * @param y [in] 2nd element
 * 
 * @return value required by qsort
 * 
 */
static int
compare_backward(WordGraph **x, WordGraph **y)
{
  if ((*x)->lefttime < (*y)->lefttime) return -1;
  else if ((*x)->lefttime > (*y)->lefttime) return 1;
  else return 0;
}

/** 
 * <EN>
 * �����п���ɽ������Ƥ����Ψ���¤�׻�����. 
 * </EN>
 * <JA>
 * compute addition of two probabilities in log10 form.
 * </JA>
 * 
 * @param x [in] first value
 * @param y [in] second value
 * 
 * @return value of log(10^x + 10^y)
 * 
 */
static LOGPROB 
addlog10(LOGPROB x, LOGPROB y)
{
  if (x < y) {
    //return(y + log10(1 + pow(10, x-y)));
    return(y + log(1 + pow(10, x-y)) * INV_LOG_TEN);
  } else {
    return(x + log(1 + pow(10, y-x)) * INV_LOG_TEN);
  }
}

/**
 * <JA>
 * �������줿��ƥ�����ˤ����ơ�forward-backward ���르�ꥺ��ˤ��
 * �����٤�׻�����. �׻����줿�ͤϳƥ����ñ��� graph_cm �˳�Ǽ�����. 
 * �����Ψ�η׻��Ǥϡ�õ����ο����ٷ׻���Ʊ��
 * ���͡�r->config->annotate.cm_alpha�ˤ��Ѥ�����. 
 * </JA>
 * <EN>
 * Compute graph-based confidence scores by forward-backward parsing on
 * the generated lattice.  The computed scores are stored in graph_cm of
 * each graph words.  The same alpha value of search-time confidence scoring
 * (r->config->annotate.cm_alpha) will be used to compute the posterior
 * probabilities.
 * </EN>
 * 
 * @param root [in] root graph node
 * @param r [in] recognition process instance
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */ 
void
graph_forward_backward(WordGraph *root, RecogProcess *r)
{
  WordGraph *wg, *left, *right;
  int i, j;
  LOGPROB s;
  LOGPROB sum1, sum2;
  int count;
  WordGraph **wlist;
  LOGPROB cm_alpha;

  cm_alpha = r->config->annotate.cm_alpha;

  /* make a wordgraph list for frame-sorted access */
  count = 0;
  for(wg=root;wg;wg=wg->next) count++;
  if (count == 0) return;
  wlist = (WordGraph **)mymalloc(sizeof(WordGraph *) * count);
  i = 0;
  for(wg=root;wg;wg=wg->next) {
    wlist[i++] = wg;
  }

  /* sort wordgraph list downward by the right frame*/
  qsort(wlist, count, sizeof(WordGraph *), (int (*)(const void *, const void *))compare_forward);
  /* clear forward scores */
  for(wg=root;wg;wg=wg->next) {
    wg->forward_score = LOG_ZERO;
  }
  /* forward procedure */
  sum1 = LOG_ZERO;
  for(i=0;i<count;i++) {
    wg = wlist[i];
    if (wg->righttime == r->peseqlen - 1) {
      /* set initial score */
      wg->forward_score = 0.0;
      //wg->forward_score += wg->lscore * cm_alpha;
    } else {
      /* (just a bogus check...) */
      if (wg->forward_score == LOG_ZERO) {
	wordgraph_dump(stdout, root, r->lm->winfo);
	put_wordgraph(stdout, wg, r->lm->winfo);
	j_internal_error("NO CONTEXT?\n");
      }
    }
    /* propagate scores */
    s = wg->amavg * (wg->righttime - wg->lefttime + 1);
    s *= cm_alpha;
    s += wg->forward_score;
    if (wg->lefttime == 0) {
      /* add for sum */
      sum1 = addlog10(sum1, s);
    } else {
      /* propagate to left words */
      for(j=0;j<wg->leftwordnum;j++) {
	left = wg->leftword[j];
	left->forward_score = addlog10(left->forward_score, s + wg->left_lscore[j] * cm_alpha);
      }
    }
  }

  /* sort wordgraph list downward by the right score */
  qsort(wlist, count, sizeof(WordGraph *), (int (*)(const void *, const void *))compare_backward);
  /* clear backward scores */
  for(wg=root;wg;wg=wg->next) {
    wg->backward_score = LOG_ZERO;
  }
  /* backward procedure */
  sum2 = LOG_ZERO;
  for(i=0;i<count;i++) {
    wg = wlist[i];
    if (wg->lefttime == 0) {
      /* set initial score */
      wg->backward_score = 0.0;
    } else {
      /* (just a bogus check...) */
      if (wg->backward_score == LOG_ZERO) {
	put_wordgraph(stdout, wg, r->lm->winfo);
	j_internal_error("NO CONTEXT?\n");
      }
    }
    /* propagate scores */
    s = wg->amavg * (wg->righttime - wg->lefttime + 1);
    s *= cm_alpha;
    s += wg->backward_score;
    if (wg->righttime == r->peseqlen - 1) {
      /* add for sum */
      //sum2 = addlog10(sum2, s + wg->lscore * cm_alpha);
      sum2 = addlog10(sum2, s);
    } else {
      for(j=0;j<wg->rightwordnum;j++) {
	right = wg->rightword[j];
	right->backward_score = addlog10(right->backward_score, s + wg->right_lscore[j] * cm_alpha);
      }
    }
  }

  if (verbose_flag) jlog("STAT: graph_cm: forward score = %f, backward score = %f\n", sum1, sum2);

  /* compute CM */
  for(wg=root;wg;wg=wg->next) {
    s = wg->amavg * (wg->righttime - wg->lefttime + 1);
    s *= cm_alpha;
    s = wg->backward_score + s + wg->forward_score;
    wg->graph_cm = pow(10, s - sum1);
    //wg->graph_cm = s - sum1;
  }

  free(wlist);

}
/* end of file */
