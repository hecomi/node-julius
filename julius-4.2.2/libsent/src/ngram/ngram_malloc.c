/**
 * @file   ngram_malloc.c
 * 
 * <JA>
 * @brief  N-gram構造体の初期メモリ確保と解放
 * </JA>
 * 
 * <EN>
 * @brief  Initial memory allocation and free for N-gram stucture
 * </EN>
 * 
 * @author Akinobu LEE
 * @date   Wed Feb 16 16:48:56 2005
 *
 * $Revision: 1.7 $
 * 
 */
/*
 * Copyright (c) 1991-2012 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2012 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#include <sent/stddefs.h>
#include <sent/ngram2.h>

/** 
 * Allocate a new N-gram structure.
 * 
 * @return pointer to the newly allocated structure.
 */
NGRAM_INFO *
ngram_info_new()
{
  NGRAM_INFO *new;

  new = (NGRAM_INFO *)mymalloc(sizeof(NGRAM_INFO));
  new->n = 0;
  new->d = NULL;
  new->bo_wt_1 = NULL;
  new->p_2 = NULL;
  new->bos_eos_swap = FALSE;
  new->mroot = NULL;

  return(new);
}

static void
free_ngram_tuple(NGRAM_TUPLE_INFO *t)
{
  if (t->is24bit) {
    if (t->bgn_upper) free(t->bgn_upper);
    if (t->bgn_lower) free(t->bgn_lower);
  } else {
    if (t->bgn) free(t->bgn);
  }
  if (t->num) free(t->num);
  if (t->nnid2wid) free(t->nnid2wid);
  free(t->prob);
  if (t->bo_wt) free(t->bo_wt);
  if (t->nnid2ctid_upper) free(t->nnid2ctid_upper);
  if (t->nnid2ctid_lower) free(t->nnid2ctid_lower);
}
/** 
 * Free N-gram data.
 * 
 * @param ndata [in] N-gram data
 */
void
ngram_info_free(NGRAM_INFO *ndata)
{
  int i;
  /* bin test only */
  /* free word names */
  if (ndata->from_bin) {
    free(ndata->wname[0]);
    free(ndata->wname);
  } else {
    WORD_ID w;
    for(w=0;w<ndata->max_word_num;w++) {
      free(ndata->wname[w]);
    }
    free(ndata->wname);
  }
  /* free 2-gram for the 1st pass */
  if (ndata->bo_wt_1) free(ndata->bo_wt_1);
  if (ndata->p_2) free(ndata->p_2);
  /* free n-gram */
  if (ndata->d) {
    for(i=0;i<ndata->n;i++) {
      free_ngram_tuple(&(ndata->d[i]));
    }
    free(ndata->d);
  }
  /* free name index tree */
  if (ndata->mroot) mybfree2(&(ndata->mroot));
  /* free whole */
  free(ndata);
}
