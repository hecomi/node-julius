/**
 * @file   result.c
 * 
 * <EN>
 * @brief  Plugin to process recognition result
 * </EN>
 * 
 * <JA>
 * @brief  ǧ����̤��������ץ饰����
 * </JA>
 * 
 * @author Akinobu Lee
 * @date   Fri Aug 22 15:17:59 2008
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
 * Result processing function
 * 
 *   - result_str()
 *   
 */

/***************************************************************************/

#include <stdio.h>
#include <string.h>

#define PLUGIN_TITLE "result process plugin for Julius"

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
 * @brief  Process a recognition result (best string)
 *
 * This function will be called each time after recognition of an
 * utterance is finished.  The best recognition result for the
 * utterance will be passed to this function, as a string in which
 * words are separated by white space.  When the recognition was failed
 * or rejected, string will be NULL.
 *
 * On short-pause segmentation mode or GMM/Decoder-VAD mode, where
 * an input utterance may be segmented into pieces, this funtion will be
 * called for each segment.  On multi decoding, the best hypothesis among
 * all the recognition instance will be given.
 * 
 * @param result_str [in] recognition result, words separated by whitespace,
 * or NULL on failure
 * 
 * </EN>
 * <JA>
 * @brief  ǧ����̤ν����ʺ���ʸ�����
 *
 * ���δؿ������Ϥ�ǧ��������뤿�Ӥ˸ƤӽФ��졤
 * ���Ϥ��Ф���ǧ����̡ʺǤ��Ψ�ι⤤����ˤ�ʸ�����Ϥ���롥
 * Ϳ������ʸ����ϡ�ñ����˥��ڡ����Ƕ��ڤ��롥
 * ǧ�������Ԥ������ϡ� ʸ����� NULL ���Ϥ���롥
 *
 * ���硼�ȥݡ����������ơ������� GMM/Decoder �١�����VAD��
 * �Ԥ���硤���ϤϾ�ñ�̤�ʬ�䤵��롥���ξ�硤���δؿ���
 * ����ʬ�䤵�줿��ñ�̤��Ȥ˸ƤФ�롥�ޤ���ʣ����ǥ�ǧ���ξ�硤
 * ��ǧ��������ǺǤ⥹�����ι⤤���⤬�Ϥ���롥
 * 
 * @param result_str [in] ǧ����̡�ñ��϶���Ƕ��ڤ��Ƥ����NULL��
 * ��硤ǧ�����ԡ�
 * 
 * </JA>
 * 
 */
void
result_best_str(char *result_str)
{
  if (result_str == NULL) {
    printf("[failed]\n");
  } else {
    printf("               <<%s>>\n", result_str);
  }
}
