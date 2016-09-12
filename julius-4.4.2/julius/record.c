/**
 * @file   record.c
 * 
 * <JA>
 * @brief  ǧ���������ϲ�����ե��������¸����. 
 *
 * ���Ϥ��줿�����ǡ�����1�Ĥ��ĥե��������¸����. 
 * �ե�����̾��Ͽ�����Υ����ॹ����פ��� "YYYY.MMDD.HHMMSS.wav" �Ȥʤ�. 
 * �ե���������� Microsoft WAVE format, 16bit, PCM (̵����) �Ǥ���. 
 *
 * Ͽ���Ϥ��ä������˳��ݤ��줺�ˡ����Ϥ�ʿ�Ԥ��ƥե������ľ��
 * �񤭹��ޤ�롣�ǽ�ϰ���ե�����˵�Ͽ���졢Ͽ����λ��ʡ���1�ѥ�
 * ��λ��ˤ˾嵭�η����Υե�����̾���ѹ�����롣
 * </JA>
 * 
 * <EN>
 * @brief  Record speech inputs into successive files.
 *
 * These functions record each input data to the corresponding file with
 * file name of their time stamp in a format of "YYYY.MMDD.HHMMSS.wav".
 * The audio format is Microsoft WAVE, 16bit, PCM (no compression).
 *
 * The recording will not be stored in memory, instead it will be directly
 * recorded to a temporary file on the fly.  After an end-of-sentence found
 * and the first pass ends, the temporary file will be moved to the
 * final filename descrived above.
 * </EN>
 * 
 * @author Akinobu Lee
 * @date   Tue Sep 06 14:13:54 2005
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

#include "app.h"
#include <time.h>

static char *record_dirname = NULL;

static char rectmpfilename[MAXLINELEN];
static char recordfilename[MAXLINELEN];
static int recordlen;
static FILE *recfile_fp = NULL;
static boolean open_error = FALSE;

/** 
 * <JA>
 * �����ƥ���֤���١����ե�����̾���������. 
 * 
 * @param t [out] ��̤��Ǽ����ʸ����Хåե�
 * @param maxlen [in] @a t �κ���Ĺ
 * </JA>
 * <EN>
 * Make base filename string from current system time.
 * 
 * @param t [out] string buffer to hold the result string.
 * @param maxlen [in] the length of @a t.
 * </EN>
 */
static void
timestring(char *t, int maxlen)
{
  time_t timep;
  struct tm *lmtm;

  time(&timep);
  lmtm = localtime(&timep);

  snprintf ( t, maxlen,"%04d.%02d%02d.%02d%02d%02d", 1900+lmtm->tm_year, 1+lmtm->tm_mon, lmtm->tm_mday, lmtm->tm_hour, lmtm->tm_min, lmtm->tm_sec);
}

/** 
 * <JA>
 * �١����ե�����̾����ºݤΥѥ�̾���������. �ǥ��쥯�ȥ������ѿ�
 * record_dirname �Ǥ��餫������ꤵ��Ƥ���. 
 * 
 * @param buf [out] ��̤Υѥ�̾���Ǽ����Хåե��ؤΥݥ���
 * @param buflen [in] @a buf �κ���Ĺ
 * @param filename [in] �١����ե�����̾
 * </JA>
 * <EN>
 * Make actual file path name from base file name.  The recording directory
 * should be specified by the global variable "record_dirname".
 * 
 * @param buf [out] buffer to hold the result string of this function
 * @param buflen [in] maximum length of @a buf.
 * @param filename [in] base filename.
 * </EN>
 */
static void
make_record_filename(char *buf, int buflen, char *basename, char *dirname)
{
  if (dirname == NULL) {
    fprintf(stderr, "no record directory specified??\n");
    return;
  }
  snprintf(buf, buflen,
#if defined(_WIN32) && !defined(__CYGWIN32__)
	   "%s\\%s.wav"
#else
	   "%s/%s.wav"
#endif
	   , dirname, basename);
}

/** 
 * <JA>
 * ����ե�����̾���������. 
 * 
 * @param buf [out] ��̤Υե�����̾���Ǽ����ݥ���
 * @param buflen [in] @a buf �κ���Ĺ
 * </JA>
 * <EN>
 * Make temporary filename to store the incoming data while recording.
 * 
 * @param buf [out] pointer of buffer to hold the resulting file name.
 * @param buflen [in] maximum length of @a buf.
 * </EN>
 */
static void
make_tmp_filename(char *buf, int buflen, char *dirname)
{
#if defined(_WIN32) && !defined(__CYGWIN32__)
  snprintf(buf, buflen, "%s\\tmprecord.000", dirname);
#else
  snprintf(buf, buflen, "%s/tmprecord.%d", dirname, getpid());
#endif
}  

/** 
 * <JA>
 * Ͽ���Τ���˰���ե�����򥪡��ץ󤹤�. 
 * 
 * </JA>
 * <EN>
 * Open temporary file for starting recording.
 * 
 * </EN>
 */
static void
record_sample_open(Recog *recog, void *dummy)
{
  if (recfile_fp != NULL) {
    fprintf(stderr, "Error: record_sample_open: re-opened before closed!\n"); 
    return;
  }

  make_tmp_filename(rectmpfilename, MAXLINELEN, record_dirname);
  if ((recfile_fp = wrwav_open(rectmpfilename, recog->jconf->input.sfreq)) == NULL) {
    perror("Error: record_sample_open");
    fprintf(stderr, "failed to open \"%s\" (temporary record file)\n", rectmpfilename);
    open_error = TRUE;
    return;
  }

  recordlen = 0;
}

/** 
 * <JA>
 * ���ϲ������Ҥ�ե�������ɲõ�Ͽ����. 
 * 
 * @param speech [in] �����ǡ����ΥХåե�
 * @param samplenum [in] �����ǡ�����Ĺ���ʥ���ץ����
 * </JA>
 * <EN>
 * Append speech segment to file previously opened by record_sample_open().
 * 
 * @param speech [in] speech buffer 
 * @param samplenum [in] length of above in samples
 * </EN>
 */
static void
record_sample_write(Recog *recog, SP16 *speech, int samplenum, void *dummy)
{
  static char tstr[20];

  if (recfile_fp == NULL) {
    if (! open_error) record_sample_open(recog, dummy);
  }

  if (wrwav_data(recfile_fp, speech, samplenum) == FALSE) {
    perror("Error: record_sample_write");
    fprintf(stderr, "failed to write samples to \"%s\"\n", rectmpfilename);
    return;
  }

  /* make timestamp of system time when an input begins */
  /* the current temporal recording file will be renamed to this time-stamp filename */
  if (recordlen == 0) {		/* beginning of recording */
    timestring(tstr, 18);
  }
  make_record_filename(recordfilename, MAXLINELEN, tstr, record_dirname);
  
  recordlen += samplenum;
}

/** 
 * <JA>
 * Ͽ����λ����. Ͽ���Ѥΰ���ե�����򥯥������������̾����rename���롣
 * 
 * </JA>
 * <EN>
 * End recording.  Close the current temporary recording file, and
 * rename the temporary file to the final time-stamp file name.
 * 
 * </EN>
 */
static void
record_sample_close(Recog *recog, void *dummy)
{
  open_error = FALSE;
  if (recfile_fp == NULL) {
    fprintf(stderr, "Warning: record_sample_close; file not opened yet!?\n");
    return;
  }

  if (wrwav_close(recfile_fp) == FALSE) {
    perror("Error: record_sample_close");
  }
  recfile_fp = NULL;

  if (recordlen == 0) {
    unlink(rectmpfilename);
    if (verbose_flag) {
      fprintf(stderr, "No input, not recorded\n");
    }
    return;
  }

  /* now rename the temporary file to time-stamp filename */
  if (rename(rectmpfilename, recordfilename) < 0) {
    perror("Error: record_sample_close");
    fprintf(stderr, "failed to move %s to %s\n", rectmpfilename, recordfilename);
    return;
  }
  if (verbose_flag) {
    fprintf(stderr, "recorded to \"%s\" (%lu bytes, %.2f sec.)\n", recordfilename, recordlen * sizeof(SP16), (float)recordlen / (float) recog->jconf->input.sfreq);
  }
}

/************************************************************************/
static boolean
opt_record(Jconf *jconf, char *arg[], int argnum)
{
  record_dirname = strdup(arg[0]);
#if !defined(_WIN32) || defined(__CYGWIN32__)
  if (access(record_dirname, R_OK | W_OK | X_OK) == -1) {
    perror("checkdir");
    fprintf(stderr, "Error: cannot write to dir %s\n", record_dirname);
    return FALSE;
  }
#endif
  return TRUE;
}
void
record_add_option()
{
  j_add_option("-record", 1, 1, "record input waveform to file in dir", opt_record);
}

/************************************************************************/
void
record_setup(Recog *recog, void *data)
{
  if (record_dirname) {
    /* regist callbacks */
    callback_add_adin(recog, CALLBACK_ADIN_TRIGGERED, record_sample_write, data);
    callback_add(recog, CALLBACK_EVENT_SPEECH_STOP, record_sample_close, data);
    printf("Input speech data will be stored to = %s/\n", record_dirname);
  }
}
