/**
 * @file   htk_param.h
 *
 * <EN>
 * @brief Structures for storing input speech parameters
 *
 * This file defines a structure for holding a vector sequence of
 * input speech parameters.  The speech parameter sequence will be
 * stored in HTK_Param.  The HTK_Param also holds information about
 * the extraction condition, i.e., frame shift length, window size and
 * so on.
 *
 * The speech input vector can be read from HTK parameter file, or
 * Julius extracts the parameters directly from input speech.  Julius
 * supports extraction of only MFCC format of fixed dimension.
 * More precisely, only parameter type of MFCC_{0|E}_D[_Z][_N] with {25|26}
 * dimensions is supported.
 *
 * When recognition, the parameter types of both acoustic model and
 * input parameter should be the same.  Please note that only the
 * parameter type is checked, and other parameters such as source sampling
 * rate, frame shift length and window sizes will not be checked.
 * </EN>
 * <JA>
 * @brief HTK����ħ�ѥ�᡼�����ݻ����빽¤�Τ˴�Ϣ�������
 *
 * ���Υե�����ˤϡ�������ħ�̤Υ٥��ȥ������ݻ����빽¤�Τ�
 * �������Ƥ��ޤ������ϲ�������׻����줿MFCC���β�����ħ�̤ϡ�
 * �������������빽¤�� HTK_Param ����¸����ޤ���HTK_Param�ˤ�
 * �ޤ�����ħ����л��Υե졼�ॷ�ե����䥦����ɥ�Ĺ�ʤɤξ���
 * �ݻ�����ޤ���
 *
 * ������ħ�̤ϳ����� HTK �ʤɤˤ�ä���Ф��줿HTK��������ħ�̥ե������
 * �ɤ߹��ळ�Ȥ��Ǥ��ޤ����ޤ���MFCC �����Ǥ���� Julius ���
 * ľ�ܲ����ȷ�������Ф��뤳�Ȥ��Ǥ��ޤ����ºݤ�Julius����������Ф���
 * ���뤳�Ȥ��Ǥ�����ħ�̤� {25|26} ������ MFCC_{0|E}_D[_Z][_N] �ΤߤǤ���
 *
 * ���Ѥ��벻����ǥ�(%HMM)���ؽ������Ѥ�����ħ�̤�ǧ���оݤȤ������Ϥ�
 * ��ħ�̤η����ϰ��פ�����ɬ�פ�����ޤ���ǧ���¹Ի��ˤϡ�������ǥ������
 * �ե��������ħ�̷����������å����졤Ŭ�礷�ʤ����ϥ��顼�Ȥʤ�ޤ���
 * �����������ϲ����Υ���ץ�󥰼��ȿ���ե졼�ॷ�ե�����������ɥ�Ĺ��
 * �����HTK�����β�����ǥ�ˤ��ݻ�����Ƥ��ʤ����ᡤ�����å��Ǥ��ޤ���
 * ��դ��Ʋ�������
 * </JA>
 *
 * @sa htk_defs.h
 *
 * @author Akinobu LEE
 * @date   Fri Feb 11 02:52:52 2005
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

#ifndef __SENT_HTK_PARAM_H__
#define __SENT_HTK_PARAM_H__

#include <sent/stddefs.h>
#include <sent/htk_defs.h>

/// Parameter types and extraction conditions
typedef struct {
  unsigned int samplenum;	///< Number of samples (or frames)
  unsigned int wshift;		///< Window shift (unit: 100ns) 
  unsigned short sampsize;	///< Bytes per sample 
  short samptype;		///< Parameter type, see also htk_defs.h
} HTK_Param_Header;

/// Input speech parameter
typedef struct {
  HTK_Param_Header header;	///< Parameter header information
  unsigned int samplenum;	///< Number of sample (same in header.samplenum) 
  short veclen;			///< Vector length of a sample
  VECT **parvec;		///< Actual parameter vectors [0..samplenum-1][0..veclen-1]
  short veclen_alloc;		///< Allocated vector length of a sample
  unsigned int samplenum_alloc;	///< Alllocated number of samples
  BMALLOC_BASE *mroot;		///< Pointer for block memory allocation
  boolean is_outprob;		///< TRUE if this is outprob vector
} HTK_Param;

/**
 * Increment step of HTK Parameter holder in frames
 * 
 */
#define HTK_PARAM_INCREMENT_STEP_FRAME 200

#ifdef __cplusplus
extern "C" {
#endif

boolean rdparam(char *, HTK_Param *);
HTK_Param *new_param();
void free_param(HTK_Param *);
short param_qualstr2code(char *);
short param_str2code(char *);
char *param_qualcode2str(char *, short, boolean);
char *param_code2str(char *, short, boolean);
int guess_basenum(HTK_Param *p, short qualtype);
boolean param_strip_zero(HTK_Param *param);

void param_init_content(HTK_Param *p);
boolean param_alloc(HTK_Param *p, unsigned int samplenum, short veclen);
void param_free_content(HTK_Param *p);


/* hmminfo/put_htkdata_info.c */
void put_param_head(FILE *fp, HTK_Param_Header *h);
void put_vec(FILE *fp, VECT **p, int num, short veclen);
void put_param(FILE *fp, HTK_Param *pinfo);
void put_param_info(FILE *fp, HTK_Param *pinfo);

#ifdef __cplusplus
}
#endif

#endif /* __SENT_HTK_PARAM_H__ */
