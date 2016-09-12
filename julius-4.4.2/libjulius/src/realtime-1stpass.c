/**
 * @file   realtime-1stpass.c
 * 
 * <JA>
 * @brief  ��1�ѥ����ե졼��Ʊ���ӡ���õ���ʼ»��ֽ����ǡ�
 *
 * ��1�ѥ������ϳ��Ϥ�Ʊ���˥������Ȥ������Ϥ�ʿ�Ԥ���ǧ��������Ԥ������
 * �ؿ����������Ƥ���. 
 * 
 * �Хå������ξ�硤Julius �β���ǧ�������ϰʲ��μ���
 * main_recognition_loop() ��Ǽ¹Ԥ����. 
 *
 *  -# �������� adin_go()  �� ���ϲ����� speech[] �˳�Ǽ�����
 *  -# ��ħ����� new_wav2mfcc() ��speech������ħ�ѥ�᡼���� param �˳�Ǽ
 *  -# ��1�ѥ��¹� get_back_trellis() ��param �ȥ�ǥ뤫��ñ��ȥ�ꥹ������
 *  -# ��2�ѥ��¹� wchmm_fbs()
 *  -# ǧ����̽���
 *
 * ��1�ѥ���ʿ�Խ��������硤�嵭�� 1 �� 3 ��ʿ�Ԥ��ƹԤ���. 
 * Julius �Ǥϡ������¹Խ����򡤲������Ϥ����Ҥ������뤿�Ӥ�
 * ǧ�������򤽤�ʬ��������Ū�˿ʤ�뤳�ȤǼ������Ƥ���. 
 * 
 *  - ��ħ����Ф���1�ѥ��¹Ԥ򡤰�ĤˤޤȤ�ƥ�����Хå��ؿ��Ȥ������. 
 *  - �������ϴؿ� adin_go() �Υ�����Хå��Ȥ��ƾ嵭�δؿ���Ϳ����
 *
 * ����Ū�ˤϡ��������������Ƥ��� RealTimePipeLine() ��������Хå��Ȥ���
 * adin_go() ��Ϳ������. adin_go() �ϲ������Ϥ��ȥꥬ����Ȥ�������줿����
 * ���Ҥ��Ȥ� RealTimePipeLine() ��ƤӽФ�. RealTimePipeLine() ������줿
 * ����ʬ�ˤĤ�����ħ����Ф���1�ѥ��η׻���ʤ��. 
 *
 * CMN �ˤĤ�����դ�ɬ�פǤ���. CMN ���̾�ȯ��ñ�̤ǹԤ��뤬��
 * �ޥ������Ϥ�ͥåȥ�����ϤΤ褦�ˡ���1�ѥ���ʿ�Ԥ�ǧ����Ԥ�
 * ��������ȯ�����ΤΥ��ץ��ȥ��ʿ�Ѥ����뤳�Ȥ��Ǥ��ʤ�. �С������ 3.5
 * �����Ǥ�ľ����ȯ��5��ʬ(���Ѥ��줿���Ϥ����)�� CMN �����Τޤ޼�ȯ�ä�
 * ή�Ѥ���Ƥ�������3.5.1 ����ϡ��嵭��ľ��ȯ�� CMN �����ͤȤ���
 * ȯ���� CMN �� MAP-CMN ��������Ʒ׻�����褦�ˤʤä�. �ʤ���
 * �ǽ��ȯ���Ѥν��CMN�� "-cmnload" ��Ϳ���뤳�Ȥ�Ǥ����ޤ�
 * "-cmnnoupdate" �����Ϥ��Ȥ� CMN ������Ԥ�ʤ��褦�ˤǤ���. 
 * "-cmnnoupdate" �� "-cmnload" ���Ȥ߹�碌�뤳�Ȥ�, �ǽ�˥����Х��
 * ���ץ��ȥ��ʿ�Ѥ�Ϳ����������˽���ͤȤ��� MAP-CMN ���뤳�Ȥ��Ǥ���. 
 *
 * ���פʴؿ��ϰʲ����̤�Ǥ���. 
 *
 *  - RealTimeInit() - ��ư���ν����
 *  - RealTimePipeLinePrepare() - ���Ϥ��Ȥν����
 *  - RealTimePipeLine() - ��1�ѥ�ʿ�Խ����ѥ�����Хå��ʾ�ҡ�
 *  - RealTimeResume() - ���硼�ȥݡ����������ơ���������ǧ������
 *  - RealTimeParam() - ���Ϥ��Ȥ���1�ѥ���λ����
 *  - RealTimeCMNUpdate() - CMN �ι���
 *  
 * </JA>
 * 
 * <EN>
 * @brief  The first pass: frame-synchronous beam search (on-the-fly version)
 *
 * These are functions to perform on-the-fly decoding of the 1st pass
 * (frame-synchronous beam search).  These function can be used
 * instead of new_wav2mfcc() and get_back_trellis().  These functions enable
 * recognition as soon as an input triggers.  The 1st pass processing
 * will be done concurrently with the input.
 *
 * The basic recognition procedure of Julius in main_recognition_loop()
 * is as follows:
 *
 *  -# speech input: (adin_go())  ... buffer `speech' holds the input
 *  -# feature extraction: (new_wav2mfcc()) ... compute feature vector
 *     from `speech' and store the vector sequence to `param'.
 *  -# recognition 1st pass: (get_back_trellis()) ... frame-wise beam decoding
 *     to generate word trellis index from `param' and models.
 *  -# recognition 2nd pass: (wchmm_fbs())
 *  -# Output result.
 *
 * At on-the-fly decoding, procedures from 1 to 3 above will be performed
 * in parallel.  It is implemented by a simple scheme, processing the captured
 * small speech fragments one by one progressively:
 *
 *  - Define a callback function that can do feature extraction and 1st pass
 *    processing progressively.
 *  - The callback will be given to A/D-in function adin_go().
 *
 * Actual procedure is as follows. The function RealTimePipeLine()
 * will be given to adin_go() as callback.  Then adin_go() will watch
 * the input, and if speech input starts, it calls RealTimePipeLine()
 * for every captured input fragments.  RealTimePipeLine() will
 * compute the feature vector of the given fragment and proceed the
 * 1st pass processing for them, and return to the capture function.
 * The current status will be hold to the next call, to perform
 * inter-frame processing (computing delta coef. etc.).
 *
 * Note about CMN: With acoustic models trained with CMN, Julius performs
 * CMN to the input.  On file input, the whole sentence mean will be computed
 * and subtracted.  At the on-the-fly decoding, the ceptral mean will be
 * performed using the cepstral mean of last 5 second input (excluding
 * rejected ones).  This was a behavier earlier than 3.5, and 3.5.1 now
 * applies MAP-CMN at on-the-fly decoding, using the last 5 second cepstrum
 * as initial mean.  Initial cepstral mean at start can be given by option
 * "-cmnload", and you can also prohibit the updates of initial cepstral
 * mean at each input by "-cmnnoupdate".  The last option is useful to always
 * use static global cepstral mean as initial mean for each input.
 *
 * The primary functions in this file are:
 *  - RealTimeInit() - initialization at application startup
 *  - RealTimePipeLinePrepare() - initialization before each input
 *  - RealTimePipeLine() - callback for on-the-fly 1st pass decoding
 *  - RealTimeResume() - recognition resume procedure for short-pause segmentation.
 *  - RealTimeParam() - finalize the on-the-fly 1st pass when input ends.
 *  - RealTimeCMNUpdate() - update CMN data for next input
 * 
 * </EN>
 * 
 * @author Akinobu Lee
 * @date   Tue Aug 23 11:44:14 2005
 *
 * $Revision: 1.14 $
 * 
 */
/*
 * Copyright (c) 1991-2013 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2013 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#include <julius/julius.h>

#undef RDEBUG			///< Define if you want local debug message

/** 
 * <JA>
 * MFCC�׻����󥹥��������ħ�ѥ�᡼���٥��ȥ��Ǽ���ꥢ���������.
 * 
 * mfcc->para �ξ���˴�Ť��ƥإå�������Ǽ���������Ǽ�ΰ����ݤ���. 
 * ��Ǽ�ΰ�ϡ����ϻ���ɬ�פ˱����Ƽ�ưŪ�˿�Ĺ�����Τǡ������Ǥ�
 * ���ν��������Ԥ�. ���Ǥ˳�Ǽ�ΰ褬���ݤ���Ƥ���Ȥ��Ϥ���򥭡��פ���. 
 * 
 * ���������/ǧ��1�󤴤Ȥ˷����֤��ƤФ��.
 * 
 * </JA>
 * <EN>
 * 
 * Prepare parameter holder in MFCC calculation instance to store MFCC
 * vectors.
 *
 * This function will store header information based on the parameters
 * in mfcc->para, and allocate initial buffer for the incoming
 * vectors.  The vector buffer will be expanded as needed while
 * recognition, so at this time only the minimal amount is allocated.
 * If the instance already has a certain length of vector buffer, it
 * will be kept.
 * 
 * This function will be called each time a new input begins.
 * 
 * </EN>
 *
 * @param mfcc [i/o] MFCC calculation instance
 * 
 */
static void
init_param(MFCCCalc *mfcc)
{
  Value *para;

  para = mfcc->para;

  /* ���줫��׻������ѥ�᡼���η���إå������� */
  /* set header types */
  mfcc->param->header.samptype = para->basetype;
  if (para->delta) mfcc->param->header.samptype |= F_DELTA;
  if (para->acc) mfcc->param->header.samptype |= F_ACCL;
  if (para->energy) mfcc->param->header.samptype |= F_ENERGY;
  if (para->c0) mfcc->param->header.samptype |= F_ZEROTH;
  if (para->absesup) mfcc->param->header.samptype |= F_ENERGY_SUP;
  if (para->cmn) mfcc->param->header.samptype |= F_CEPNORM;
  
  mfcc->param->header.wshift = para->smp_period * para->frameshift;
  mfcc->param->header.sampsize = para->veclen * mfcc->splice * sizeof(VECT); /* not compressed */
  mfcc->param->veclen = para->veclen * mfcc->splice;
  
  /* ǧ��������/��λ��˥��åȤ�����ѿ�:
     param->parvec (�ѥ�᡼���٥��ȥ����)
     param->header.samplenum, param->samplenum (���ե졼���)
  */
  /* variables that will be set while/after computation has been done:
     param->parvec (parameter vector sequence)
     param->header.samplenum, param->samplenum (total number of frames)
  */
  /* MAP-CMN �ν���� */
  /* Prepare for MAP-CMN */
  if (mfcc->para->cmn || mfcc->para->cvn) CMN_realtime_prepare(mfcc->cmn.wrk);
}

/** 
 * <JA>
 * @brief  ��1�ѥ�ʿ��ǧ�������ν����.
 *
 * MFCC�׻��Υ�����ꥢ���ݤ�Ԥ�. �ޤ�ɬ�פʾ��ϡ����ڥ��ȥ븺���Ѥ�
 * ������ꥢ�������Υ������ڥ��ȥ�Υ��ɡ�CMN�Ѥν�����ץ��ȥ��
 * ʿ�ѥǡ����Υ��ɤʤɤ�Ԥ���. 
 *
 * ���δؿ��ϡ������ƥ൯ư��1������ƤФ��.
 * </JA>
 * <EN>
 * @brief  Initializations for the on-the-fly 1st pass decoding.
 *
 * Work areas for all MFCC caculation instances are allocated.
 * Additionaly,
 * some initialization will be done such as allocating work area
 * for spectral subtraction, loading noise spectrum from file,
 * loading initial ceptral mean data for CMN from file, etc.
 *
 * This will be called only once, on system startup.
 * </EN>
 *
 * @param recog [i/o] engine instance
 *
 * @callgraph
 * @callergraph
 */
boolean
RealTimeInit(Recog *recog)
{
  Value *para;
  RealBeam *r;
  MFCCCalc *mfcc;


  r = &(recog->real);

  /* ����ե졼��Ĺ��������ϻ��ֿ�����׻� */
  /* set maximum allowed frame length */
  r->maxframelen = MAXSPEECHLEN / recog->jconf->input.frameshift;

  /* -ssload �����, SS�ѤΥΥ������ڥ��ȥ��ե����뤫���ɤ߹��� */
  /* if "-ssload", load noise spectrum for spectral subtraction from file */
  for(mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    if (mfcc->frontend.ssload_filename && mfcc->frontend.ssbuf == NULL) {
      if ((mfcc->frontend.ssbuf = new_SS_load_from_file(mfcc->frontend.ssload_filename, &(mfcc->frontend.sslen))) == NULL) {
	jlog("ERROR: failed to read \"%s\"\n", mfcc->frontend.ssload_filename);
	return FALSE;
      }
      /* check ssbuf length */
      if (mfcc->frontend.sslen != mfcc->wrk->bflen) {
	jlog("ERROR: noise spectrum length not match\n");
	return FALSE;
      }
      mfcc->wrk->ssbuf = mfcc->frontend.ssbuf;
      mfcc->wrk->ssbuflen = mfcc->frontend.sslen;
      mfcc->wrk->ss_alpha = mfcc->frontend.ss_alpha;
      mfcc->wrk->ss_floor = mfcc->frontend.ss_floor;
    }
  }
  for(mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
  
    para = mfcc->para;

    /* �п����ͥ륮���������Τ���ν���� */
    /* set initial value for log energy normalization */
    if (para->energy && para->enormal) energy_max_init(&(mfcc->ewrk));
    /* �ǥ륿�׻��Τ���Υ�������Хåե����Ѱ� */
    /* initialize cycle buffers for delta and accel coef. computation */
    if (para->delta) mfcc->db = WMP_deltabuf_new(para->baselen, para->delWin);
    if (para->acc) mfcc->ab = WMP_deltabuf_new(para->baselen * 2, para->accWin);
    /* �ǥ륿�׻��Τ���Υ�����ꥢ����� */
    /* allocate work area for the delta computation */
    mfcc->tmpmfcc = (VECT *)mymalloc(sizeof(VECT) * para->vecbuflen);
    /* splice buffer */
    if (mfcc->splice > 1) {
      mfcc->splicedmfcc = (VECT *)mymalloc(sizeof(VECT) * para->veclen * mfcc->splice);
      mfcc->splicedlen = 0;
    }
    /* MAP-CMN �Ѥν�����ץ��ȥ��ʿ�Ѥ��ɤ߹���ǽ�������� */
    /* Initialize the initial cepstral mean data from file for MAP-CMN */
    if (para->cmn || para->cvn) mfcc->cmn.wrk = CMN_realtime_new(para, mfcc->cmn.map_weight, mfcc->cmn.map_cmn);
    /* -cmnload �����, CMN�ѤΥ��ץ��ȥ��ʿ�Ѥν���ͤ�ե����뤫���ɤ߹��� */
    /* if "-cmnload", load initial cepstral mean data from file for CMN */
    if (mfcc->cmn.load_filename) {
      if (para->cmn) {
	if ((mfcc->cmn.loaded = CMN_load_from_file(mfcc->cmn.wrk, mfcc->cmn.load_filename))== FALSE) {
	  jlog("ERROR: failed to read initial cepstral mean from \"%s\", do flat start\n", mfcc->cmn.load_filename);
	  return FALSE;
	}
      } else {
	jlog("WARNING: CMN not required on AM, file \"%s\" ignored\n", mfcc->cmn.load_filename);
      }
    }

  }
  /* ��Ĺ�򥻥å� */
  /* set window length */
  r->windowlen = recog->jconf->input.framesize + 1;
  /* �뤫���ѥХåե������ */
  /* set window buffer */
  r->window = mymalloc(sizeof(SP16) * r->windowlen);

  return TRUE;
}

/** 
 * <EN>
 * Prepare work are a for MFCC calculation.
 * Reset values in work area for starting the next input.
 * Output probability cache for each acoustic model will be also
 * prepared at this function.
 *
 * This function will be called before starting each input (segment).
 * </EN>
 * <JA>
 * MFCC�׻����������. 
 * �����Ĥ��Υ�����ꥢ��ꥻ�åȤ���ǧ����������. 
 * �ޤ���������ǥ뤴�Ȥν��ϳ�Ψ�׻�����å�����������. 
 *
 * ���δؿ��ϡ��������ϡʤ��뤤�ϥ������ȡˤ�ǧ����
 * �Ϥޤ�����ɬ���ƤФ��. 
 * 
 * </JA>
 * 
 * @param recog [i/o] engine instance
 * 
 * @callgraph
 * @callergraph
 */
void
reset_mfcc(Recog *recog) 
{
  Value *para;
  MFCCCalc *mfcc;

  /* ��ħ��Х⥸�塼������� */
  /* initialize parameter extraction module */
  for(mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {

    para = mfcc->para;

    /* �п����ͥ륮���������Τ���ν���ͤ򥻥å� */
    /* set initial value for log energy normalization */
    if (para->energy && para->enormal) energy_max_prepare(&(mfcc->ewrk), para);
    /* �ǥ륿�׻��ѥХåե������ */
    /* set the delta cycle buffer */
    if (para->delta) WMP_deltabuf_prepare(mfcc->db);
    if (para->acc) WMP_deltabuf_prepare(mfcc->ab);
    if (mfcc->splice > 1) mfcc->splicedlen = 0;
  }

}

/** 
 * <JA>
 * @brief  ��1�ѥ�ʿ��ǧ�������ν���
 *
 * �׻����ѿ���ꥻ�åȤ����Ƽ�ǡ������������. 
 * ���δؿ��ϡ��������ϡʤ��뤤�ϥ������ȡˤ�ǧ����
 * �Ϥޤ����˸ƤФ��. 
 * 
 * </JA>
 * <EN>
 * @brief  Preparation for the on-the-fly 1st pass decoding.
 *
 * Variables are reset and data are prepared for the next input recognition.
 *
 * This function will be called before starting each input (segment).
 * 
 * </EN>
 *
 * @param recog [i/o] engine instance
 *
 * @return TRUE on success. FALSE on failure.
 *
 * @callgraph
 * @callergraph
 * 
 */
boolean
RealTimePipeLinePrepare(Recog *recog)
{
  RealBeam *r;
  PROCESS_AM *am;
  MFCCCalc *mfcc;
#ifdef SPSEGMENT_NAIST
  RecogProcess *p;
#endif

  r = &(recog->real);

  /* �׻��Ѥ��ѿ������� */
  /* initialize variables for computation */
  r->windownum = 0;
  /* parameter check */
  for(mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    /* �ѥ�᡼������� */
    /* parameter initialization */
    if (recog->jconf->input.speech_input == SP_MFCMODULE) {
      if (mfc_module_set_header(mfcc, recog) == FALSE) return FALSE;
    } else {
      init_param(mfcc);
    }
    /* �ե졼�ऴ�ȤΥѥ�᡼���٥��ȥ���¸���ΰ����� */
    /* ���Ȥ�ɬ�פ˱����ƿ�Ĺ����� */
    if (param_alloc(mfcc->param, 1, mfcc->param->veclen) == FALSE) {
      j_internal_error("ERROR: segmented: failed to allocate memory for rest param\n");
    }
    /* �ե졼�����ꥻ�å� */
    /* reset frame count */
    mfcc->f = 0;
  }
  /* �������� param ��¤�ΤΥǡ����Υѥ�᡼�����򲻶���ǥ�ȥ����å����� */
  /* check type coherence between param and hmminfo here */
  if (recog->jconf->input.paramtype_check_flag) {
    for(am=recog->amlist;am;am=am->next) {
      if (am->config->dnn.enabled) continue;
      if (!check_param_coherence(am->hmminfo, am->mfcc->param)) {
	jlog("ERROR: input parameter type does not match AM\n");
	return FALSE;
      }
    }
  }

  /* �׻��ѤΥ�����ꥢ����� */
  /* prepare work area for calculation */
  if (recog->jconf->input.type == INPUT_WAVEFORM) {
    reset_mfcc(recog);
  }
  /* �������ٷ׻��ѥ���å������� */
  /* prepare cache area for acoustic computation of HMM states and mixtures */
  for(am=recog->amlist;am;am=am->next) {
    outprob_prepare(&(am->hmmwrk), r->maxframelen);
  }

#ifdef BACKEND_VAD
  if (recog->jconf->decodeopt.segment) {
    /* initialize segmentation parameters */
    spsegment_init(recog);
  }
#else
  recog->triggered = FALSE;
#endif

#ifdef DEBUG_VTLN_ALPHA_TEST
  /* store speech */
  recog->speechlen = 0;
#endif

  return TRUE;
}

/* splice */
static boolean
splice_mfcc(MFCCCalc *mfcc)
{
  if (mfcc->splicedlen >= mfcc->splice) {
    memmove(mfcc->splicedmfcc, &(mfcc->splicedmfcc[mfcc->para->veclen]), sizeof(VECT) * (mfcc->splice - 1) * mfcc->para->veclen);
    mfcc->splicedlen --;
  }
  memcpy(&(mfcc->splicedmfcc[mfcc->para->veclen * mfcc->splicedlen]), mfcc->tmpmfcc, sizeof(VECT) * mfcc->para->veclen);
  mfcc->splicedlen++;
  if (mfcc->splicedlen < mfcc->splice) {
    /* if ret == FALSE, there is no available frame.  So just wait for
       next input */
    return FALSE;
  }
  return TRUE;
}

/** 
 * <JA>
 * @brief  �����ȷ�����ѥ�᡼���٥��ȥ��׻�����.
 * 
 * ��ñ�̤Ǽ��Ф��줿�����ȷ�����MFCC�٥��ȥ��׻�����.
 * �׻���̤� mfcc->tmpmfcc ����¸�����. 
 * 
 * @param mfcc [i/o] MFCC�׻����󥹥���
 * @param window [in] ��ñ�̤Ǽ��Ф��줿�����ȷ��ǡ���
 * @param windowlen [in] @a window ��Ĺ��
 * 
 * @return �׻���������TRUE ���֤�. �ǥ륿�׻��ˤ��������ϥե졼�ब
 * ���ʤ��ʤɡ��ޤ������Ƥ��ʤ����� FALSE ���֤�. 
 * </JA>
 * <EN>
 * @brief  Compute a parameter vector from a speech window.
 *
 * This function calculates an MFCC vector from speech data windowed from
 * input speech.  The obtained MFCC vector will be stored to mfcc->tmpmfcc.
 * 
 * @param mfcc [i/o] MFCC calculation instance
 * @param window [in] speech input (windowed from input stream)
 * @param windowlen [in] length of @a window
 * 
 * @return TRUE on success (an vector obtained).  Returns FALSE if no
 * parameter vector obtained yet (due to delta delay).
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
boolean
RealTimeMFCC(MFCCCalc *mfcc, SP16 *window, int windowlen)
{
  int i;
  boolean ret;
  VECT *tmpmfcc;
  Value *para;

  tmpmfcc = mfcc->tmpmfcc;
  para = mfcc->para;

  /* �����ȷ����� base MFCC ��׻� (recog->mfccwrk ������) */
  /* calculate base MFCC from waveform (use recog->mfccwrk) */
  for (i=0; i < windowlen; i++) {
    mfcc->wrk->bf[i+1] = (float) window[i];
  }
  WMP_calc(mfcc->wrk, tmpmfcc, para);

  if (para->energy && para->enormal) {
    /* �п����ͥ륮��������������� */
    /* normalize log energy */
    /* �ꥢ�륿�������ϤǤ�ȯ�ä��Ȥκ��票�ͥ륮���������ʤ��Τ�
       ľ����ȯ�äΥѥ�����Ѥ��� */
    /* Since the maximum power of the whole input utterance cannot be
       obtained at real-time input, the maximum of last input will be
       used to normalize.
    */
    tmpmfcc[para->baselen-1] = energy_max_normalize(&(mfcc->ewrk), tmpmfcc[para->baselen-1], para);
  }

  if (para->delta) {
    /* �ǥ륿��׻����� */
    /* calc delta coefficients */
    ret = WMP_deltabuf_proceed(mfcc->db, tmpmfcc);
#ifdef RDEBUG
    printf("DeltaBuf: ret=%d, status=", ret);
    for(i=0;i<mfcc->db->len;i++) {
      printf("%d", mfcc->db->is_on[i]);
    }
    printf(", nextstore=%d\n", mfcc->db->store);
#endif
    /* ret == FALSE �ΤȤ��Ϥޤ��ǥ��쥤��ʤΤ�ǧ���������������Ϥ� */
    /* if ret == FALSE, there is no available frame.  So just wait for
       next input */
    if (! ret) {
      return FALSE;
    }

    /* db->vec �˸��ߤθ��ǡ����ȥǥ륿���������äƤ���Τ� tmpmfcc �˥��ԡ� */
    /* now db->vec holds the current base and full delta, so copy them to tmpmfcc */
    memcpy(tmpmfcc, mfcc->db->vec, sizeof(VECT) * para->baselen * 2);
  }

  if (para->acc) {
    /* Acceleration��׻����� */
    /* calc acceleration coefficients */
    /* base+delta �򤽤Τޤ������ */
    /* send the whole base+delta to the cycle buffer */
    ret = WMP_deltabuf_proceed(mfcc->ab, tmpmfcc);
#ifdef RDEBUG
    printf("AccelBuf: ret=%d, status=", ret);
    for(i=0;i<mfcc->ab->len;i++) {
      printf("%d", mfcc->ab->is_on[i]);
    }
    printf(", nextstore=%d\n", mfcc->ab->store);
#endif
    /* ret == FALSE �ΤȤ��Ϥޤ��ǥ��쥤��ʤΤ�ǧ���������������Ϥ� */
    /* if ret == FALSE, there is no available frame.  So just wait for
       next input */
    if (! ret) {
      return FALSE;
    }
    /* ab->vec �ˤϡ�(base+delta) �Ȥ��κ�ʬ���������äƤ���. 
       [base] [delta] [delta] [acc] �ν�����äƤ���Τ�,
       [base] [delta] [acc] �� tmpmfcc �˥��ԡ�����. */
    /* now ab->vec holds the current (base+delta) and their delta coef. 
       it holds a vector in the order of [base] [delta] [delta] [acc], 
       so copy the [base], [delta] and [acc] to tmpmfcc.  */
    memcpy(tmpmfcc, mfcc->ab->vec, sizeof(VECT) * para->baselen * 2);
    memcpy(&(tmpmfcc[para->baselen*2]), &(mfcc->ab->vec[para->baselen*3]), sizeof(VECT) * para->baselen);
  }

#ifdef POWER_REJECT
  if (para->energy || para->c0) {
    mfcc->avg_power += tmpmfcc[para->baselen-1];
  }
#endif

  if (para->delta && (para->energy || para->c0) && para->absesup) {
    /* �����ͥѥ����� */
    /* suppress absolute power */
    memmove(&(tmpmfcc[para->baselen-1]), &(tmpmfcc[para->baselen]), sizeof(VECT) * (para->vecbuflen - para->baselen));
  }

  /* ���λ����� tmpmfcc �˸������Ǥκǿ�����ħ�٥��ȥ뤬��Ǽ����Ƥ��� */
  /* tmpmfcc[] now holds the latest parameter vector */

  /* CMN ��׻� */
  /* perform CMN */
  if (para->cmn || para->cvn) CMN_realtime(mfcc->cmn.wrk, tmpmfcc);

  /* splice */
  if (mfcc->splice > 1) {
    return(splice_mfcc(mfcc));
  }

  return TRUE;
}

static int
proceed_one_frame(Recog *recog)
{
  MFCCCalc *mfcc;
  RealBeam *r;
  int maxf;
  PROCESS_AM *am;
  int rewind_frame;
  boolean reprocess;
  boolean ok_p;

  r = &(recog->real);

  /* call recognition start callback */
  ok_p = FALSE;
  maxf = 0;
  for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    if (!mfcc->valid) continue;
    if (maxf < mfcc->f) maxf = mfcc->f;
    if (mfcc->f == 0) {
      ok_p = TRUE;
    }
  }
  if (ok_p && maxf == 0) {
    /* call callback when at least one of MFCC has initial frame */
    if (recog->jconf->decodeopt.segment) {
#ifdef BACKEND_VAD
      /* not exec pass1 begin callback here */
#else
      if (!recog->process_segment) {
	callback_exec(CALLBACK_EVENT_RECOGNITION_BEGIN, recog);
      }
      callback_exec(CALLBACK_EVENT_SEGMENT_BEGIN, recog);
      callback_exec(CALLBACK_EVENT_PASS1_BEGIN, recog);
      recog->triggered = TRUE;
#endif
    } else {
      callback_exec(CALLBACK_EVENT_RECOGNITION_BEGIN, recog);
      callback_exec(CALLBACK_EVENT_PASS1_BEGIN, recog);
      recog->triggered = TRUE;
    }
  }
  /* �ƥ��󥹥��󥹤ˤĤ��� mfcc->f ��ǧ��������1�ե졼��ʤ�� */
  switch (decode_proceed(recog)) {
  case -1: /* error */
    return -1;
    break;
  case 0:			/* success */
    break;
  case 1:			/* segmented */
    /* ǧ�������Υ��������׵�ǽ���ä����Ȥ�ե饰�˥��å� */
    /* set flag which indicates that the input has ended with segmentation request */
    r->last_is_segmented = TRUE;
    /* tell the caller to be segmented by this function */
    /* �ƤӽФ����ˡ����������Ϥ��ڤ�褦������ */
    return 1;
  }
#ifdef BACKEND_VAD
  /* check up trigger in case of VAD segmentation */
  if (recog->jconf->decodeopt.segment) {
    if (recog->triggered == FALSE) {
      if (spsegment_trigger_sync(recog)) {
	if (!recog->process_segment) {
	  callback_exec(CALLBACK_EVENT_RECOGNITION_BEGIN, recog);
	}
	callback_exec(CALLBACK_EVENT_SEGMENT_BEGIN, recog);
	callback_exec(CALLBACK_EVENT_PASS1_BEGIN, recog);
	recog->triggered = TRUE;
      }
    }
  }
#endif
  
  if (spsegment_need_restart(recog, &rewind_frame, &reprocess) == TRUE) {
    /* set total length to the current frame */
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      if (!mfcc->valid) continue;
      mfcc->param->header.samplenum = mfcc->f + 1;
      mfcc->param->samplenum = mfcc->f + 1;
    }
    /* do rewind for all mfcc here */
    spsegment_restart_mfccs(recog, rewind_frame, reprocess);
    /* also tell adin module to rehash the concurrent audio input */
    recog->adin->rehash = TRUE;
    /* reset outprob cache for all AM */
    for(am=recog->amlist;am;am=am->next) {
      outprob_prepare(&(am->hmmwrk), am->mfcc->param->samplenum);
    }
    if (reprocess) {
      /* process the backstep MFCCs here */
      while(1) {
	ok_p = TRUE;
	for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
	  if (! mfcc->valid) continue;
	  mfcc->f++;
	  if (mfcc->f < mfcc->param->samplenum) {
	    mfcc->valid = TRUE;
	    ok_p = FALSE;
	  } else {
	    mfcc->valid = FALSE;
	  }
	}
	if (ok_p) {
	  /* ���٤Ƥ� MFCC ��������ã�����Τǥ롼�׽�λ */
	  /* all MFCC has been processed, end of loop  */
	  for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
	    if (! mfcc->valid) continue;
	    mfcc->f--;
	  }
	  break;
	}
	/* �ƥ��󥹥��󥹤ˤĤ��� mfcc->f ��ǧ��������1�ե졼��ʤ�� */
	switch (decode_proceed(recog)) {
	case -1: /* error */
	  return -1;
	  break;
	case 0:			/* success */
	  break;
	case 1:			/* segmented */
	  /* ignore segmentation while in the backstep segment */
	  break;
	}
	/* call frame-wise callback */
	callback_exec(CALLBACK_EVENT_PASS1_FRAME, recog);
      }
    }
  }
  /* call frame-wise callback if at least one of MFCC is valid at this frame */
  for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    if (mfcc->valid) {
      callback_exec(CALLBACK_EVENT_PASS1_FRAME, recog);
      break;
    }
  }
  
  return 0;
}


/** 
 * <JA>
 * @brief  ��1�ѥ�ʿ�Բ���ǧ�������Υᥤ��
 *
 * ���δؿ���Ǥϡ�����Ū����ħ����Ф������1�ѥ���ǧ�����Ԥ���. 
 * ���ϥǡ������Ф�����ݤ������եȤ�Ԥ�MFCC�׻���Ԥ��ʤ��顤
 * ����ǧ����1�ե졼�ऺ������¹Ԥ���. 
 *
 * ǧ��������decode_proceed()�ˤˤ����ơ�������ֽ�λ���׵ᤵ���
 * ���Ȥ�����. ���ξ�硤̤�����β�������¸������1�ѥ���λ����
 * �褦�ƽи����׵᤹��. 
 *
 * SPSEGMENT_NAIST ���뤤�� GMM_VAD �ʤɤΥХå������VAD������ϡ��ǥ������١�����
 * VAD �ʲ�����ֳ��ϸ��Сˤ�ȼ���ǥ����ǥ������椬�Ԥ���. 
 * �ȥꥬ���ϡ�ǧ���������ƤФ�뤬���ºݤˤϳƴؿ����ǧ��������
 * �Ԥ��Ƥ��ʤ�. ���Ϥ򸡽Ф����������δؿ��Ϥ����ޤǤ�����줿
 * MFCC������ե졼��Ĺʬ���ᤷ�����δ��ᤷ�褫���̾��ǧ��������
 * �Ƴ�����. �ʤ���ʣ���������󥹥��󥹴֤������硤���ϥȥꥬ��
 * �ɤ줫�Υ��󥹥��󥹤����Ф������������Ƥγ��Ϥ�Ʊ�������. 
 * 
 * ���δؿ��ϡ��������ϥ롼����Υ�����Хå��Ȥ��ƸƤФ��.
 * �����ǡ����ο��饵��ץ�Ͽ�����Ȥˤ��δؿ����ƤӽФ����. 
 * 
 * @param Speech [in] �����ǡ����ؤΥХåե��ؤΥݥ���
 * @param nowlen [in] �����ǡ�����Ĺ��
 * @param recog [i/o] engine instance
 * 
 * @return ���顼���� -1 ��������� 0 ���֤�. �ޤ�����1�ѥ���
 * ��λ����褦�ƽи����׵᤹��Ȥ��� 1 ���֤�. 
 * </JA>
 * <EN>
 * @brief  Main function of the on-the-fly 1st pass decoding
 *
 * This function performs sucessive MFCC calculation and 1st pass decoding.
 * The given input data are windowed to a certain length, then converted
 * to MFCC, and decoding for the input frame will be performed in one
 * process cycle.  The loop cycle will continue with window shift, until
 * the whole given input has been processed.
 *
 * In case of input segment request from decoding process (in
 * decode_proceed()), this function keeps the rest un-processed speech
 * to a buffer and tell the caller to stop input and end the 1st pass.
 *
 * When back-end VAD such as SPSEGMENT_NAIST or GMM_VAD is defined,  Decoder-based
 * VAD is enabled and its decoding control will be managed here.
 * In decoder-based VAD mode, the recognition will be processed but
 * no output will be done at the first un-triggering input area.
 * when speech input start is detected, this function will rewind the
 * already obtained MFCC sequence to a certain frames, and re-start
 * normal recognition at that point.  When multiple recognition process
 * instance is running, their segmentation will be synchronized.
 * 
 * This function will be called each time a new speech sample comes as
 * as callback from A/D-in routine.
 * 
 * @param Speech [in] pointer to the speech sample segments
 * @param nowlen [in] length of above
 * @param recog [i/o] engine instance
 * 
 * @return -1 on error (will close stream and terminate recognition),
 * 0 on success (allow caller to call me for the next segment).  It
 * returns 1 when telling the caller to segment now at the middle of
 * input , and 2 when input length overflow is detected.
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
int
RealTimePipeLine(SP16 *Speech, int nowlen, Recog *recog) /* Speech[0...nowlen] = input */
{
  int i, now, ret;
  MFCCCalc *mfcc;
  RealBeam *r;
  VECT *mfccvec;

  r = &(recog->real);

#ifdef DEBUG_VTLN_ALPHA_TEST
  /* store speech */
  adin_cut_callback_store_buffer(Speech, nowlen, recog);
#endif

  /* window[0..windownum-1] ������θƤӽФ��ǻĤä������ǡ�������Ǽ����Ƥ��� */
  /* window[0..windownum-1] are speech data left from previous call */

  /* �����ѥݥ��󥿤����� */
  /* initialize pointer for local processing */
  now = 0;
  
  /* ǧ�����������������׵�ǽ���ä��Τ��ɤ����Υե饰��ꥻ�å� */
  /* reset flag which indicates whether the input has ended with segmentation request */
  r->last_is_segmented = FALSE;

#ifdef RDEBUG
  printf("got %d samples\n", nowlen);
#endif

  while (now < nowlen) {	/* till whole input is processed */
    /* ����Ĺ�� maxframelen ��ã�����餳���Ƕ�����λ */
    /* if input length reaches maximum buffer size, terminate 1st pass here */
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      if (mfcc->f >= r->maxframelen) {
	jlog("Warning: too long input (> %d frames), segment it now\n", r->maxframelen);
	return(1);
      }
    }
    /* ��Хåե���������������� */
    /* fill window buffer as many as possible */
    for(i = min(r->windowlen - r->windownum, nowlen - now); i > 0 ; i--)
      r->window[r->windownum++] = (float) Speech[now++];
    /* �⤷��Хåե�����ޤ�ʤ����, ���Υ������Ȥν����Ϥ����ǽ����. 
       ��������ʤ��ä�����ץ� (window[0..windownum-1]) �ϼ���˻����ۤ�. */
    /* if window buffer was not filled, end processing here, keeping the
       rest samples (window[0..windownum-1]) in the window buffer. */
    if (r->windownum < r->windowlen) break;
#ifdef RDEBUG
    /*    printf("%d used, %d rest\n", now, nowlen - now);

	  printf("[f = %d]\n", f);*/
#endif

    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      mfcc->valid = FALSE;
      /* ����β����ȷ�������ħ�̤�׻����� r->tmpmfcc �˳�Ǽ  */
      /* calculate a parameter vector from current waveform windows
	 and store to r->tmpmfcc */
      if ((*(recog->calc_vector))(mfcc, r->window, r->windowlen)) {
	if (mfcc->splice > 1) {
	  mfccvec = mfcc->splicedmfcc;
	} else {
	  mfccvec = mfcc->tmpmfcc;
	}
#ifdef ENABLE_PLUGIN
	/* call post-process plugin if exist */
	plugin_exec_vector_postprocess(mfccvec, mfcc->param->veclen, mfcc->f);
#endif
	/* MFCC��������Ͽ */
  	mfcc->valid = TRUE;
	/* now get the MFCC vector of current frame, now store it to param */
	if (param_alloc(mfcc->param, mfcc->f + 1, mfcc->param->veclen) == FALSE) {
	  jlog("ERROR: failed to allocate memory for incoming MFCC vectors\n");
	  return -1;
	}
	memcpy(mfcc->param->parvec[mfcc->f], mfccvec, sizeof(VECT) * mfcc->param->veclen);
#ifdef RDEBUG
	printf("DeltaBuf: %02d: got frame %d\n", mfcc->id, mfcc->f);
#endif
      }
    }

    /* ������1�ե졼��ʤ�� */
    /* proceed one frame */
    ret = proceed_one_frame(recog);

    if (ret == 1 && recog->jconf->decodeopt.segment) {
      /* ���硼�ȥݡ����������ơ������: �Хåե��˻ĤäƤ���ǡ�����
	 �̤��ݻ����ơ�����κǽ�˽������� */
      /* short pause segmentation: there is some data left in buffer, so
	 we should keep them for next processing */
      r->rest_len = nowlen - now;
      if (r->rest_len > 0) {
	/* copy rest samples to rest_Speech */
	if (r->rest_Speech == NULL) {
	  r->rest_alloc_len = r->rest_len;
	  r->rest_Speech = (SP16 *)mymalloc(sizeof(SP16)*r->rest_alloc_len);
	} else if (r->rest_alloc_len < r->rest_len) {
	  r->rest_alloc_len = r->rest_len;
	  r->rest_Speech = (SP16 *)myrealloc(r->rest_Speech, sizeof(SP16)*r->rest_alloc_len);
	}
	memcpy(r->rest_Speech, &(Speech[now]), sizeof(SP16) * r->rest_len);
      }
    }
    if (ret != 0) return ret;

    /* 1�ե졼��������ʤ���Τǥݥ��󥿤�ʤ�� */
    /* proceed frame pointer */
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      if (!mfcc->valid) continue;
      mfcc->f++;
    }

    /* ��Хåե������������ä�ʬ���ե� */
    /* shift window */
    memmove(r->window, &(r->window[recog->jconf->input.frameshift]), sizeof(SP16) * (r->windowlen - recog->jconf->input.frameshift));
    r->windownum -= recog->jconf->input.frameshift;
  }

  /* Ϳ����줿�����������Ȥ��Ф���ǧ�����������ƽ�λ
     �ƤӽФ�����, ���Ϥ�³����褦������ */
  /* input segment is fully processed
     tell the caller to continue input */
  return(0);			
}

/** 
 * <JA>
 * @brief  �������Ȥ�ǧ���Ƴ�����
 *
 * ���δؿ��ϥǥ������١���VAD�䥷�硼�ȥݡ����������ơ������ˤ�ä�
 * ���Ϥ��������Ȥ��ڤ�줿���ˡ����θ��ǧ���κƳ��˴ؤ��������Ԥ�. 
 * ����Ū�ˤϡ����Ϥ�ǧ���򳫻Ϥ������ˡ���������ϥ������Ȥˤ�����
 * ���ᤷʬ��MFCC�󤫤�ǧ���򳫻Ϥ���. ����ˡ�����Υ������ơ���������
 * ̤�������ä��Ĥ�β�������ץ뤬����Ф�����������.
 *
 * @param recog [i/o] ���󥸥󥤥󥹥���
 * 
 * @return ���顼�� -1������� 0 ���֤�. �ޤ��������������Ҥν������
 * ʸ�Ϥζ��ڤ꤬���Ĥ��ä��Ȥ�����1�ѥ��򤳤������Ǥ��뤿��� 1 ���֤�. 
 * </JA>
 * </JA>
 * <EN>
 * @brief  Resuming recognition for short pause segmentation.
 *
 * This function process overlapped data and remaining speech prior
 * to the next input when input was segmented at last processing.
 *
 * @param recog [i/o] engine instance
 *
 * @return -1 on error (tell caller to terminate), 0 on success (allow caller
 * to call me for the next segment), or 1 when an end-of-sentence detected
 * at this point (in that case caller will stop input and go to 2nd pass)
 * </EN>
 *
 * @callgraph
 * @callergraph
 * 
 */
int
RealTimeResume(Recog *recog)
{
  MFCCCalc *mfcc;
  RealBeam *r;
  boolean ok_p;
#ifdef SPSEGMENT_NAIST
  RecogProcess *p;
#endif
  PROCESS_AM *am;

  r = &(recog->real);

  /* �׻��ѤΥ�����ꥢ����� */
  /* prepare work area for calculation */
  if (recog->jconf->input.type == INPUT_WAVEFORM) {
    reset_mfcc(recog);
  }
  /* �������ٷ׻��ѥ���å������� */
  /* prepare cache area for acoustic computation of HMM states and mixtures */
  for(am=recog->amlist;am;am=am->next) {
    outprob_prepare(&(am->hmmwrk), r->maxframelen);
  }

  /* param �ˤ������ѥ�᡼�������������� */
  /* prepare to process all data in param */
  for(mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    if (mfcc->param->samplenum == 0) mfcc->valid = FALSE;
    else mfcc->valid = TRUE;
#ifdef RDEBUG
    printf("Resume: %02d: f=%d\n", mfcc->id, mfcc->mfcc->param->samplenum-1);
#endif
    /* �ե졼�����ꥻ�å� */
    /* reset frame count */
    mfcc->f = 0;
    /* MAP-CMN �ν���� */
    /* Prepare for MAP-CMN */
    if (mfcc->para->cmn || mfcc->para->cvn) CMN_realtime_prepare(mfcc->cmn.wrk);
  }

#ifdef BACKEND_VAD
  if (recog->jconf->decodeopt.segment) {
    spsegment_init(recog);
  }
  /* not exec pass1 begin callback here */
#else
  recog->triggered = FALSE;
  for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    if (!mfcc->valid) continue;
    callback_exec(CALLBACK_EVENT_SEGMENT_BEGIN, recog);
    callback_exec(CALLBACK_EVENT_PASS1_BEGIN, recog);
    recog->triggered = TRUE;
    break;
  }
#endif

  /* param ������ե졼��ˤĤ���ǧ��������ʤ�� */
  /* proceed recognition for all frames in param */

  while(1) {
    ok_p = TRUE;
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      if (! mfcc->valid) continue;
      if (mfcc->f < mfcc->param->samplenum) {
	mfcc->valid = TRUE;
	ok_p = FALSE;
      } else {
	mfcc->valid = FALSE;
      }
    }
    if (ok_p) {
      /* ���٤Ƥ� MFCC ��������ã�����Τǥ롼�׽�λ */
      /* all MFCC has been processed, end of loop  */
      break;
    }

    /* �ƥ��󥹥��󥹤ˤĤ��� mfcc->f ��ǧ��������1�ե졼��ʤ�� */
    switch (decode_proceed(recog)) {
    case -1: /* error */
      return -1;
      break;
    case 0:			/* success */
      break;
    case 1:			/* segmented */
      /* segmented, end procs ([0..f])*/
      r->last_is_segmented = TRUE;
      return 1;		/* segmented by this function */
    }

#ifdef BACKEND_VAD
    /* check up trigger in case of VAD segmentation */
    if (recog->jconf->decodeopt.segment) {
      if (recog->triggered == FALSE) {
	if (spsegment_trigger_sync(recog)) {
	  callback_exec(CALLBACK_EVENT_SEGMENT_BEGIN, recog);
	  callback_exec(CALLBACK_EVENT_PASS1_BEGIN, recog);
	  recog->triggered = TRUE;
	}
      }
    }
#endif

    /* call frame-wise callback */
    callback_exec(CALLBACK_EVENT_PASS1_FRAME, recog);

    /* 1�ե졼��������ʤ���Τǥݥ��󥿤�ʤ�� */
    /* proceed frame pointer */
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      if (!mfcc->valid) continue;
      mfcc->f++;
    }

  }
  /* ����Υ������Ȼ������Ϥ򥷥եȤ��Ƥ��ʤ�ʬ�򥷥եȤ��� */
  /* do the last shift here */
  if (recog->jconf->input.type == INPUT_WAVEFORM) {
    memmove(r->window, &(r->window[recog->jconf->input.frameshift]), sizeof(SP16) * (r->windowlen - recog->jconf->input.frameshift));
    r->windownum -= recog->jconf->input.frameshift;
    /* ����ǺƳ��ν��������ä��Τ�,�ޤ�������ν����ǻĤäƤ��������ǡ�������
       �������� */
    /* now that the search status has been prepared for the next input, we
       first process the rest unprocessed samples at the last session */
    if (r->rest_len > 0) {
      return(RealTimePipeLine(r->rest_Speech, r->rest_len, recog));
    }
  }

  /* ���������Ϥ��Ф���ǧ��������³���� */
  /* the recognition process will continue for the newly incoming samples... */
  return 0;

}


/** 
 * <JA>
 * @brief  ��1�ѥ�ʿ��ǧ�������ν�λ������Ԥ�.
 *
 * ���δؿ�����1�ѥ���λ���˸ƤФ졤����Ĺ����ꤷ�����ȡ�
 * decode_end() �ʥ������Ȥǽ�λ�����Ȥ��� decode_end_segmented()�ˤ�
 * �ƤӽФ�����1�ѥ���λ������Ԥ�. 
 *
 * �⤷�������ϥ��ȥ꡼��ν�λ�ˤ�ä�ǧ��������ä����ʥե��������Ϥ�
 * ��ü��ã�������ʤɡˤϡ��ǥ륿�Хåե���̤���������Ϥ��ĤäƤ���Τǡ�
 * ����򤳤��ǽ�������. 
 *
 * @param recog [i/o] ���󥸥󥤥󥹥���
 * 
 * @return ���������� TRUE, ���顼�� FALSE ���֤�. 
 * </JA>
 * <EN>
 * @brief  Finalize the 1st pass on-the-fly decoding.
 *
 * This function will be called after the 1st pass processing ends.
 * It fix the input length of parameter vector sequence, call
 * decode_end() (or decode_end_segmented() when last input was ended
 * by segmentation) to finalize the 1st pass.
 *
 * If the last input was ended by end-of-stream (in case input reached
 * EOF in file input etc.), process the rest samples remaining in the
 * delta buffers.
 *
 * @param recog [i/o] engine instance
 * 
 * @return TRUE on success, or FALSE on error.
 * </EN>
 */
boolean
RealTimeParam(Recog *recog)
{
  boolean ret1, ret2;
  RealBeam *r;
  int ret;
  int maxf;
  boolean ok_p;
  MFCCCalc *mfcc;
  Value *para;
#ifdef RDEBUG
  int i;
#endif
  VECT *mfccvec;

  r = &(recog->real);

  if (r->last_is_segmented) {

    /* RealTimePipeLine ��ǧ������¦����ͳ�ˤ��ǧ�������Ǥ������,
       �����֤�MFCC�׻��ǡ����򤽤Τޤ޼�����ݻ�����ɬ�פ�����Τ�,
       MFCC�׻���λ������Ԥ鷺���裱�ѥ��η�̤Τ߽��Ϥ��ƽ����. */
    /* When input segmented by recognition process in RealTimePipeLine(),
       we have to keep the whole current status of MFCC computation to the
       next call.  So here we only output the 1st pass result. */
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      mfcc->param->header.samplenum = mfcc->f + 1;/* len = lastid + 1 */
      mfcc->param->samplenum = mfcc->f + 1;
    }
    decode_end_segmented(recog);

    /* ���ζ�֤� param �ǡ������裲�ѥ��Τ�����֤� */
    /* return obtained parameter for 2nd pass */
    return(TRUE);
  }

  if (recog->jconf->input.type == INPUT_VECTOR) {
    /* finalize real-time 1st pass */
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      mfcc->param->header.samplenum = mfcc->f;
      mfcc->param->samplenum = mfcc->f;
    }
    /* �ǽ��ե졼�������Ԥ���ǧ���η�̽��ϤȽ�λ������Ԥ� */
    decode_end(recog);
    return TRUE;
  }

  /* MFCC�׻��ν�λ������Ԥ�: �Ǹ���ٱ�ե졼��ʬ����� */
  /* finish MFCC computation for the last delayed frames */
  for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    if (mfcc->para->delta || mfcc->para->acc) {
      mfcc->valid = TRUE;
    } else {
      mfcc->valid = FALSE;
    }
  }

  /* loop until all data has been flushed */
  while (1) {

    /* check frame overflow */
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      if (! mfcc->valid) continue;
      if (mfcc->f >= r->maxframelen) mfcc->valid = FALSE;
    }

    /* if all mfcc became invalid, exit loop here */
    ok_p = FALSE;
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      if (mfcc->valid) {
	ok_p = TRUE;
	break;
      }
    }
    if (!ok_p) break;

    /* try to get 1 frame for all mfcc instances */
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      
      para = mfcc->para;
      
      if (! mfcc->valid) continue;
      
      /* check if there is data in cycle buffer of delta */
      ret1 = WMP_deltabuf_flush(mfcc->db);
#ifdef RDEBUG
      printf("DeltaBufLast: ret=%d, status=", ret1);
      for(i=0;i<mfcc->db->len;i++) {
	printf("%d", mfcc->db->is_on[i]);
      }
      printf(", nextstore=%d\n", mfcc->db->store);
#endif
      if (ret1) {
	/* uncomputed delta has flushed, compute it with tmpmfcc */
	if (para->energy && para->absesup) {
	  memcpy(mfcc->tmpmfcc, mfcc->db->vec, sizeof(VECT) * (para->baselen - 1));
	  memcpy(&(mfcc->tmpmfcc[para->baselen-1]), &(mfcc->db->vec[para->baselen]), sizeof(VECT) * para->baselen);
	} else {
	  memcpy(mfcc->tmpmfcc, mfcc->db->vec, sizeof(VECT) * para->baselen * 2);
	}
	if (para->acc) {
	  /* this new delta should be given to the accel cycle buffer */
	  ret2 = WMP_deltabuf_proceed(mfcc->ab, mfcc->tmpmfcc);
#ifdef RDEBUG
	  printf("AccelBuf: ret=%d, status=", ret2);
	  for(i=0;i<mfcc->ab->len;i++) {
	    printf("%d", mfcc->ab->is_on[i]);
	  }
	  printf(", nextstore=%d\n", mfcc->ab->store);
#endif
	  if (ret2) {
	    /* uncomputed accel was given, compute it with tmpmfcc */
	    memcpy(mfcc->tmpmfcc, mfcc->ab->vec, sizeof(VECT) * (para->veclen - para->baselen));
	    memcpy(&(mfcc->tmpmfcc[para->veclen - para->baselen]), &(mfcc->ab->vec[para->veclen - para->baselen]), sizeof(VECT) * para->baselen);
	  } else {
	    /* still no input is given: */
	    /* in case of very short input: go on to the next input */
	    continue;
	  }
	}
	
      } else {
      
	/* no data left in the delta buffer */
	if (para->acc) {
	  /* no new data, just flush the accel buffer */
	  ret2 = WMP_deltabuf_flush(mfcc->ab);
#ifdef RDEBUG
	  printf("AccelBuf: ret=%d, status=", ret2);
	  for(i=0;i<mfcc->ab->len;i++) {
	    printf("%d", mfcc->ab->is_on[i]);
	  }
	  printf(", nextstore=%d\n", mfcc->ab->store);
#endif
	  if (ret2) {
	    /* uncomputed data has flushed, compute it with tmpmfcc */
	    memcpy(mfcc->tmpmfcc, mfcc->ab->vec, sizeof(VECT) * (para->veclen - para->baselen));
	    memcpy(&(mfcc->tmpmfcc[para->veclen - para->baselen]), &(mfcc->ab->vec[para->veclen - para->baselen]), sizeof(VECT) * para->baselen);
	  } else {
	    /* actually no data exists in both delta and accel */
	    mfcc->valid = FALSE; /* disactivate this instance */
	    continue;		/* end this loop */
	  }
	} else {
	  /* only delta: input fully flushed */
	  mfcc->valid = FALSE; /* disactivate this instance */
	  continue;		/* end this loop */
	}
      }
      /* a new frame has been obtained from delta buffer to tmpmfcc */
      if(para->cmn || para->cvn) CMN_realtime(mfcc->cmn.wrk, mfcc->tmpmfcc);
      /* splice */
      if (mfcc->splice > 1) {
	if (splice_mfcc(mfcc) == FALSE) {
	  continue;
	}
	mfccvec = mfcc->splicedmfcc;
      } else {
	mfccvec = mfcc->tmpmfcc;
      }
      if (param_alloc(mfcc->param, mfcc->f + 1, mfcc->param->veclen) == FALSE) {
	jlog("ERROR: failed to allocate memory for incoming MFCC vectors\n");
	return FALSE;
      }
      /* store to mfcc->f */
      memcpy(mfcc->param->parvec[mfcc->f], mfccvec, sizeof(VECT) * mfcc->param->veclen);
#ifdef ENABLE_PLUGIN
      /* call postprocess plugin if any */
      plugin_exec_vector_postprocess(mfcc->param->parvec[mfcc->f], mfcc->param->veclen, mfcc->f);
#endif
    }

    /* call recognition start callback */
    ok_p = FALSE;
    maxf = 0;
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      if (!mfcc->valid) continue;
      if (maxf < mfcc->f) maxf = mfcc->f;
      if (mfcc->f == 0) {
	ok_p = TRUE;
      }
    }

    if (ok_p && maxf == 0) {
      /* call callback when at least one of MFCC has initial frame */
      if (recog->jconf->decodeopt.segment) {
#ifdef BACKEND_VAD
	  /* not exec pass1 begin callback here */
#else
	if (!recog->process_segment) {
	  callback_exec(CALLBACK_EVENT_RECOGNITION_BEGIN, recog);
	}
	callback_exec(CALLBACK_EVENT_SEGMENT_BEGIN, recog);
	callback_exec(CALLBACK_EVENT_PASS1_BEGIN, recog);
	recog->triggered = TRUE;
#endif
      } else {
	callback_exec(CALLBACK_EVENT_RECOGNITION_BEGIN, recog);
	callback_exec(CALLBACK_EVENT_PASS1_BEGIN, recog);
	recog->triggered = TRUE;
      }
    }

    /* proceed for the curent frame */
    ret = decode_proceed(recog);
    if (ret == -1) {		/* error */
      return -1;
    } else if (ret == 1) {	/* segmented */
      /* loop out */
      break;
    } /* else no event occured */

#ifdef BACKEND_VAD
    /* check up trigger in case of VAD segmentation */
    if (recog->jconf->decodeopt.segment) {
      if (recog->triggered == FALSE) {
	if (spsegment_trigger_sync(recog)) {
	  if (!recog->process_segment) {
	    callback_exec(CALLBACK_EVENT_RECOGNITION_BEGIN, recog);
	  }
	  callback_exec(CALLBACK_EVENT_SEGMENT_BEGIN, recog);
	  callback_exec(CALLBACK_EVENT_PASS1_BEGIN, recog);
	  recog->triggered = TRUE;
	}
      }
    }
#endif

    /* call frame-wise callback */
    callback_exec(CALLBACK_EVENT_PASS1_FRAME, recog);

    /* move to next */
    for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
      if (! mfcc->valid) continue;
      mfcc->f++;
    }
  }

  /* finalize real-time 1st pass */
  for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    mfcc->param->header.samplenum = mfcc->f;
    mfcc->param->samplenum = mfcc->f;
  }
  /* �ǽ��ե졼�������Ԥ���ǧ���η�̽��ϤȽ�λ������Ԥ� */
  decode_end(recog);

  return(TRUE);
}

/** 
 * <JA>
 * ���ץ��ȥ��ʿ�Ѥι���. 
 * �����ǧ���������ơ����ϥǡ�������CMN�ѤΥ��ץ��ȥ��ʿ�Ѥ򹹿�����. 
 * 
 * @param mfcc [i/o] �׻��оݤ� MFCC�׻����󥹥���
 * @param recog [i/o] ���󥸥󥤥󥹥���
 *
 * </JA>
 * <EN>
 * Update cepstral mean.
 *
 * This function updates the initial cepstral mean for CMN of the next input.
 *
 * @param mfcc [i/o] MFCC Calculation instance to update its CMN
 * @param recog [i/o] engine instance
 * </EN>
 */
void
RealTimeCMNUpdate(MFCCCalc *mfcc, Recog *recog)
{
  boolean cmn_update_p;
  Value *para;
  RecogProcess *r;

  para = mfcc->para;
  
  /* update CMN vector for next speech */
  if(para->cmn) {
    if (mfcc->cmn.update) {
      cmn_update_p = TRUE;
      for(r=recog->process_list;r;r=r->next) {
	if (!r->live) continue;
	if (r->am->mfcc != mfcc) continue;
	if (r->result.status < 0) { /* input rejected */
	  cmn_update_p = FALSE;
	  break;
	}
      }
      if (cmn_update_p) {
	/* update last CMN parameter for next spech */
	CMN_realtime_update(mfcc->cmn.wrk, mfcc->param);
      } else {
	/* do not update, because the last input is bogus */
	if (verbose_flag) {
#ifdef BACKEND_VAD
	  if (!recog->jconf->decodeopt.segment || recog->triggered) {
	    jlog("STAT: skip CMN parameter update since last input was invalid\n");
	  }
#else
	  jlog("STAT: skip CMN parameter update since last input was invalid\n");
#endif
	}
      }
    }
    /* if needed, save the updated CMN parameter to a file */
    if (mfcc->cmn.save_filename) {
      if (CMN_save_to_file(mfcc->cmn.wrk, mfcc->cmn.save_filename) == FALSE) {
	jlog("WARNING: failed to save CMN parameter to \"%s\"\n", mfcc->cmn.save_filename);
      }
    }
  }
}

/** 
 * <JA>
 * ��1�ѥ�ʿ��ǧ�����������Ǥ���. 
 *
 * @param recog [i/o] ���󥸥󥤥󥹥���
 * </JA>
 * <EN>
 * Terminate the 1st pass on-the-fly decoding.
 *
 * @param recog [i/o] engine instance
 * </EN>
 */
void
RealTimeTerminate(Recog *recog)
{
  MFCCCalc *mfcc;

  for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
    mfcc->param->header.samplenum = mfcc->f;
    mfcc->param->samplenum = mfcc->f;
  }

  /* �ǽ��ե졼�������Ԥ���ǧ���η�̽��ϤȽ�λ������Ԥ� */
  decode_end(recog);
}

/** 
 * <EN>
 * Free the whole work area for 1st pass on-the-fly decoding
 * </EN>
 * <JA>
 * ��1�ѥ��¹Խ����Τ���Υ�����ꥢ��������
 * </JA>
 * 
 * @param recog [in] engine instance
 * 
 */
void
realbeam_free(Recog *recog)
{
  if (recog->real.window) {
    free(recog->real.window);
    recog->real.window = NULL;
  }
  if (recog->real.rest_Speech) {
    free(recog->real.rest_Speech);
    recog->real.rest_Speech = NULL;
  }
}



/************************************************************************/
/************************************************************************/
/************************************************************************/
/************************************************************************/

/* MFCC realtime input */
/** 
 * <EN>
 * 
 * </EN>
 * <JA>
 * 
 * </JA>
 * 
 * @param recog 
 * @param ad_check 
 * 
 * @return 2 when input termination requested by recognition process,
 * 1 when segmentation request returned from input module, 0 when end
 * of input returned from input module, -1 on error, -2 when input
 * termination requested by ad_check().
 * 
 */
int
mfcc_go(Recog *recog, int (*ad_check)(Recog *))
{
  RealBeam *r;
  MFCCCalc *mfcc;
  int new_f;
  int ret, ret3;

  r = &(recog->real);

  r->last_is_segmented = FALSE;
  
  while(1/*in_data_vec*/) {

    ret = mfc_module_read(recog->mfcclist, &new_f);

    if (debug2_flag) {
      if (recog->mfcclist->f < new_f) {
	jlog("%d: %d (%d)\n", recog->mfcclist->f, new_f, ret);
      }
    }
 
    /* callback poll */
    if (ad_check != NULL) {
      if ((ret3 = (*(ad_check))(recog)) < 0) {
	if ((ret3 == -1 && recog->mfcclist->f == 0) || ret3 == -2) {
	  return(-2);
	}
      }
    }

    while(recog->mfcclist->f < new_f) {

      recog->mfcclist->valid = TRUE;

#ifdef ENABLE_PLUGIN
      /* call post-process plugin if exist */
      plugin_exec_vector_postprocess(recog->mfcclist->param->parvec[recog->mfcclist->f], recog->mfcclist->param->veclen, recog->mfcclist->f);
#endif

      /* ������1�ե졼��ʤ�� */
      /* proceed one frame */
      
      switch(proceed_one_frame(recog)) {
      case -1:			/* error */
	return -1;
      case 0:			/* normal */
	break;
      case 1:			/* segmented by process */
	return 2;
      }

      /* 1�ե졼��������ʤ���Τǥݥ��󥿤�ʤ�� */
      /* proceed frame pointer */
      for (mfcc = recog->mfcclist; mfcc; mfcc = mfcc->next) {
	if (!mfcc->valid) continue;
	mfcc->f++;
      }
    }
    
    /* check if input end */
    switch(ret) {
    case -1: 			/* end of input */
      return 0;
    case -2:			/* error */
      return -1;
    case -3:			/* end of segment request */
      return 1;
    }
  }
  /* Ϳ����줿�����������Ȥ��Ф���ǧ�����������ƽ�λ
     �ƤӽФ�����, ���Ϥ�³����褦������ */
  /* input segment is fully processed
     tell the caller to continue input */
  return(1);
}

/* end of file */


