/**
 * @file   jlog.c
 * 
 * <JA>
 * @brief  ��å������ƥ����Ƚ��Ϥȥ��顼��λ
 *
 * ��å����������Ѥ����Ѵؿ�������Ǥ���
 * ǧ����̤�ü�����Ϥ�ͥåȥ���ؤν��ϤȤ��ä���
 * Julius �μ��פʽ��ϤϤ����δؿ���ȤäƹԤ��ޤ���
 * �ޤ�������ʸ�������ɤ��Ѵ��⤳���ǹԤ��ޤ���
 * </JA>
 * 
 * <EN>
 * @brief  Message text output and error exit functions
 *
 * These are generic functions for text message output.
 * Most of text messages such as recognition results, status, informations
 * in Julius/Julian will be output to TTY or via network using these functions.
 * The character set conversion will also be performed here.
 * </EN>
 * 
 * @author Akinobu LEE
 * @date   Thu Feb 17 16:02:41 2005
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

#include <sent/stddefs.h>
#include <sent/tcpip.h>
#include <stdarg.h>

static FILE *outdev;
static boolean initialized = FALSE;

#define MAX_PRINTF_LEN 4096	///< Maximum string length at one printf call

/** 
 * Set file pointer to output the log.  Set to NULL to totally disable
 * the log outputs.
 * 
 * @param fp [in] file pointer or NULL to disable
 * 
 */
void
jlog_set_output(FILE *fp)
{
  outdev = fp;
  initialized = TRUE;
}

/** 
 * Return the current file pointer to output log.
 * 
 * @return the file pointer.
 * 
 */
FILE *
jlog_get_fp()
{
  if (initialized == FALSE) return stdout;
  return outdev;
}

/** 
 * @brief  Output a normal message to log
 *
 * 
 * @param fmt [in] format string, like printf.
 * @param ... [in] variable length argument like printf.
 * 
 */
void
jlog(char *fmt, ...)
{
  va_list ap;

  if (initialized == FALSE) {
    outdev = stdout;
  } else if (outdev == NULL) return;
  
  va_start(ap,fmt);
  vfprintf(outdev, fmt, ap);
  va_end(ap);

  return;
}

/** 
 * @brief  Flush text message
 *
 * The output device can be changed by set_print_func().
 * 
 * @return the same as fflush, i.e. 0 on success, other if failed.
 */
int
jlog_flush()
{
  if (initialized == FALSE) {
    outdev = stdout;
  } else if (outdev == NULL) return 0;
  return(fflush(outdev));
}

