/**
 * @file   search_bestfirst_v2.c
 * 
 * <JA>
 * @brief  ��2�ѥ���Viterbi�黻����Ӳ��⥹�����׻� (�̾���)
 *
 * �����Ǥϡ���2�ѥ��ˤ�����õ����β����Viterbi�������ι����黻��
 * ��ñ��ȤΥȥ�ꥹ��³������Ӳ���Υ������׻���Ԥ��ؿ�����������
 * ���ޤ�. 
 *
 * ñ����³����ñ��ֲ��ǴĶ���¸���ϡ����Τ� nextscan ���르�ꥺ����Ѥ��ޤ�. 
 * ���Υե�������������Ƥ���ؿ��ϡ�config.h �ˤ����� PASS2_STRICT_IWCD
 * �� define �Ǥ���Ȥ��˻��Ѥ���ޤ�. �դ˾嵭�� undef �Ǥ���Ȥ��ϡ�
 * search_bestfirst_v1.c �δؿ����Ѥ����ޤ�. 
 *
 * Backscan �Ǥϡ��ǥ����ǥ��󥰤����٤�Ż뤷�ơ���ñ��Ȥ�������ñ���
 * ������ñ��ֲ��ǥ���ƥ����Ȥϲ���Ÿ�����ˤ��٤Ƹ�̩�˷׻�����ޤ�. 
 * Backscan ��Ԥʤ� search_bestfirst_v1.c ��������� POP ���˹Ԥʤ��Τ�
 * ��٤ơ������Ǥϲ��������λ��������Τʥ�������׻����뤿�ᡤ
 * ���������٤Ϲ⤤. ����������������뤹�٤Ƥβ�����Ф���
 * (���Ȥ������å�������ʤ�����Ǥ��äƤ�)�ȥ饤�ե���κƷ׻���Ԥʤ����ᡤ
 * �׻��̤� backscan ����٤����礷�ޤ�. 
 * </JA>
 * 
 * <EN>
 * @brief  Viterbi path update and scoring on the second pass (standard version)
 *
 * This file has functions for score calculations on the 2nd pass.
 * It includes Viterbi path update calculation of a hypothesis, calculations
 * of scores and word trellis connection at word expansion.
 * 
 * The cross-word triphone will be computed just at word expansion time,
 * for precise scoring.  This is called "nextscan" altgorithm. These
 * functions are enabled when PASS2_STRICT_IWCD is DEFINED in config.h.
 * If undefined, the "backscan" functions in search_bestfirst_v1.c will be
 * used instead.
 *
 * Here in nextscan algorithm, all cross-word context dependencies between
 * next word and source hypothesis are computed as soon as a new hypotheses
 * is expanded.  As the precise cross-word triphone score is applied on
 * hypothesis generation with no delay, more accurate search-time score can
 * be obtained than the delayed backscan method in search_bestfirst_v1.c.
 * On the other hand, the computational cost grows much by re-calculating
 * forward score of cross-word triphones for all the generated hypothethes,
 * even non-promising ones.
 * </EN>
 * 
 * @author Akinobu Lee
 * @date   Mon Sep 12 00:58:50 2005
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

/* By "fast" setting (default), search_bestfirst_v1.c is used for faster
   decoding.  Please specify option "--enable-setup=standard" or
   "--enable-strict-iwcd2" at "./configure" to activate this. */

#include <julius/julius.h>

#ifdef PASS2_STRICT_IWCD

#undef TCD			///< Define if want triphone debug messages


/**********************************************************************/
/************ ����Ρ��ɤδ������                         ************/
/************ Basic functions for hypothesis node handling ************/
/**********************************************************************/

#undef STOCKER_DEBUG

#ifdef STOCKER_DEBUG
static int stocked_num = 0;
static int reused_num = 0;
static int new_num = 0;
static int request_num = 0;
#endif

/** 
 * <JA>
 * ����Ρ��ɤ�ºݤ˥���夫���������. 
 * 
 * @param node [in] ����Ρ���
 * </JA>
 * <EN>
 * Free a hypothesis node actually.
 * 
 * @param node [in] hypothesis node
 * </EN>
 */
static void
free_node_exec(NODE *node)
{
  if (node == NULL) return;
  free(node->g);
#ifdef GRAPHOUT_PRECISE_BOUNDARY
  if (node->region->graphout) {
    free(node->wordend_frame);
    free(node->wordend_gscore);
  }
#endif
  free(node);
}

/** 
 * <JA>
 * ����Ρ��ɤ����Ѥ�λ���ƥꥵ�������Ѥ˥��ȥå�����
 * 
 * @param node [in] ����Ρ���
 * </JA>
 * <EN>
 * Stock an unused hypothesis node for recycle.
 * 
 * @param node [in] hypothesis node
 * </EN>
 * @callgraph
 * @callergraph
 */
void
free_node(NODE *node)
{
  if (node == NULL) return;

  if (node->region->graphout) {
    if (node->prevgraph != NULL && node->prevgraph->saved == FALSE) {
      wordgraph_free(node->prevgraph);
    }
  }

  /* save to stocker */
  node->next = node->region->pass2.stocker_root;
  node->region->pass2.stocker_root = node;

#ifdef STOCKER_DEBUG
  stocked_num++;
#endif
}

/** 
 * <JA>
 * �ꥵ�������ѥΡ��ɳ�Ǽ�ˤ���ˤ���. 
 * 
 * @param s [in] stack decoding work area
 * 
 * </JA>
 * <EN>
 * Clear the node stocker for recycle.
 * 
 * @param s [in] stack decoding work area
 * 
 * </EN>
 * @callgraph
 * @callergraph
 */
void
clear_stocker(StackDecode *s)
{
  NODE *node, *tmp;
  node = s->stocker_root;
  while(node) {
    tmp = node->next;
    free_node_exec(node);
    node = tmp;
  }
  s->stocker_root = NULL;

#ifdef STOCKER_DEBUG
  jlog("DEBUG: %d times requested, %d times newly allocated, %d times reused\n", request_num, new_num, reused_num);
  stocked_num = 0;
  reused_num = 0;
  new_num = 0;
  request_num = 0;
#endif
}

/** 
 * <JA>
 * ����򥳥ԡ�����. 
 * 
 * @param dst [out] ���ԡ���β���
 * @param src [in] ���ԡ����β���
 * 
 * @return @a dst ���֤�. 
 * </JA>
 * <EN>
 * Copy the content of node to another.
 * 
 * @param dst [out] target hypothesis
 * @param src [in] source hypothesis
 * 
 * @return the value of @a dst.
 * </EN>
 * @callgraph
 * @callergraph
 */
NODE *
cpy_node(NODE *dst, NODE *src)
{
  int peseqlen;

  peseqlen = src->region->peseqlen;
  
  dst->next = src->next;
  dst->prev = src->prev;
  memcpy(dst->g, src->g, sizeof(LOGPROB) * peseqlen);
  memcpy(dst->seq, src->seq, sizeof(WORD_ID) * MAXSEQNUM);
#ifdef CM_SEARCH
#ifdef CM_MULTIPLE_ALPHA
  {
    int w;
    for(w=0;w<src->seqnum;w++) {
      memcpy(dst->cmscore[w], src->cmscore[w], sizeof(LOGPROB) * src->region->config->annotate.cm_alpha_num);
    }
  }     
#else
  memcpy(dst->cmscore, src->cmscore, sizeof(LOGPROB) * MAXSEQNUM);
#endif
#endif /* CM_SEARCH */
  dst->seqnum = src->seqnum;
  dst->score = src->score;
  dst->bestt = src->bestt;
  dst->estimated_next_t = src->estimated_next_t;
  dst->endflag = src->endflag;
  dst->state = src->state;
  dst->tre = src->tre;
  if (src->region->ccd_flag) {
    dst->last_ph = src->last_ph;
    dst->last_ph_sp_attached = src->last_ph_sp_attached;
  }
  dst->totallscore = src->totallscore;
  dst->final_g = src->final_g;
#ifdef VISUALIZE
  dst->popnode = src->popnode;
#endif

  if (src->region->graphout) {
#ifdef GRAPHOUT_PRECISE_BOUNDARY
    memcpy(dst->wordend_frame, src->wordend_frame, sizeof(short) * peseqlen);
    memcpy(dst->wordend_gscore, src->wordend_gscore, sizeof(LOGPROB) * peseqlen);
#endif
    dst->prevgraph = src->prevgraph;
    dst->lastcontext = src->lastcontext;
#ifndef GRAPHOUT_PRECISE_BOUNDARY
    dst->tail_g_score = src->tail_g_score;
#endif
  }
  return(dst);
}

/** 
 * <JA>
 * �����ʲ���Ρ��ɤ����դ���. �⤷��Ǽ�ˤ˰������Ѥ���ʤ��ʤä�
 * �Ρ��ɤ�������Ϥ��������Ѥ���. �ʤ���п����˳���դ���.
 *
 * @param r [in] ǧ���������󥹥���
 * 
 * @return �����˳���դ���줿����Ρ��ɤؤΥݥ��󥿤��֤�. 
 * </JA>
 * <EN>
 * Allocate a new hypothesis node.  If the node stocker is not empty,
 * the one in the stocker is re-used.  Otherwise, allocate as new.
 * 
 * @param r [in] recognition process instance
 * 
 * @return pointer to the newly allocated node.
 * </EN>
 * @callgraph
 * @callergraph
 */
NODE *
newnode(RecogProcess *r)
{
  NODE *tmp;
  int i;
  int peseqlen;

  peseqlen = r->peseqlen;

#ifdef STOCKER_DEBUG
  request_num++;
#endif
  if ((tmp = r->pass2.stocker_root) != NULL) {
    /* re-use ones in the stocker */
    r->pass2.stocker_root = tmp->next;
#ifdef STOCKER_DEBUG
    stocked_num--;
    reused_num++;
#endif
  } else {
    /* allocate new */
    tmp = (NODE *)mymalloc(sizeof(NODE));
    tmp->g = (LOGPROB *)mymalloc(sizeof(LOGPROB) * peseqlen);
#ifdef GRAPHOUT_PRECISE_BOUNDARY
    if (r->graphout) {
      tmp->wordend_frame = (short *)mymalloc(sizeof(short) * peseqlen);
      tmp->wordend_gscore = (LOGPROB *)mymalloc(sizeof(LOGPROB) * peseqlen);
    }
#endif
#ifdef STOCKER_DEBUG
    new_num++;
#endif
  }

  /* clear the data */
  /*bzero(tmp,sizeof(NODE));*/
  tmp->next=NULL;
  tmp->prev=NULL;
  tmp->last_ph = NULL;
  tmp->last_ph_sp_attached = FALSE;
  if (r->ccd_flag) {
    tmp->totallscore = LOG_ZERO;
  }
  tmp->endflag = FALSE;
  tmp->seqnum = 0;
  for(i = 0; i < peseqlen; i++) {
    tmp->g[i] = LOG_ZERO;
  }
  tmp->final_g = LOG_ZERO;
#ifdef VISUALIZE
  tmp->popnode = NULL;
#endif
  if (r->graphout) {
    tmp->prevgraph = NULL;
    tmp->lastcontext = NULL;
  }

  tmp->region = r;

  return(tmp);
}


/**********************************************************************/
/************ �������ȥ�ꥹŸ�������ٷ׻�             ****************/
/************ Expand trellis and update forward score *****************/
/**********************************************************************/

/** 
 * <JA>
 * 1ñ��ʬ�Υȥ�ꥹ�׻��ѤΥ�����ꥢ�����. 
 * 
 * @param r [in] ǧ���������󥹥���
 * 
 * </JA>
 * <EN>
 * Allocate work area for trellis computation of a word.
 * 
 * @param r [in] recognition process instance
 * 
 * </EN>
 * @callgraph
 * @callergraph
 */
void
malloc_wordtrellis(RecogProcess *r)
{
  int maxwn;
  StackDecode *dwrk;

  maxwn = r->lm->winfo->maxwn + 10;	/* CCD�ˤ����ư���θ */
  dwrk = &(r->pass2);

  dwrk->wordtrellis[0] = (LOGPROB *)mymalloc(sizeof(LOGPROB) * maxwn);
  dwrk->wordtrellis[1] = (LOGPROB *)mymalloc(sizeof(LOGPROB) * maxwn);

  dwrk->g = (LOGPROB *)mymalloc(sizeof(LOGPROB) * r->peseqlen);

  dwrk->phmmlen_max = r->lm->winfo->maxwlen + 2;
  dwrk->phmmseq = (HMM_Logical **)mymalloc(sizeof(HMM_Logical *) * dwrk->phmmlen_max);
  if (r->lm->config->enable_iwsp && r->am->hmminfo->multipath) {
    dwrk->has_sp = (boolean *)mymalloc(sizeof(boolean) * dwrk->phmmlen_max);
  } else {
    dwrk->has_sp = NULL;
  }

  dwrk->wef = NULL;
  dwrk->wes = NULL;
  dwrk->wend_token_frame[0] = NULL;
  dwrk->wend_token_frame[1] = NULL;
  dwrk->wend_token_gscore[0] = NULL;
  dwrk->wend_token_gscore[1] = NULL;
#ifdef GRAPHOUT_PRECISE_BOUNDARY
  if (r->graphout) {
    dwrk->wef = (short *)mymalloc(sizeof(short) * r->peseqlen);
    dwrk->wes = (LOGPROB *)mymalloc(sizeof(LOGPROB) * r->peseqlen);
    dwrk->wend_token_frame[0] = (short *)mymalloc(sizeof(short) * maxwn);
    dwrk->wend_token_frame[1] = (short *)mymalloc(sizeof(short) * maxwn);
    dwrk->wend_token_gscore[0] = (LOGPROB *)mymalloc(sizeof(LOGPROB) * maxwn);
    dwrk->wend_token_gscore[1] = (LOGPROB *)mymalloc(sizeof(LOGPROB) * maxwn);
  }
#endif
}

/** 
 * <JA>
 * 1ñ��ʬ�Υȥ�ꥹ�׻��ѤΥ�������ꥢ�����
 * 
 * </JA>
 * <EN>
 * Free the work area for trellis computation of a word.
 * 
 * </EN>
 * @callgraph
 * @callergraph
 */
void
free_wordtrellis(StackDecode *dwrk)
{
  free(dwrk->wordtrellis[0]);
  free(dwrk->wordtrellis[1]);
  free(dwrk->g);
  free(dwrk->phmmseq);
  if (dwrk->has_sp) {
    free(dwrk->has_sp);
    dwrk->has_sp = NULL;
  }
#ifdef GRAPHOUT_PRECISE_BOUNDARY
  if (dwrk->wef) {
    free(dwrk->wef);
    free(dwrk->wes);
    free(dwrk->wend_token_frame[0]);
    free(dwrk->wend_token_frame[1]);
    free(dwrk->wend_token_gscore[0]);
    free(dwrk->wend_token_gscore[1]);
    dwrk->wef = NULL;
  }
#endif
}


/**********************************************************************/
/************ ��������������ٷ׻�                  *******************/
/************ Compute forward score of a hypothesis *******************/
/**********************************************************************/

/* Ϳ����줿���ǤΤʤ�� phmmseq[0..phmmlen-1]���Ф���viterbi�׻���Ԥ�. 
   g[0..framelen-1] �Υ����������ͤȤ��� g_new[0..framelen-1]�˹����ͤ�����. 
   ���� least_frame �ޤǤ�scan����. */
/* Viterbi computation for the given phoneme sequence 'phmmseq[0..phmmlen-1]'
   with g[0..framelen-1] as initial values.  The results are stored in
   g_new[0..framelen-1].  Scan should not terminate at least it reaches
   'least_frame'. */
/** 
 * <JA>
 * Ϳ����줿���Ǥ��¤Ӥ��Ф��� Viterbi �׻���Ԥ�����������������
 * �����������Ѵؿ�. 
 * 
 * @param g [in] ���ߤλ��֤��Ȥ�������������
 * @param g_new [out] ������ο��������������������Ǽ����Хåե�
 * @param phmmseq [in] ����HMM���¤�
 * @param has_sp [in] short-pause location
 * @param phmmlen [in] @a phmmseq ��Ĺ��
 * @param param [in] ���ϥѥ�᡼��
 * @param framelen [in] ���ϥե졼��Ĺ
 * @param least_frame [in] �ӡ�������������Υե졼����ʾ�� Viterbi�׻�����
 * @param final_g [in] final g scores
 * @param wordend_frame_src [in] ���ߤ�ñ�콪ü�ե졼��ȡ�����
 * @param wordend_frame_dst [out] ������ο�����ñ�콪ü�ե졼��ȡ�����
 * @param wordend_gscore_src [in] ���ߤ�ñ�콪ü�������ȡ�����
 * @param wordend_gscore_dst [out] ������ο�����ñ�콪ü�������ȡ�����
 * @param r [in] recognition process instance
 * </JA>
 * <EN>
 * Generic function to perform Viterbi path updates for given phoneme
 * sequence.
 * 
 * @param g [in] current forward scores at each input frame
 * @param g_new [out] buffer to save the resulting score updates
 * @param phmmseq [in] phoneme sequence to perform Viterbi
 * @param has_sp [in] short-pause location
 * @param phmmlen [in] length of @a phmmseq.
 * @param param [in] input parameter vector
 * @param framelen [in] input frame length to compute
 * @param least_frame [in] Least frame length to force viterbi even with beam
 * @param final_g [in] final g scores
 * @param wordend_frame_src [in] current word-end frame tokens
 * @param wordend_frame_dst [out] buffer to store updated word-end frame tokens
 * @param wordend_gscore_src [in] current word-end score tokens
 * @param wordend_gscore_dst [out] buffer to store updated word-end score tokens
 * @param r [in] recognition process instance
 * 
 * </EN>
 */
static void
do_viterbi(LOGPROB *g, LOGPROB *g_new, HMM_Logical **phmmseq, boolean *has_sp, int phmmlen, HTK_Param *param, int framelen, int least_frame, LOGPROB *final_g, short *wordend_frame_src, short *wordend_frame_dst, LOGPROB *wordend_gscore_src, LOGPROB *wordend_gscore_dst, RecogProcess *r) /* has_sp and final_g is for multipath only */
{
  HMM *whmm;			/* HMM */
  int wordhmmnum;		/* length of above */
  int startt;			/* scan start frame */
  LOGPROB tmpmax,tmpscore;	/* variables for Viterbi process */
  A_CELL *ac;
  int t,i,j;
  boolean node_exist_p;
  int tn;		       ///< Temporal pointer to current buffer
  int tl;		       ///< Temporal pointer to previous buffer

  /* store global values to local for rapid access */
  StackDecode *dwrk;
  WORD_INFO *winfo;
  HTK_HMM_INFO *hmminfo;
  LOGPROB *framemaxscore;
#ifdef SCAN_BEAM
  LOGPROB scan_beam_thres;
#endif

  dwrk = &(r->pass2);
  winfo = r->lm->winfo;
  hmminfo = r->am->hmminfo;
  framemaxscore = r->pass2.framemaxscore;
#ifdef SCAN_BEAM
  scan_beam_thres = r->config->pass2.scan_beam_thres;
#endif


#ifdef TCD
  jlog("DEBUG: scan for:");
  for (i=0;i<phmmlen;i++) {
    jlog(" %s", phmmseq[i]->name);
  }
  jlog("\n");
#endif
  
  /* ñ��HMM���� */
  /* make word HMM */
  whmm = new_make_word_hmm(hmminfo, phmmseq, phmmlen, has_sp);
  if (whmm == NULL) {
    j_internal_error("Error: failed to make word hmm\n");
  }
  wordhmmnum = whmm->len;
  if (wordhmmnum >= winfo->maxwn + 10) {
    j_internal_error("do_viterbi: word too long (>%d)\n", winfo->maxwn + 10);
  }

  /* scan�������򸡺� -> startt��*/
  /* search for the start frame -> set to startt */
  for(t = framelen-1; t >=0 ; t--) {
    if (
#ifdef SCAN_BEAM
	g[t] > framemaxscore[t] - scan_beam_thres &&
#endif
	g[t] > LOG_ZERO) {
      break;
    }
  }
  if (t < 0) {			/* no node has score > LOG_ZERO */
    /* reset all scores and end */
    for(t=0;t<framelen;t++) {
      g_new[t] = LOG_ZERO;
#ifdef GRAPHOUT_PRECISE_BOUNDARY
      if (r->graphout) {
	wordend_frame_dst[t] = -1;
	wordend_gscore_dst[t] = LOG_ZERO;
      }
#endif
    }
    free_hmm(whmm);
    return;
  }
  startt = t;
  
  /* �������ʹ�[startt+1..framelen-1] �� g_new[] ��ꥻ�å� */
  /* clear g_new[] for [startt+1..framelen-1] */
  for(t=framelen-1;t>startt;t--) {
    g_new[t] = LOG_ZERO;
#ifdef GRAPHOUT_PRECISE_BOUNDARY
    if (r->graphout) {
      wordend_frame_dst[t] = -1;
      wordend_gscore_dst[t] = LOG_ZERO;
    }
#endif
  }

  /*****************/
  /* viterbi start */
  /*****************/

  /* set initial swap buffer */
  tn = 0; tl = 1;

#ifdef GRAPHOUT_PRECISE_BOUNDARY
  if (r->graphout) {
    for(i=0;i<wordhmmnum;i++) {
      dwrk->wend_token_frame[tn][i] = -1;
      dwrk->wend_token_gscore[tn][i] = LOG_ZERO;
    }
  }
#endif

  if (! hmminfo->multipath) {
    /* ���� [startt] ����ͤ����� */
    /* initialize scores on frame [startt] */
    for(i=0;i<wordhmmnum-1;i++) dwrk->wordtrellis[tn][i] = LOG_ZERO;
    if (g[startt] <= LOG_ZERO)
      dwrk->wordtrellis[tn][wordhmmnum-1] = LOG_ZERO;
    else
      dwrk->wordtrellis[tn][wordhmmnum-1] = g[startt] + outprob(&(r->am->hmmwrk), startt, &(whmm->state[wordhmmnum-1]), param);
    g_new[startt] = dwrk->wordtrellis[tn][0];
#ifdef GRAPHOUT_PRECISE_BOUNDARY
    if (r->graphout) {
      dwrk->wend_token_frame[tn][wordhmmnum-1] = wordend_frame_src[startt];
      dwrk->wend_token_gscore[tn][wordhmmnum-1] = wordend_gscore_src[startt];
      wordend_frame_dst[startt] = dwrk->wend_token_frame[tn][0];
      wordend_gscore_dst[startt] = dwrk->wend_token_gscore[tn][0];
    }
#endif
  }
  
  /* �ᥤ��롼��: startt ����Ϥޤ� 0 �˸����ä� Viterbi �׻� */
  /* main loop: start from [startt], and compute Viterbi toward [0] */
  for(t = hmminfo->multipath ? startt : startt - 1; t >= 0; t--) {
    
    /* wordtrellis�Υ�����ꥢ�򥹥�å� */
    /* swap workarea of wordtrellis */
    i = tn; tn = tl; tl = i;

    node_exist_p = FALSE;	/* TRUE if there is at least 1 survived node in this frame */

    if (! hmminfo->multipath) {
    
      /* ü�ΥΡ��� [t][wordhmmnum-1]�ϡ��������� �� g[]�ι⤤���ˤʤ� */
      /* the edge node [t][wordhmmnum-1] is either internal transitin or g[] */
      tmpscore = LOG_ZERO;
      for (ac=whmm->state[wordhmmnum-1].ac;ac;ac=ac->next) {
	if (tmpscore < dwrk->wordtrellis[tl][ac->arc] + ac->a) {
	  j = ac->arc;
	  tmpscore = dwrk->wordtrellis[tl][ac->arc] + ac->a;
	}
      }
      if (g[t] > tmpscore) {
	tmpmax = g[t];
#ifdef GRAPHOUT_PRECISE_BOUNDARY
	if (r->graphout) {
	  dwrk->wend_token_frame[tn][wordhmmnum-1] = wordend_frame_src[t];
	  dwrk->wend_token_gscore[tn][wordhmmnum-1] = wordend_gscore_src[t];
	}
#endif
      } else {
	tmpmax = tmpscore;
#ifdef GRAPHOUT_PRECISE_BOUNDARY
	if (r->graphout) {
	  dwrk->wend_token_frame[tn][wordhmmnum-1] = dwrk->wend_token_frame[tl][j];
	  dwrk->wend_token_gscore[tn][wordhmmnum-1] = dwrk->wend_token_gscore[tl][j];
	}
#endif
      }
      
      /* ü�ΥΡ��ɤΥ���������٥��ץ����å�: ���������ʤ���Ȥ� */
      /* check if the edge node is within score envelope */
      if (
#ifdef SCAN_BEAM
	  tmpmax <= framemaxscore[t] - scan_beam_thres ||
#endif
	  tmpmax <= LOG_ZERO
	  ) {
	dwrk->wordtrellis[tn][wordhmmnum-1] = LOG_ZERO;
#ifdef GRAPHOUT_PRECISE_BOUNDARY
	if (r->graphout) {
	  dwrk->wend_token_frame[tn][wordhmmnum-1] = -1;
	  dwrk->wend_token_gscore[tn][wordhmmnum-1] = LOG_ZERO;
	}
#endif
      } else {
	node_exist_p = TRUE;
	dwrk->wordtrellis[tn][wordhmmnum-1] = tmpmax + outprob(&(r->am->hmmwrk), t, &(whmm->state[wordhmmnum-1]), param);
      }

    }

    /* node[wordhmmnum-2..0]�ˤĤ��ƥȥ�ꥹ��Ÿ�� */
    /* expand trellis for node [t][wordhmmnum-2..0] */
    for(i=wordhmmnum-2;i>=0;i--) {
      
      /* ����ѥ��Ⱥ��ॹ���� tmpmax �򸫤Ĥ��� */
      /* find most likely path and the max score 'tmpmax' */
      tmpmax = LOG_ZERO;
      for (ac=whmm->state[i].ac;ac;ac=ac->next) {
	if (hmminfo->multipath) {
	  if (ac->arc == wordhmmnum-1) tmpscore = g[t];
	  else if (t + 1 > startt) tmpscore = LOG_ZERO;
	  else tmpscore = dwrk->wordtrellis[tl][ac->arc];
	  tmpscore += ac->a;
	} else {
	  tmpscore = dwrk->wordtrellis[tl][ac->arc] + ac->a;
	}
	if (tmpmax < tmpscore) {
	  tmpmax = tmpscore;
	  j = ac->arc;
	}
      }
      
      /* ����������٥��ץ����å�: ���������ʤ���Ȥ� */
      /* check if score of this node is within the score envelope */
      if (
#ifdef SCAN_BEAM
	  tmpmax <= framemaxscore[t] - scan_beam_thres ||
#endif
	  tmpmax <= LOG_ZERO
	  ) {
	/* invalid node */
	dwrk->wordtrellis[tn][i] = LOG_ZERO;
#ifdef GRAPHOUT_PRECISE_BOUNDARY
	if (r->graphout) {
	  dwrk->wend_token_frame[tn][i] = -1;
	  dwrk->wend_token_gscore[tn][i] = LOG_ZERO;
	}
#endif
      } else {
	/* survived node */
	node_exist_p = TRUE;
 	dwrk->wordtrellis[tn][i] = tmpmax;
	if (! hmminfo->multipath || i > 0) {
	  dwrk->wordtrellis[tn][i] += outprob(&(r->am->hmmwrk), t, &(whmm->state[i]), param);
	}
#ifdef GRAPHOUT_PRECISE_BOUNDARY
	if (r->graphout) {
	  if (hmminfo->multipath) {
	    if (j == wordhmmnum-1) {
	      dwrk->wend_token_frame[tn][i] = wordend_frame_src[t];
	      dwrk->wend_token_gscore[tn][i] = wordend_gscore_src[t];
	    } else {
	      dwrk->wend_token_frame[tn][i] = dwrk->wend_token_frame[tl][j];
	      dwrk->wend_token_gscore[tn][i] = dwrk->wend_token_gscore[tl][j];
	    }
	  } else {
	    dwrk->wend_token_frame[tn][i] = dwrk->wend_token_frame[tl][j];
	    dwrk->wend_token_gscore[tn][i] = dwrk->wend_token_gscore[tl][j];
	  }
	}
#endif
      }
    } /* end of node loop */

    /* ���� t ��Viterbi�׻���λ. ������������������ g_new[t] �򥻥å� */
    /* Viterbi end for frame [t].  set the new forward score g_new[t] */
    g_new[t] = dwrk->wordtrellis[tn][0];
#ifdef GRAPHOUT_PRECISE_BOUNDARY
    if (r->graphout) {
    /* new wordend */
      wordend_frame_dst[t] = dwrk->wend_token_frame[tn][0];
      wordend_gscore_dst[t] = dwrk->wend_token_gscore[tn][0];
    }
#endif
    /* ���ꤵ�줿 least_frame �����ޤ� t ���ʤ�Ǥ��ꡤ���Ĥ��� t �ˤ�����
       ����������٥��פˤ�ä������Ĥä��Ρ��ɤ���Ĥ�̵���ä����,
       ���Υե졼��Ƿ׻����Ǥ��ڤꤽ��ʾ���([0..t-1])�Ϸ׻����ʤ� */
    /* if frame 't' already reached the 'least_frame' and no node was
       survived in this frame (all nodes pruned by score envelope),
       terminate computation at this frame and do not computer further
       frame ([0..t-1]). */
    if (t < least_frame && (!node_exist_p)) {
      /* crear the rest scores */
      for (i=t-1;i>=0;i--) {
	g_new[i] = LOG_ZERO;
#ifdef GRAPHOUT_PRECISE_BOUNDARY
	if (r->graphout) {
	  wordend_frame_dst[i] = -1;
	  wordend_gscore_dst[i] = LOG_ZERO;
	}
#endif
      }
      /* terminate loop */
      break;
    }
    
  } /* end of time loop */

  if (hmminfo->multipath) {
    /* �������������κǽ��ͤ�׻� (���� 0 ������� 0 �ؤ�����) */
    /* compute the total forward score (transition from state 0 to frame 0 */
    if (t < 0) {			/* computed till the end */
      tmpmax = LOG_ZERO;
      for(ac=whmm->state[0].ac;ac;ac=ac->next) {
	tmpscore = dwrk->wordtrellis[tn][ac->arc] + ac->a;
	if (tmpmax < tmpscore) tmpmax = tmpscore;
      }
      *final_g = tmpmax;
    } else {
      *final_g = LOG_ZERO;
    }
  }

  /* free work area */
  free_hmm(whmm);
}

/** 
 * <JA>
 * �Ǹ��1���Ǥ��Ф��� Viterbi �׻���ʤ��. 
 * 
 * @param now [in] Ÿ������ʸ����. �첻�������������������� g[] �ˤ���Ȥ���. 
 * @param new [out] �׻������������������ g[] �˳�Ǽ�����. 
 * @param lastphone [in] Viterbi�׻���Ԥ�����HMM
 * @param sp [in] short-pause insertion
 * @param param [in] ���ϥ٥��ȥ���
 * @param r [in] ǧ���������󥹥���
 * </JA>
 * <EN>
 * Proceed Viterbi for the last one phoneme.
 * 
 * @param now [in] source hypothesis where the forward scores prior to the
 * last one phone is stored at g[]
 * @param new [out] the resulting updated forward scores will be saved to g[]
 * @param lastphone [in] phone HMM for the Viterbi processing
 * @param sp [in] short-pause insertion
 * @param param [in] input vectors
 * @param r [in] recognition process instance
 * </EN>
 */
static void
do_viterbi_next_word(NODE *now, NODE *new, HMM_Logical *lastphone, boolean sp, HTK_Param *param, RecogProcess *r) /* sp is for multipath only */
{
  int t, n;
  LOGPROB a_value;		/* for non multi-path */
  int peseqlen;
  boolean multipath;
  StackDecode *dwrk;

  dwrk = &(r->pass2);

  multipath = r->am->hmminfo->multipath;

  peseqlen = r->peseqlen;
  
  if (! multipath) {

    /* �⤷Ÿ��������κǸ��ñ��β���Ĺ�� 1 �Ǥ���С����β��Ǥ�
       ľ���� scan_word �Ƿ׻�����Ƥ��ʤ�. ���ξ��, now->g[] �˰�����
       ����ͤ���Ǽ����Ƥ���. 
       �⤷����Ĺ�����ʾ�Ǥ���С�now->g[] �Ϥ��μ����ޤǷ׻���������
       �Υ����������äƤ���Τ�,now->g[t] �������ͤ����ꤹ��ɬ�פ����� */
    /* If the length of last word is 1, it means the last phone was not
       scanned in the last call of scan_word().  In this case, now->g[]
       keeps the previous initial value, so start viterbi with the old scores.
       If the length is more than 1, the now->g[] keeps the values of the
       scan result till the previous phone, so make initial value
       considering last transition probability. */
    if (r->lm->winfo->wlen[now->seq[now->seqnum-1]] > 1) {
      n = hmm_logical_state_num(lastphone);
      a_value = (hmm_logical_trans(lastphone))->a[n-2][n-1];
      for(t=0; t<peseqlen-1; t++) dwrk->g[t] = now->g[t+1] + a_value;
      dwrk->g[peseqlen-1] = LOG_ZERO;
    } else {
      for(t=0; t<peseqlen; t++) dwrk->g[t] = now->g[t];
    }

  } else {
  
    for(t=0; t<peseqlen; t++) dwrk->g[t] = now->g[t];
    dwrk->phmmseq[0] = lastphone;
    if (r->lm->config->enable_iwsp) dwrk->has_sp[0] = sp;

  }
  
  do_viterbi(dwrk->g, new->g,
	     multipath ? dwrk->phmmseq : &lastphone,
	     (r->lm->config->enable_iwsp && multipath) ? dwrk->has_sp : NULL,
	     1, param, peseqlen, now->estimated_next_t, &(new->final_g)
#ifdef GRAPHOUT_PRECISE_BOUNDARY
	     , now->wordend_frame, new->wordend_frame
	     , now->wordend_gscore, new->wordend_gscore
#else
	     , NULL, NULL
	     , NULL, NULL
#endif
	     , r
	     );

#ifdef GRAPHOUT_PRECISE_BOUNDARY
  if (! multipath) {
    if (r->graphout) {
      /* ����� next_word �Ѥ˶��������Ĵ�� */
      /* proceed word boundary for one step for next_word */
      new->wordend_frame[r->peseqlen-1] = new->wordend_frame[0];
      new->wordend_gscore[r->peseqlen-1] = new->wordend_gscore[0];
      for (t=0;t<r->peseqlen-1;t++) {
	new->wordend_frame[t] = new->wordend_frame[t+1];
	new->wordend_gscore[t] = new->wordend_gscore[t+1];
      }
    }
  }
#endif
}

/** 
 * <JA>
 * �Ǹ��1ñ����������ȥ�ꥹ��׻����ơ�ʸ��������������٤򹹿�����. 
 * 
 * @param now [i/o] ʸ����
 * @param param [in] ���ϥѥ�᡼����
 * @param r [in] ǧ���������󥹥���
 * </JA>
 * <EN>
 * Compute the forward viterbi for the last word to update forward scores
 * and ready for word connection.
 * 
 * @param now [i/o] hypothesis
 * @param param [in] input parameter vectors
 * @param r [in] recognition process instance
 * </EN>
 * @callgraph
 * @callergraph
 */
void
scan_word(NODE *now, HTK_Param *param, RecogProcess *r)
{
  int   i,t;
  WORD_ID word;
  int phmmlen;
  HMM_Logical *tailph;

  /* store global values to local for rapid access */
  WORD_INFO *winfo;
  HTK_HMM_INFO *hmminfo;
  int peseqlen;
  boolean ccd_flag;
  boolean enable_iwsp;		/* multipath */
  StackDecode *dwrk;

  dwrk = &(r->pass2);
  winfo = r->lm->winfo;
  hmminfo = r->am->hmminfo;
  peseqlen = r->peseqlen;
  ccd_flag = r->ccd_flag;
  if (hmminfo->multipath) {
    enable_iwsp = r->lm->config->enable_iwsp;
  }
  
#ifndef GRAPHOUT_PRECISE_BOUNDARY
  if (r->graphout) {
    if (ccd_flag) {
      now->tail_g_score = now->g[now->bestt];
    }
  }
#endif

  /* ----------------------- prepare phoneme sequence ------------------ */
  /* triphone�ʤ���Ƭ��1���ǤϤ����Ǥ��оݳ�(���Ȥ�next_word�Ǥ��) */
  /*             ������1���Ǥϥ���ƥ����Ȥˤ������ä��ִ� */
  /* with triphone, modify the tail phone of the last word according to the
     previous word, and do not compute the head phone here (that will be
     computed later in next_word() */
  word = now->seq[now->seqnum-1];
  
#ifdef TCD
    jlog("DEBUG: w=");
    for(i=0;i<winfo->wlen[word];i++) {
      jlog(" %s",(winfo->wseq[word][i])->name);
    }
    if (ccd_flag) {
      if (now->last_ph != NULL) {
	jlog(" | %s", (now->last_ph)->name);
      }
    }
    jlog("\n");
#endif /* TCD */
    
  if (ccd_flag) {
    
    /* the tail triphone of the last word varies by context */
    if (now->last_ph != NULL) {
      tailph = get_right_context_HMM(winfo->wseq[word][winfo->wlen[word]-1], now->last_ph->name, hmminfo);
      if (tailph == NULL) {
	/* fallback to the original bi/mono-phone */
	/* error if the original is pseudo phone (not explicitly defined
	   in hmmdefs/hmmlist) */
	/* exception: word with 1 phone (triphone may exist in the next expansion */
	if (winfo->wlen[word] > 1 && winfo->wseq[word][winfo->wlen[word]-1]->is_pseudo){
	  error_missing_right_triphone(winfo->wseq[word][winfo->wlen[word]-1], now->last_ph->name);
	}

	tailph = winfo->wseq[word][winfo->wlen[word]-1];
      }
    } else {
      tailph = winfo->wseq[word][winfo->wlen[word]-1];
    }
    /* Ĺ��1��ñ��ϼ���nextword�Ǥ�����Ѳ�����ΤǤ����Ǥ�scan���ʤ� */
    /* do not scan word if the length is 1, as it further varies in the
       following next_word() */
    if (winfo->wlen[word] == 1) {
      now->last_ph = tailph;
      if (enable_iwsp && hmminfo->multipath) now->last_ph_sp_attached = TRUE;
#ifdef GRAPHOUT_PRECISE_BOUNDARY
      if (r->graphout) {
	/* ñ�춭�����¾�������� */
	/* initialize word boundary propagation info */
	for (t=0;t<peseqlen;t++) {
	  now->wordend_frame[t] = t;
	  now->wordend_gscore[t] = now->g[t];
	}
      }
#endif
#ifdef TCD
      jlog("DEBUG: suspended as %s\n", (now->last_ph)->name);
#endif
      return;
    }

    /* scan�ϰϤβ��������� */
    /* prepare HMM of the scan range */
    phmmlen = winfo->wlen[word] - 1;
    if (phmmlen > dwrk->phmmlen_max) {
      j_internal_error("scan_word: num of phonemes in a word exceed phmmlenmax (%d) ?\n", dwrk->phmmlen_max);
    }
    for (i=0;i<phmmlen-1;i++) {
      dwrk->phmmseq[i] = winfo->wseq[word][i+1];
    }
    dwrk->phmmseq[phmmlen-1] = tailph;
    if (enable_iwsp && hmminfo->multipath) {
      for (i=0;i<phmmlen-1;i++) dwrk->has_sp[i] = FALSE;
      dwrk->has_sp[phmmlen-1] = TRUE;
    }

  } else {			/* ~ccd_flag */

    phmmlen = winfo->wlen[word];
    for (i=0;i<phmmlen;i++) dwrk->phmmseq[i] = winfo->wseq[word][i];
    if (enable_iwsp && hmminfo->multipath) {
      for (i=0;i<phmmlen;i++) dwrk->has_sp[i] = FALSE;
      dwrk->has_sp[phmmlen-1] = TRUE;
    }

  }

  /* ����g[]�򤤤ä������򤷤Ƥ��� */
  /* temporally keeps the original g[] */
  for (t=0;t<peseqlen;t++) dwrk->g[t] = now->g[t];

#ifdef GRAPHOUT_PRECISE_BOUNDARY
  if (r->graphout) {
    /* ñ�춭�����¾�������� */
    /* initialize word boundary propagation info */
    for (t=0;t<peseqlen;t++) {
      dwrk->wef[t] = t;
      dwrk->wes[t] = now->g[t];
    }
  }
#endif

  /* viterbi��¹Ԥ��� g[] ���� now->g[] �򹹿����� */
  /* do viterbi computation for phmmseq from g[] to now->g[] */
  do_viterbi(dwrk->g, now->g, dwrk->phmmseq, (enable_iwsp && hmminfo->multipath) ? dwrk->has_sp : NULL, 
	     phmmlen, param, peseqlen, now->estimated_next_t, &(now->final_g)
#ifdef GRAPHOUT_PRECISE_BOUNDARY
	     /* ñ�춭������ we[] ���� now->wordend_frame[] �򹹿����� */
	     /* propagate word boundary info from we[] to now->wordend_frame[] */
	     , dwrk->wef, now->wordend_frame
	     , dwrk->wes, now->wordend_gscore
#else
	     , NULL, NULL
	     , NULL, NULL
#endif
	     , r
	     );
#ifdef GRAPHOUT_PRECISE_BOUNDARY
  if (! hmminfo->multipath) {
    if (r->graphout) {
      /* ����� next_word �Ѥ˶��������Ĵ�� */
      /* proceed word boundary for one step for next_word */
      now->wordend_frame[peseqlen-1] = now->wordend_frame[0];
      now->wordend_gscore[peseqlen-1] = now->wordend_gscore[0];
      for (t=0;t<peseqlen-1;t++) {
	now->wordend_frame[t] = now->wordend_frame[t+1];
	now->wordend_gscore[t] = now->wordend_gscore[t+1];
      }
    }
  }
#endif

  if (ccd_flag) {
    /* ����Τ���� now->last_ph �򹹿� */
    /* update 'now->last_ph' for future scan_word() */
    now->last_ph = winfo->wseq[word][0];
    if (enable_iwsp && hmminfo->multipath) now->last_ph_sp_attached = FALSE; /* wlen > 1 here */
#ifdef TCD
    jlog("DEBUG: last_ph = %s\n", (now->last_ph)->name);
#endif
  }
}


/**************************************************************************/
/*** �������Ÿ���ȥҥ塼�ꥹ�ƥ��å���Ҥ������Υ�������׻�           ***/
/*** Expand new hypothesis and compute the total score (with heuristic) ***/
/**************************************************************************/

/** 
 * <JA>
 * Ÿ��������˼�ñ�����³���ƿ������������������. ��ñ���ñ��ȥ�ꥹ���
 * ���������������³�����ᡤ���⥹������׻�����. 
 * 
 * @param now [in] Ÿ��������
 * @param new [out] �������������줿���⤬��Ǽ�����
 * @param nword [in] ��³���뼡ñ��ξ���
 * @param param [in] ���ϥѥ�᡼����
 * @param r [in] ǧ���������󥹥���
 * </JA>
 * <EN>
 * Connect a new word to generate a next hypothesis.  The optimal connection
 * point and new sentence score of the new hypothesis will be estimated by
 * looking up the corresponding words on word trellis.
 * 
 * @param now [in] source hypothesis
 * @param new [out] pointer to save the newly generated hypothesis
 * @param nword [in] next word to be connected
 * @param param [in] input parameter vector
 * @param r [in] recognition process instance
 * </EN>
 * @callgraph
 * @callergraph
 */
void
next_word(NODE *now, NODE *new, NEXTWORD *nword, HTK_Param *param, RecogProcess *r)
{
  HMM_Logical *lastphone, *newphone;
  LOGPROB *g_src;
  int   t;
  int lastword;
  int   i;
  LOGPROB a_value;
  LOGPROB tmpp;
  int   startt;
  int word;
  TRELLIS_ATOM *tre;
  LOGPROB totalscore;
  BACKTRELLIS *backtrellis;
  WORD_INFO *winfo;
  HTK_HMM_INFO *hmminfo;
  int peseqlen;
  boolean ccd_flag;
  StackDecode *dwrk;

  dwrk = &(r->pass2);
  backtrellis = r->backtrellis;
  winfo = r->lm->winfo;
  hmminfo = r->am->hmminfo;
  peseqlen = r->peseqlen;
  ccd_flag = r->ccd_flag;

  word = nword->id;
  lastword = now->seq[now->seqnum-1];

  /* lastphone (ľ��ñ�����Ƭ����) ����� */
  /* prepare lastphone (head phone of previous word) */
  if (ccd_flag) {
    /* �ǽ����� triphone ����³ñ��˲�碌���Ѳ� */
    /* modify triphone of last phone according to the next word */
    lastphone = get_left_context_HMM(now->last_ph, winfo->wseq[word][winfo->wlen[word]-1]->name, hmminfo);
    if (lastphone == NULL) {
      /* fallback to the original bi/mono-phone */
      /* error if the original is pseudo phone (not explicitly defined
	 in hmmdefs/hmmlist) */
      /* exception: word with 1 phone (triphone may exist in the next expansion */
      if (now->last_ph->is_pseudo){
	error_missing_left_triphone(now->last_ph, winfo->wseq[word][winfo->wlen[word]-1]->name);
      }
      lastphone = now->last_ph;
    }
  }

  /* newphone (��³ñ�����������) ����� */
  /* prepare newphone (tail phone of next word) */
  if (ccd_flag) {
    newphone = get_right_context_HMM(winfo->wseq[word][winfo->wlen[word]-1], now->last_ph->name, hmminfo);
    if (newphone == NULL) {
      /* fallback to the original bi/mono-phone */
      /* error if the original is pseudo phone (not explicitly defined
	 in hmmdefs/hmmlist) */
      /* exception: word with 1 phone (triphone may exist in the next expansion */
      if (winfo->wlen[word] > 1 && winfo->wseq[word][winfo->wlen[word]-1]->is_pseudo){
	error_missing_right_triphone(winfo->wseq[word][winfo->wlen[word]-1], now->last_ph->name);
      }
      newphone = winfo->wseq[word][winfo->wlen[word]-1];
    }
  } else {
    newphone = winfo->wseq[word][winfo->wlen[word]-1];
  }
  
  /* ñ���¤ӡ�DFA�����ֹ桢���쥹������ new �طѾ������� */
  /* inherit and update word sequence, DFA state and total LM score to 'new' */
  new->score = LOG_ZERO;
  for (i=0;i< now->seqnum;i++){
    new->seq[i] = now->seq[i];
#ifdef CM_SEARCH
#ifdef CM_MULTIPLE_ALPHA
    memcpy(new->cmscore[i], now->cmscore[i], sizeof(LOGPROB) * r->config->annotate.cm_alpha_num);
#else
    new->cmscore[i] = now->cmscore[i];
#endif
#endif /* CM_SEARCH */
  }
  new->seq[i] = word;
  new->seqnum = now->seqnum+1;
  new->state = nword->next_state;
  new->totallscore = now->totallscore + nword->lscore;
  if (ccd_flag) {
    /* ��������������Ȥ�����¸ */
    /* keep the lastphone for next scan_word() */
    new->last_ph = lastphone;
    new->last_ph_sp_attached = now->last_ph_sp_attached;
  }

  if (ccd_flag) {
    /* �Ǹ��1����(lastphone)ʬ��scan�������������������� new ����¸ */
    /* scan the lastphone and set the updated score to new->g[] */
    do_viterbi_next_word(now, new, lastphone,
			 hmminfo->multipath ? now->last_ph_sp_attached : FALSE,
			 param, r);
    g_src = new->g;
  } else {
    g_src = now->g;
#ifdef GRAPHOUT_PRECISE_BOUNDARY
    if (r->graphout) {
      memcpy(new->wordend_frame, now->wordend_frame, sizeof(short)*peseqlen);
      memcpy(new->wordend_gscore, now->wordend_gscore, sizeof(LOGPROB)*peseqlen);
    }
#endif
  }
      
  /* ����� scan_word �������� new->g[] ���ѹ����Ƥ��� */
  /* prepare new->g[] for next scan_word() */
  if (hmminfo->multipath) {
    startt = peseqlen-1;
  } else {
    startt = peseqlen-2;
  }
  i = hmm_logical_state_num(newphone);
  a_value = (hmm_logical_trans(newphone))->a[i-2][i-1];
  if (hmminfo->multipath) {
    for(t=0; t <= startt; t++) {
      new->g[t] = g_src[t] + nword->lscore;
    }
  } else {
    for(t=0; t <= startt; t++) {
      new->g[t] = g_src[t+1] + a_value + nword->lscore;
    }
  }

  /***************************************************************************/
  /* ������(�裲�ѥ�),������(�裱�ѥ�)�ȥ�ꥹ����³��������³���򸫤Ĥ��� */
  /* connect forward/backward trellis to look for the best connection time   */
  /***************************************************************************/
  /*-----------------------------------------------------------------*/
  /* ñ��ȥ�ꥹ��õ����, ��ñ��κ�����³����ȯ������ */
  /* determine the best connection time of the new word, seeking the word
     trellis */
  /*-----------------------------------------------------------------*/

  if (r->lmtype == LM_DFA && !r->config->pass2.looktrellis_flag) {
    /* ���٤ƤΥե졼��ˤ錄�äƺ����õ�� */
    /* search for best trellis word throughout all frame */
    for(t = startt; t >= 0; t--) {
      if (new->g[t] <= LOG_ZERO) continue;
      tre = bt_binsearch_atom(backtrellis, t, (WORD_ID) word);
      if (tre == NULL) continue;
      totalscore = new->g[t] + tre->backscore;
      if (! hmminfo->multipath) {
	if (newphone->is_pseudo) {
	  tmpp = outprob_cd(&(r->am->hmmwrk), t, &(newphone->body.pseudo->stateset[newphone->body.pseudo->state_num-2]), param);
	} else {
	  tmpp = outprob_state(&(r->am->hmmwrk), t, newphone->body.defined->s[newphone->body.defined->state_num-2], param);
	}
	totalscore += tmpp;
      }
      if (new->score < totalscore) {
	new->score = totalscore;
	new->bestt = t;
	new->estimated_next_t = tre->begintime - 1;
	new->tre = tre;
      }
    }

    return;
  }

  /* �Ǹ�˻��Ȥ���TRELLIS_ATOM�ν�ü���֤����� */
  /* new�ο�����֤ϡ��嵭�Ǻ��Ѥ���TRELLIS_ATOM�λ�ü���� */

  /* ����Ÿ��ñ��Υȥ�ꥹ��ν�ü���֤�����Τߥ�����󤹤�
     �����Ϣ³����¸�ߤ���ե졼��ˤĤ��ƤΤ߷׻� */
  /* search for best trellis word only around the estimated time */
  /* 1. search forward */
  for(t = (nword->tre)->endtime; t >= 0; t--) {
    if (new->g[t] <= LOG_ZERO) continue;
    tre = bt_binsearch_atom(backtrellis, t, (WORD_ID) word);
    if (tre == NULL) break;	/* go to 2 if the trellis word disappear */
    totalscore = new->g[t] + tre->backscore;
    if (! hmminfo->multipath) {
      if (newphone->is_pseudo) {
	tmpp = outprob_cd(&(r->am->hmmwrk), t, &(newphone->body.pseudo->stateset[newphone->body.pseudo->state_num-2]), param);
      } else {
	tmpp = outprob_state(&(r->am->hmmwrk), t, newphone->body.defined->s[newphone->body.defined->state_num-2], param);
      }
      totalscore += tmpp;
    }
    if (new->score < totalscore) {
      new->score = totalscore;
      new->bestt = t;
      new->estimated_next_t = tre->begintime - 1;
      new->tre = tre;
    }
  }
  /* 2. search bckward */
  for(t = (nword->tre)->endtime + 1; t <= startt; t++) {
    if (new->g[t] <= LOG_ZERO) continue;
    tre = bt_binsearch_atom(backtrellis, t, (WORD_ID) word);
    if (tre == NULL) break;	/* end if the trellis word disapper */
    totalscore = new->g[t] + tre->backscore;
    if (! hmminfo->multipath) {
      if (newphone->is_pseudo) {
	tmpp = outprob_cd(&(r->am->hmmwrk), t, &(newphone->body.pseudo->stateset[newphone->body.pseudo->state_num-2]), param);
      } else {
	tmpp = outprob_state(&(r->am->hmmwrk), t, newphone->body.defined->s[newphone->body.defined->state_num-2], param);
      }
      totalscore += tmpp;
    }
    if (new->score < totalscore) {
      new->score = totalscore;
      new->bestt = t;
      new->estimated_next_t = tre->begintime - 1;
      new->tre = tre;
    }
  }

  /* set current LM score */
  new->lscore = nword->lscore;
  
}


/**********************************************************************/
/********** ������������                 ****************************/
/********** Generate an initial hypothesis ****************************/
/**********************************************************************/

/** 
 * <JA>
 * Ϳ����줿ñ�줫�����������������. 
 * 
 * @param new [out] �������������줿���⤬��Ǽ�����
 * @param nword [in] �������ñ��ξ���
 * @param param [in] ���ϥѥ�᡼����
 * @param r [in] ǧ���������󥹥���
 * </JA>
 * <EN>
 * Generate an initial hypothesis from given word.
 * 
 * @param new [out] pointer to save the newly generated hypothesis
 * @param nword [in] words of the first candidates
 * @param param [in] input parameter vector
 * @param r [in] recognition process instance
 *
 * </EN>
 * @callgraph
 * @callergraph
 */
void
start_word(NODE *new, NEXTWORD *nword, HTK_Param *param, RecogProcess *r)
{
  HMM_Logical *newphone;
  WORD_ID word;
  TRELLIS_ATOM *tre = NULL;
  LOGPROB tmpp;
  int t;

  BACKTRELLIS *backtrellis;
  WORD_INFO *winfo;

  int peseqlen;
  boolean ccd_flag;
  boolean multipath;

  backtrellis = r->backtrellis;
  winfo = r->lm->winfo;
  peseqlen = r->peseqlen;
  ccd_flag = r->ccd_flag;
  multipath = r->am->hmminfo->multipath;

  /* initialize data */
  word = nword->id;
  new->score = LOG_ZERO;
  new->seqnum = 1;
  new->seq[0] = word;

  new->state = nword->next_state;
  new->totallscore = nword->lscore;

  /* set current LM score */
  new->lscore = nword->lscore;

  /* cross-word triphone need not be handled on startup */
  newphone = winfo->wseq[word][winfo->wlen[word]-1];
  if (ccd_flag) {
    new->last_ph = NULL;
  }
  
  new->g[peseqlen-1] = nword->lscore;
  
  for (t=peseqlen-1; t>=0; t--) {
    tre = bt_binsearch_atom(backtrellis, t, word);
    if (tre != NULL) {
      if (r->graphout) {
	new->bestt = peseqlen-1;
      } else {
	new->bestt = t;
      }
      new->score = new->g[peseqlen-1] + tre->backscore;
      if (! multipath) {
	if (newphone->is_pseudo) {
	  tmpp = outprob_cd(&(r->am->hmmwrk), peseqlen-1, &(newphone->body.pseudo->stateset[newphone->body.pseudo->state_num-2]), param);
	} else {
	  tmpp = outprob_state(&(r->am->hmmwrk), peseqlen-1, newphone->body.defined->s[newphone->body.defined->state_num-2], param);
	}
	new->score += tmpp;
      }
      new->estimated_next_t = tre->begintime - 1;
      new->tre = tre;
      break;
    }
  }
  if (tre == NULL) {		/* no word in backtrellis */
    new->score = LOG_ZERO;
  }
}

/** 
 * <JA>
 * ��ü��������ü�ޤ�ã����ʸ����κǽ�Ū�ʥ������򥻥åȤ���. 
 * 
 * @param now [in] ��ü�ޤ�ã��������
 * @param new [out] �ǽ�Ū��ʸ����Υ��������Ǽ������ؤΥݥ���
 * @param param [in] ���ϥѥ�᡼����
 * @param r [in] ǧ���������󥹥���
 * </JA>
 * <EN>
 * Hypothesis termination: set the final sentence scores of hypothesis
 * that has already reached to the end.
 * 
 * @param now [in] hypothesis that has already reached to the end
 * @param new [out] pointer to save the final sentence information
 * @param param [in] input parameter vectors
 * @param r [in] recognition process instance
 * </EN>
 * @callgraph
 * @callergraph
 */
void
last_next_word(NODE *now, NODE *new, HTK_Param *param, RecogProcess *r)
{
  cpy_node(new, now);
  if (r->ccd_flag) {
    /* �ǽ�����ʬ�� viterbi ���ƺǽ������������� */
    /* scan the last phone and update the final score */
    if (r->am->hmminfo->multipath) {
      do_viterbi_next_word(now, new, now->last_ph, now->last_ph_sp_attached, param, r);
      new->score = new->final_g;
    } else {
      do_viterbi_next_word(now, new, now->last_ph, FALSE, param, r);
      new->score = new->g[0];
    }
  } else {
    if (r->am->hmminfo->multipath) {
      new->score = now->final_g;
    } else {
      new->score = now->g[0];
    }
#ifdef GRAPHOUT_PRECISE_BOUNDARY
    if (r->graphout) {
      /* last boundary has moved to [peseqlen-1] in last scan_word() */
      memcpy(new->wordend_frame, now->wordend_frame, sizeof(short)*r->peseqlen);
      memcpy(new->wordend_gscore, now->wordend_gscore, sizeof(LOGPROB)*r->peseqlen);
    }
#endif
  }
}

#endif /* PASS2_STRICT_IWCD */

/* end of file */
