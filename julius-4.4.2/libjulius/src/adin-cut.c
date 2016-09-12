/**
 * @file   adin-cut.c
 *
 * <JA>
 * @brief  ��������ץ��㤪���ͭ����ָ���
 *
 * �������ϥǥХ�������β����ǡ����μ����ߡ������
 * ����¸�ߤ����֤θ��Ф�Ԥʤ��ޤ�. 
 *
 * ͭ����֤θ��Фϡ�������٥����򺹿����Ѥ��ƹԤʤ��ޤ�. 
 * �������Ҥ��Ȥˡ���٥뤷�����ͤ�ۤ��뿶���ˤĤ�����򺹿��򥫥���Ȥ���
 * ���줬���ꤷ�����ʾ�ˤʤ�С����ζ�ֳ��ϸ��ФȤ���
 * �����ߤ򳫻Ϥ��ޤ�. �����������򺹿���������ʲ��ˤʤ�С�
 * �����ߤ���ߤ��ޤ�. �ºݤˤϴ����ڤ�Ф���Ԥʤ����ᡤ��������
 * �����������˥ޡ��������������ڤ�Ф��ޤ�. 
 * 
 * �ޤ������ץ������� (-zmean)�ˤ�� DC offset �ν���򤳤��ǹԤʤ��ޤ�. 
 * offset �Ϻǽ�� @a ZMEANSAMPLES �ĤΥ���ץ��ʿ�Ѥ���׻�����ޤ�. 
 *
 * �����ǡ����μ����ߤ��¹Ԥ������ϲ����ν�����Ԥʤ��ޤ�. ���Τ��ᡤ
 * ������������ǡ����Ϥ��μ�����ñ�̡�live���ϤǤϰ�����֡������ե�����
 * �ǤϥХåե��������ˤ��Ȥˡ�����������Ȥ��ƥ�����Хå��ؿ����ƤФ�ޤ�. 
 * ���Υ�����Хå��ؿ��Ȥ��ƥǡ�������¸����ħ����С�
 * �ʥե졼��Ʊ���Ρ�ǧ��������ʤ��ؿ�����ꤷ�ޤ�. 
 *
 * �ޥ������Ϥ� NetAudio ���Ϥʤɤ� Live ���ϤǤϡ�
 * ������Хå���ν������Ť����������Ϥ�®�٤��ɤ��դ��ʤ��ȡ�
 * �ǥХ����ΥХåե�����졤�������Ҥ����Ȥ����礬����ޤ�. 
 * ���Υ��顼���ɤ����ᡤ�¹ԴĶ��� pthread �����Ѳ�ǽ�Ǥ����硤
 * ���������ߡ���ָ����������Τ���Ω��������åɤ�ư��ޤ�. 
 * ���ξ�硤���Υ���åɤ��ܥ���åɤȥХåե� @a speech ��𤷤�
 * �ʲ��Τ褦�˶�Ĵư��ޤ�. 
 * 
 *    - Thread 1: ���������ߡ�����ָ��Х���å�
 *        - �ǥХ������鲻���ǡ������ɤ߹��ߤʤ��鲻��ָ��Ф�Ԥʤ�. 
 *          ���Ф�������֤Υ���ץ�ϥХåե� @a speech ���������༡
 *          �ɲä����. 
 *        - ���Υ���åɤϵ�ư�������ܥ���åɤ�����Ω����ư���
 *          �嵭��ư���Ԥʤ�³����. 
 *    - Thread 2: ����������ǧ��������Ԥʤ��ܥ���å�
 *        - �Хåե� @a speech �������֤��Ȥ˴ƻ뤷�������ʥ���ץ뤬
 *          Thread 1 �ˤ�ä��ɲä��줿�餽�������������������λ����
 *          ʬ�Хåե���ͤ��. 
 *
 * </JA>
 * <EN>
 * @brief  Capture audio and detect sound trigger
 *
 * This file contains functions to get waveform from an audio device
 * and detect speech/sound input segment
 *
 * Sound detection at this stage is based on level threshold and zero
 * cross count.  The number of zero cross are counted for each
 * incoming sound fragment.  If the number becomes larger than
 * specified threshold, the fragment is treated as a beginning of
 * sound/speech input (trigger on).  If the number goes below the threshold,
 * the fragment will be treated as an end of input (trigger
 * off).  In actual detection, margins are considered on the beginning
 * and ending point, which will be treated as head and tail silence
 * part.  DC offset normalization will be also performed if configured
 * so (-zmean).
 *
 * The triggered input data should be processed concurrently with the
 * detection for real-time recognition.  For this purpose, after the
 * beginning of input has been detected, the following triggered input
 * fragments (samples of a certain period in live input, or buffer size in
 * file input) are passed sequencially in turn to a callback function.
 * The callback function should be specified by the caller, typicaly to
 * store the recoded data, or to process them into a frame-synchronous
 * recognition process.
 *
 * When source is a live input such as microphone, the device buffer will
 * overflow if the processing callback is slow.  In that case, some input
 * fragments may be lost.  To prevent this, the A/D-in part together with
 * sound detection will become an independent thread if @em pthread functions
 * are supported.  The A/D-in and detection thread will cooperate with
 * the original main thread through @a speech buffer, like the followings:
 *
 *    - Thread 1: A/D-in and speech detection thread
 *        - reads audio input from source device and perform sound detection.
 *          The detected fragments are immediately appended
 *          to the @a speech buffer.
 *        - will be detached after created, and run forever till the main
 *          thread dies.
 *    - Thread 2: Main thread
 *        - performs input processing and recognition.
 *        - watches @a speech buffer, and if detect appendings of new samples
 *          by the Thread 1, proceed the processing for the appended samples
 *          and purge the finished samples from @a speech buffer.
 *
 * </EN>
 *
 * @sa adin.c
 *
 * @author Akinobu LEE
 * @date   Sat Feb 12 13:20:53 2005
 *
 * $Revision: 1.22 $
 * 
 */
/*
 * Copyright (c) 1991-2013 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2013 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#include <julius/julius.h>
#ifdef HAVE_PTHREAD
#include <pthread.h>
#endif

/// Define this if you want to output a debug message for threading
#undef THREAD_DEBUG
/// Enable some fixes relating adinnet+module
#define TMP_FIX_200602		

/** 
 * <EN>
 * @brief  Set up parameters for A/D-in and input detection.
 *
 * Set variables in work area according to the configuration values.
 * 
 * </EN>
 * <JA>
 * @brief  �����ڤ�Ф��ѳƼ�ѥ�᡼���򥻥å�
 *
 * ����򸵤��ڤ�Ф��ѤΥѥ�᡼����׻�����������ꥢ�˥��åȤ��ޤ�. 
 * 
 * </JA>
 * @param adin [in] AD-in work area
 * @param jconf [in] configuration data
 *
 * @callergraph
 * @callgraph
 */
boolean
adin_setup_param(ADIn *adin, Jconf *jconf)
{
  float samples_in_msec;
  int freq;

  if (jconf->input.sfreq <= 0) {
    jlog("ERROR: adin_setup_param: going to set smpfreq to %d\n", jconf->input.sfreq);
    return FALSE;
  }
  if (jconf->detect.silence_cut < 2) {
    adin->adin_cut_on = (jconf->detect.silence_cut == 1) ? TRUE : FALSE;
  } else {
    adin->adin_cut_on = adin->silence_cut_default;
  }
  adin->strip_flag = jconf->preprocess.strip_zero_sample;
  adin->thres = jconf->detect.level_thres;
#ifdef HAVE_PTHREAD
  if (adin->enable_thread && jconf->decodeopt.segment) {
    adin->ignore_speech_while_recog = FALSE;
  } else {
    adin->ignore_speech_while_recog = TRUE;
  }
#endif
  adin->need_zmean = jconf->preprocess.use_zmean;
  adin->level_coef = jconf->preprocess.level_coef;
  /* calc & set internal parameter from configuration */
  freq = jconf->input.sfreq;
  samples_in_msec = (float) freq / (float)1000.0;
  adin->chunk_size = jconf->detect.chunk_size;
  /* cycle buffer length = head margin length */
  adin->c_length = (int)((float)jconf->detect.head_margin_msec * samples_in_msec);	/* in msec. */
  if (adin->chunk_size > adin->c_length) {
    jlog("ERROR: adin_setup_param: chunk size (%d) > header margin (%d)\n", adin->chunk_size, adin->c_length);
    return FALSE;
  }
  /* compute zerocross trigger count threshold in the cycle buffer */
  adin->noise_zerocross = jconf->detect.zero_cross_num * adin->c_length / freq;
  /* variables that comes from the tail margin length (in wstep) */
  adin->nc_max = (int)((float)(jconf->detect.tail_margin_msec * samples_in_msec / (float)adin->chunk_size)) + 2;
  adin->sbsize = jconf->detect.tail_margin_msec * samples_in_msec + (adin->c_length * jconf->detect.zero_cross_num / 200);
  adin->c_offset = 0;

#ifdef HAVE_PTHREAD
  adin->transfer_online = FALSE;
  adin->speech = NULL;
  if (jconf->reject.rejectlonglen >= 0) {
    adin->freezelen = (jconf->reject.rejectlonglen + 500.0) * jconf->input.sfreq / 1000.0;
  } else {
    adin->freezelen = MAXSPEECHLEN;
  }
#endif

  /**********************/
  /* initialize buffers */
  /**********************/
  adin->buffer = (SP16 *)mymalloc(sizeof(SP16) * MAXSPEECHLEN);
  adin->cbuf = (SP16 *)mymalloc(sizeof(SP16) * adin->c_length);
  adin->swapbuf = (SP16 *)mymalloc(sizeof(SP16) * adin->sbsize);
  if (adin->down_sample) {
    adin->io_rate = 3;		/* 48 / 16 (fixed) */
    adin->buffer48 = (SP16 *)mymalloc(sizeof(SP16) * MAXSPEECHLEN * adin->io_rate);
  }
  if (adin->adin_cut_on) {
    init_count_zc_e(&(adin->zc), adin->c_length);
  }
  
  adin->need_init = TRUE;

  adin->rehash = FALSE;

  return TRUE;

}

/** 
 * <EN>
 * Purge samples already processed in the temporary buffer.
 * </EN>
 * <JA>
 * �ƥ�ݥ��Хåե��ˤ���������줿����ץ��ѡ�������.
 * </JA>
 * 
 * @param a [in] AD-in work area
 * @param from [in] Purge samples in range [0..from-1].
 * 
 */
static void
adin_purge(ADIn *a, int from)
{
  if (from > 0 && a->current_len - from > 0) {
    memmove(a->buffer, &(a->buffer[from]), (a->current_len - from) * sizeof(SP16));
  }
  a->bp = a->current_len - from;
}

/** 
 * <EN>
 * @brief  Main A/D-in and sound detection function
 *
 * This function read inputs from device and do sound detection
 * (both up trigger and down trigger) until end of device.
 *
 * In threaded mode, this function will detach and loop forever as
 * ad-in thread, (adin_thread_create()) storing triggered samples in
 * speech[], and telling the status to another process thread via @a
 * transfer_online in work area.  The process thread, called from
 * adin_go(), polls the length of speech[] and transfer_online in work area
 * and process them if new samples has been stored.
 *
 * In non-threaded mode, this function will be called directly from
 * adin_go(), and triggered samples are immediately processed within here.
 *
 * Threaded mode should be used for "live" input such as microphone input
 * where input is infinite and capture delay is severe.  For file input,
 * adinnet input and other "buffered" input, non-threaded mode will be used.
 *
 * Argument "ad_process()" should be a function to process the triggered
 * input samples.  On real-time recognition, a frame-synchronous search
 * function for the first pass will be specified by the caller.  The current
 * input will be segmented if it returns 1, and will be terminated as error
 * if it returns -1.
 *
 * When the argument "ad_check()" specified, it will be called periodically.
 * When it returns less than 0, this function will be terminated.
 * 
 * </EN>
 * <JA>
 * @brief  �������ϤȲ����Ф�Ԥ��ᥤ��ؿ�
 *
 * �����Ǥϲ������Ϥμ����ߡ�����֤γ��ϡ���λ�θ��Ф�Ԥ��ޤ�. 
 *
 * ����åɥ⡼�ɻ������δؿ�����Ω����AD-in����åɤȤ��ƥǥ��å�����ޤ�. 
 * (adin_thread_create()), �����Ϥ��Τ���Ȥ��δؿ��ϥ�����ꥢ���
 * speech[] �˥ȥꥬ��������ץ��Ͽ�������� transfer_online �� TRUE ��
 * ���åȤ��ޤ�. Julius �Υᥤ���������å� (adin_go()) ��
 * adin_thread_process() �˰ܹԤ��������� transfer_online ���� speech[] ��
 * ���Ȥ��ʤ���ǧ��������Ԥ��ޤ�. 
 *
 * �󥹥�åɥ⡼�ɻ��ϡ��ᥤ������ؿ� adin_go() ��ľ�ܤ��δؿ���Ƥӡ�
 * ǧ�������Ϥ���������ľ�ܹԤ��ޤ�. 
 *
 * ����åɥ⡼�ɤϥޥ������Ϥʤɡ����Ϥ�̵�¤ǽ������ٱ䤬�ǡ�����
 * ��ꤳ�ܤ��򾷤��褦�� live input ���Ѥ����ޤ�. �������ե���������
 * ��adinnet ���ϤΤ褦�� buffered input �Ǥ��󥹥�åɥ⡼�ɤ��Ѥ����ޤ�. 
 *
 * ������ ad_process �ϡ������������ץ���Ф��ƽ�����Ԥ��ؿ���
 * ���ꤷ�ޤ�. �ꥢ�륿����ǧ����Ԥ����ϡ���������1�ѥ���ǧ��������
 * �Ԥ��ؿ������ꤵ��ޤ�. �֤��ͤ� 1 �Ǥ���С����Ϥ򤳤��Ƕ��ڤ�ޤ�. 
 * -1 �Ǥ���Х��顼��λ���ޤ�. 
 * 
 * ������ ad_check �ϰ���������Ȥ˷����֤��ƤФ��ؿ�����ꤷ�ޤ�. ����
 * �ؿ����֤��ͤ� 0 �ʲ����ä���硤���Ϥ�¨�����Ǥ��ƴؿ���λ���ޤ�. 
 * </JA>
 *
 * @param ad_process [in] function to process triggerted input.
 * @param ad_check [in] function to be called periodically.
 * @param recog [in] engine instance
 * 
 * @return 2 when input termination requested by ad_process(), 1 when
 * if detect end of an input segment (down trigger detected after up
 * trigger), 0 when reached end of input device, -1 on error, -2 when
 * input termination requested by ad_check().
 *
 * @callergraph
 * @callgraph
 *
 */
static int
adin_cut(int (*ad_process)(SP16 *, int, Recog *), int (*ad_check)(Recog *), Recog *recog)
{
  ADIn *a;
  int i;
  int ad_process_ret;
  int imax, len, cnt;
  int wstep;
  int end_status = 0;	/* return value */
  boolean transfer_online_local;	/* local repository of transfer_online */
  int zc;		/* count of zero cross */

  a = recog->adin;

  /*
   * there are 3 buffers:
   *   temporary storage queue: buffer[]
   *   cycle buffer for zero-cross counting: (in zc_e)
   *   swap buffer for re-starting after short tail silence
   *
   * Each samples are first read to buffer[], then passed to count_zc_e()
   * to find trigger.  Samples between trigger and end of speech are 
   * passed to (*ad_process) with pointer to the first sample and its length.
   *
   */

  if (a->need_init) {
    a->bpmax = MAXSPEECHLEN;
    a->bp = 0;
    a->is_valid_data = FALSE;
    /* reset zero-cross status */
    if (a->adin_cut_on) {
      reset_count_zc_e(&(a->zc), a->thres, a->c_length, a->c_offset);
    }
    a->end_of_stream = FALSE;
    a->nc = 0;
    a->sblen = 0;
    a->need_init = FALSE;		/* for next call */
  }

  /****************/
  /* resume input */
  /****************/
  //  if (!a->adin_cut_on && a->is_valid_data == TRUE) {
  //    callback_exec(CALLBACK_EVENT_SPEECH_START, recog);
  //  }

  /*************/
  /* main loop */
  /*************/
  for (;;) {

    /* check end of input by end of stream */
    if (a->end_of_stream && a->bp == 0) break;

    /****************************/
    /* read in new speech input */
    /****************************/
    if (a->end_of_stream) {
      /* already reaches end of stream, just process the rest */
      a->current_len = a->bp;
    } else {
      /*****************************************************/
      /* get samples from input device to temporary buffer */
      /*****************************************************/
      /* buffer[0..bp] is the current remaining samples */
      /*
	mic input - samples exist in a device buffer
        tcpip input - samples exist in a socket
        file input - samples in a file
	   
	Return value is the number of read samples.
	If no data exists in the device (in case of mic input), ad_read()
	will return 0.  If reached end of stream (in case end of file or
	receive end ack from tcpip client), it will return -1.
	If error, returns -2. If the device requests segmentation, returns -3.
      */
      if (a->down_sample) {
	/* get 48kHz samples to temporal buffer */
	cnt = (*(a->ad_read))(a->buffer48, (a->bpmax - a->bp) * a->io_rate);
      } else {
	cnt = (*(a->ad_read))(&(a->buffer[a->bp]), a->bpmax - a->bp);
      }
      if (cnt < 0) {		/* end of stream / segment or error */
	/* set the end status */
	switch(cnt) {
	case -1:		/* end of stream */
	  a->input_side_segment = FALSE;
	  end_status = 0;
	  break;
	case -2:
	  a->input_side_segment = FALSE;
	  end_status = -1;
	  break;
	case -3:
	  a->input_side_segment = TRUE;
	  end_status = 0;
	}
	/* now the input has been ended, 
	   we should not get further speech input in the next loop, 
	   instead just process the samples in the temporary buffer until
	   the entire data is processed. */
	a->end_of_stream = TRUE;		
	cnt = 0;			/* no new input */
	/* in case the first trial of ad_read() fails, exit this loop */
	if (a->bp == 0) break;
      }
      if (a->down_sample && cnt != 0) {
	/* convert to 16kHz  */
	cnt = ds48to16(&(a->buffer[a->bp]), a->buffer48, cnt, a->bpmax - a->bp, a->ds);
	if (cnt < 0) {		/* conversion error */
	  jlog("ERROR: adin_cut: error in down sampling\n");
	  end_status = -1;
	  a->end_of_stream = TRUE;
	  cnt = 0;
	  if (a->bp == 0) break;
	}
      }
      if (cnt > 0 && a->level_coef != 1.0) {
	/* scale the level of incoming input */
	for (i = a->bp; i < a->bp + cnt; i++) {
	  a->buffer[i] = (SP16) ((float)a->buffer[i] * a->level_coef);
	}
      }

      /*************************************************/
      /* execute callback here for incoming raw data stream.*/
      /* the content of buffer[bp...bp+cnt-1] or the   */
      /* length can be modified in the functions.      */
      /*************************************************/
      if (cnt > 0) {
#ifdef ENABLE_PLUGIN
	plugin_exec_adin_captured(&(a->buffer[a->bp]), cnt);
#endif
	callback_exec_adin(CALLBACK_ADIN_CAPTURED, recog, &(a->buffer[a->bp]), cnt);
	/* record total number of captured samples */
	a->total_captured_len += cnt;
      }

      /*************************************************/
      /* some speech processing for the incoming input */
      /*************************************************/
      if (cnt > 0) {
	if (a->strip_flag) {
	  /* strip off successive zero samples */
	  len = strip_zero(&(a->buffer[a->bp]), cnt);
	  if (len != cnt) cnt = len;
	}
	if (a->need_zmean) {
	  /* remove DC offset */
	  sub_zmean(&(a->buffer[a->bp]), cnt);
	}
      }
      
      /* current len = current samples in buffer */
      a->current_len = a->bp + cnt;
    }
#ifdef THREAD_DEBUG
    if (a->end_of_stream) {
      jlog("DEBUG: adin_cut: stream already ended\n");
    }
    if (cnt > 0) {
      jlog("DEBUG: adin_cut: get %d samples [%d-%d]\n", a->current_len - a->bp, a->bp, a->current_len);
    }
#endif

    /**************************************************/
    /* call the periodic callback (non threaded mode) */
    /*************************************************/
    /* this function is mainly for periodic checking of incoming command
       in module mode */
    /* in threaded mode, this will be done in process thread, not here in adin thread */
    if (ad_check != NULL
#ifdef HAVE_PTHREAD
	&& !a->enable_thread
#endif
	) {
      /* if ad_check() returns value < 0, termination of speech input is required */
      if ((i = (*ad_check)(recog)) < 0) { /* -1: soft termination -2: hard termination */
	//	if ((i == -1 && current_len == 0) || i == -2) {
 	if (i == -2 ||
	    (i == -1 && a->is_valid_data == FALSE)) {
	  end_status = -2;	/* recognition terminated by outer function */
	  /* execute callback */
	  if (a->current_len > 0) {
	    callback_exec(CALLBACK_EVENT_SPEECH_STOP, recog);
	  }
	  a->need_init = TRUE; /* bufer status shoule be reset at next call */
	  goto break_input;
	}
      }
    }

    /***********************************************************************/
    /* if no data has got but not end of stream, repeat next input samples */
    /***********************************************************************/
    if (a->current_len == 0) continue;

    /* When not adin_cut mode, all incoming data is valid.
       So is_valid_data should be set to TRUE when some input first comes
       till this input ends.  So, if some data comes, set is_valid_data to
       TRUE here. */ 
    if (!a->adin_cut_on && a->is_valid_data == FALSE && a->current_len > 0) {
      a->is_valid_data = TRUE;
      callback_exec(CALLBACK_EVENT_SPEECH_START, recog);
    }

    /******************************************************/
    /* prepare for processing samples in temporary buffer */
    /******************************************************/
    
    wstep = a->chunk_size;	/* process unit (should be smaller than cycle buffer) */

    /* imax: total length that should be processed at one ad_read() call */
    /* if in real-time mode and not threaded, recognition process 
       will be called and executed as the ad_process() callback within
       this function.  If the recognition speed is over the real time,
       processing all the input samples at the loop below may result in the
       significant delay of getting next input, that may result in the buffer
       overflow of the device (namely a microphone device will suffer from
       this). So, in non-threaded mode, in order to avoid buffer overflow and
       input frame dropping, we will leave here by processing 
       only one segment [0..wstep], and leave the rest in the temporary buffer.
    */
#ifdef HAVE_PTHREAD
    if (a->enable_thread) imax = a->current_len; /* process whole */
    else imax = (a->current_len < wstep) ? a->current_len : wstep; /* one step */
#else
    imax = (a->current_len < wstep) ? a->current_len : wstep;	/* one step */
#endif
    
    /* wstep: unit length for the loop below */
    if (wstep > a->current_len) wstep = a->current_len;

#ifdef THREAD_DEBUG
    jlog("DEBUG: process %d samples by %d step\n", imax, wstep);
#endif

#ifdef HAVE_PTHREAD
    if (a->enable_thread) {
      /* get transfer status to local */
      pthread_mutex_lock(&(a->mutex));
      transfer_online_local = a->transfer_online;
      pthread_mutex_unlock(&(a->mutex));
    }
#endif

    /*********************************************************/
    /* start processing buffer[0..current_len] by wstep step */
    /*********************************************************/
    i = 0;
    while (i + wstep <= imax) {

      if (a->adin_cut_on) {

	/********************/
	/* check triggering */
	/********************/
	/* the cycle buffer in count_zc_e() holds the last
	   samples of (head_margin) miliseconds, and the zerocross
	   over the threshold level are counted within the cycle buffer */
	
	/* store the new data to cycle buffer and update the count */
	/* return zero-cross num in the cycle buffer */
	zc = count_zc_e(&(a->zc), &(a->buffer[i]), wstep);
	
	if (zc > a->noise_zerocross) { /* now triggering */
	  
	  if (a->is_valid_data == FALSE) {
	    /*****************************************************/
	    /* process off, trigger on: detect speech triggering */
	    /*****************************************************/
	    a->is_valid_data = TRUE;   /* start processing */
	    a->nc = 0;
#ifdef THREAD_DEBUG
	    jlog("DEBUG: detect on\n");
#endif
	    /* record time */
	    a->last_trigger_sample = a->total_captured_len - a->current_len + i + wstep - a->zc.valid_len;
	    callback_exec(CALLBACK_EVENT_SPEECH_START, recog);
	    a->last_trigger_len = 0;
	    if (a->zc.valid_len > wstep) {
	      a->last_trigger_len += a->zc.valid_len - wstep;
	    }

	    /****************************************/
	    /* flush samples stored in cycle buffer */
	    /****************************************/
	    /* (last (head_margin) msec samples */
	    /* if threaded mode, processing means storing them to speech[].
	       if ignore_speech_while_recog is on (default), ignore the data
	       if transfer is offline (=while processing second pass).
	       Else, datas are stored even if transfer is offline */
	    if ( ad_process != NULL
#ifdef HAVE_PTHREAD
		 && (!a->enable_thread || !a->ignore_speech_while_recog || transfer_online_local)
#endif
		 ) {
	      /* copy content of cycle buffer to cbuf */
	      zc_copy_buffer(&(a->zc), a->cbuf, &len);
	      /* Note that the last 'wstep' samples are the same as
		 the current samples 'buffer[i..i+wstep]', and
		 they will be processed later.  So, here only the samples
		 cbuf[0...len-wstep] will be processed
	      */
	      if (len - wstep > 0) {
#ifdef THREAD_DEBUG
		jlog("DEBUG: callback for buffered samples (%d bytes)\n", len - wstep);
#endif
#ifdef ENABLE_PLUGIN
		plugin_exec_adin_triggered(a->cbuf, len - wstep);
#endif
		callback_exec_adin(CALLBACK_ADIN_TRIGGERED, recog, a->cbuf, len - wstep);
		ad_process_ret = (*ad_process)(a->cbuf, len - wstep, recog);
		switch(ad_process_ret) {
		case 1:		/* segmentation notification from process callback */
#ifdef HAVE_PTHREAD
		  if (a->enable_thread) {
		    /* in threaded mode, just stop transfer */
		    pthread_mutex_lock(&(a->mutex));
		    a->transfer_online = transfer_online_local = FALSE;
		    pthread_mutex_unlock(&(a->mutex));
		  } else {
		    /* in non-threaded mode, set end status and exit loop */
		    end_status = 2;
		    adin_purge(a, i);
		    goto break_input;
		  }
		  break;
#else
		  /* in non-threaded mode, set end status and exit loop */
		  end_status = 2;
		  adin_purge(a, i);
		  goto break_input;
#endif
		case -1:		/* error occured in callback */
		  /* set end status and exit loop */
		  end_status = -1;
		  goto break_input;
		}
	      }
	    }
	    
	  } else {		/* is_valid_data == TRUE */
	    /******************************************************/
	    /* process on, trigger on: we are in a speech segment */
	    /******************************************************/
	    
	    if (a->nc > 0) {
	      
	      /*************************************/
	      /* re-triggering in trailing silence */
	      /*************************************/
	      
#ifdef THREAD_DEBUG
	      jlog("DEBUG: re-triggered\n");
#endif
	      /* reset noise counter */
	      a->nc = 0;

	      if (a->sblen > 0) {
		a->last_trigger_len += a->sblen;
	      }

#ifdef TMP_FIX_200602
	      if (ad_process != NULL
#ifdef HAVE_PTHREAD
		  && (!a->enable_thread || !a->ignore_speech_while_recog || transfer_online_local)
#endif
		  ) {
#endif
	      
	      /*************************************************/
	      /* process swap buffer stored while tail silence */
	      /*************************************************/
	      /* In trailing silence, the samples within the tail margin length
		 will be processed immediately, but samples after the tail
		 margin will not be processed, instead stored in swapbuf[].
		 If re-triggering occurs while in the trailing silence,
		 the swapped samples should be processed now to catch up
		 with current input
	      */
	      if (a->sblen > 0) {
#ifdef THREAD_DEBUG
		jlog("DEBUG: callback for swapped %d samples\n", a->sblen);
#endif
#ifdef ENABLE_PLUGIN
		plugin_exec_adin_triggered(a->swapbuf, a->sblen);
#endif
		callback_exec_adin(CALLBACK_ADIN_TRIGGERED, recog, a->swapbuf, a->sblen);
		ad_process_ret = (*ad_process)(a->swapbuf, a->sblen, recog);
		a->sblen = 0;
		switch(ad_process_ret) {
		case 1:		/* segmentation notification from process callback */
#ifdef HAVE_PTHREAD
		  if (a->enable_thread) {
		    /* in threaded mode, just stop transfer */
		    pthread_mutex_lock(&(a->mutex));
		    a->transfer_online = transfer_online_local = FALSE;
		    pthread_mutex_unlock(&(a->mutex));
		  } else {
		    /* in non-threaded mode, set end status and exit loop */
		    end_status = 2;
		    adin_purge(a, i);
		    goto break_input;
		  }
		  break;
#else
		  /* in non-threaded mode, set end status and exit loop */
		  end_status = 2;
		  adin_purge(a, i);
		  goto break_input;
#endif
		case -1:		/* error occured in callback */
		  /* set end status and exit loop */
		  end_status = -1;
		  goto break_input;
		}
	      }
#ifdef TMP_FIX_200602
	      }
#endif
	    }
	  } 
	} else if (a->is_valid_data == TRUE) {
	  
	  /*******************************************************/
	  /* process on, trigger off: processing tailing silence */
	  /*******************************************************/
	  
#ifdef THREAD_DEBUG
	  jlog("DEBUG: TRAILING SILENCE\n");
#endif
	  if (a->nc == 0) {
	    /* start of tail silence: prepare valiables for start swapbuf[] */
	    a->rest_tail = a->sbsize - a->c_length;
	    a->sblen = 0;
#ifdef THREAD_DEBUG
	    jlog("DEBUG: start tail silence, rest_tail = %d\n", a->rest_tail);
#endif
	  }

	  /* increment noise counter */
	  a->nc++;
	}
      }	/* end of triggering handlers */
      
      
      /********************************************************************/
      /* process the current segment buffer[i...i+wstep] if process == on */
      /********************************************************************/
      
      if (a->adin_cut_on && a->is_valid_data && a->nc > 0 && a->rest_tail == 0) {
	
	/* The current trailing silence is now longer than the user-
	   specified tail margin length, so the current samples
	   should not be processed now.  But if 're-triggering'
	   occurs in the trailing silence later, they should be processed
	   then.  So we just store the overed samples in swapbuf[] and
	   not process them now */
	
#ifdef THREAD_DEBUG
	jlog("DEBUG: tail silence over, store to swap buffer (nc=%d, rest_tail=%d, sblen=%d-%d)\n", a->nc, a->rest_tail, a->sblen, a->sblen+wstep);
#endif
	if (a->sblen + wstep > a->sbsize) {
	  jlog("ERROR: adin_cut: swap buffer for re-triggering overflow\n");
	}
	memcpy(&(a->swapbuf[a->sblen]), &(a->buffer[i]), wstep * sizeof(SP16));
	a->sblen += wstep;
	
      } else {

	/* we are in a normal speech segment (nc == 0), or
	   trailing silence (shorter than tail margin length) (nc>0,rest_tail>0)
	   The current trailing silence is shorter than the user-
	   specified tail margin length, so the current samples
	   should be processed now as same as the normal speech segment */
	
#ifdef TMP_FIX_200602
	if (!a->adin_cut_on || a->is_valid_data == TRUE) {
#else
	if(
	   (!a->adin_cut_on || a->is_valid_data == TRUE)
#ifdef HAVE_PTHREAD
	   && (!a->enable_thread || !a->ignore_speech_while_recog || transfer_online_local)
#endif
	   ) {
#endif
	  if (a->nc > 0) {
	    /* if we are in a trailing silence, decrease the counter to detect
	     start of swapbuf[] above */
	    if (a->rest_tail < wstep) a->rest_tail = 0;
	    else a->rest_tail -= wstep;
#ifdef THREAD_DEBUG
	    jlog("DEBUG: %d processed, rest_tail=%d\n", wstep, a->rest_tail);
#endif
	  }
	  a->last_trigger_len += wstep;

#ifdef TMP_FIX_200602
	  if (ad_process != NULL
#ifdef HAVE_PTHREAD
	      && (!a->enable_thread || !a->ignore_speech_while_recog || transfer_online_local)
#endif
	      ) {

#else
	  if ( ad_process != NULL ) {
#endif
#ifdef THREAD_DEBUG
	    jlog("DEBUG: callback for input sample [%d-%d]\n", i, i+wstep);
#endif
	    /* call external function */
#ifdef ENABLE_PLUGIN
	    plugin_exec_adin_triggered(&(a->buffer[i]), wstep);
#endif
	    callback_exec_adin(CALLBACK_ADIN_TRIGGERED, recog, &(a->buffer[i]), wstep);
	    ad_process_ret = (*ad_process)(&(a->buffer[i]), wstep, recog);
	    switch(ad_process_ret) {
	    case 1:		/* segmentation notification from process callback */
#ifdef HAVE_PTHREAD
	      if (a->enable_thread) {
		/* in threaded mode, just stop transfer */
		pthread_mutex_lock(&(a->mutex));
		a->transfer_online = transfer_online_local = FALSE;
		pthread_mutex_unlock(&(a->mutex));
	      } else {
		/* in non-threaded mode, set end status and exit loop */
		adin_purge(a, i+wstep);
		end_status = 2;
		goto break_input;
	      }
	      break;
#else
	      /* in non-threaded mode, set end status and exit loop */
	      adin_purge(a, i+wstep);
	      end_status = 2;
	      goto break_input;
#endif
	    case -1:		/* error occured in callback */
	      /* set end status and exit loop */
	      end_status = -1;
	      goto break_input;
	    }
	  }
	}
      }	/* end of current segment processing */

      
      if (a->adin_cut_on && a->is_valid_data && a->nc >= a->nc_max) {
	/*************************************/
	/* process on, trailing silence over */
	/* = end of input segment            */
	/*************************************/
#ifdef THREAD_DEBUG
	jlog("DEBUG: detect off\n");
#endif
	/* end input by silence */
	a->is_valid_data = FALSE;	/* turn off processing */
	a->sblen = 0;
	callback_exec(CALLBACK_EVENT_SPEECH_STOP, recog);
#ifdef HAVE_PTHREAD
	if (a->enable_thread) { /* just stop transfer */
	  pthread_mutex_lock(&(a->mutex));
	  a->transfer_online = transfer_online_local = FALSE;
	  pthread_mutex_unlock(&(a->mutex));
	} else {
	  adin_purge(a, i+wstep);
	  end_status = 1;
	  goto break_input;
	}
#else
	adin_purge(a, i+wstep);
	end_status = 1;
	goto break_input;
#endif
      }

      /*********************************************************/
      /* end of processing buffer[0..current_len] by wstep step */
      /*********************************************************/
      i += wstep;		/* increment to next wstep samples */
    }
    
    /* purge processed samples and update queue */
    adin_purge(a, i);

  }

break_input:

  /****************/
  /* pause input */
  /****************/
  if (a->end_of_stream) {			/* input already ends */
    /* execute callback */
    callback_exec(CALLBACK_EVENT_SPEECH_STOP, recog);
    if (a->bp == 0) {		/* rest buffer successfully flushed */
      /* reset status */
      a->need_init = TRUE;		/* bufer status shoule be reset at next call */
    }
    if (end_status >= 0) {
      end_status = (a->bp) ? 1 : 0;
    }
  }
  
  return(end_status);
}

#ifdef HAVE_PTHREAD
/***********************/
/* threading functions */
/***********************/

/*************************/
/* adin thread functions */
/*************************/

/**
 * <EN>
 * Callback to store triggered samples within A/D-in thread.
 * </EN>
 * <JA>
 * A/D-in ����åɤˤƥȥꥬ�������ϥ���ץ����¸���륳����Хå�.
 * </JA>
 * 
 * @param now [in] triggered fragment
 * @param len [in] length of above
 * @param recog [in] engine instance
 * 
 * @return always 0, to tell caller to just continue the input
 */
static int
adin_store_buffer(SP16 *now, int len, Recog *recog)
{
  ADIn *a;

  a = recog->adin;
  if (a->speechlen + len > MAXSPEECHLEN) {
    /* just mark as overflowed, and continue this thread */
    pthread_mutex_lock(&(a->mutex));
    a->adinthread_buffer_overflowed = TRUE;
    pthread_mutex_unlock(&(a->mutex));
    return(0);
  }
  pthread_mutex_lock(&(a->mutex));
  memcpy(&(a->speech[a->speechlen]), now, len * sizeof(SP16));
  a->speechlen += len;
  pthread_mutex_unlock(&(a->mutex));
#ifdef THREAD_DEBUG
  jlog("DEBUG: input: stored %d samples, total=%d\n", len, a->speechlen);
#endif

  return(0);			/* continue */
}

/**
 * <EN>
 * A/D-in thread main function.
 * </EN>
 * <JA>
 * A/D-in ����åɤΥᥤ��ؿ�.
 * </JA>
 * 
 * @param dummy [in] a dummy data, not used.
 */
static void
adin_thread_input_main(void *dummy)
{
  Recog *recog;
  int ret;

  recog = dummy;

  ret = adin_cut(adin_store_buffer, NULL, recog);

  if (ret == -2) {		/* termination request by ad_check? */
    jlog("Error: adin thread exit with termination request by checker\n");
  } else if (ret == -1) {	/* error */
    jlog("Error: adin thread exit with error\n");
  } else if (ret == 0) {	/* EOF */
    jlog("Stat: adin thread end with EOF\n");
  }
  recog->adin->adinthread_ended = TRUE;

  /* return to end this thread */
}

/**
 * <EN>
 * Start new A/D-in thread, and initialize buffer.
 * </EN>
 * <JA>
 * �Хåե����������� A/D-in ����åɤ򳫻Ϥ���. 
 * </JA>
 * @param recog [in] engine instance
 *
 * @callergraph
 * @callgraph
 */
boolean
adin_thread_create(Recog *recog)
{
  ADIn *a;

  a = recog->adin;

  /* init storing buffer */
  a->speech = (SP16 *)mymalloc(sizeof(SP16) * MAXSPEECHLEN);
  a->speechlen = 0;

  a->transfer_online = FALSE; /* tell adin-mic thread to wait at initial */
  a->adinthread_buffer_overflowed = FALSE;
  a->adinthread_ended = FALSE;

  if (pthread_mutex_init(&(a->mutex), NULL) != 0) { /* error */
    jlog("ERROR: adin_thread_create: failed to initialize mutex\n");
    return FALSE;
  }
  if (pthread_create(&(recog->adin->adin_thread), NULL, (void *)adin_thread_input_main, recog) != 0) {
    jlog("ERROR: adin_thread_create: failed to create AD-in thread\n");
    return FALSE;
  }
  if (pthread_detach(recog->adin->adin_thread) != 0) { /* not join, run forever */
    jlog("ERROR: adin_thread_create: failed to detach AD-in thread\n");
    return FALSE;
  }
  jlog("STAT: AD-in thread created\n");
  return TRUE;
}

/**
 * <EN>
 * Delete A/D-in thread
 * </EN>
 * <JA>
 * A/D-in ����åɤ�λ����
 * </JA>
 * @param recog [in] engine instance
 *
 * @callergraph
 * @callgraph
 */
boolean
adin_thread_cancel(Recog *recog)
{
  int ret;

  if (recog->adin->adinthread_ended) return TRUE;

  /* send a cencellation request to the A/D-in thread */
  ret = pthread_cancel(recog->adin->adin_thread);
  if (ret != 0) {
    if (ret == ESRCH) {
      jlog("STAT: adin_thread_cancel: no A/D-in thread\n");
      recog->adin->adinthread_ended = TRUE;
      return TRUE;
    } else {
      jlog("Error: adin_thread_cancel: failed to cancel A/D-in thread\n");
      return FALSE;
    }
  }
  /* wait for the thread to terminate */
  ret = pthread_join(recog->adin->adin_thread, NULL);
  if (ret != 0) {
    if (ret == EINVAL) {
      jlog("InternalError: adin_thread_cancel: AD-in thread is invalid\n");
      recog->adin->adinthread_ended = TRUE;
      return FALSE;
    } else if (ret == ESRCH) {
      jlog("STAT: adin_thread_cancel: no A/D-in thread\n");
      recog->adin->adinthread_ended = TRUE;
      return TRUE;
    } else if (ret == EDEADLK) {
      jlog("InternalError: adin_thread_cancel: dead lock or self thread?\n");
      recog->adin->adinthread_ended = TRUE;
      return FALSE;
    } else {
      jlog("Error: adin_thread_cancel: failed to wait end of A/D-in thread\n");
      return FALSE;
    }
  }

  jlog("STAT: AD-in thread deleted\n");
  recog->adin->adinthread_ended = TRUE;
  return TRUE;
}

/****************************/
/* process thread functions */
/****************************/

/**
 * <EN>
 * @brief  Main processing function for thread mode.
 *
 * It waits for the new samples to be stored in @a speech by A/D-in thread,
 * and if found, process them.  The interface are the same as adin_cut().
 * </EN>
 * <JA>
 * @brief  ����åɥ⡼���ѥᥤ��ؿ�
 *
 * ���δؿ��� A/D-in ����åɤˤ�äƥ���ץ뤬��¸�����Τ��Ԥ���
 * ��¸���줿����ץ��缡�������Ƥ����ޤ�. �������֤��ͤ� adin_cut() ��
 * Ʊ��Ǥ�. 
 * </JA>
 * 
 * @param ad_process [in] function to process triggerted input.
 * @param ad_check [in] function to be called periodically.
 * @param recog [in] engine instance
 * 
 * @return 2 when input termination requested by ad_process(), 1 when
 * if detect end of an input segment (down trigger detected after up
 * trigger), 0 when reached end of input device, -1 on error, -2 when
 * input termination requested by ad_check().
 */
static int
adin_thread_process(int (*ad_process)(SP16 *, int, Recog *), int (*ad_check)(Recog *), Recog *recog)
{
  int prev_len, nowlen;
  int ad_process_ret;
  int i;
  boolean overflowed_p;
  boolean transfer_online_local;
  boolean ended_p;
  ADIn *a;

  a = recog->adin;

  /* reset storing buffer --- input while recognition will be ignored */
  pthread_mutex_lock(&(a->mutex));
  /*if (speechlen == 0) transfer_online = TRUE;*/ /* tell adin-mic thread to start recording */
  a->transfer_online = TRUE;
#ifdef THREAD_DEBUG
  jlog("DEBUG: process: reset, speechlen = %d, online=%d\n", a->speechlen, a->transfer_online);
#endif
  a->adinthread_buffer_overflowed = FALSE;
  pthread_mutex_unlock(&(a->mutex));

  /* main processing loop */
  prev_len = 0;
  for(;;) {
    /* get current length (locking) */
    pthread_mutex_lock(&(a->mutex));
    nowlen = a->speechlen;
    overflowed_p = a->adinthread_buffer_overflowed;
    transfer_online_local = a->transfer_online;
    ended_p = a->adinthread_ended;
    pthread_mutex_unlock(&(a->mutex));
    /* check if thread is alive */
    if (ended_p) {
      /* adin thread has already exited, so return EOF to stop this input */
      return(0);
    }
    /* check if other input thread has overflowed */
    if (overflowed_p) {
      jlog("WARNING: adin_thread_process: too long input (> %d samples), segmented now\n", MAXSPEECHLEN);
      /* segment input here */
      pthread_mutex_lock(&(a->mutex));
      a->speechlen = 0;
      a->transfer_online = transfer_online_local = FALSE;
      pthread_mutex_unlock(&(a->mutex));
      return(1);		/* return with segmented status */
    }
    /* callback poll */
    if (ad_check != NULL) {
      if ((i = (*(ad_check))(recog)) < 0) {
	if ((i == -1 && nowlen == 0) || i == -2) {
	  pthread_mutex_lock(&(a->mutex));
	  a->transfer_online = transfer_online_local = FALSE;
	  a->speechlen = 0;
	  pthread_mutex_unlock(&(a->mutex));
	  return(-2);
	}
      }
    }
    if (prev_len < nowlen && nowlen <= a->freezelen) {
#ifdef THREAD_DEBUG
      jlog("DEBUG: process: proceed [%d-%d]\n",prev_len, nowlen);
#endif
      /* got new sample, process */
      /* As the speech[] buffer is monotonously increase,
	 content of speech buffer [prev_len..nowlen] would not alter
	 in both threads
	 So locking is not needed while processing.
       */
      /*jlog("DEBUG: main: read %d-%d\n", prev_len, nowlen);*/
      if (ad_process != NULL) {
	ad_process_ret = (*ad_process)(&(a->speech[prev_len]), nowlen - prev_len, recog);
#ifdef THREAD_DEBUG
	jlog("DEBUG: ad_process_ret=%d\n", ad_process_ret);
#endif
	switch(ad_process_ret) {
	case 1:			/* segmented */
	  /* segmented by callback function */
	  /* purge processed samples and keep transfering */
	  pthread_mutex_lock(&(a->mutex));
	  if(a->speechlen > nowlen) {
	    memmove(a->speech, &(a->speech[nowlen]), (a->speechlen - nowlen) * sizeof(SP16));
	    a->speechlen -= nowlen;
	  } else {
	    a->speechlen = 0;
	  }
	  a->transfer_online = transfer_online_local = FALSE;
	  pthread_mutex_unlock(&(a->mutex));
	  /* keep transfering */
	  return(2);		/* return with segmented status */
	case -1:		/* error */
	  pthread_mutex_lock(&(a->mutex));
	  a->transfer_online = transfer_online_local = FALSE;
	  pthread_mutex_unlock(&(a->mutex));
	  return(-1);		/* return with error */
	}
      }
      if (a->rehash) {
	/* rehash */
	pthread_mutex_lock(&(a->mutex));
	if (debug2_flag) jlog("STAT: adin_cut: rehash from %d to %d\n", a->speechlen, a->speechlen - prev_len);
	a->speechlen -= prev_len;
	nowlen -= prev_len;
	memmove(a->speech, &(a->speech[prev_len]), a->speechlen * sizeof(SP16));
	pthread_mutex_unlock(&(a->mutex));
	a->rehash = FALSE;
      }
      prev_len = nowlen;
    } else {
      if (transfer_online_local == FALSE) {
	/* segmented by zero-cross */
	/* reset storing buffer for next input */
	pthread_mutex_lock(&(a->mutex));
	a->speechlen = 0;
	pthread_mutex_unlock(&(a->mutex));
        break;
      }
      usleep(50000);   /* wait = 0.05sec*/            
    }
  }

  /* as threading assumes infinite input */
  /* return value should be 1 (segmented) */
  return(1);
}
#endif /* HAVE_PTHREAD */




/**
 * <EN>
 * @brief  Top function to start input processing
 *
 * If threading mode is enabled, this function simply enters to
 * adin_thread_process() to process triggered samples detected by
 * another running A/D-in thread.
 *
 * If threading mode is not available or disabled by either device requirement
 * or OS capability, this function simply calls adin_cut() to detect speech
 * segment from input device and process them concurrently by one process.
 * </EN>
 * <JA>
 * @brief  ���Ͻ�����Ԥ��ȥå״ؿ�
 *
 * ����åɥ⡼�ɤǤϡ����δؿ��� adin_thead_process() ��ƤӽФ���
 * �󥹥�åɥ⡼�ɤǤ� adin_cut() ��ľ�ܸƤӽФ�. �������֤��ͤ�
 * adin_cut() ��Ʊ��Ǥ���. 
 * </JA>
 * 
 * @param ad_process [in] function to process triggerted input.
 * @param ad_check [in] function to be called periodically.
 * @param recog [in] engine instance
 * 
 * @return 2 when input termination requested by ad_process(), 1 when
 * if detect end of an input segment (down trigger detected after up
 * trigger), 0 when reached end of input device, -1 on error, -2 when
 * input termination requested by ad_check().
 *
 * @callergraph
 * @callgraph
 * 
 */
int
adin_go(int (*ad_process)(SP16 *, int, Recog *), int (*ad_check)(Recog *), Recog *recog)
{
  /* output listening start message */
  callback_exec(CALLBACK_EVENT_SPEECH_READY, recog);
#ifdef HAVE_PTHREAD
  if (recog->adin->enable_thread) {
    return(adin_thread_process(ad_process, ad_check, recog));
  }
#endif
  return(adin_cut(ad_process, ad_check, recog));
}

/** 
 * <EN>
 * Call device-specific initialization.
 * </EN>
 * <JA>
 * �ǥХ�����¸�ν�����ؿ���ƤӽФ�. 
 * </JA>
 * 
 * @param a [in] A/D-in work area
 * @param freq [in] sampling frequency
 * @param arg [in] device-dependent argument
 * 
 * @return TRUE if succeeded, FALSE if failed.
 * 
 * @callergraph
 * @callgraph
 * 
 */
boolean
adin_standby(ADIn *a, int freq, void *arg)
{
  if (a->need_zmean) zmean_reset();
  if (a->ad_standby != NULL) return(a->ad_standby(freq, arg));
  return TRUE;
}
/** 
 * <EN>
 * Call device-specific function to begin capturing of the audio stream.
 * </EN>
 * <JA>
 * ���μ����ߤ򳫻Ϥ���ǥХ�����¸�δؿ���ƤӽФ�. 
 * </JA>
 * 
 * @param a [in] A/D-in work area
 * @param file_or_dev_name [in] device / file path to open or NULL for default
 * 
 * @return TRUE on success, FALSE on failure.
 * 
 * @callergraph
 * @callgraph
 * 
 */
boolean
adin_begin(ADIn *a, char *file_or_dev_name)
{
  if (debug2_flag && a->input_side_segment) jlog("Stat: adin_begin: skip\n");
  if (a->input_side_segment == FALSE) {
    a->total_captured_len = 0;
    a->last_trigger_len = 0;
    if (a->need_zmean) zmean_reset();
    if (a->ad_begin != NULL) return(a->ad_begin(file_or_dev_name));
  }
  return TRUE;
}
/** 
 * <EN>
 * Call device-specific function to end capturing of the audio stream.
 * </EN>
 * <JA>
 * ���μ����ߤ�λ����ǥХ�����¸�δؿ���ƤӽФ�. 
 * </JA>
 * 
 * @param a [in] A/D-in work area
 * 
 * @return TRUE on success, FALSE on failure.
 *
 * @callergraph
 * @callgraph
 */
boolean
adin_end(ADIn *a)
{
  if (debug2_flag && a->input_side_segment) jlog("Stat: adin_end: skip\n");
  if (a->input_side_segment == FALSE) {
    if (a->ad_end != NULL) return(a->ad_end());
  }
  return TRUE;
}

/** 
 * <EN>
 * Free memories of A/D-in work area.
 * </EN>
 * <JA>
 * ���������ѥ�����ꥢ�Υ����������. 
 * </JA>
 * 
 * @param recog [in] engine instance
 *
 * @callergraph
 * @callgraph
 * 
 */
void
adin_free_param(Recog *recog)
{
  ADIn *a;

  a = recog->adin;

  if (a->ds) {
    ds48to16_free(a->ds);
    a->ds = NULL;
  }
  if (a->adin_cut_on) {
    free_count_zc_e(&(a->zc));
  }
  if (a->down_sample) {
    free(a->buffer48);
  }
  free(a->swapbuf);
  free(a->cbuf);
  free(a->buffer);
#ifdef HAVE_PTHREAD
  if (a->speech) free(a->speech);
#endif
}

/* end of file */
