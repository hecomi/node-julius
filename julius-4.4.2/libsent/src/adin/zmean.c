/**
 * @file   zmean.c
 *
 * <JA>
 * @brief  ���ϲ�����ľή��ʬ�ν���
 *
 * ľή��ʬ�ν����Ԥʤ��ޤ���
 * 
 * ľή��ʬ�ο�������ϥǥХ������Ȥ˰ۤʤ�ޤ���
 * �ե��������ϤǤϡ��ǡ������Το�����ʿ�Ѥ��Ѥ����ޤ���
 * �ޥ������Ϥ�ͥåȥ�����Ϥξ�硤���ϥ��ȥ꡼���
 * �ǽ�� @a ZMEANSAMPLES �ĤΥ���ץ��ʿ�Ѥ���׻����졤
 * �����ͤ����θ�����Ϥ��Ѥ����ޤ���
 * </JA>
 * <EN>
 * @brief  Remove DC offset from input speech
 *
 * These function removes DC offset from input speech, like the ZMEANSOURCE
 * feature in HTK.
 *
 * The estimation method of DC offset depends on the type of input device.
 * On file input, the mean of entire samples is used as estimated offset.
 * On microphone input and network input, The first @a
 * ZMEANSAMPLES samples in the input stream are used to estimate the offset,
 * and the value will be used for the rest of the input.
 * </EN>
 *
 * @author Akinobu LEE
 * @date   Sun Feb 13 20:31:23 2005
 *
 * $Revision: 1.5 $
 * 
 */
/*
 * Copyright (c) 1991-2013 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2013 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#include <sent/adin.h>

static int zlen = 0;		///< Current recorded length for DC offset estimation
static float zmean = 0.0;	///< Current mean

/** 
 * Reset status.
 * 
 */
void
zmean_reset()
{
  zlen = 0;
  zmean = 0.0;
}

/** 
 * @brief Remove DC offset.
 *
 * The DC offset is estimated by the first samples after zmean_reset() was
 * called.  If the first input segment is longer than ZMEANSAMPLES, the
 * whole input is used to estimate the zero mean.  Otherwise, the zero mean
 * will continue to be updated until the read length exceed ZMEANSAMPLES.
 * 
 * @param speech [I/O] input speech data, will be subtracted by DC offset.
 * @param samplenum [in] length of above.
 * 
 */
void
sub_zmean(SP16 *speech, int samplenum)
{
  int i;
  float d, sum;

  if (zlen < ZMEANSAMPLES) {
    /* update zmean */
    sum = zmean * zlen;
    for (i=0;i<samplenum;i++) {
      sum += speech[i];
    }
    zlen += samplenum;
    zmean = sum / (float)zlen;
  }
  for (i=0;i<samplenum;i++) {
    d = (float)speech[i] - zmean;
    /* clip overflow */
    if (d < -32768.0) d = -32768.0;
    if (d > 32767.0) d = 32767.0;
    /* round to SP16 (normally short) */
    if (d > 0) speech[i] = (SP16)(d + 0.5);
    else speech[i] = (SP16)(d - 0.5);
  }
}
