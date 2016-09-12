/**
 * @file   feature_postprocess.c
 * 
 * <EN>
 * @brief  A sample plugin for feature vector postprocessing
 * </EN>
 * 
 * <JA>
 * @brief  ��ħ�̤θ�����ץ饰����Υ���ץ�
 * </JA>
 * 
 * @author Akinobu Lee
 * @date   Sun Aug 10 15:14:19 2008
 * 
 * $Revision: 1.1 $
 * 
 */

/**
 * Required for a file
 *   - get_plugin_info()
 *
 * Optional for a file
 *   - initialize()
 * 
 */
/**
 * Feature vector input postprocessing functions
 * 
 * Required:
 *   - fvin_postprocess()
 *   
 */

/***************************************************************************/

#include <stdio.h>
#include <string.h>
#include "plugin_defs.h"

#define PLUGIN_TITLE "feature vector postprocess plugin for Julius"

/** 
 * <EN>
 * @brief  Initialization at loading time (optional)
 * 
 * If defined, this will be called just before this plugin is loaded to Julius.
 * if this returns -1, the whole functions in this file will not be loaded.
 *
 * This function is OPTIONAL.
 * </EN>
 * <JA>
 * @brief  �ɤ߹��߻��ν������Ǥ�ա�
 *
 * ��ư����Julius �����Υץ饰������ɤ߹���ݤ˺ǽ�˸ƤФ�롥
 * -1 ���֤��ȡ����Υץ饰�������Τ��ɤ߹��ޤ�ʤ��ʤ롥
 * �¹Բ�ǽ���Υ����å��˻Ȥ��롥
 *
 * </JA>
 * 
 * 
 * @return 0 on success, -1 on failure.
 * 
 */
int
initialize()
{
  return 0;
}

/** 
 * <EN>
 * @brief  Get information of this plugin (required)
 *
 * This function should return informations of this plugin file.
 * The required info will be specified by opcode:
 *  - 0: return description string of this file into buf
 *
 * This will be called just after Julius find this file and after
 * initialize().
 *
 * @param opcode [in] requested operation code
 * @param buf [out] buffer to store the return string
 * @param buflen [in] maximum length of buf
 *
 * @return 0 on success, -1 on failure.  On failure, Julius will ignore this
 * plugin.
 * 
 * </EN>
 * <JA>
 * @brief  �ץ饰������������ɬ�ܡ�
 *
 * ���Υץ饰����˴ؤ��������֤���Ϳ����줿 opcode �ˤ�ä�ư��롥
 *  - 0 �ξ�硤���Υץ饰����ե������̾�Τ�Ϳ����줿�Хåե��˳�Ǽ����
 *
 * ���δؿ��ϡ�Julius �����Υץ饰������ɤ߹����ľ��˸ƤФ�롥
 * 
 * @param opcode [in] �׵�ư����� (���� 0 �Τ߼���)
 * @param buf [out] �ͤ��Ǽ����Хåե�
 * @param buflen [in] buf �κ���Ĺ
 * 
 * @return ���顼�� -1, ������ 0 ���֤������顼�Ȥ��� -1 ���֤�����硤
 * ���Υץ饰�������Τ��ɤ߹��ޤ�ʤ���
 * </JA>
 * 
 */
int
get_plugin_info(int opcode, char *buf, int buflen)
{
  switch(opcode) {
  case 0:
    /* plugin description string */
    strncpy(buf, PLUGIN_TITLE, buflen);
    break;
  }

  return 0;
}

/** 
 * <EN>
 * @brief  Post-processing function for a feature vector
 *
 * When defined, this function will be called at every input vector
 * before recognition.  This will be called successively for every input
 * at each frame.
 *
 * You can monitor the feature vector to be recognized, and also can
 * modify or overwrite the content to do some transformation like a
 * feature-space adaptation.
 *
 * If multiple plugins have this functions, they are all executed in order
 * of loading.
 * 
 * @param vecbuf [i/o] a feature vector
 * @param veclen [in] length of @a vecbuf
 * @param nframe [in] frame number in a recognition, staring with 0.
 * 
 * </EN>
 * <JA>
 * @brief  ��ħ�̥٥��ȥ���Ф��������ؿ�
 *
 * ���δؿ���������줿��硤Julius �ϸġ�����ħ�̥٥��ȥ�ˤĤ��ơ�
 * ǧ�����Ԥ������ˤ��δؿ���ƤӽФ������δؿ��ϡ����Ϥ��ʤि�Ӥ�
 * ���γƥե졼�����ħ�̥٥��ȥ�ˤĤ��ƸƤФ�롥
 *
 * ���δؿ���Ȥä����Ϥ���ħ�̥٥��ȥ���˥��Ǥ���ۤ����Хåե����
 * �ǡ�����ľ�ܽ񤭴����뤳�Ȥ�Ǥ��롥����ǧ���Ϥ��δؿ�������ä����Ȥ�
 * �ǡ������Ф��ƹԤ���Τǡ��㤨���ü�Ŭ�����ü��������Τ褦�ʽ�����
 * �����ǹԤ����Ȥ��ǽ�Ǥ��롥
 *
 * ʣ���Υץ饰����Ǥ��δؿ������ꤵ��Ƥ����硤�������ɤ߹��߽��
 * �¹Ԥ���롥
 * 
 * @param vecbuf [i/o] ��ħ�̥٥��ȥ�
 * @param veclen [in] @a vecbuf ��Ĺ��
 * @param nframe [in] �ե졼���ֹ�
 * 
 * </JA>
 * 
 */
void
fvin_postprocess(float *vecbuf, int veclen, int nframe)
{
  int i;
  /* just output the vectors to stdout */
  printf("%d:", nframe);
  for(i=0;i<veclen;i++) {
    printf(" %f", vecbuf[i]);
  }
  printf("\n");
}
/* end of file */
