/**
 * @file   spsegment.c
 * 
 * <EN>
 * @brief  Short-pause segmentation and decoder-based VAD
 *
 * In short-pause segmentation mode, Julius tries to find a "pause
 * frame" by watching the word hypotheses at each frame.  Julius treat
 * words with only a silence model as "pause word", and judge whether
 * the input frame is "pause frame" or not by watching if any of the
 * pause words gets maximum score at each frame.  Then it will segment the
 * input when the duration of pause frame reaches a limit.
 *
 * On normal short-pause segmentation (as of ver.3.x), the pause
 * frames will not be eliminated.  The input will be segment at the
 * frame where a speech begins after the pause frames, and the next
 * input will be processed from the beginning of the pause frames.  In
 * other words, the detected area of pause frames are processed twice,
 * as end-of-segment silence at the former input segment and
 * beginning-of-segment silence at the latter input segment.
 *
 * When SPSEGMENT_NAIST is defined, a long pause area will be dropped
 * from recognition.  When the detecting pause frames gets longer than
 * threshold, it segments the input at that point and skip the continuing
 * pauses until a speech frame comes.  The recognition process will
 * be kept with a special status while in the pause segment.  This scheme
 * works as a decoder-driven VAD.
 * 
 * </EN>
 * 
 * <JA>
 * @brief  ���硼�ȥݡ����������ơ�����󤪤�ӥǥ������١���VAD
 *
 * ���硼�ȥݡ����������ơ������Ǥϡ���1�ѥ��ˤ����ơ�̵��ñ��פ�
 * ��������ե졼�ऴ�Ȥ�Ĵ�١����줬��̤Ǥ���ե졼����̵���ե졼���
 * �Ȥ��ޤ�. �����ơ�̵���ե졼�ब����ʾ�Υե졼����ˤ錄�ä��Ȥ��ˡ�
 * ���Ϥ򤽤��Ƕ��ڤ�ޤ�. 
 *
 * ��̵��ñ��פϡ�ñ�켭��ˤ����ơ��ɤߤ�̵�����б����룱��ǥ�Τߤ���
 * �ʤ�ñ���ؤ��ޤ�. ̵����ǥ�� -spmodel �ǻ��ꤵ����ǥ롤�����
 * N-gram ��ǥ���ѻ�����Ƭ��������̵����ǥ�Ȥ���ޤ�������Ū�˻���
 * ����ˤ� -pausemodels ���ץ�������Ѥ��ޤ���
 *
 * �̾�Υ��硼�ȥݡ����������ơ������(Ver.3.x ������Ʊ��)�Ǥϡ�̵
 * ����֤ν���ϹԤ��ޤ���. ���Ϥϡ�̵���ե졼���֤���λ���Ƥդ�
 * ���Ӳ������ȥꥬ���������Ƕ��ڤ�졤���������Ȥ�ǧ���Ϥ���̵����
 * �졼���֤γ���������Ƴ�����ޤ�. ���ʤ�������Ф��줿̵����֤ϡ�
 * ���������Ȥ�������̵����֤��ļ��������Ȥγ��Ϥ�̵����֤Ȥ��ơ�
 * �������ȴ֤ǥ����С���åפ��ƽ�������ޤ�. 
 *
 * SPSEGMENT_NAIST ������ϡ�̵���ե졼���֤�Ĺ�����Ϥ����Ǥ��ä���
 * ���Ϥ���ڤꡤ�������ϺƳ��ޤǤδ֤�̵����֤򥹥��åפ���褦�ˤʤ�ޤ�. 
 * ̵�������⡤������������ʤ����̤�ǧ�����֤����뤳�Ȥǡ�
 * ǧ�����֤��ݤ��ޤ�. ����ˤ�äơ����̵�����֤�Ĺ���������ꤷ����
 * �ǥ������١����� VAD ��Ԥ����Ȥ�����ޤ�. 
 * </JA>
 * 
 * @author Akinobu Lee
 * @date   Wed Oct 17 12:47:29 2007
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
 * @brief  ���硼�ȥݡ���ñ�줫�ɤ���Ƚ��
 *
 * Ϳ����줿ñ�줬���硼�ȥݡ���ñ��Ǥ��뤫�ɤ���Ĵ�٤�. 
 *
 * @param w [in] ñ��ID
 * @param r [in] ����ǧ���������󥹥���
 * 
 * @return ���硼�ȥݡ���ñ��Ǥ���� TRUE�������Ǥʤ���� FALSE. 
 * </JA>
 * <EN>
 * Check if the fiven word is a short-pause word.
 *
 * @param w [in] word id
 * @param r [in] recognition process instance
 * 
 * @return TRUE if it is short pause word, FALSE if not.
 * </EN>
 * 
 * @callgraph
 * @callergraph
 */
boolean
is_sil(WORD_ID w, RecogProcess *r)
{
  WORD_INFO *winfo;
  HTK_HMM_INFO *hmm;
  int i;

  winfo = r->lm->winfo;
  hmm = r->am->hmminfo;

  /* num of phones should be 1 */
  if (winfo->wlen[w] > 1) return FALSE;

  if (r->pass1.pausemodel) {
    /* has pause model list */
    for(i=0;i<r->pass1.pausemodelnum;i++) {
      if (strmatch(winfo->wseq[w][0]->name, r->pass1.pausemodel[i])) {
        return TRUE;
      }
    }
  } else {
    /* short pause (specified by "-spmodel") */
    if (winfo->wseq[w][0] == hmm->sp) return TRUE;
    
    if (r->lmtype == LM_PROB) {
      /* head/tail sil */
      if (w == winfo->head_silwid || w == winfo->tail_silwid) return TRUE;
    }
  }

  return FALSE;
}

/** 
 * <EN>
 * @brief  Split input parameter for segmentation.
 * 
 * Copy the rest samples in param to rest_param, and shrink the param
 * in mfcc instance.  [start...param->samplenum] will be copied to
 * rest_param, and [0...end] will be left in param.
 * </EN>
 * <JA>
 * @brief  �������ơ������������ϥѥ�᡼����ʬ�䤹��. 
 * 
 * �Ĥ�Υ���ץ�ʸ��ߤΥե졼�फ�齪���ޤǡˤ� rest_param ��
 * ���ԡ��������� param ��û������. [start...param->samplenum] ��
 * rest_param �˥��ԡ����졤���� param �ˤ� [0...end] ���Ĥ�. 
 * </JA>
 * 
 * @param mfcc [i/o] MFCC calculation instance
 * @param start [in] copy start frame
 * @param end [in] original end frame
 * 
 * @callgraph
 * @callergraph
 */
void
mfcc_copy_to_rest_and_shrink(MFCCCalc *mfcc, int start, int end)
{
  int t;

  /* copy rest parameters for next process */
  mfcc->rest_param = new_param();
  memcpy(&(mfcc->rest_param->header), &(mfcc->param->header), sizeof(HTK_Param_Header));
  mfcc->rest_param->samplenum = mfcc->param->samplenum - start;
  mfcc->rest_param->header.samplenum = mfcc->rest_param->samplenum;
  mfcc->rest_param->veclen = mfcc->param->veclen;
  mfcc->rest_param->is_outprob = mfcc->param->is_outprob;
  if (param_alloc(mfcc->rest_param, mfcc->rest_param->samplenum, mfcc->rest_param->veclen) == FALSE) {
    j_internal_error("ERROR: segmented: failed to allocate memory for rest param\n");
  }
  /* copy data */
  for(t=start;t<mfcc->param->samplenum;t++) {
    memcpy(mfcc->rest_param->parvec[t-start], mfcc->param->parvec[t], sizeof(VECT) * mfcc->rest_param->veclen);
  }

  /* shrink original param */
  /* just shrink the length */
  mfcc->param->samplenum = end;
  mfcc->param->header.samplenum = end;
}

/** 
 * <EN>
 * Shrink the parameter sequence.  Drop the first (p-1) frames and
 * move [p..samplenum] to 0.
 * </EN>
 * <JA>
 * �ѥ�᡼����û������. �ǽ�� (p-1) �ե졼���ä��ơ�[p..samplenum]
 * �Υ���ץ��ǽ�˵ͤ��. 
 * </JA>
 * 
 * @param mfcc [i/o] MFCC Calculation instance
 * @param p [in] frame point to remain
 * 
 * @callgraph
 * @callergraph
 */
void
mfcc_shrink(MFCCCalc *mfcc, int p)
{
  int t;
  int len;

  if (p > 0) {
    /* copy data */
    for(t=p;t<mfcc->param->samplenum;t++) {
      memcpy(mfcc->param->parvec[t-p], mfcc->param->parvec[t], sizeof(VECT) * mfcc->param->veclen);
    }
    /* shrink original param */
    /* just shrink the length */
    len = mfcc->param->samplenum - p;
    mfcc->param->samplenum = len;
    mfcc->param->header.samplenum = len;
  }
}

/** 
 * <JA>
 * @brief  ȯ�ö�ֽ�λ�θ���
 * 
 * ���硼�ȥݡ����������ơ������������
 * ȯ�ö�֤ν�λ�򸡽Ф���. ̵��ñ�줬Ϣ³���ƺ������Ȥʤ�ե졼�����
 * ������Ȥ���������ֻ�³��ˤդ����Ӳ������ȥꥬ�������������Ϥ�
 * ���ڤ�. 
 *
 * SPSEGMENT_NAIST ������ϡ���ꥻ���������塦�֤�̵�����֤�Ĺ������
 * ���ꤷ���ǥ������١����� VAD ���ڤ��ؤ��. ���ξ�硤�����ȥꥬ������
 * (r->pass1.after_triger == FALSE)�Ǥϡ�������������ʤ����֤�ǧ��������
 * ³����. �������Ϥ򸡽Ф�������ħ�̤����Ĺ (r->config->successive.sp_margin)
 * ʬ���������ᤷ�ơ��̾��ǧ���򳫻Ϥ���(r->pass1.after_trigger == TRUE). 
 * �̾��ǧ�����̵����֤�Ĺ�� (r->config->successive.sp_frame_duration �ʾ�)
 * ³�����顤���������Ϥ���ڤ�. 
 * 
 * @param r [i/o] ����ǧ���������󥹥���
 * @param time [in] ���ߤ����ϥե졼��
 * 
 * @return TRUE (���Υե졼��Ǥν�λ�򸡽Ф�����), FALSE (��λ�Ǥʤ����)
 * </JA>
 * <EN>
 * @brief  Speech end point detection.
 * 
 * Detect end-of-input by duration of short-pause words when short-pause
 * segmentation is enabled.  When a pause word gets maximum score for a
 * successive frames, the segment will be treated as a pause frames.
 * When speech re-triggers, the current input will be segmented at that point.
 *
 * When SPSEGMENT_NAIST is defined, this function performs extended version
 * of the short pause segmentation, called "decoder-based VAD".  When before
 * speech trigger (r->pass1.after_trigger == FALSE), it tells the recognition
 * functions not to generate word trellis and continue calculation.  If a
 * speech trigger is found (not a pause word gets maximum score), the
 * input frames are 'rewinded' for a certain frame
 * (r->config->successive.sp_margin) and start the normal recognition
 * process from the rewinded frames (r->pass1.after_trigger = TRUE).
 * When a pause frame duration reaches a limit
 * (r->config->successive.sp_frame_duration), it terminate the search.
 * 
 * @param r [i/o] recognition process instance
 * @param time [in] current input frame
 * 
 * @return TRUE if end-of-input detected at this frame, FALSE if not.
 * </EN>
 * @callgraph
 * @callergraph
 */
boolean
detect_end_of_segment(RecogProcess *r, int time)
{
  FSBeam *d;
  TRELLIS_ATOM *tre;
  LOGPROB maxscore = LOG_ZERO;
  TRELLIS_ATOM *tremax = NULL;
  int count = 0;
  boolean detected = FALSE;
#ifdef SPSEGMENT_NAIST
  MFCCCalc *mfcc;
  WORD_ID wid;
  int j;
  TOKEN2 *tk;
  int startframe;
#endif

  d = &(r->pass1);

#ifdef SPSEGMENT_NAIST

  if (! d->after_trigger) {
    /* we are in the first long pause segment before trigger */

    /* find word end of maximum score from beam status */
    for (j = d->n_start; j <= d->n_end; j++) {
      tk = &(d->tlist[d->tn][d->tindex[d->tn][j]]);
      if (r->wchmm->stend[tk->node] != WORD_INVALID) {
        if (maxscore < tk->score) {
          maxscore = tk->score;
          wid = r->wchmm->stend[tk->node];
        }
      }
    }
    if (maxscore == LOG_ZERO) detected = TRUE;
    else if (is_sil(wid, r)) detected = TRUE;
 
    if (detected) {
      /***********************/
      /* this is noise frame */
      /***********************/

      /* reset trigger duration */
      d->trigger_duration = 0;
      
      /* if noise goes more than a certain frame, shrink the noise area
         to avoid unlimited memory usage */
      if (r->am->mfcc->f > SPSEGMENT_NAIST_AUTOSHRINK_LIMIT) {
        d->want_rewind = TRUE;
        d->rewind_frame = r->am->mfcc->f - r->config->successive.sp_margin;
        d->want_rewind_reprocess = FALSE;
        if (debug2_flag) {
          jlog("DEBUG: pause exceeded %d, rewind\n", SPSEGMENT_NAIST_AUTOSHRINK_LIMIT);
        }
        return FALSE;
      }

      /* keep going */
      d->want_rewind = FALSE;

    } else {
      /************************/
      /* this is speech frame */
      /************************/

      /* increment trigger duration */
      d->trigger_duration++;
      
      /* if not enough duration, not treat as up trigger */
      if (d->trigger_duration < r->config->successive.sp_delay) {
        /* just continue detection */
        return FALSE;
      }

      /***************************/
      /* found speech up-trigger */
      /***************************/
      /* set backstep point */
      if (r->am->mfcc->f < r->config->successive.sp_margin) {
        startframe = 0;
      } else {
        startframe = r->am->mfcc->f - r->config->successive.sp_margin;
      }
      if (debug2_flag) {
        jlog("DEBUG: speech triggered\n");
        jlog("DEBUG: word=[%s] dur=%d\n", r->lm->winfo->woutput[wid], d->trigger_duration);
        jlog("DEBUG: backstep behind %d (from %d to %d) frame and start process\n", r->config->successive.sp_margin, r->am->mfcc->f, startframe);
      }

      /* if the pause segment was short, keep the context of last segment.
         else, reset the context */
      if (r->lmtype == LM_PROB) {
        if (startframe > 0) {
          r->sp_break_last_word = WORD_INVALID;
        }
      }

      /* reset sp duration */
      d->sp_duration = 0;

      /* request the caller to rewind the search to the backstep point and
         re-start with normal search */
      d->want_rewind = TRUE;
      d->rewind_frame = startframe;
      d->want_rewind_reprocess = TRUE;
      /* this will enter to normal search in the next processing */
      d->after_trigger = TRUE;
    }
    /* tell the caller not to segment */
    return FALSE;
  }

#endif /* SPSEGMENT_NAIST */

  /* look for the best trellis word on the given time frame */
  for(tre = r->backtrellis->list; tre != NULL && tre->endtime == time; tre = tre->next) {
    if (maxscore < tre->backscore) {
      maxscore = tre->backscore;
      tremax = tre;
    }
    count++;
  }
  if (tremax == NULL) { /* no word end: possible in the very beggining of input*/
    detected = TRUE;            /* assume it's in the short-pause duration */
  } else if (count > 0) {       /* many words found --- check if maximum is sp */
    if (is_sil(tremax->wid, r)) {
      detected = TRUE;
    }
  }


#ifdef SPSEGMENT_NAIST
  /************************************************************************/
  /************************************************************************/

  /* detected = TRUE if noise frame, or FALSE if speech frame */

  /* sp��ֻ�³�����å� */
  /* check sp segment duration */
  if (d->first_sparea) {
    /* we are in the first sp segment */
    if (d->in_sparea && detected) {
      /* sp continues */
      d->sp_duration++;
      /* when sp continues more than -spdur plus -spmargin,
	 it means that although a speech trigger has been detected
	 by some reason, no actual speech has been found at first. */
      /* in this case we force trigger to end this input */
      if (d->sp_duration > r->config->successive.sp_delay + r->config->successive.sp_margin + r->config->successive.sp_frame_duration) {
	d->in_sparea = FALSE;
	d->first_sparea = FALSE;
	if (debug2_flag) {
	  jlog("DEBUG: no valid speech starts, force trigger at %d\n", r->am->mfcc->f);
	}
      }
    } else if (d->in_sparea && !detected) {
      /* found speech frame */
      d->in_sparea = FALSE;
      d->first_sparea = FALSE;
      if (debug2_flag) {
        jlog("DEBUG: speech segment start at %d\n", r->am->mfcc->f);
      }
    }
  } else {
    /* we are either in speech segment, or trailing sp segment */
    if (!d->in_sparea) {
      /* we are in speech segment */
      if (detected) {
        /* detected end of speech segment (begin of sp segment) */
        /* ���Ū�˳��ϥե졼��Ȥ��ƥޡ��� */
        /* mark this frame as "temporal" begging of short-pause segment */
        d->tmp_sparea_start = time;
#ifdef SP_BREAK_RESUME_WORD_BEGIN
        if (r->lmtype == LM_PROB) {
          /* sp ��ֳ��ϻ����κ���ñ�����¸ */
          /* store the best word in this frame as resuming word */
          d->tmp_sp_break_last_word = tremax ? tremax->wid : WORD_INVALID;
        }
#endif
        d->in_sparea = TRUE;
        d->sp_duration = 1;
      } else {
        /* speech continues */
        /* keep recognizing */
      }
    } else {
      /* we are in trailing sp segment */
      if (detected) {
        /* short pause frame continues */
        d->sp_duration++;
        /* keep word as the "beggining" of next sp segment */
        if (r->lmtype == LM_PROB) {
#ifdef SP_BREAK_RESUME_WORD_BEGIN
          /* if this segment has triggered by (tremax == NULL) (in case the first
             several frame of input), the sp word (to be used as resuming
             word in the next segment) is not yet set. it will be detected here */
          if (d->tmp_sp_break_last_word == WORD_INVALID) {
            if (tremax != NULL) d->tmp_sp_break_last_word = tremax->wid;
          }
#else
          /* resume word at the "end" of sp segment */
          /* simply update the best sp word */
          if (tremax != NULL) d->last_tre_word = tremax->wid;
#endif
        }

        if (d->sp_duration >= r->config->successive.sp_frame_duration) {
          /* silence over, segment the recognition here */
          /* store begging frame of the segment */
          //d->sparea_start = d->tmp_sparea_start;
          r->am->mfcc->sparea_start = time - r->config->successive.sp_frame_duration;
          if (r->lmtype == LM_PROB) {
#ifdef SP_BREAK_RESUME_WORD_BEGIN
            /* resume word = most likely sp word on beginning frame of the segment */
            r->sp_break_last_word = d->tmp_sp_break_last_word;
#else
            /* resume word = most likely sp word on end frame of the segment */
            r->sp_break_last_word = d->last_tre_word;
#endif
          }

          if (debug2_flag) {
            jlog("DEBUG: trailing silence end, end this segment at %d\n", r->am->mfcc->f);
          }
          
          d->after_trigger = FALSE;
          d->trigger_duration = 0;
          d->want_rewind = FALSE;

          /*** segment: [sparea_start - time-1] ***/
          return(TRUE);
        }
        /* else, keep recognition */
      } else {
        /* speech re-triggered */
        /* keep recognition */
        d->in_sparea = FALSE;
      }
    }
  }

  d->want_rewind = FALSE;


#else  /* ~SPSEGMENT_NAIST */
  /************************************************************************/
  /************************************************************************/

  /* sp��ֻ�³�����å� */
  /* check sp segment duration */
  if (d->in_sparea && detected) {       /* we are already in sp segment and sp continues */
    d->sp_duration++;           /* increment count */
#ifdef SP_BREAK_RESUME_WORD_BEGIN
    /* resume word at the "beggining" of sp segment */
    /* if this segment has triggered by (tremax == NULL) (in case the first
       several frame of input), the sp word (to be used as resuming
       word in the next segment) is not yet set. it will be detected here */
    if (d->tmp_sp_break_last_word == WORD_INVALID) {
      if (tremax != NULL) d->tmp_sp_break_last_word = tremax->wid;
    }
#else
    /* resume word at the "end" of sp segment */
    /* simply update the best sp word */
    if (tremax != NULL) d->last_tre_word = tremax->wid;
#endif
  }

  /* sp��ֳ��ϥ����å� */
  /* check if sp segment begins at this frame */
  else if (!d->in_sparea && detected) {
    /* ���Ū�˳��ϥե졼��Ȥ��ƥޡ��� */
    /* mark this frame as "temporal" begging of short-pause segment */
    d->tmp_sparea_start = time;
#ifdef SP_BREAK_RESUME_WORD_BEGIN
    /* sp ��ֳ��ϻ����κ���ñ�����¸ */
    /* store the best word in this frame as resuming word */
    d->tmp_sp_break_last_word = tremax ? tremax->wid : WORD_INVALID;
#endif
    d->in_sparea = TRUE;                /* yes, we are in sp segment */
    d->sp_duration = 1;         /* initialize duration count */
#ifdef SP_BREAK_DEBUG
    jlog("DEBUG: sp start %d\n", time);
#endif /* SP_BREAK_DEBUG */
  }
  
  /* sp ��ֽ�λ�����å� */
  /* check if sp segment ends at this frame */
  else if (d->in_sparea && !detected) {
    /* (time-1) is end frame of pause segment */
    d->in_sparea = FALSE;               /* we are not in sp segment */
#ifdef SP_BREAK_DEBUG
    jlog("DEBUG: sp end %d\n", time);
#endif /* SP_BREAK_DEBUG */
    /* sp ���Ĺ�����å� */
    /* check length of the duration*/
    if (d->sp_duration < r->config->successive.sp_frame_duration) {
      /* û������: �裱�ѥ������Ǥ���³�� */
      /* too short segment: not break, continue 1st pass */
#ifdef SP_BREAK_DEBUG
      jlog("DEBUG: too short (%d<%d), ignored\n", d->sp_duration, r->config->successive.sp_frame_duration);
#endif /* SP_BREAK_DEBUG */
    } else if (d->first_sparea) {
      /* �ǽ��sp��֤� silB �ˤ�����Τ�,�裱�ѥ������Ǥ���³�� */
      /* do not break at first sp segment: they are silB */
      d->first_sparea = FALSE;
#ifdef SP_BREAK_DEBUG
      jlog("DEBUG: first silence, ignored\n");
#endif /* SP_BREAK_DEBUG */
    } else {
      /* ��ֽ�λ����, �裱�ѥ������Ǥ�����2�ѥ��� */
      /* break 1st pass */
#ifdef SP_BREAK_DEBUG
      jlog("DEBUG: >> segment [%d..%d]\n", r->am->mfcc->sparea_start, time-1);
#endif /* SP_BREAK_DEBUG */
      /* store begging frame of the segment */
      r->am->mfcc->sparea_start = d->tmp_sparea_start;
#ifdef SP_BREAK_RESUME_WORD_BEGIN
      /* resume word = most likely sp word on beginning frame of the segment */
      r->sp_break_last_word = d->tmp_sp_break_last_word;
#else
      /* resume word = most likely sp word on end frame of the segment */
      r->sp_break_last_word = d->last_tre_word;
#endif

      /*** segment: [sparea_start - time-1] ***/
      return(TRUE);
    }
  }


#endif  /* ~SPSEGMENT_NAIST */

    
#ifdef SP_BREAK_EVAL
  jlog("DEBUG: [%d %d %d]\n", time, count, (detected) ? 50 : 0);
#endif
  return (FALSE);
}

/*******************************************************************/
/* �裱�ѥ��������Ƚ�λ���� (���硼�ȥݡ����������ơ��������) */
/* end of 1st pass for a segment (for short pause segmentation)    */
/*******************************************************************/
/** 
 * <JA>
 * @brief  �༡�ǥ����ǥ��󥰤Τ�����裱�ѥ���λ���ν���
 *
 * �༡�ǥ����ǥ��󥰻��ѻ������δؿ��� finalize_1st_pass() ��˸ƤФ졤
 * ���Υ������Ȥ��裱�ѥ��ν�λ������Ԥ�. ����Ū�ˤϡ�
 * ³���裲�ѥ��Τ���λϽ�üñ��Υ��åȡ������
 * ����ǥ����ǥ��󥰤�Ƴ�����Ȥ��Τ���ˡ����ϥ٥��ȥ����̤������ʬ��
 * ���ԡ��� rest_param �˻Ĥ�. 
 * 
 * @param recog [in] ���󥸥󥤥󥹥���
 * </JA>
 * <EN>
 * @brief  Finalize the first pass for successive decoding
 *
 * When successive decoding mode is enabled, this function will be
 * called just after finalize_1st_pass() to finish the beam search
 * of the last segment.  The beginning and ending words for the 2nd pass
 * will be set according to the 1st pass result.  Then the current
 * input will be shrinked to the segmented length and the unprocessed
 * region are copied to rest_param for the next decoding.
 * 
 * @param recog [in] engine instance
 * </EN>
 * @callgraph
 * @callergraph
 */
void
finalize_segment(Recog *recog)
{
  int spstart;
  RecogProcess *r;
  MFCCCalc *mfcc;
  boolean ok_p;

  /* �ȥ�ꥹ�Ͻ�ü�ˤ��������ñ�����2�ѥ��λϽ�üñ��Ȥ��Ƴ�Ǽ */
  /* fix initial/last word hypothesis of the next 2nd pass to the best
     word hypothesis at the first/last frame in backtrellis*/
  for(r=recog->process_list;r;r=r->next) {
    if (!r->live) continue;
    if (r->lmtype == LM_PROB) {
      set_terminal_words(r);
    }
  }

  /* �ѥ�᡼����, ���裱�ѥ�����λ�����������ȶ�֤ȻĤ�ζ�֤�ʬ�䤹��. 
     ��������³����sp�����ʬ(sparea_start..len-1)�ϡ֤Τꤷ��פȤ���ξ����
     ���ԡ����� */
  /* Divide input parameter into two: the last segment and the rest.
     The short-pause area (sparea_start..len-1) is considered as "tab",
     copied in both parameters
   */
  /* param[sparea_start..framelen] -> rest_param
     param[0..len-1] -> param
     [sparea_start...len-1] overlapped
  */

  ok_p = FALSE;
  for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    if (mfcc->segmented) {
      spstart = mfcc->sparea_start;
      ok_p = TRUE;
      break;
    }
  }

  if (ok_p) {
    /* the input was segmented in an instance */
    /* shrink all param the len and store restart parameters in rest_param */
    /* for each mfcc */
    if (verbose_flag) jlog("STAT: segmented: next decoding will restart from %d\n", spstart);

    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      if (verbose_flag) jlog("STAT: MFCC%02d: segmented: processed length=%d\n", mfcc->id, mfcc->last_time);

      /* copy the rest to mfcc->rest_param and shrink mfcc->param */
      mfcc_copy_to_rest_and_shrink(mfcc, spstart, mfcc->last_time);
    }

    /* reset last_word info */
    for(r=recog->process_list;r;r=r->next) {
      if (!r->live) continue;
      r->sp_break_last_nword_allow_override = TRUE;
    }
    
  } else {
    
    /* last segment is on end of input: no rest parameter */
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      mfcc->rest_param = NULL;
    }
    /* reset last_word info */
    for(r=recog->process_list;r;r=r->next) {
      if (!r->live) continue;
      r->sp_break_2_begin_word = WORD_INVALID;
      r->sp_break_last_word = WORD_INVALID;
      r->sp_break_last_nword = WORD_INVALID;
      r->sp_break_last_nword_allow_override = FALSE;
    }
  }
}

#ifdef BACKEND_VAD

/** 
 * <EN>
 * Initialize parameters for decoder/GMM-based VAD.
 * This will be called before recognition start for each segment.
 * </EN>
 * <JA>
 * Decode/GMM-based VAD �Τ���˥ѥ�᡼������������. 
 * �����ϥ������Ȥ�ǧ��������Ϥ�����˸ƤФ��. 
 * </JA>
 * 
 * @param recog [i/o] engine instance
 * 
 * @callgraph
 * @callergraph
 */
void
spsegment_init(Recog *recog)
{
  RecogProcess *p;
  /* at first time, recognition does not start yet */
#ifdef SPSEGMENT_NAIST
  for(p=recog->process_list;p;p=p->next) {
    p->pass1.after_trigger = FALSE;
    p->pass1.trigger_duration = 0;
  }
#endif
#ifdef GMM_VAD
  if (recog->gmm) {
    recog->gc->after_trigger = FALSE;
    recog->gc->duration = 0;
  }
#endif
  recog->triggered = FALSE;
}

/** 
 * <EN>
 * @brief  Detect speech up-trigger and synhronize among instances.
 * 
 * This function inspects all recognition instancces and gmm components
 * to see if any of them has detected trigger up (beginning of speech)
 * at the last recognition process.  If trigger has been detected,
 * set trigger-up status for all the instances.
 * </EN>
 * <JA>
 * @brief  ������ֳ��Ϥθ��Ф���ӥ��󥹥��󥹴�Ʊ��. 
 * 
 * ���Ƥ�ǧ���������󥹥��󥹤�GMM�������ˤĤ��ơ�ľ����ǧ��������
 * �ȥꥬ���åסʲ�����ֳ��ϡˤ�Ƚ�ꤵ�줿���ɤ�����Ĵ�٤�. 
 * ���Ϥ��줿���ϡ����Ƥ�ǧ���������󥹥��󥹤ǥ��åץȥꥬ��ޡ�������. 
 * </JA>
 * 
 * @param recog [in] engine instance
 * 
 * @return TRUE if triggered, or FALSE if not.
 * 
 * @callgraph
 * @callergraph
 */
boolean
spsegment_trigger_sync(Recog *recog)
{
  RecogProcess *p;
  boolean ok_p;

  ok_p = FALSE;
  if (recog->jconf->decodeopt.segment) {
#ifdef SPSEGMENT_NAIST
    for(p = recog->process_list; p; p = p->next) {
      if (!p->live) continue;
      if (p->pass1.after_trigger) {
	ok_p = TRUE;
	break;
      }
    }
#endif
#ifdef GMM_VAD
    if (recog->gmm) {
      if (recog->gc->after_trigger) {
	ok_p = TRUE;
      }
    }
#endif
  }
  if (ok_p) {
    /* up trigger detected */
#ifdef SPSEGMENT_NAIST
    for(p = recog->process_list; p; p = p->next) {
      if (!p->live) continue;
      p->pass1.after_trigger = TRUE;
    }
#endif
#ifdef GMM_VAD
    if (recog->gmm) {
      recog->gc->after_trigger = TRUE;
    }
#endif
  }
  
  return ok_p;
}

#endif /* BACKEND_VAD */

/** 
 * <EN>
 * @brief  Check if rewind and restart of recognition is needed.
 *
 * This function checks if an instance requires rewinding of input
 * samples, and if recognition re-processing is needed after rewinding.
 * 
 * </EN>
 * <JA>
 * @brief  �����ᤷ��ǧ���Ƴ���ɬ����������å�����. 
 * 
 * ����ǧ�������ˤ����ƴ����ᤷ��ɬ�פ��ɤ���Ĵ�١�ɬ�פʾ���
 * �ե졼����ȡ������ᤷ����˴��ᤷʬ��ǧ��������Ԥ����ɤ������֤�. 
 * </JA>
 * 
 * @param recog [in] engine instance
 * @param rf_ret [out] length of frame to rewind
 * @param repro_ret [out] TRUE if re-process is required after rewinding
 * 
 * @return TRUE if rewinding is required, or FALSE if not.
 * 
 * @callgraph
 * @callergraph
 */
boolean
spsegment_need_restart(Recog *recog, int *rf_ret, boolean *repro_ret)
{
#ifdef SPSEGMENT_NAIST
  RecogProcess *p;
#endif
  boolean ok_p;
  int rewind_frame = 0;
  boolean reprocess = FALSE;

  ok_p = FALSE;
  if (recog->jconf->decodeopt.segment) {
#ifdef SPSEGMENT_NAIST
    /* check for rewind request from each process */
    for(p = recog->process_list; p; p = p->next) {
      if (!p->live) continue;
      if (p->pass1.want_rewind) {
	p->pass1.want_rewind = FALSE;
	rewind_frame = p->pass1.rewind_frame;
	reprocess = p->pass1.want_rewind_reprocess;
	ok_p = TRUE;
	break;
      }
    }
#endif /* SPSEGMENT_NAIST */
#ifdef GMM_VAD
    if (recog->gmm) {
      if (recog->gc->want_rewind) {
	recog->gc->want_rewind = FALSE;
#ifdef SPSEGMENT_NAIST
	/* set to earlier one */
	if (rewind_frame > recog->gc->rewind_frame) rewind_frame = recog->gc->rewind_frame;
#else
	rewind_frame = recog->gc->rewind_frame;
#endif
	reprocess = recog->gc->want_rewind_reprocess;
	ok_p = TRUE;
      }
    }
#endif
    *rf_ret = rewind_frame;
    *repro_ret = reprocess;
  }

  return(ok_p);
}

/** 
 * <EN>
 * @brief  Execute rewinding.
 *
 * This function will set re-start point for the following processing,
 * and shrink the parameters for the rewinded part.  The re-start point
 * is 0 (beginning of rest samples) for recognition restart, or
 * simply go back to the specified rewind frames for non restart.
 * 
 * </EN>
 * <JA>
 * @brief  �����ᤷ����
 *
 * ��������Ͻ����γ���������ꤷ�������ᤷʬ�ѥ�᡼����ͤ��. 
 * �Ƴ�����ξ�糫�����ϥѥ�᡼������Ƭ�ˡ�����ʳ��ξ��ϴ��ᤷ��
 * ʬ������ä����֤˥��åȤ����. 
 * 
 * </JA>
 * 
 * @param recog [i/o] engine instance
 * @param rewind_frame [in] frame length to rewind
 * @param reprocess [in] TRUE if re-processing recognition is required for the following processing
 * 
 * @callgraph
 * @callergraph
 */
void
spsegment_restart_mfccs(Recog *recog, int rewind_frame, boolean reprocess)
{
  MFCCCalc *mfcc;

  for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    if (!mfcc->valid) continue;
    /* set last segmented time */
    mfcc->last_time = mfcc->f - 1;
    /* reset frame pointers */
    if (reprocess) {
      /* set all mfcc to initial point for re-process the whole frames */
      mfcc->f = -1;
    } else {
      /* just bring back to the new last point after shrink */
      mfcc->f -= rewind_frame;
    }
    /* shrink the current mfcc */
    mfcc_shrink(mfcc, rewind_frame);
  }
}

/* end of file */
