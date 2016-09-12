/**
 * @file   gprune_safe.c
 * 
 * <JA>
 * @brief  ���祬����ʬ�۷׻�: Gaussian pruning (safe algorithm)
 *
 * gprune_safe()�Ϻ��祬����ʬ�۽���η׻��롼����ΰ�ĤǤ���
 * safe pruning ��Ȥäƾ�̤Υ�����ʬ�ۤν��ϳ�Ψ�Τߤ��®�˵��ޤ���
 * Tied-mixture %HMM ���ѻ��� Julius ��GPRUNE_DEFAULT_SAFE ���������Ƥ��뤫��
 * ���뤤��Julius�Υ��ץ���� "-gprune safe" ����ꤹ�뤳�ȤǤ��δؿ���
 * ���Ѥ���ޤ���
 *
 * safe pruning �ϺǤ�����ʻ޴���ˡ�Ǥ������N�ĤΥ�����ʬ�ۤ��μ¤�
 * �����ޤ�������®���θ��̤�¾�μ�ˡ����٤ƾ������Ǥ���
 * 
 * gprune_safe() �� outprob_init() �ˤ�äƤ��δؿ��ؤΥݥ��󥿤�
 * compute_gaussset �˥��åȤ���뤳�Ȥǻ��Ѥ���ޤ������Υݥ��󥿤�
 * calc_tied_mix() �ޤ��� calc_mix() ����ƤӽФ���ޤ���
 * </JA>
 * 
 * <EN>
 * @brief  Calculate probability of a set of Gaussian densities by
 * Gaussian pruning: safe algorithm
 *
 * gprune_safe() is one of the functions to compute output probability of
 * a set of Gaussian densities.  This function does safe pruning, trying
 * to compute only the best ones for faster computation.  If a tied-mixture
 * %HMM model with GPRUNE_DEFAULT_SAFE defined in Julius, or explicitly
 * specified by "-gprune safe" option, this function will be used.
 *
 * The safe pruning is the most safe method that can find the exact N-best
 * Gaussians, but the efficiency is smaller.
 *
 * gprune_safe() will be used by calling outprob_init() to set its pointer
 * to the global variable @a compute_gaussset.  Then it will be called from
 * calc_tied_mix() or calc_mix().
 * </EN>
 * 
 * @author Akinobu LEE
 * @date   Thu Feb 17 05:28:12 2005
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

/* gprune_safe.c --- calculate probability of Gaussian densities */
/*                   with Gaussian pruning (safe) */

/* $Id: gprune_safe.c,v 1.7 2013/06/20 17:14:25 sumomo Exp $ */

#include <sent/stddefs.h>
#include <sent/htk_hmm.h>
#include <sent/htk_param.h>
#include <sent/hmm.h>
#include <sent/hmm_calc.h>

/** 
 * @brief  Calculate probability with safe pruning.
 * 
 * Calculate probability of a Gaussian toward OP_vec,
 * performing pruning using the scholar threshold.
 * 
 * @param wrk [i/o] HMM computation work area
 * @param binfo [in] Gaussian density
 * @param thres [in] threshold
 * 
 * @return the output log probability.
 */
LOGPROB
compute_g_safe(HMMWork *wrk, HTK_HMM_Dens *binfo, LOGPROB thres)
{
  VECT tmp, x;
  VECT *mean;
  VECT *var;
  VECT *vec = wrk->OP_vec;
  short veclen = wrk->OP_veclen;
  VECT fthres = thres * (-2.0);

  if (binfo == NULL) return(LOG_ZERO);
  mean = binfo->mean;
  var = binfo->var->vec;
  tmp = binfo->gconst;
  for (; veclen > 0; veclen--) {
    x = *(vec++) - *(mean++);
    tmp += x * x * *(var++);
    if (tmp > fthres)  return LOG_ZERO;
  }
  return(tmp * -0.5);
}



/** 
 * Initialize and setup work area for Gaussian pruning by safe algorithm.
 * 
 * @param wrk [i/o] HMM computation work area
 * 
 * @return TRUE on success, FALSE on failure.
 */
boolean
gprune_safe_init(HMMWork *wrk)
{
  int i;

  /* maximum Gaussian set size = maximum mixture size * nstream */
  wrk->OP_calced_maxnum = wrk->OP_hmminfo->maxmixturenum * wrk->OP_nstream;
  wrk->OP_calced_score = (LOGPROB *)mymalloc(sizeof(LOGPROB) * wrk->OP_calced_maxnum);
  wrk->OP_calced_id = (int *)mymalloc(sizeof(int) * wrk->OP_calced_maxnum);
  wrk->mixcalced = (boolean *)mymalloc(sizeof(int) * wrk->OP_calced_maxnum);
  for(i=0;i<wrk->OP_calced_maxnum;i++) wrk->mixcalced[i] = FALSE;
  return TRUE;
}

/**
 * Free gprune_safe related work area.
 * 
 * @param wrk [i/o] HMM computation work area
 * 
 */
void
gprune_safe_free(HMMWork *wrk)
{
  free(wrk->OP_calced_score);
  free(wrk->OP_calced_id);
  free(wrk->mixcalced);
}

/** 
 * @brief  Compute a set of Gaussians with safe pruning.
 *
 * If the N-best mixtures in the previous frame is specified in @a last_id,
 * They are first computed to set the initial threshold.
 * After that, the rest of the Gaussians will be computed with the thresholds
 * to drop unpromising Gaussians from computation at early stage
 * of likelihood computation.  If the computation of a Gaussian reached to
 * the end, the threshold will be updated to always hold the likelihood of
 * current N-best score.
 *
 * The calculated scores will be stored to OP_calced_score, with its
 * corresponding mixture id to OP_calced_id.  These are done by calling
 * cache_push().
 * The number of calculated mixtures is also stored in OP_calced_num.
 * 
 * This can be called from calc_tied_mix() or calc_mix().
 * 
 * @param wrk [i/o] HMM computation work area
 * @param g [in] set of Gaussian densities to compute the output probability
 * @param gnum [in] length of above
 * @param last_id [in] ID list of N-best mixture in previous input frame,
 * or NULL if not exist
 * @param lnum [in] length of last_id
 */
void
gprune_safe(HMMWork *wrk, HTK_HMM_Dens **g, int gnum, int *last_id, int lnum)
{
  int i, j, num = 0;
  LOGPROB score, thres;

  if (last_id != NULL) {	/* compute them first to form threshold */
    /* 1. calculate first $OP_gprune_num and set initial threshold */
    for (j=0; j<lnum; j++) {
      i = last_id[j];
      score = compute_g_base(wrk, g[i]);
      num = cache_push(wrk, i, score, num);
      wrk->mixcalced[i] = TRUE;      /* mark them as calculated */
    }
    thres = wrk->OP_calced_score[num-1];
    /* 2. calculate the rest with pruning*/
    for (i = 0; i < gnum; i++) {
      /* skip calced ones in 1. */
      if (wrk->mixcalced[i]) {
        wrk->mixcalced[i] = FALSE;
        continue;
      }
      /* compute with safe pruning */
      score = compute_g_safe(wrk, g[i], thres);
      if (score <= thres) continue;
      num = cache_push(wrk, i, score, num);
      thres = wrk->OP_calced_score[num-1];
    }
  } else {			/* in case the last_id not available */
    /* not tied-mixture, or at the first 0 frame */
    thres = LOG_ZERO;
    for (i = 0; i < gnum; i++) {
      if (num < wrk->OP_gprune_num) {
	score = compute_g_base(wrk, g[i]);
      } else {
	score = compute_g_safe(wrk, g[i], thres);
	if (score <= thres) continue;
      }
      num = cache_push(wrk, i, score, num);
      thres = wrk->OP_calced_score[num-1];
    }
  }
  wrk->OP_calced_num = num;
}
