/**
 * @file   japi_misc.c
 * 
 * <JA>
 * @brief  �⥸�塼�륳�ޥ��������
 * </JA>
 * 
 * <EN>
 * @brief  Sending module commands
 * </EN>
 * 
 * @author Akinobu LEE
 * @date   Thu Mar 24 11:24:18 2005
 *
 * $Revision: 1.5 $
 * 
 */
/*
 * Copyright (c) 2002-2013 Kawahara Lab., Kyoto University
 * Copyright (c) 2002-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2013 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#include "japi.h"

/** 
 * <JA>
 * ���ޥ�� DIE: ǧ�������Ф�λ�����롥
 * 
 * @param sd [in] ���������å�
 * </JA>
 * <EN>
 * Command "DIE": kill the recognition server.
 * 
 * @param sd [in] socket to send data
 * </EN>
 */
void
japi_die(int sd)
{
  do_send(sd, "DIE\n");
}

/** 
 * <JA>
 * ���ޥ�� VERSION: �С�������������������롥
 * 
 * @param sd [in] ���������å�
 * </JA>
 * <EN>
 * Command "VERSION": let the server send version information.
 * 
 * @param sd [in] socket to send data
 * </EN>
 */
void
japi_get_version(int sd)
{
  do_send(sd, "VERSION\n");
}

/** 
 * <JA>
 * ���ޥ�� STATUS: ǧ�������Фθ��ߤξ���(ǧ����/�����)�����������롥
 * 
 * @param sd [in] ���������å�
 * </JA>
 * <EN>
 * Command "STATUS": ask server about its current status (run/stop)
 * 
 * @param sd [in] socket to send data
 * </EN>
 */
void
japi_get_status(int sd)
{
  do_send(sd, "STATUS\n");
}

/** 
 * <JA>
 * ���ޥ�� PAUSE: ǧ�������Ф�����ߤ��롥�����Ф�ǧ����ξ�硤��������
 * ������äƤ�����ߤ��롥
 * 
 * @param sd [in] ���������å�
 * </JA>
 * <EN>
 * Command "PAUSE": tell server to pause recognition.  If audio input is
 * processing at that time, recognition will stop after the current input
 * has ended.
 * 
 * @param sd [in] socket to send data
 * </EN>
 */
void
japi_pause_recog(int sd)
{
  do_send(sd, "PAUSE\n");
}

/** 
 * <JA>
 * ���ޥ�� TERMINATE: ǧ�������Ф�����ߤ��롥�����Ф�ǧ����ξ�硤
 * �������Ϥ�ΤƤ�¨����ߤ��롥
 * 
 * @param sd [in] ���������å�
 * </JA>
 * <EN>
 * Command "TERMINATE": tell server to pause recognition immediately,
 * even if audio input is processing at that time.
 * 
 * @param sd [in] socket to send data
 * </EN>
 */
void
japi_terminate_recog(int sd)
{
  do_send(sd, "TERMINATE\n");
}

/** 
 * <JA>
 * ���ޥ�� RESUME: PAUSE��TERMINATE�ˤ�äư����ߤ���ǧ�������Ф�
 * �Ƴ������롥
 * 
 * @param sd [in] ���������å�
 * </JA>
 * <EN>
 * Command "RESUME": tell server to restart recognition.
 * 
 * @param sd [in] socket to send data
 * </EN>
 */
void
japi_resume_recog(int sd)
{
  do_send(sd, "RESUME\n");
}

/** 
 * <JA>
 * ���ޥ�� INPUTONCHANGE: ʸˡ�ڤ��ؤ��ؼ�����ǧ����Ǥ��ä�����ư���
 * ���ꤹ�롥
 * 
 * @param sd [in] ���������å�
 * @param arg [in] "TERMINATE" for immediate rejection of current input,
 * "PAUSE" for immediate input segmentation followed by recognition, or
 * "WAIT" for waiting the input to be segmented.
 * </JA>
 * <EN>
 * Command "INPUTONCHANGE": specify grammar changing timing policy when
 * input is being recognized.
 * 
 * @param sd [in] socket to send data
 * @param arg [in] "TERMINATE" for immediate rejection of current input,
 * "PAUSE" for immediate input segmentation followed by recognition, or
 * "WAIT" for waiting the input to be segmented.
 * </EN>
 */
void
japi_set_input_handler_on_change(int sd, char *arg)
{
  /* argument should be checked here... */
  /* one of TERMINATE, PAUSE, WAIT */

  /* send */
  do_sendf(sd, "INPUTONCHANGE\n%s\n", arg);
}
