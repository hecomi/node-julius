/**
 * @file   search_bestfirst_main.c
 * 
 * <JA>
 * @brief  ��2�ѥ��������å��ǥ����ǥ���
 *
 * Julius ����2�ѥ��Ǥ��륹���å��ǥ����ǥ��󥰥��르�ꥺ�ब���Ҥ���
 * �Ƥ��ޤ�. ��1�ѥ��η�̤�ñ��ȥ�ꥹ����򸵤ˡ���1�ѥ��Ȥϵո���
 * �� right-to-left ��õ����Ԥ��ޤ�. ����Υ������ϡ���1�ѥ��Υȥ��
 * ���Ȥ��Υ�������̤õ�����Υҥ塼�ꥹ�ƥ��å��Ȥ�����³���뤳�Ȥǡ�
 * ʸ���Τβ��⥹�������θ���ʤ���õ����Ԥ��ޤ�. 
 *
 * ��ñ�콸��μ����Τ���ˡ�ñ��N-gram�Ǥ� ngram_decode.c ��δؿ�����
 * ʸˡ�Ǥ� dfa_decode.c �δؿ����Ѥ����ޤ�. 
 * 
 * </JA>
 * 
 * <EN>
 * @brief  The second pass: stack decoding
 *
 * This file implements search algorithm based on best-first stack
 * decoding on the 2nd pass.  The search will be performed on backward
 * (i.e. right-to-left) direction, using the result of 1st pass (word
 * trellis) as heuristics of unreached area.  Hypothesis are stored
 * in a global stack, and the best one will be expanded according to
 * the survived words in the word trellis and language constraint.
 *
 * The expanding words will be given by ngram_decode.c for N-gram
 * based recognition, with their langugage probabilities, or by
 * dfa_decode.c for grammar-based recognition, with their emitting
 * DFA state information.
 *
 * </EN>
 * 
 * @author Akinobu Lee
 * @date   Thu Sep 08 11:51:12 2005
 *
 * $Revision: 1.15 $
 * 
 */
/*
 * Copyright (c) 1991-2016 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2016 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#include <julius/julius.h>

/* declaration of local functions */
static NODE *get_best_from_stack(NODE **start, int *stacknum);
static int put_to_stack(NODE *new, NODE **start, NODE **bottom, int *stacknum, int stacksize);
static void put_all_in_stack(NODE **start, int *stacknum, WORD_INFO *winfo);
static void free_all_nodes(NODE *node);
static void put_hypo_woutput(NODE *hypo, WORD_INFO *winfo);
static void put_hypo_wname(NODE *hypo, WORD_INFO *winfo);

/**********************************************************************/
/********** ��ñ���Ǽ�ΰ�γ������          *************************/
/********** allocate memory for nextword data *************************/
/**********************************************************************/

/** 
 * <JA>
 * ��ñ��γ�Ǽ�ΰ�γ������. 
 * ��ñ�������Ǽ���뤿��� NEXTWORD ����˥�������դ���. 
 * 
 * @param maxlen [out] ��Ǽ��ǽ��ñ���
 * @param root [out] ����դ��ΰ����Ƭ�ؤΥݥ���
 * @param max [in] ����դ����ΰ�Υ�����
 * 
 * @return ����դ���줿��ñ������ؤΥݥ��󥿤��֤�. 
 * </JA>
 * <EN>
 * Allocate memory for next word candidates.
 * Allocate NEXTWORD array for storing list of candidate next words.
 * 
 * @param maxlen [out] maximum number of words that can be stored
 * @param root [out] pointer to the top address of allocated data
 * @param max [in] number of elementes to be allocated
 * 
 * @return the newly allocated pointer of NEXTWORD array.
 * </EN>
 */
static NEXTWORD **
nw_malloc(int *maxlen, NEXTWORD **root, int max)
{
  NEXTWORD *nwtmp;
  NEXTWORD **nw;
  int i;

  nw = (NEXTWORD **)mymalloc(max * sizeof(NEXTWORD *));
  nwtmp = (NEXTWORD *)mymalloc(max * sizeof(NEXTWORD));
  for (i=0;i<max; i++) {
    nw[i] = &(nwtmp[i]);
  }
  *maxlen = max;
  *root = nwtmp;
  return nw;
}

/** 
 * <JA>
 * ��ñ��γ�Ǽ�ΰ�β���. 
 * 
 * @param nw [in] NEXTWORD����
 * @param root [in] nw_malloc() ��Ϳ����줿�ΰ���Ƭ�ؤΥݥ���
 * </JA>
 * <EN>
 * Free next word candidate area.
 * 
 * @param nw [in] pointer to NEXTWORD structure to be free.
 * @param root [in] pointer to the top address of allocated data previously
 * returned by nw_malloc()
 * </EN>
 */
static void
nw_free(NEXTWORD **nw, NEXTWORD *root)
{
  free(root);
  free(nw);
}

/** 
 * <JA>
 * @brief  ��ñ������Ǽ�Ѥ� NEXTWORD ����Υ����ΰ��ĥ����. 
 *
 * ���δؿ���õ����˼�ñ����佸�礬��줿�ݤ˸ƤФ졤����ˤ��¿����
 * ��ñ�������Ǽ�Ǥ���褦 NEXTWORD ����Ȥ� realloc() ����. 
 * �ºݤˤϺǽ�� nw_malloc() �Ǽ����ñ���ʬ�����ΰ����ݤ��Ƥ��ꡤ
 * ñ��N-gram���ѻ��ϸƤФ�뤳�ȤϤʤ�. ʸˡǧ���Ǥϡ����硼�ȥݡ�����
 * �����å׽����ˤ����֤ΰۤʤ�����Ʊ����Ÿ������Τǡ�
 * ��ñ��������ÿ������礭�����Ȥ������ꤦ��. 
 * 
 * @param nwold [i/o] NEXTWORD����
 * @param maxlen [i/o] �����Ǽ�����Ǽ����ݥ���. ���ߤκ����Ǽ����
 * ����ƸƤӡ��ؿ���ǿ����˳��ݤ��줿�����ѹ������. 
 * @param root [i/o] �ΰ���Ƭ�ؤΥݥ��󥿤��Ǽ���륢�ɥ쥹. �ؿ����
 * �񤭴�������.
 * @param num [in] ��Ĺ����Ĺ��
 * 
 * @return ��ĥ���줿�����ʼ�ñ������ؤΥݥ��󥿤��֤�. 
 * </JA>
 * <EN>
 * @brief  expand data area of NEXTWORD.
 *
 * In DFA mode, the number of nextwords can exceed the vocabulary size when
 * more than one DFA states are expanded by short-pause skipping.
 * In such case, the nextword data area should expanded here.
 * 
 * @param nwold [i/o] NEXTWORD array
 * @param maxlen [i/o] pointer to the maximum number of words that can be
 * stored.  The current number should be stored before calling this function,
 * and the resulting new number will be stored within this function.
 * @param root [i/o] address to the pointer of the allocated data.  The
 * value will be updated by reallocation in this function.
 * @param num [in] size to expand
 * 
 * @return the newlly re-allocated pointer of NEXTWORD array.
 * </EN>
 */
static NEXTWORD **
nw_expand(NEXTWORD **nwold, int *maxlen, NEXTWORD **root, int num)
{
  NEXTWORD *nwtmp;
  NEXTWORD **nw;
  int i;
  int nwmaxlen;

  nwmaxlen = *maxlen + num;

  nwtmp = (NEXTWORD *)myrealloc(*root, nwmaxlen * sizeof(NEXTWORD));
  nw = (NEXTWORD **)myrealloc(nwold, nwmaxlen * sizeof(NEXTWORD *));
  nw[0] = nwtmp;
  for (i=1;i<nwmaxlen; i++) {
    nw[i] = &(nwtmp[i]);
  }
  *maxlen = nwmaxlen;
  *root = nwtmp;
  return nw;
}


/**********************************************************************/
/********** ���⥹���å������         ********************************/
/********** Hypothesis stack operation ********************************/
/**********************************************************************/

/** 
 * <JA>
 * �����å��ȥåפκ��ಾ�����Ф�. 
 * 
 * @param start [i/o] �����å�����Ƭ�Ρ��ɤؤΥݥ��󥿡ʽ񴹤������礢���
 * @param stacknum [i/o] ���ߤΥ����å��������ؤΥݥ��󥿡ʽ񤭴��������
 * 
 * @return ���Ф������ಾ��Υݥ��󥿤��֤�. 
 * </JA>
 * <EN>
 * Pop the best hypothesis from stack.
 * 
 * @param start [i/o] pointer to stack top node (will be modified if necessary)
 * @param stacknum [i/o] pointer to the current stack size (will be modified
 * if necessary)
 * 
 * @return pointer to the popped hypothesis.
 * </EN>
 */
static NODE *
get_best_from_stack(NODE **start, int *stacknum)
{
  NODE *tmp;

  /* return top */
  tmp=(*start);
  if ((*start)!=NULL) {		/* delete it from stack */
    (*start)=(*start)->next;
    if ((*start)!=NULL) (*start)->prev=NULL;
    (*stacknum)--;
    return(tmp);
  }
  else {
    return(NULL);
  }
}

/** 
 * <JA>
 * ���벾�⤬�����å���˳�Ǽ����뤫�ɤ��������å�����. 
 * 
 * @param new [in] �����å����벾��
 * @param bottom [in] �����å�����Ρ��ɤؤΥݥ���
 * @param stacknum [in] �����å��˸��߳�Ǽ����Ƥ���Ρ��ɿ��ؤΥݥ���
 * @param stacksize [in] �����å��ΥΡ��ɿ��ξ��
 * 
 * @return �����å��Υ���������¤�ã���Ƥ��ʤ���������������Ρ��ɤ���
 * �褱��г�Ǽ�����Ȥ��� 0 �򡤤���ʳ��Ǥ���г�Ǽ�Ǥ��ʤ��Ȥ��� -1 ��
 * �֤�. 
 * </JA>
 * <EN>
 * Check whether a hypothesis will be stored in the stack.
 * 
 * @param new [in] hypothesis to be checked
 * @param bottom [in] pointer to stack bottom node
 * @param stacknum [in] pointer to current stack size
 * @param stacksize [in] pointer to maximum stack size limit
 * 
 * @return 0 if it will be stored in the stack (in case @a stacknum <
 * @a stacksize or the score of @a new is better than bottom.  Otherwise
 * returns -1, which means it can not be pushed to the stack.
 * </EN>
 */
static int
can_put_to_stack(NODE *new, NODE **bottom, int *stacknum, int stacksize)
{
  /* stack size check */
  if ((*stacknum + 1) > stacksize && (*bottom)->score >= new->score) {
    /* new node is below the bottom: discard it */
    return(-1);
  }
  return(0);
}

/** 
 * <JA>
 * �����å��˿����ʲ�����Ǽ����. 
 * �����å���Υ���������θ�������֤����������. 
 * ��Ǽ�Ǥ��ʤ��ä���硤Ϳ����줿����� free_node() �����. 
 * 
 * @param new [in] �����å����벾��
 * @param start [i/o] �����å��ΥȥåץΡ��ɤؤΥݥ���
 * @param bottom [i/o] �����å�����Ρ��ɤؤΥݥ���
 * @param stacknum [i/o] �����å��˸��߳�Ǽ����Ƥ���Ρ��ɿ��ؤΥݥ���
 * @param stacksize [in] �����å��ΥΡ��ɿ��ξ��
 * 
 * @return ��Ǽ�Ǥ���� 0 �򡤤Ǥ��ʤ��ä����� -1 ���֤�. 
 * </JA>
 * <EN>
 * Push a new hypothesis into the stack, keeping score order.
 * If not succeeded, the given new hypothesis will be freed by free_node().
 * 
 * @param new [in] hypothesis to be checked
 * @param start [i/o] pointer to stack top node
 * @param bottom [i/o] pointer to stack bottom node
 * @param stacknum [i/o] pointer to current stack size
 * @param stacksize [in] pointer to maximum stack size limit
 * 
 * @return 0 if succeded, or -1 if failed to push because of number
 * limitation or too low score.
 * </EN>
 */
static int
put_to_stack(NODE *new, NODE **start, NODE **bottom, int *stacknum, int stacksize)
{
  NODE *tmp;

  /* stack size is going to increase... */
  (*stacknum)++;

  /* stack size check */
  if ((*stacknum)>stacksize) {
    /* stack size overflow */
    (*stacknum)--;
    if ((*bottom)->score < new->score) {
      /* new node will be inserted in the stack: free the bottom */
      tmp=(*bottom);
      (*bottom)->prev->next=NULL;
      (*bottom)=(*bottom)->prev;
      free_node(tmp);
    } else {
      /* new node is below the bottom: discard it */
      free_node(new);
      return(-1);
    }
  }

  /* insert new node on edge */
  if ((*start)==NULL) {		/* no node in stack */
    /* new node is the only node */
    (*start)=new;
    (*bottom)=new;
    new->next=NULL;
    new->prev=NULL;
    return(0);
  }
  if ((*start)->score <= new->score) {
    /* insert on the top */
    new->next = (*start);
    new->next->prev = new;
    (*start)=new;
    new->prev=NULL;
    return(0);
  }
  if ((*bottom)->score >= new->score) {
    /* insert on the bottom */
    new->prev = (*bottom);
    new->prev->next = new;
    (*bottom)=new;
    new->next=NULL;
    return(0);
  }
    
  /* now the new node is between (*start) and (*bottom) */
  if (((*start)->score + (*bottom)->score) / 2 > new->score) {
    /* search from bottom */
    tmp=(*bottom);
    while(tmp->score < new->score) tmp=tmp->prev;
    new->prev=tmp;
    new->next=tmp->next;
    tmp->next->prev=new;
    tmp->next=new;
  } else {
    /* search from start */
    tmp=(*start);
    while(tmp->score > new->score) tmp=tmp->next;
    new->next=tmp;
    new->prev=tmp->prev;
    tmp->prev->next=new;
    tmp->prev=new;
  }
  return(0);
}

/** 
 * <JA>
 * �����å�����Ȥ����ƽ��Ϥ���. �����å�����Ȥϼ�����. (�ǥХå���)
 * 
 * @param start [i/o] �����å��ΥȥåץΡ��ɤؤΥݥ���
 * @param stacknum [i/o] �����å��˸��߳�Ǽ����Ƥ���Ρ��ɿ��ؤΥݥ���
 * @param winfo [in] ñ�켭��
 * </JA>
 * <EN>
 * Output all nodes in the stack. All nodes will be lost (for debug).
 * 
 * @param start [i/o] pointer to stack top node
 * @param stacknum [i/o] pointer to current stack size
 * @param winfo [in] word dictionary
 * </EN>
 */
static void
put_all_in_stack(NODE **start, int *stacknum, WORD_INFO *winfo)
{
  NODE *ntmp;
  
  jlog("DEBUG: hypotheses remained in global stack\n");
  while ((ntmp = get_best_from_stack(start, stacknum)) != NULL) {
    jlog("DEBUG: %3d: s=%f",*stacknum, ntmp->score);
    put_hypo_woutput(ntmp, winfo);
    free_node(ntmp);
  }
}

/** 
 * <JA>
 * �����å������������������. 
 * 
 * @param start [i/o] �����å��ΥȥåץΡ���
 * </JA>
 * <EN>
 * Free all nodes in a stack.
 * 
 * @param start [i/o] stack top node
 * </EN>
 */
static void
free_all_nodes(NODE *start)
{
  NODE *tmp;
  NODE *next;

  tmp=start;
  while(tmp) {
    next=tmp->next;
    free_node(tmp);
    tmp=next;
  }
}


#ifdef CONFIDENCE_MEASURE

/**********************************************************************/
/********** ñ�쿮���٤η׻� ******************************************/
/********** Confidence scoring ****************************************/
/**********************************************************************/

#ifdef CM_SEARCH
/**************************************************************/
/**** CM computation method 1(default):                  ******/
/****   - Use local posterior probabilities while search ******/
/****   - Obtain CM at hypothesis expansion time         ******/
/**************************************************************/

/** 
 * <JA>
 * CM�׻��ѤΥѥ�᡼������������. CM�׻���ľ���˸ƤӽФ����. 
 *
 * @param sd [i/o] ��2�ѥ��ѥ�����ꥢ
 * @param wnum [in] �����å�������
 * @param cm_alpha [in] ���Ѥ��륹���������
 * 
 * </JA>
 * <EN>
 * Initialize parameters for confidence scoring (will be called at
 * each startup of 2nd pass)
 *
 * @param sd [i/o] work area for 2nd pass
 * @param wnum [in] stack size
 * @param cm_alpha [in] scaling value to use at confidence scoring
 * </EN>
 */
static void
cm_init(StackDecode *sd, int wnum, LOGPROB cm_alpha
#ifdef CM_MULTIPLE_ALPHA
	, cm_alpha_num
#endif
	)
{
  sd->l_stacksize = wnum;
  sd->l_start = sd->l_bottom = NULL;
  sd->l_stacknum = 0;
  sd->cm_alpha = cm_alpha;
#ifdef CM_MULTIPLE_ALPHA
  if (sd->cmsumlist) {
    if (sd->cmsumlistlen < cm_alpha_num) {
      free(sd->cmsumlist);
      sd->cmsumlist = NULL;
    }
  }
  if (sd->cmsumlist == NULL) {
    sd->cmsumlist = (LOGPROB *)mymalloc(sizeof(LOGPROB) * cm_alpha_num);
    sd->cmsumlistlen = cm_alpha_num;
  }
#endif    
}

/** 
 * <JA>
 * CM�׻��Τ���˥����륹���å���Ÿ���������Ū����¸����. 
 * 
 * @param sd [i/o] ��2�ѥ��ѥ�����ꥢ
 * @param new [in] Ÿ������
 * </JA>
 * <EN>
 * Store an expanded hypothesis to the local stack for later CM scoring
 * 
 * @param sd [i/o] work area for 2nd pass
 * @param new [in] expanded hypothesis
 * </EN>
 */
static void
cm_store(StackDecode *sd, NODE *new)
{
  /* store the generated hypo into local stack */
  put_to_stack(new, &(sd->l_start), &(sd->l_bottom), &(sd->l_stacknum), sd->l_stacksize);
}

/** 
 * <JA>
 * CM�׻��Τ���˥����륹���å���β���νи���Ψ�ι�פ����.
 *
 * @param sd [i/o] ��2�ѥ��ѥ�����ꥢ
 * 
 * </JA>
 * <EN>
 * Compute sum of probabilities for hypotheses in the local stack for
 * CM scoring.
 *
 * @param sd [i/o] work area for 2nd pass
 * 
 * </EN>
 */
static void
cm_sum_score(StackDecode *sd
#ifdef CM_MULTIPLE_ALPHA
	     , bgn, end, step
#endif
)
{
  NODE *node;
  LOGPROB sum;
#ifdef CM_MULTIPLE_ALPHA
  LOGPROB a;
  int j;
#endif

  if (sd->l_start == NULL) return;	/* no hypo */
  sd->cm_tmpbestscore = sd->l_start->score; /* best hypo is at the top of the stack */

#ifdef CM_MULTIPLE_ALPHA
  for (j = 0, a = bgn; a <= end; a += step) {
    sum = 0.0;
    for(node = sd->l_start; node; node = node->next) {
      sum += pow(10, a * (node->score - sd->cm_tmpbestscore));
    }
    sd->cmsumlist[j++] = sum;	/* store sums for each alpha coef. */
  }
#else
  sum = 0.0;
  for(node = sd->l_start; node; node = node->next) {
    sum += pow(10, sd->cm_alpha * (node->score - sd->cm_tmpbestscore));
  }
  sd->cm_tmpsum = sum;		/* store sum */
#endif

}

/** 
 * <JA>
 * Ÿ�����줿����ʸ����ˤĤ��ơ�����Ÿ��ñ��ο����٤򡤻����Ψ��
 * ��Ť��Ʒ׻�����. 
 * 
 * @param sd [i/o] ��2�ѥ��ѥ�����ꥢ
 * @param node [i/o] Ÿ�����줿����ʸ����
 * </JA>
 * <EN>
 * Compute confidence score of a new word at the end of the given hypothesis,
 * based on the local posterior probabilities.
 * 
 * @param sd [i/o] work area for 2nd pass
 * @param node [i/o] expanded hypothesis
 * </EN>
 */
static void
cm_set_score(StackDecode *sd, NODE *node
#ifdef CM_MULTIPLE_ALPHA
	     , bgn, end, step
#endif
	     )
{
#ifdef CM_MULTIPLE_ALPHA
  int j;
  LOGPROB a;
#endif

#ifdef CM_MULTIPLE_ALPHA
  for (j = 0, a = bgn; a <= end; a += step) {
    node->cmscore[node->seqnum-1][j] = pow(10, a * (node->score - sd->cm_tmpbestscore)) / sd->cmsumlist[j];
    j++;
  }
#else
  node->cmscore[node->seqnum-1] = pow(10, sd->cm_alpha * (node->score - sd->cm_tmpbestscore)) / sd->cm_tmpsum;
#endif
}

/** 
 * <JA>
 * CM�׻��ѤΥ����륹���å����鲾�����Ф�. 
 * 
 * @param sd [i/o] ��2�ѥ��ѥ�����ꥢ
 * 
 * @return ���Ф��줿ʸ������֤�. 
 * </JA>
 * <EN>
 * Pop one node from local stack for confidence scoring.
 * 
 * @param sd [i/o] work area for 2nd pass
 * 
 * @return the popped hypothesis.
 * </EN>
 */
static NODE *
cm_get_node(StackDecode *sd)
{
  return(get_best_from_stack(&(sd->l_start), &(sd->l_stacknum)));
}

#endif /* CM_SEARCH */

#ifdef CM_NBEST
/*****************************************************************/
/**** CM computation method 2: conventional N-best scoring *******/
/**** NOTE: enough N-best should be computed (-n 10 ~ -n 100) ****/
/*****************************************************************/

/** 
 * <JA>
 * �����å���ˤ���ʸ���䤫��ñ�쿮���٤�׻�����. 
 * 
 * @param sd [i/o] ��2�ѥ��ѥ�����ꥢ
 * @param start [in] �����å�����Ƭ�Ρ���
 * @param stacknum [in] �����å�������
 * @param jconf [in] SEARCH������ѥ�᡼��
 * </JA>
 * <EN>
 * Compute confidence scores from N-best sentence candidates in the
 * given stack.
 * 
 * @param sd [i/o] work area for 2nd pass
 * @param start [in] stack top node 
 * @param stacknum [in] current stack size
 * @param jconf [in] SEARCH configuration parameters
 * </EN>
 */
static void
cm_compute_from_nbest(StackDecode *sd, NODE *start, int stacknum, JCONF_SEARCH *jconf)
{
  NODE *node;
  LOGPROB bestscore, sum, s;
  WORD_ID w;
  int i;
  LOGPROB cm_alpha;
  int j;

  /* prepare buffer */
#ifdef CM_MULTIPLE_ALPHA
  if (sd->cmsumlist) {
    if (sd->cmsumlistlen < jconf->annotate.cm_alpha_num) {
      free(sd->cmsumlist);
      sd->cmsumlist = NULL;
    }
  }
  if (sd->cmsumlist == NULL) {
    sd->cmsumlist = (LOGPROB *)mymalloc(sizeof(LOGPROB) * jconf->annotate.cm_alpha_num);
    sd->cmsumlistlen = cm_alpha_num;
  }
#endif    
  if (sd->sentcm == NULL) {		/* not allocated yet */
    sd->sentcm = (LOGPROB *)mymalloc(sizeof(LOGPROB)*stacknum);
    sd->sentnum = stacknum;
  } else if (sd->sentnum < stacknum) { /* need expanded */
    sd->sentcm = (LOGPROB *)myrealloc(sd->sentcm, sizeof(LOGPROB)*stacknum);
    sd->sentnum = stacknum;
  }
  if (sd->wordcm == NULL) {
    sd->wordcm = (LOGPROB *)mymalloc(sizeof(LOGPROB) * winfo->num);
    sd->wordnum = winfo->num;
  } else if (sd->wordnum < winfo->num) {
    sd->wordcm = (LOGPROB *)myremalloc(sd->wordcm, sizeof(LOGPROB) * winfo->num);
    sd->wordnum = winfo->num;
  }
  
  cm_alpha = jconf->annotate.cm_alpha;
#ifdef CM_MULTIPLE_ALPHA
  for (j = 0, cm_alpha = jconf->annotate.cm_alpha_bgn; cm_alpha <= jconf->annotate.cm_alpha_end; cm_alpha += jconf->annotate.cm_alpha_step) {
#endif
    /* clear whole word cm buffer */
    for(w=0;w<sd->wordnum;w++) {
      sd->wordcm[w] = 0.0;
    }
    /* get best score */
    bestscore = start->score;
    /* compute sum score of all hypothesis */
    sum = 0.0;
    for (node = start; node != NULL; node = node->next) {
      sum += pow(10, cm_alpha * (node->score - bestscore));
    }
    /* compute sentence posteriori probabilities */
    i = 0;
    for (node = start; node != NULL; node = node->next) {
      sd->sentcm[i] = pow(10, cm_alpha * (node->score - bestscore)) / sum;
      i++;
    }
    /* compute word posteriori probabilities */
    i = 0;
    for (node = start; node != NULL; node = node->next) {
      for (w=0;w<node->seqnum;w++) {
	sd->wordcm[node->seq[w]] += sd->sentcm[i];
      }
      i++;
    }
    /* store the probabilities to node */
    for (node = start; node != NULL; node = node->next) {
      for (w=0;w<node->seqnum;w++) {
#ifdef CM_MULTIPLE_ALPHA
	node->cmscore[w][j] = sd->wordcm[node->seq[w]];
#else	
	node->cmscore[w] = sd->wordcm[node->seq[w]];
#endif
      }
    }
#ifdef CM_MULTIPLE_ALPHA
    j++;
  }
#endif
}

#endif /* CM_NBEST */

#endif /* CONFIDENCE_MEASURE */


/**********************************************************************/
/********** Enveloped best-first search *******************************/
/**********************************************************************/

/*
 * 1. Word envelope
 *
 * ���β���ӡ�����������: Ÿ�����Ȥʤä�����ο��򤽤β���Ĺ(ñ���)
 * ���Ȥ˥�����Ȥ���. �������ۤ����餽����û������ϰʸ�Ÿ�����ʤ�. 
 * 
 * Introduce a kind of beam width to search tree: count the number of
 * popped hypotheses per the depth of the hypotheses, and when a count
 * in a certain depth reaches the threshold, all hypotheses shorter than
 * the depth will be dropped from candidates.
 *
 */

/** 
 * <JA>
 * Word envelope �Ѥ˥����󥿤���������.
 *
 * @param s [i/o] ��2�ѥ��ѥ�����ꥢ
 * 
 * </JA>
 * <EN>
 * Initialize counters fro word enveloping.
 * 
 * @param s [i/o] work area for 2nd pass
 * </EN>
 */
static void
wb_init(StackDecode *s)
{
  int i;
  for(i=0;i<=MAXSEQNUM;i++) s->hypo_len_count[i] = 0;
  s->maximum_filled_length = -1;
}

/** 
 * <JA>
 * Word envelope �򻲾Ȥ��ơ�Ϳ����줿�����Ÿ�����Ƥ褤���ɤ������֤�. 
 * �ޤ���Word envelope �Υ����󥿤򹹿�����. 
 * 
 * @param s [i/o] ��2�ѥ��ѥ�����ꥢ
 * @param now [in] ������Ÿ�����褦�Ȥ��Ƥ��벾��
 * @param width [in] Ÿ��������Ȥξ����
 * 
 * @return Ÿ����ǽ��Ÿ��������Ȥ���¤�ã���Ƥ��ʤ��ˤʤ� TRUE,
 * Ÿ���Բ�ǽ�ʥ�����Ȥ���¤�ã���Ƥ���ˤʤ� FALSE ���֤�. 
 * </JA>
 * <EN>
 * Consult the current word envelope to check if word expansion from
 * the hypothesis node is allowed or not.  Also increment the counter
 * of word envelope if needed.
 * 
 * @param s [i/o] work area for 2nd pass
 * @param now [in] popped hypothesis
 * @param width [in] maximum limit of expansion count
 * 
 * @return TRUE if word expansion is allowed (in case word envelope count
 * of the corresponding hypothesis depth does not reach the limit), or
 * FALSE if already prohibited.
 * </EN>
 */
static boolean
wb_ok(StackDecode *s, NODE *now, int width)
{
  if (now->seqnum <= s->maximum_filled_length) {
    /* word expansion is not allowed because a word expansion count
       at deeper level has already reached the limit */
    return FALSE;
  } else {
    /* word expansion is possible.  Increment the word expansion count
       of the given depth */
    s->hypo_len_count[now->seqnum]++;
    if (s->hypo_len_count[now->seqnum] > width) {
      /* the word expansion count of this level has reached the limit, so
	 set the beam-filled depth to this level to inhibit further
	 expansion of shorter hypotheses. */
      if (s->maximum_filled_length < now->seqnum) s->maximum_filled_length = now->seqnum;
    }
    return TRUE;
  }
}

#ifdef SCAN_BEAM
/*
 * 2. Score envelope
 *
 * Viterbi�׻��̤κ︺: ���ϥե졼�ऴ�Ȥκ������� (score envelope) ��
 * ������ˤ錄�äƵ�Ͽ���Ƥ���. ��������������ٷ׻����ˡ����� envelope
 * ����������ʾ她�����������Ȥ���Viterbi �ѥ��α黻�����Ǥ���. 
 *
 * �����Ǥϡ����Ф������⤫��ե졼�ऴ�Ȥ� score envelope �򹹿�����
 * ��ʬ�����Ҥ���Ƥ���. Envelope ���θ���� Viterbi �׻��μºݤ�
 * scan_word() �򻲾ȤΤ���. 
 *
 * Reduce computation cost of hypothesis Viterbi processing by setting a
 * "score envelope" that holds the maximum scores at every frames
 * throughout the expanded hypotheses.  When calculating Viterbi path
 * on HMM trellis for updating score of popped hypothesis, Viterbi paths
 * that goes below a certain range from the score envelope are dropped.
 *
 * These functions are for updating the score envelope according to the
 * popped hypothesis.  For actual Viterbi process with score envelope, 
 * see scan_word().
 *
 */

/** 
 * <JA>
 * Score envelope ����������. ��2�ѥ��γ��ϻ��˸ƤФ��. 
 * 
 * @param s [i/o] ��2�ѥ��ѥ�����ꥢ
 * @param framenum [in] ���ϥե졼��Ĺ
 * </JA>
 * <EN>
 * Initialize score envelope.  This will be called once at the beginning
 * of 2nd pass.
 * 
 * @param s [i/o] work area for 2nd pass
 * @param framenum [in] input frame length
 * </EN>
 */
static void
envl_init(StackDecode *s, int framenum)
{
  int i;
  for(i=0;i<framenum;i++) s->framemaxscore[i] = LOG_ZERO;
}

/** 
 * <JA>
 * ��������������������� score envelope �򹹿�����. 
 * 
 * @param s [i/o] ��2�ѥ��ѥ�����ꥢ
 * @param n [in] ����
 * @param framenum [in] ���ϥե졼��Ĺ
 * </JA>
 * <EN>
 * Update the score envelope using forward score of the given hypothesis.
 * 
 * @param s [i/o] work area for 2nd pass
 * @param n [in] hypothesis
 * @param framenum [in] input frame length
 * </EN>
 */
static void
envl_update(StackDecode *s, NODE *n, int framenum)
{
  int t;
  for(t=framenum-1;t>=0;t--) {
    if (s->framemaxscore[t] < n->g[t]) s->framemaxscore[t] = n->g[t];
  }
}
#endif /* SCAN_BEAM */


/**********************************************************************/
/********** Short pause segmentation **********************************/
/**********************************************************************/

/** 
 * <JA>
 * ǧ����̤��顤�������϶�֤�ǧ���򳫻Ϥ���ݤν��ñ������򥻥åȤ���. 
 * Ʃ��줪��Ӳ���ν�ʣ���θ���ƽ��ñ�����򤬷��ꤵ���. 
 * 
 * @param hypo [in] ���ߤ����϶�֤�ǧ����̤Ȥ��Ƥ�ʸ����
 * @param r [in] ǧ���������󥹥���
 * </JA>
 * <EN>
 * Set the previous word context for the recognition of the next input
 * segment from the current recognition result.  The initial context word
 * will be chosen from the current recognition result skipping transparent
 * word and multiplied words.
 * 
 * @param hypo [in] sentence candidate as a recognition result of current
 * input segment
 * @param r [in] recognition process instance
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
void
segment_set_last_nword(NODE *hypo, RecogProcess *r)
{
  int i;
  WORD_ID w;

  if (r->sp_break_last_nword_allow_override) {
    for(i=0;i<hypo->seqnum;i++) {
      w = hypo->seq[i];
      if (w != r->sp_break_last_word
	  && !is_sil(w, r)
	  && !r->lm->winfo->is_transparent[w]
	  ) {
	r->sp_break_last_nword = w;
	break;
      }
    }
#ifdef SP_BREAK_DEBUG
    printf("sp_break_last_nword=%d[%s]\n", r->sp_break_last_nword, r->lm->winfo->woutput[r->sp_break_last_nword]);
#endif
  } else {
    r->sp_break_last_nword = WORD_INVALID;
  }
}


/**********************************************************************/
/********* Debug output of hypothesis while search ********************/
/**********************************************************************/

/** 
 * <JA>
 * �ǥХå��Ѥ˲����ñ�����ɽ������. 
 * 
 * @param hypo [in] ����
 * @param winfo [in] ñ�켭��
 * </JA>
 * <EN>
 * Output word sequence of a hypothesis for debug.
 * 
 * @param hypo [in] hypothesis
 * @param winfo [in] word dictionary
 * </EN>
 */
static void
put_hypo_woutput(NODE *hypo, WORD_INFO *winfo)
{
  int i,w;

  if (hypo != NULL) {
    for (i=hypo->seqnum-1;i>=0;i--) {
      w = hypo->seq[i];
      jlog(" %s", winfo->woutput[w]);
    }
  }
  jlog("\n");  
}

/** 
 * <JA>
 * �ǥХå��Ѥ˲����ñ��N-gram����ȥ�̾��Julian�Ǥϥ��ƥ����ֹ�ˤ���Ϥ���. 
 * 
 * @param hypo [in] ����
 * @param winfo [in] ñ�켭��
 * </JA>
 * <EN>
 * Output N-gram entries (or DFA category IDs) of a hypothesis for debug.
 * 
 * @param hypo [in] hypothesis
 * @param winfo [in] word dictionary
 * </EN>
 */
static void
put_hypo_wname(NODE *hypo, WORD_INFO *winfo)
{
  int i,w;

  if (hypo != NULL) {
    for (i=hypo->seqnum-1;i>=0;i--) {
      w = hypo->seq[i];
      jlog(" %s", winfo->wname[w]);
    }
  }
  jlog("\n");  
}

/** 
 * <EN>
 * Save a hypothesis as a recognition result f 2nd pass.
 * </EN>
 * <JA>
 * ��2�ѥ��η�̤Ȥ��Ʋ������¸����. 
 * </JA>
 * 
 * @param hypo [in] hypothesis to save
 * @param r [in] recognition process instance
 * 
 */
static void
store_result_pass2(NODE *hypo, RecogProcess *r)
{
  int i;
  Sentence *s;

  s = &(r->result.sent[r->result.sentnum]);

  s->word_num = hypo->seqnum;
  for (i = 0; i < hypo->seqnum; i++) {
    s->word[i] = hypo->seq[hypo->seqnum - 1 - i];
  }
#ifdef CONFIDENCE_MEASURE
  for (i = 0; i < hypo->seqnum; i++) {
    s->confidence[i] = hypo->cmscore[hypo->seqnum - 1 - i];
  }
#endif

  s->score = hypo->score;
  s->score_lm = hypo->totallscore;
  s->score_am = hypo->score - hypo->totallscore;

#ifdef USE_MBR
  s->score_mbr = hypo->score_mbr;
#endif

  if (r->lmtype == LM_DFA) {
    /* output which grammar the hypothesis belongs to on multiple grammar */
    /* determine only by the last word */
    if (multigram_get_all_num(r->lm) > 0) {
      s->gram_id = multigram_get_gram_from_category(r->lm->winfo->wton[hypo->seq[0]], r->lm);
    } else {
      s->gram_id = 0;
    }
  }

  r->result.sentnum++;
  /* add to tail */
}


/**********************************************************************/
/******** Output top 'ncan' hypotheses in a stack and free all ********/
/**********************************************************************/

/** 
 * <JA>
 * �����å������̤β������Ф���ǧ����̤Ȥ��ƽ��Ϥ���. ����ˡ�
 * �����å��˳�Ǽ����Ƥ������Ƥβ�����������. 
 *
 * ����줿ʸ����ϡ����ä����̳�Ǽ�ѤΥ����å��˳�Ǽ�����. õ����
 * λ��"-n" �ο�����ʸ���䤬���Ĥ��뤫��õ�������Ǥ����ˤθ塤���Ū
 * ������줿ʸ������椫����N�ġ�"-output" �ǻ��ꤵ�줿���ˤβ����
 * ���Ϥ���.
 *
 * ���꤬����Х��饤����Ȥ⤳���Ǽ¹Ԥ���. 
 * 
 * @param r_start [i/o] ��̳�Ǽ�ѥ����å�����Ƭ�Ρ��ɤؤΥݥ���
 * @param r_bottom [i/o] ��̳�Ǽ�ѥ����å�����Ρ��ɤؤΥݥ���
 * @param r_stacknum [i/o] �����å��˳�Ǽ����Ƥ���Ρ��ɿ��ؤΥݥ���
 * @param ncan [in] ���Ϥ����̲����
 * @param r [in] ǧ���������󥹥���
 * @param param [in] ���ϥѥ�᡼��
 * </JA>
 * <EN>
 * Output top N-best hypotheses in a stack as a recognition result, and
 * free all hypotheses.
 *
 * The sentence candidates found at the 2nd pass will be pushed to the
 * "result stack" instead of immediate output.  After recognition is
 * over (in case the number of found sentences reaches the number
 * specified by "-n", or search has been terminated in other reason),
 * the top N sentence candidates in the stack will be output as a
 * final result (where N should be specified by "-output").  After
 * then, all the hypotheses in the stack will be freed.
 *
 * Additionally, forced alignment for the recognized sentence
 * will be executed here if required.
 * 
 * @param r_start [i/o] pointer to the top node of the result stack
 * @param r_bottom [i/o] pointer to the bottom node of the result stack
 * @param r_stacknum [i/o] number of candidates in the current result stack
 * @param ncan [in] number of sentence candidates to be output
 * @param r [in] recognition process instance
 * @param param [in] input parameter
 * </EN>
 */
static void
result_reorder_and_output(NODE **r_start, NODE **r_bottom, int *r_stacknum, int ncan, RecogProcess *r, HTK_Param *param)
{
  NODE *now;
  int num;

#ifdef CM_NBEST 
  /* compute CM from the N-best sentence candidates */
  cm_compute_from_nbest(&(r->pass2), *r_start, *r_stacknum, r->config);
#endif
  num = 0;
  while ((now = get_best_from_stack(r_start,r_stacknum)) != NULL && num < ncan) {
    num++;
    /* output result */
    store_result_pass2(now, r);

    /* if in sp segmentation mode, */
    /* set the last context-aware word for next recognition */
    if (r->lmtype == LM_PROB && r->config->successive.enabled && num == 1) segment_set_last_nword(now, r);

    free_node(now);
  }
  /* free the rest */
  if (now != NULL) free_node(now);
  free_all_nodes(*r_start);
}  

/** 
 * <EN>
 * @brief  Post-process of 2nd pass when no result is obtained.
 *
 * This is a post-process for the 2nd pass which should be called when
 * the 2nd pass has no result.  This will occur when the 2nd pass was
 * executed but failed with no sentence candidate, or skipped by
 * an option.
 *
 * When the 2nd argument is set to TRUE, the result of the 1st pass
 * will be copied as final result of 2nd pass and the recognition
 * status flag is set to SUCCESS.  If FALSE, recognition status will
 * be set to FAILED.  On sp-segment decoding, the initial hypothesis
 * marker for the next input segment will be set up from the 1st pass
 * result also.
 * 
 * @param r [in] recognition process instance
 * @param use_1pass_as_final [in] when TRUE the 1st pass result will be used as final recognition result of 2nd pass.
 * 
 * </EN>
 * <JA>
 * @brief  ��2�ѥ��β������ʤ����ν�λ����
 *
 * ��2�ѥ������Ԥ���������2�ѥ����¹Ԥ���ʤ�����ξ���
 * ǧ����λ������Ԥ���use_1pass_as_final �� TRUE �ΤȤ���
 * ��1�ѥ��η�̤���2�ѥ��η�̤Ȥ��ƥ��ԡ����Ƴ�Ǽ����ǧ�������Ȥ��롥
 * FALSE����ǧ�����ԤȤ��롥
 * �ޤ���sp-segment ���ϡ�����ǧ������Ѥν�������������1�ѥ���
 * ��̤���Ԥ���
 * 
 * @param r [in] ǧ���������󥹥���
 * @param use_1pass_as_final [in] TRUE ����1�ѥ��η�̤���2�ѥ���̤˳�Ǽ����
 * 
 * </JA>
 */
void
pass2_finalize_on_no_result(RecogProcess *r, boolean use_1pass_as_final)
{
  NODE *now;
  int i, j;

  /* õ������ */
  /* search failed */

  /* make temporal hypothesis data from the result of previous 1st pass */
  now = newnode(r);
  for (i=0;i<r->pass1_wnum;i++) {
    now->seq[i] = r->pass1_wseq[r->pass1_wnum-1-i];
  }
  now->seqnum = r->pass1_wnum;
  now->score = r->pass1_score;
#ifdef CONFIDENCE_MEASURE
  /* fill in null values */
#ifdef CM_MULTIPLE_ALPHA
  for(j=0;j<jconf->annotate.cm_alpha_num;j++) {
    for(i=0;i<now->seqnum;i++) now->cmscore[i][j] = 0.0;
  }
#else
  for(i=0;i<now->seqnum;i++) now->cmscore[i] = 0.0;
#endif
#endif /* CONFIDENCE_MEASURE */
  
  if (r->lmtype == LM_PROB && r->config->successive.enabled) {
    /* if in sp segment mode, */
    /* find segment restart words from 1st pass result */
    segment_set_last_nword(now, r);
  }
    
  if (use_1pass_as_final) {
    /* ��1�ѥ��η�̤򤽤Τޤ޽��Ϥ��� */
    /* output the result of the previous 1st pass as a final result. */
    store_result_pass2(now, r);
    r->result.status = J_RESULT_STATUS_SUCCESS;
  } else {
    /* store output as failure */
    r->result.status = J_RESULT_STATUS_FAIL;
    //callback_exec(CALLBACK_RESULT, r);
  }
  
  free_node(now);
}


/**********************************************************************/
/********* Main stack decoding function *******************************/
/**********************************************************************/

/** 
 * <JA>
 * ��2õ���ѥ��Ǥ��륹���å��ǥ����ǥ��󥰤�Ԥ��ᥤ��ؿ�
 *
 * �����Τ��� cate_bgn, cate_num ��ñ��N-gram�Ǥ�̵�뤵���. 
 * 
 * @param param [in] ���ϥѥ�᡼���٥��ȥ���
 * @param r [i/o] ǧ���������󥹥���
 * @param cate_bgn [in] Ÿ���оݤȤ��٤����ƥ���γ����ֹ�
 * @param cate_num [in] Ÿ���оݤȤ��٤����ƥ���ο�
 * </JA>
 * <EN>
 * Main function to perform stack decoding of the 2nd search pass.
 *
 * The cate_bgn and cate_num (third and fourth argument) will have
 * no effect when N-gram is used.
 * 
 * @param param [in] input parameter vector
 * @param r [i/o] recognition process instance
 * @param cate_bgn [in] category id to allow word expansion from (ignored in Julius)
 * @param cate_num [in] num of category to allow word expansion from (ignored in Julius)
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
void
wchmm_fbs(HTK_Param *param, RecogProcess *r, int cate_bgn, int cate_num)
{
  /* ʸ���⥹���å� */
  /* hypothesis stack (double-linked list) */
  int stacknum;			/* current stack size */
  NODE *start = NULL;		/* top node */
  NODE *bottom = NULL;		/* bottom node */

  /* ǧ����̳�Ǽ�����å�(��̤Ϥ����ؤ��ä��󽸤����) */
  /* result sentence stack (found results will be stored here and then re-ordered) */
  int r_stacksize;
  int r_stacknum;
  NODE *r_start = NULL;
  NODE *r_bottom = NULL;

  /* ������ꥢ */
  /* work area */
  NEXTWORD fornoise = {0}; /* Dummy NEXTWORD data for short-pause insertion handling */

  NEXTWORD **nextword, *nwroot;	/* buffer to store predicted words */
  int maxnwnum;			/* current allocated number of words in nextword */
  int nwnum;			/* current number of words in nextword */
  NODE *now, *new;		/* popped current hypo., expanded new hypo. */
  NODE *now_noise;	       /* for inserting/deleting noise word */
  boolean now_noise_calced;
  boolean acc;
  int t;
  int w,i,j;
  LOGPROB last_score = LOG_ZERO;
  /* for graph generation */
  LOGPROB prev_score;
  WordGraph *wordgraph_root = NULL;
  boolean merged_p;
#ifdef GRAPHOUT_DYNAMIC
  int dynamic_merged_num = 0;
  WordGraph *wtmp;
  LOGPROB lscore_prev;
#endif
#ifdef GRAPHOUT_SEARCH
  int terminate_search_num = 0;
#endif

  /* local temporal parameter */
  int stacksize, ncan, maxhypo, peseqlen;
  JCONF_SEARCH *jconf;
  WORD_INFO *winfo;
  NGRAM_INFO *ngram;
  DFA_INFO *gdfa;
  BACKTRELLIS *backtrellis;
  StackDecode *dwrk;

  if (r->lmtype == LM_DFA) {
    if (debug2_flag) jlog("DEBUG: only words in these categories will be expanded: %d-%d\n", cate_bgn, cate_bgn + cate_num-1);
  }

  /*
   * �����
   * Initialize
   */

  /* just for quick access */
  jconf = r->config;
  winfo = r->lm->winfo;
  if (r->lmtype == LM_PROB) {
    ngram = r->lm->ngram;
  } else if (r->lmtype == LM_DFA) {
    gdfa = r->lm->dfa;
  }
  backtrellis = r->backtrellis;
  dwrk = &(r->pass2);

  stacksize = jconf->pass2.stack_size;
  ncan = jconf->pass2.nbest;
  maxhypo = jconf->pass2.hypo_overflow;
  peseqlen = backtrellis->framelen;

  /* store data for sub routines */
  r->peseqlen = backtrellis->framelen;
  //recog->ccd_flag = recog->jconf->am.ccd_flag;
  /* ͽ¬ñ���Ǽ�ΰ����� */
  /* malloc area for word prediction */
  /* the initial maximum number of nextwords is the size of vocabulary */
  nextword = nw_malloc(&maxnwnum, &nwroot, winfo->num);
  /* �������������׻��Ѥ��ΰ����� */
  /* malloc are for forward viterbi (scan_word()) */
  malloc_wordtrellis(r);		/* scan_word���ΰ� */
  /* ���⥹���å������ */
  /* initialize hypothesis stack */
  start = bottom = NULL;
  stacknum = 0;
  /* ��̳�Ǽ�����å������ */
  /* initialize result stack */
  r_stacksize = ncan;
  r_start = r_bottom = NULL;
  r_stacknum = 0;

  /* �����󥿽���� */
  /* initialize counter */
  dwrk->popctr = 0;
  dwrk->genectr = 0;
  dwrk->pushctr = 0;
  dwrk->finishnum = 0;
  
#ifdef CM_SEARCH
  /* initialize local stack */
  cm_init(dwrk, winfo->num, jconf->annotate.cm_alpha
#ifdef CM_MULTIPLE_ALPHA
	  , jconf->annotate.cm_alpha_num
#endif
	  );
#endif
#ifdef SCAN_BEAM
  /* prepare and initialize score envelope */
  dwrk->framemaxscore = (LOGPROB *)mymalloc(sizeof(LOGPROB)*peseqlen);
  envl_init(dwrk, peseqlen);
#endif /* SCAN_BEAM */

  /* ����٥���õ���Ѥ�ñ��Ĺ��Ÿ���������󥿤����� */
  /* initialize counters for envelope search */
  if (jconf->pass2.enveloped_bestfirst_width >= 0) wb_init(dwrk);

  if (jconf->graph.enabled) {
    wordgraph_init(r->wchmm);
  }

  /* 
   * �������(1ñ�줫��ʤ�)����, ʸ���⥹���å��ˤ����
   * get a set of initial words from LM function and push them as initial
   * hypotheses
   */
  /* the first words will be stored in nextword[] */
  if (r->lmtype == LM_PROB) {
    nwnum = ngram_firstwords(nextword, peseqlen, maxnwnum, r);
  } else if (r->lmtype == LM_DFA) {
    nwnum = dfa_firstwords(nextword, peseqlen, maxnwnum, r);
    /* ��줿�顢�Хåե������䤷�ƺƥ����� */
    /* If the number of nextwords can exceed the buffer size, expand the
       nextword data area */
    while (nwnum < 0) {
      nextword = nw_expand(nextword, &maxnwnum, &nwroot, winfo->num);
      nwnum = dfa_firstwords(nextword, peseqlen, maxnwnum, r);
    }
  }

  if (debug2_flag) {
    jlog("DEBUG: %d words in wordtrellis as first hypothesis\n", nwnum);
  }
  
  /* store them to stack */
  for (w = 0; w < nwnum; w++) {
    if (r->lmtype == LM_DFA) {
      /* limit word hypothesis */
      if (! (winfo->wton[nextword[w]->id] >= cate_bgn && winfo->wton[nextword[w]->id] < cate_bgn + cate_num)) {
	continue;
      }
    }
    /* generate new hypothesis */
    new = newnode(r);
    start_word(new, nextword[w], param, r);
    if (r->lmtype == LM_DFA) {
      if (new->score <= LOG_ZERO) { /* not on trellis */
	free_node(new);
	continue;
      }
    }
    dwrk->genectr++;
#ifdef CM_SEARCH
    /* store the local hypothesis to temporal stack */
    cm_store(dwrk, new);
#else 
    /* put to stack */
    if (put_to_stack(new, &start, &bottom, &stacknum, stacksize) != -1) {
      dwrk->current = new;
      //callback_exec(CALLBACK_DEBUG_PASS2_PUSH, r);
      if (jconf->graph.enabled) {
	new->prevgraph = NULL;
	new->lastcontext = NULL;
      }
      dwrk->pushctr++;
    }
#endif
  }
#ifdef CM_SEARCH
  /* compute score sum */
  cm_sum_score(dwrk
#ifdef CM_MULTIPLE_ALPHA
	       , jconf->annotate.cm_alpha_bgn
	       , jconf->annotate.cm_alpha_end
	       , jconf->annotate.cm_alpha_step
#endif
	       );

  /* compute CM and put the generated hypotheses to global stack */
  while ((new = cm_get_node(dwrk)) != NULL) {
    cm_set_score(dwrk, new
#ifdef CM_MULTIPLE_ALPHA
		 , jconf->annotate.cm_alpha_bgn
		 , jconf->annotate.cm_alpha_end
		 , jconf->annotate.cm_alpha_step
#endif
		 );
#ifdef CM_SEARCH_LIMIT
    if (new->cmscore[new->seqnum-1] < jconf->annotate.cm_cut_thres
#ifdef CM_SEARCH_LIMIT_AFTER
	&& dwrk->finishnum > 0
#endif
	) {
      free_node(new);
      continue;
    }
#endif /* CM_SEARCH_LIMIT */
    
    if (put_to_stack(new, &start, &bottom, &stacknum, stacksize) != -1) {
      dwrk->current = new;
      //callback_exec(CALLBACK_DEBUG_PASS2_PUSH, r);
      if (r->graphout) {
	new->prevgraph = NULL;
	new->lastcontext = NULL;
      }
      dwrk->pushctr++;
    }
  }
#endif

  if (debug2_flag) {
    jlog("DEBUG: %d pushed\n", dwrk->pushctr);
  }
  
  /********************/
  /* main search loop */
  /********************/

  for (;;) {

    /* if terminate signal has been received, cancel this input */
    /* 
     * if (recog->process_want_terminate) {
     *	 jlog("DEBUG: process terminated by request\n");
     *	 break;
     * }
     */
    
    /* 
     * ���⥹���å�����Ǥ⥹�����ι⤤�������Ф�
     * pop the top hypothesis from stack
     */
#ifdef DEBUG
    jlog("DEBUG: get one hypothesis\n");
#endif
    now = get_best_from_stack(&start,&stacknum);
    if (now == NULL) {  /* stack empty ---> õ����λ*/
      jlog("WARNING: %02d %s: hypothesis stack exhausted, terminate search now\n", r->config->id, r->config->name);
      if (verbose_flag) {
	jlog("STAT: %02d %s: %d sentences have been found\n", r->config->id, r->config->name, dwrk->finishnum);
      }
      break;
    }
    /* (bogus score check) */
    if (now->score <= LOG_ZERO) {
      free_node(now);
      continue;
    }


    /* ñ�쥰����Ѥ� pop ����� f ������������¸ */
    if (r->graphout) {
      prev_score = now->score;
    }

    /* word envelope �����å� */
    /* consult word envelope */
    if (jconf->pass2.enveloped_bestfirst_width >= 0) {
      if (!wb_ok(dwrk, now, jconf->pass2.enveloped_bestfirst_width)) {
	/* ���β���Ĺ�ˤ�����Ÿ������������߷׿��ϴ������ͤ�ۤ��Ƥ���. 
	   ���Τ��ᡤ���β���ϼΤƤ�. */
	/* the number of popped hypotheses at the length already
	   reaches its limit, so the current popped hypothesis should
	   be discarded here with no expansion */
	if (debug2_flag) {
	  jlog("DEBUG: popped but pruned by word envelope:");
	  put_hypo_woutput(now, r->lm->winfo);
	}
	free_node(now);
	continue;
      }
    }
    
#ifdef CM_SEARCH_LIMIT_POP
    if (now->cmscore[now->seqnum-1] < jconf->annotate.cm_cut_thres_pop) {
      free_node(now);
      continue;
    }
#endif /* CM_SEARCH_LIMIT_POP */

    dwrk->popctr++;

    /* (for debug) ���Ф�������Ȥ��Υ���������� */
    /*             output information of the popped hypothesis to stdout */
    if (debug2_flag) {
      jlog("DEBUG: --- pop %d:\n", dwrk->popctr);
      jlog("DEBUG:  "); put_hypo_woutput(now, r->lm->winfo);
      jlog("DEBUG:  "); put_hypo_wname(now, r->lm->winfo);
      jlog("DEBUG:  %d words, f=%f, g=%f\n", now->seqnum, now->score, now->g[now->bestt]);
      jlog("DEBUG:  last word on trellis: [%d-%d]\n", now->estimated_next_t + 1, now->bestt);
    }

    dwrk->current = now;
    //callback_exec(CALLBACK_DEBUG_PASS2_POP, r);

    if (r->graphout) {

#ifdef GRAPHOUT_DYNAMIC
      /* merge last word in popped hypo if possible */
      wtmp = wordgraph_check_merge(now->prevgraph, &wordgraph_root, now->seq[now->seqnum-1], &merged_p, jconf);
      if (wtmp != NULL) {		/* wtmp holds merged word */
	dynamic_merged_num++;

	lscore_prev = (now->prevgraph) ? now->prevgraph->lscore_tmp : 0.0;

	if (now->prevgraph != NULL) {
	  if (now->prevgraph->saved) {
	    j_internal_error("wchmm_fbs: already saved??\n");
	  }
	  wordgraph_free(now->prevgraph);
	}

	if (now->lastcontext != NULL
	    && now->lastcontext != wtmp	/* avoid self loop */
	    ) {
	  wordgraph_check_and_add_leftword(now->lastcontext, wtmp, lscore_prev);
#ifdef GRAPHOUT_SEARCH_CONSIDER_RIGHT
	  if (merged_p) {
	    if (wordgraph_check_and_add_rightword(wtmp, now->lastcontext, lscore_prev) == FALSE) {
	      merged_p = TRUE;
	    } else {
	      merged_p = FALSE;
	    }
	  } else {
	    wordgraph_check_and_add_rightword(wtmp, now->lastcontext, lscore_prev);
	  }
#else
	  wordgraph_check_and_add_rightword(wtmp, now->lastcontext, lscore_prev);
#endif	  
	}
	
	now->prevgraph = wtmp;	/* change word to the merged one */

	/*printf("last word merged\n");*/
	/* previous still remains at memory here... (will be purged later) */
      } else {
	wordgraph_save(now->prevgraph, now->lastcontext, &wordgraph_root);
      }
#ifdef GRAPHOUT_SEARCH
      /* if recent hypotheses are included in the existing graph, terminate */
      if (merged_p && now->endflag == FALSE
#ifdef GRAPHOUT_SEARCH_DELAY_TERMINATION
	  /* Do not apply search termination by graph merging
	     until the first sentence candidate is found. */
	  && (jconf->graph.graphout_search_delay == FALSE || dwrk->finishnum > 0)
#endif
	  ) {
	terminate_search_num++;
	free_node(now);
	continue;
      }
#endif
#else  /* ~GRAPHOUT_DYNAMIC */
      /* always save */
      wordgraph_save(now->prevgraph, now->lastcontext, &wordgraph_root);
#endif /* ~GRAPHOUT_DYNAMIC */
    }

    /* ���Ф�������Υ������򸵤� score envelope �򹹿� */
    /* update score envelope using the popped hypothesis */
    envl_update(dwrk, now, peseqlen);

    /* 
     * ���Ф�������μ����ե饰������Ω�äƤ���С�
     * ���β����õ����λ�Ȥߤʤ�����̤Ȥ��ƽ��Ϥ��Ƽ��Υ롼�פ�. 
     *
     * If the popped hypothesis already reached to the end, 
     * we can treat it as a recognition result.
     */
#ifdef DEBUG
    VERMES("endflag check\n");
#endif
    
    if (now->endflag) {
      if (debug2_flag) {
	jlog("DEBUG:  This is a full sentence candidate\n");
      }
      /* quick, dirty hack */
      if (now->score == last_score) {
	free_node(now);
	continue;
      } else {
	last_score = now->score;
      }
      
      dwrk->finishnum++;
      if (debug2_flag) {
	jlog("DEBUG:  %d-th sentence found\n", dwrk->finishnum);
      }

	/* ������β��⤬����줿���ȥ������ǥ����Ȥ��뤿�ᡤ
	   ���Ū���̤Υ����å��˳�Ǽ���Ƥ��� */
	/* store the result to result stack
	   after search is finished, they will be re-ordered and output */
	put_to_stack(now, &r_start, &r_bottom, &r_stacknum, r_stacksize);
	/* �������ʸ���⤬����줿�ʤ�õ����λ���� */
	/* finish search if specified number of results are found */
	if (dwrk->finishnum >= ncan) {
	  break;
	} else {
	  continue;
	}
      
    } /* end of now->endflag */

    
    /* 
     * õ�����Ԥ򸡽Ф���. 
     * ������� maxhypo �ʾ�Ÿ�����줿��, �⤦����ʾ��õ�����ʤ�
     *
     * detecting search failure:
     * if the number of expanded hypotheses reaches maxhypo, giveup further search
     */
#ifdef DEBUG
    jlog("DEBUG: loop end check\n");
#endif
    if (dwrk->popctr >= maxhypo) {
      jlog("WARNING: %02d %s: num of popped hypotheses reached the limit (%d)\n", r->config->id, r->config->name, maxhypo);
      /* (for debug) õ�����Ի��ˡ������å��˻Ĥä�������Ǥ��Ф� */
      /* (for debug) output all hypothesis remaining in the stack */
      if (debug2_flag) put_all_in_stack(&start, &stacknum, r->lm->winfo);
      free_node(now);
      break;			/* end of search */
    }
    /* ����Ĺ�������ͤ�ۤ����Ȥ������β�����˴����� */
    /* check hypothesis word length overflow */
    if (now->seqnum >= MAXSEQNUM) {
      jlog("ERROR: sentence length exceeded system limit ( > %d)\n", MAXSEQNUM);
      free_node(now);
      continue;
    }

#ifndef GRAPHOUT_PRECISE_BOUNDARY
    if (r->graphout) {
      /* if monophone (= no backscan), the tail g score should be kept here */
      /* else, updated tail g score will be computed in scan_word()  */
      if(!jconf->am.ccd_flag) {
	now->tail_g_score = now->g[now->bestt];
      }
    }
#endif

    /*
     * �������������򹹿����롧 �Ǹ��ñ�����ʬ����������������׻�����. 
     * update forward score: compute forward trellis for the last word
     */
#ifdef DEBUG
    jlog("DEBUG: scan_word\n");
#endif
    scan_word(now, param, r);
    if (now->score < LOG_ZERO) { /* another end-of-search detecter */
      if (verbose_flag) {
	jlog("WARNING: too low score, ignore: score=%f",now->score);
      }
      put_hypo_woutput(now, r->lm->winfo);
      free_node(now);
      continue;
    }

    /* 
     * ���Ф������⤬ʸ�Ȥ��Ƽ�����ǽ�Ǥ���С�
     * �����ե饰��Ω�ƤƤ򥹥��å��ˤ���ľ���Ƥ���. 
     * (���˼��Ф��줿���Ȥʤ�)
     *
     * if the current popped hypothesis is acceptable, set endflag
     * and return it to stack: it will become the recognition result
     * when popped again.
     */
#ifdef DEBUG
    jlog("DEBUG: accept check\n");
#endif

    if (r->lmtype == LM_PROB) {
      acc = ngram_acceptable(now, r);
    } else if (r->lmtype == LM_DFA) {
      acc = dfa_acceptable(now, r);
    }
    if (acc && now->estimated_next_t <= 5) {
      new = newnode(r);
      /* new �� now ����Ȥ򥳥ԡ����ơ��ǽ�Ū�ʥ�������׻� */
      /* copy content of 'now' to 'new', and compute the final score */
      last_next_word(now, new, param, r);
      if (debug2_flag) {
	jlog("DEBUG:  This is acceptable as a sentence candidate\n");
      }
      /* g[] �����ϻ�ü��ã���Ƥ��ʤ���д��� */
      /* reject this sentence candidate if g[] does not reach the end */
      if (new->score <= LOG_ZERO) {
	if (debug2_flag) {
	  jlog("DEBUG:  But invalid because Viterbi pass does not reach the 0th frame\n");
	}
	free_node(new);
	free_node(now);
	continue;
      }
      /* �����ե饰��Ω�Ƥ�����ľ�� */
      /* set endflag and push again  */
      if (debug2_flag) {
	jlog("DEBUG  This hypo itself was pushed with final score=%f\n", new->score);
      }
      new->endflag = TRUE;
      if (put_to_stack(new, &start, &bottom, &stacknum, stacksize) != -1) {
	if (r->graphout) {
	  if (new->score > LOG_ZERO) {
	    new->lastcontext = now->prevgraph;
	    new->prevgraph = wordgraph_assign(new->seq[new->seqnum-1],
					      WORD_INVALID,
					      (new->seqnum >= 2) ? new->seq[new->seqnum-2] : WORD_INVALID,
					      0,
#ifdef GRAPHOUT_PRECISE_BOUNDARY
					      /* wordend are shifted to the last */
#ifdef PASS2_STRICT_IWCD
					      new->wordend_frame[0],
#else
					      now->wordend_frame[0],
#endif
#else
					      now->bestt,
#endif
					      new->score,
					      prev_score,
					      now->g[0],
#ifdef GRAPHOUT_PRECISE_BOUNDARY
#ifdef PASS2_STRICT_IWCD
					      new->wordend_gscore[0],
#else
					      now->wordend_gscore[0],
#endif
#else
					      now->tail_g_score,
#endif
					      now->lscore,
#ifdef CM_SEARCH
					      new->cmscore[new->seqnum-1],
#else
					      LOG_ZERO,
#endif
					      r
					      );
	  } else {
	    new->lastcontext = now->lastcontext;
	    new->prevgraph = now->prevgraph;
	  }
	} /* put_to_stack() != -1 */
      }	/* recog->graphout */
      /* ���β���Ϥ����ǽ���餺��, �������餵���ñ��Ÿ������ */
      /* continue with the 'now' hypothesis, not terminate here */
    }
    
    /*
     * ���β��⤫�顤��ñ�콸�����ꤹ��. 
     * ��ñ�콸���, ���β���ο����ü�ե졼����դ�¸�ߤ���
     * �裱�ѥ��Υȥ�ꥹñ�콸��. 
     *
     * N-gram�ξ��ϳ�ñ��� n-gram ��³��Ψ���ޤޤ��. 
     * DFA �ξ���, ������Ǥ���� DFA �����³��ǽ�ʤ�ΤΤߤ��֤äƤ���
     */
    /*
     * Determine next word set that can connect to this hypothesis.
     * They come from the trellis word that has been survived at near the
     * beginning of the last word.
     *
     * In N-gram mode, they also contain N-gram probabilities toward the
     * source hypothesis.  In DFA mode, the word set is further reduced
     * by the grammatical constraint
     */
#ifdef DEBUG
    jlog("DEBUG: get next words\n");
#endif
    if (r->lmtype == LM_PROB) {
      nwnum = ngram_nextwords(now, nextword, maxnwnum, r);
    } else if (r->lmtype == LM_DFA) {
      nwnum = dfa_nextwords(now, nextword, maxnwnum, r);
      /* nextword ����줿�顢�Хåե������䤷�ƺƥ����� */
      /* If the number of nextwords can exceed the buffer size, expand the
	 nextword data area */
      while (nwnum < 0) {
	nextword = nw_expand(nextword, &maxnwnum, &nwroot, winfo->num);
	nwnum = dfa_nextwords(now, nextword, maxnwnum, r);
      }
    }
    if (debug2_flag) {
      jlog("DEBUG:  %d words extracted from wordtrellis\n", nwnum);
    }

    /* 
     * ����ȼ�ñ�콸�礫�鿷����ʸ������������������å��ˤ����. 
     */
    /*
     * generate new hypotheses from 'now' and 'nextword', 
     * and push them to stack
     */
#ifdef DEBUG
    jlog("DEBUG: generate hypo\n");
#endif

    if (r->lmtype == LM_DFA) {
      now_noise_calced = FALSE;	/* TRUE is noise-inserted score has been calculated */
    }
    i = dwrk->pushctr;		/* store old value */

#ifdef CM_SEARCH
    /* initialize local stack */
    cm_init(dwrk, winfo->num, jconf->annotate.cm_alpha
#ifdef CM_MULTIPLE_ALPHA
	    , jconf->annotate.cm_alpha_num
#endif
	    );
#endif

    /* for each nextword, generate a new hypothesis */
    for (w = 0; w < nwnum; w++) {
      if (r->lmtype == LM_DFA) {
	/* limit word hypothesis */
	if (! (winfo->wton[nextword[w]->id] >= cate_bgn && winfo->wton[nextword[w]->id] < cate_bgn + cate_num)) {
	  continue;
	}
      }
      new = newnode(r);

      if (r->lmtype == LM_DFA) {

	if (nextword[w]->can_insert_sp == TRUE) {
	  /* �Υ����򶴤���ȥ�ꥹ��������׻��������ޤʤ����Ȥκ����ͤ��� */
	  /* compute hypothesis score with noise inserted */
	  
	  if (now_noise_calced == FALSE) {
	    /* now �� sp ��Ĥ������� now_noise ����,���Υ�������׻� */
	    /* generate temporal hypothesis 'now_noise' which has short-pause
	       word after the original 'now' */
	    fornoise.id = gdfa->sp_id;
	    now_noise = newnode(r);
	    cpy_node(now_noise, now);
#if 0
	    now_noise_tmp = newnode(r);
	    next_word(now, now_noise_tmp, &fornoise, param, r);
	    scan_word(now_noise_tmp, param, r);
	    for(t=0;t<peseqlen;t++) {
	      now_noise->g[t] = max(now_noise_tmp->g[t], now->g[t]);
	    }
	    free_node(now_noise_tmp);
#else
	    /* expand NOISE only if it exists in backward trellis */
	    /* begin patch by kashima */
	    if (jconf->pass2.looktrellis_flag) {
	      if(!dfa_look_around(&fornoise, now, r)){
		free_node(now_noise);
		free_node(new);
		continue;
	      }
	    }
	    /* end patch by kashima */
	    
	    /* now_nosie �� ������ g[] ��׻��������� now �� g[] ����Ӥ���
	       �⤤������� */
	    /* compute trellis score g[], and adopt the maximum score
	       for each frame compared with now->g[] */
	    next_word(now, now_noise, &fornoise, param, r);
	    scan_word(now_noise, param, r);
	    for(t=0;t<peseqlen;t++) {
	      now_noise->g[t] = max(now_noise->g[t], now->g[t]);
	    }
	    /* �Υ����򶴤���ݤ��θ������������׻������Τǡ�
	       �����ǺǸ�ΥΥ���ñ��� now_noise ����ä� */
	    /* now that score has been computed considering pause insertion,
	       we can delete the last noise word from now_noise here */
	    now_noise->seqnum--;
#endif
	    now_noise_calced = TRUE;
	  }
	  
	  /* expand word only if it exists in backward trellis */
	  /* begin patch by kashima */
	  if (jconf->pass2.looktrellis_flag) {
	    if(!dfa_look_around(nextword[w], now_noise, r)){
	      free_node(new);
	      continue;
	    }
	  }
	  /* end patch by kashima */
	  
	  /* ����������' new' �� 'now_noise' �������� */
	  /* generate a new hypothesis 'new' from 'now_noise' */
	  next_word(now_noise, new, nextword[w], param, r);
	  
	} else {
	  
	  /* expand word only if it exists in backward trellis */
	  /* begin patch by kashima */
	  if (jconf->pass2.looktrellis_flag) {
	    if(!dfa_look_around(nextword[w], now, r)){
	      free_node(new);
	      continue;
	    }
	  }
	  /* end patch by kashima */
	  
	  /* ����������' new' �� 'now_noise' �������� */
	  /* generate a new hypothesis 'new' from 'now_noise' */
	  next_word(now, new, nextword[w], param, r);
	  
	}
      }

      if (r->lmtype == LM_PROB) {

	/* ����������' new' �� 'now_noise' ��������
	   N-gram �ξ��ϥΥ��������̰������ʤ� */
	/* generate a new hypothesis 'new' from 'now'.
	   pause insertion is treated as same as normal words in N-gram mode. */
	next_word(now, new, nextword[w], param, r);

      }

      if (new->score <= LOG_ZERO) { /* not on trellis */
	free_node(new);
	continue;
      }
	
      dwrk->genectr++;

#ifdef CM_SEARCH
      /* store the local hypothesis to temporal stack */
      cm_store(dwrk, new);
#else 
      /* ������������ 'new' �򥹥��å�������� */
      /* push the generated hypothesis 'new' to stack */

      /* stack overflow */
      if (can_put_to_stack(new, &bottom, &stacknum, stacksize) == -1) {
	free_node(new);
	continue;
      }
      
      if (r->graphout) {
	/* assign a word arc to the last fixed word */
	new->lastcontext = now->prevgraph;
	new->prevgraph = wordgraph_assign(new->seq[new->seqnum-2],
					  new->seq[new->seqnum-1],
					  (new->seqnum >= 3) ? new->seq[new->seqnum-3] : WORD_INVALID,
					  new->bestt + 1,
#ifdef GRAPHOUT_PRECISE_BOUNDARY
#ifdef PASS2_STRICT_IWCD
					  /* most up-to-date wordend_gscore is on new, because the last phone of 'now' will be computed at next_word() */
					  new->wordend_frame[new->bestt],
#else
					  now->wordend_frame[new->bestt],
#endif
#else
					  now->bestt,
#endif
					  new->score, prev_score,
#ifdef PASS2_STRICT_IWCD
					  new->g[new->bestt] - new->lscore,
#else
					  now->g[new->bestt+1],
#endif
#ifdef GRAPHOUT_PRECISE_BOUNDARY
#ifdef PASS2_STRICT_IWCD
					  /* most up-to-date wordend_gscore is on new, because the last phone of 'now' will be computed at next_word() */
					  new->wordend_gscore[new->bestt],
#else
					  now->wordend_gscore[new->bestt],
#endif
#else
					  now->tail_g_score,
#endif
					  now->lscore,
#ifdef CM_SEARCH
					  new->cmscore[new->seqnum-2],
#else
					  LOG_ZERO,
#endif
					  r
					  );
      }	/* recog->graphout */
      put_to_stack(new, &start, &bottom, &stacknum, stacksize);
      if (debug2_flag) {
	j = new->seq[new->seqnum-1];
	jlog("DEBUG:  %15s [%15s](id=%5d)(%f) [%d-%d] pushed\n",winfo->wname[j], winfo->woutput[j], j, new->score, new->estimated_next_t + 1, new->bestt);
      }
      dwrk->current = new;
      //callback_exec(CALLBACK_DEBUG_PASS2_PUSH, r);
      dwrk->pushctr++;
#endif
    } /* end of nextword loop */

#ifdef CM_SEARCH
    /* compute score sum */
    cm_sum_score(dwrk
#ifdef CM_MULTIPLE_ALPHA
		 , jconf->annotate.cm_alpha_bgn 
		 , jconf->annotate.cm_alpha_end
		 , jconf->annotate.cm_alpha_step
#endif
		 );
    /* compute CM and put the generated hypotheses to global stack */
    while ((new = cm_get_node(dwrk)) != NULL) {
      cm_set_score(dwrk, new
#ifdef CM_MULTIPLE_ALPHA
		   , jconf->annotate.cm_alpha_bgn
		   , jconf->annotate.cm_alpha_end
		   , jconf->annotate.cm_alpha_step
#endif
		   );
#ifdef CM_SEARCH_LIMIT
      if (new->cmscore[new->seqnum-1] < jconf->annotate.cm_cut_thres
#ifdef CM_SEARCH_LIMIT_AFTER
	  && dwrk->finishnum > 0
#endif
	  ) {
	free_node(new);
	continue;
      }
#endif /* CM_SEARCH_LIMIT */
      /*      j = new->seq[new->seqnum-1];
	      printf("  %15s [%15s](id=%5d)(%f) [%d-%d] cm=%f\n",winfo->wname[j], winfo->woutput[j], j, new->score, new->estimated_next_t + 1, new->bestt, new->cmscore[new->seqnum-1]);*/

      /* stack overflow */
      if (can_put_to_stack(new, &bottom, &stacknum, stacksize) == -1) {
	free_node(new);
	continue;
      }

      if (r->graphout) {

	/* assign a word arc to the last fixed word */
	new->lastcontext = now->prevgraph;
	new->prevgraph = wordgraph_assign(new->seq[new->seqnum-2],
					  new->seq[new->seqnum-1],
					  (new->seqnum >= 3) ? new->seq[new->seqnum-3] : WORD_INVALID,
					  new->bestt + 1,
#ifdef GRAPHOUT_PRECISE_BOUNDARY
#ifdef PASS2_STRICT_IWCD
					  new->wordend_frame[new->bestt],
#else
					  now->wordend_frame[new->bestt],
#endif
#else
					  now->bestt,
#endif
					  new->score, prev_score,
#ifdef PASS2_STRICT_IWCD
					  new->g[new->bestt] - new->lscore,
#else
					  now->g[new->bestt+1],
#endif
#ifdef GRAPHOUT_PRECISE_BOUNDARY
#ifdef PASS2_STRICT_IWCD
					  new->wordend_gscore[new->bestt],
#else
					  now->wordend_gscore[new->bestt],
#endif
#else
					  now->tail_g_score,
#endif
					  now->lscore,
#ifdef CM_SEARCH
					  new->cmscore[new->seqnum-2],
#else
					  LOG_ZERO,
#endif
					  r
					  );
      }	/* recog->graphout */
      
      put_to_stack(new, &start, &bottom, &stacknum, stacksize);
      if (debug2_flag) {
	j = new->seq[new->seqnum-1];
	jlog("DEBUG:  %15s [%15s](id=%5d)(%f) [%d-%d] pushed\n",winfo->wname[j], winfo->woutput[j], j, new->score, new->estimated_next_t + 1, new->bestt);
      }
      dwrk->current = new;
      //callback_exec(CALLBACK_DEBUG_PASS2_PUSH, r);
      dwrk->pushctr++;
    }
#endif

    if (debug2_flag) {
      jlog("DEBUG: %d pushed\n",dwrk->pushctr-i);
    }
    if (r->lmtype == LM_DFA) {
      if (now_noise_calced == TRUE) free_node(now_noise);
    }
    
    /* 
     * ���Ф��������ΤƤ�
     * free the source hypothesis
     */
    free_node(now);

  }
  /***************/
  /* End of Loop */
  /***************/

  /* output */
  if (dwrk->finishnum == 0) {		/* if search failed */

    /* finalize result when no hypothesis was obtained */
    if (verbose_flag) {
      if (r->config->sw.fallback_pass1_flag) {
	jlog("%02d %s: got no candidates, output 1st pass result as a final result\n", r->config->id, r->config->name);
      } else {
	jlog("WARNING: %02d %s: got no candidates, search failed\n", r->config->id, r->config->name);
      }
    }
    pass2_finalize_on_no_result(r, r->config->sw.fallback_pass1_flag);

  } else {			/* if at least 1 candidate found */

    if (debug2_flag) {
      jlog("STAT: %02d %s: got %d candidates\n", r->config->id, r->config->name, dwrk->finishnum);
    }

#ifdef USE_MBR
      if(r->config->mbr.use_mbr){
       candidate_mbr(&r_start, &r_bottom, r_stacknum, r);
      }
#endif

      /* ��̤Ϥޤ����Ϥ���Ƥ��ʤ��Τǡ�ʸ�����ѥ����å���򥽡��Ȥ���
	 �����ǽ��Ϥ��� */
      /* As all of the found candidate are in result stack, we sort them
	 and output them here  */
      if (debug2_flag) jlog("DEBUG: done\n");
      result_reorder_and_output(&r_start, &r_bottom, &r_stacknum, jconf->output.output_hypo_maxnum, r, param);

      r->result.status = J_RESULT_STATUS_SUCCESS;
      //callback_exec(CALLBACK_RESULT, r);
      //callback_exec(CALLBACK_EVENT_PASS2_END, r);
  }
  
  /* �Ƽ參���󥿤���� */
  /* output counters */
  if (verbose_flag) {
    jlog("STAT: %02d %s: %d generated, %d pushed, %d nodes popped in %d\n",
	 r->config->id, r->config->name,
	 dwrk->genectr, dwrk->pushctr, dwrk->popctr, backtrellis->framelen);
    jlog_flush();
#ifdef GRAPHOUT_DYNAMIC
    if (r->graphout) {
      jlog("STAT: %02d %s: graph: %d merged", r->config->id, r->config->name, dynamic_merged_num);
#ifdef GRAPHOUT_SEARCH
      jlog("S, %d terminated", terminate_search_num);
#endif
      jlog(" in %d\n", dwrk->popctr);
    }
#endif
  }
    
  if (dwrk->finishnum > 0 && r->graphout) {
    if (verbose_flag) jlog("STAT: ------ wordgraph post-processing begin ------\n");
    /* garbage collection and word merging */
    /* words with no following word (except end of sentence) will be erased */
    wordgraph_purge_leaf_nodes(&wordgraph_root, r);
#ifdef GRAPHOUT_DEPTHCUT
    /* perform word graph depth cutting here */
    wordgraph_depth_cut(&wordgraph_root, r);
#endif

    /* if GRAPHOUT_PRECISE_BOUNDARY defined,
       propagate exact time boundary to the right context.
       words of different boundary will be duplicated here.
    */
    wordgraph_adjust_boundary(&wordgraph_root, r);

    if (jconf->graph.confnet) {
      /* CONFUSION NETWORK GENERATION */

      /* old merging functions should be skipped */

      /* finalize: sort and annotate ID */
      r->graph_totalwordnum = wordgraph_sort_and_annotate_id(&wordgraph_root, r);
      /* check coherence */
      wordgraph_check_coherence(wordgraph_root, r);
      /* compute graph CM by forward-backward processing */
      graph_forward_backward(wordgraph_root, r);
      if (verbose_flag) jlog("STAT: ------ wordgraph post-processing end ------\n");

      r->result.wg = wordgraph_root;

      /* 
       * if (jconf->graph.lattice) {
       *   callback_exec(CALLBACK_RESULT_GRAPH, r);
       * }
       */
      
      /* parse the graph to extract order relationship */
      graph_make_order(wordgraph_root, r);
      /* create confusion network */
      r->result.confnet = confnet_create(wordgraph_root, r);
      /* output confusion network */
      //callback_exec(CALLBACK_RESULT_CONFNET, r);
      /* free area for order relationship */
      graph_free_order(r);
      /* free confusion network clusters */
      //cn_free_all(&(r->result.confnet));

    } else if (jconf->graph.lattice) {
      /* WORD LATTICE POSTPROCESSING */

      /* merge words with the same time and same score */
      wordgraph_compaction_thesame(&wordgraph_root);
      /* merge words with the same time (skip if "-graphrange -1") */
      wordgraph_compaction_exacttime(&wordgraph_root, r);
      /* merge words of near time (skip if "-graphrange" value <= 0 */
      wordgraph_compaction_neighbor(&wordgraph_root, r);
      /* finalize: sort and annotate ID */
      r->graph_totalwordnum = wordgraph_sort_and_annotate_id(&wordgraph_root, r);
      /* check coherence */
      wordgraph_check_coherence(wordgraph_root, r);
      /* compute graph CM by forward-backward processing */
      graph_forward_backward(wordgraph_root, r);
      if (verbose_flag) jlog("STAT: ------ wordgraph post-processing end ------\n");
      /* output graph */
      r->result.wg = wordgraph_root;
      //callback_exec(CALLBACK_RESULT_GRAPH, r);

    } else {
      j_internal_error("InternalError: graph generation specified but no output format specified?\n");
    }

    /* clear all wordgraph */
    //wordgraph_clean(&(r->result.wg));
  } /* r->graphout */

  
  /* ��λ���� */
  /* finalize */
  nw_free(nextword, nwroot);
  free_all_nodes(start);
  free_wordtrellis(dwrk);
#ifdef SCAN_BEAM
  free(dwrk->framemaxscore);
#endif
  //result_sentence_free(r);
  clear_stocker(dwrk);
}

/** 
 * <JA>
 * ��2�ѥ��ѤΥ�����ꥢ����ݡ����������.
 *
 * �����ǳ��ݤ����Τ�ǧ�����ѥ�᡼���˰ͤ�ʤ��ͤΤߡ�
 * 
 * @param r [i/o] ǧ���������󥹥���
 * </JA>
 * <EN>
 * Initialize and allocate work area for 2nd pass.
 *
 * This function only contains input / parameter dependent initialization.
 * 
 * @param r [in] recognition process instance
 * </EN>
 */
void
wchmm_fbs_prepare(RecogProcess *r)
{
  StackDecode *dwrk;
  dwrk = &(r->pass2);
  
  /* N-gram �ѥ�����ꥢ����� */
  /* malloc work area for N-gram */
  if (r->lmtype == LM_PROB && r->lm->ngram) {
    dwrk->cnword = (WORD_ID *)mymalloc(sizeof(WORD_ID) * r->lm->ngram->n);
    dwrk->cnwordrev = (WORD_ID *)mymalloc(sizeof(WORD_ID) * r->lm->ngram->n);
  } else {
    dwrk->cnword = dwrk->cnwordrev = NULL;
  }
  dwrk->stocker_root = NULL;
#ifdef CONFIDENVE_MEASURE
#ifdef CM_MULTIPLE_ALPHA
  dwrk->cmsumlist = NULL;
#endif
#ifdef CM_NBEST;
  dwrk->sentcm = NULL;
  dwrk->wordcm = NULL;
#endif
#endif
}

/** 
 * <JA>
 * ��2�ѥ��ѤΥ�����ꥢ���������.
 *
 * �����ǲ��������Τ�ǧ�����ѥ�᡼���˰ͤ�ʤ��ͤΤߡ�
 * 
 * @param r [i/o] ǧ���������󥹥���
 * </JA>
 * <EN>
 * Free the work area for 2nd pass.
 *
 * This function only concerns input / parameter dependent work area.
 * 
 * @param r [in] recognition process instance
 * </EN>
 */
void
wchmm_fbs_free(RecogProcess *r)
{
  StackDecode *dwrk;
  dwrk = &(r->pass2);

  if (r->lmtype == LM_PROB && r->lm->ngram) {
    free(dwrk->cnword);
    free(dwrk->cnwordrev);
    dwrk->cnword = dwrk->cnwordrev = NULL;
  }

#ifdef CONFIDENVE_MEASURE
#ifdef CM_MULTIPLE_ALPHA
  if (dwrk->cmsumlist) {
    free(dwrk->cmsumlist);
    dwrk->cmsumlist = NULL;
  }
#endif
#ifdef CM_NBEST;
  if (dwrk->sentcm) {
    free(dwrk->sentcm);
    dwrk->sentcm = NULL;
  }
  if (dwrk->wordcm) {
    free(dwrk->wordcm);
    dwrk->wordcm = NULL;
  }
#endif
#endif
}

/* end of file */
