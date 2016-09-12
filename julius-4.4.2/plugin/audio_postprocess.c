/**
 * @file   audio_postprocess.c
 * 
 * <EN>
 * @brief  A sample audio postprocessing plugin
 * </EN>
 * 
 * <JA>
 * @brief  �����ǥ������Ϥθ�����ץ饰����Υ���ץ�
 * </JA>
 * 
 * @author Akinobu Lee
 * @date   Sun Aug 10 15:12:50 2008
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
 * A/D-in postprocessing functions
 * 
 * Required:
 *   - adin_postprocess()
 *   
 */

/***************************************************************************/

#include <stdio.h>
#include <string.h>
#include "plugin_defs.h"

#define PLUGIN_TITLE "audio postprocess plugin for Julius"

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
 * @brief  Post-processing function for captured audio
 *
 * When defined, this function will be called at every audio input
 * fragments before any feature analysis.  @a buf contains the small
 * fragment of captured audio input at a length of @a len, and this
 * will be called successively as input goes.
 *
 * You can monitor the incoming audio stream, and also can modify or
 * overwrite the content of @a buf to do some audio processing for the
 * incoming data like noise supression etc.
 *
 * If multiple plugins have this functions, they are all executed in order
 * of loading.
 * 
 * @param buf [i/o] a fragment of audio inputs
 * @param len [in] length of @a buf (in samples)
 * 
 * </EN>
 * <JA>
 * @brief  �������Ϥ��Ф�������
 *
 * ���δؿ���������줿��硤Julius �����Ϥ��줿�����ǡ������Ф��ơ���
 * ħ����Ф�Ԥ����ˤ��δؿ���ƤӽФ���@a buf �ˤ� @a len ��Ĺ���β�
 * �����ϥǡ������Ҥ����äƤ��롥���δؿ��ϡ����Ϥ��ʤि�Ӥˤ���û��
 * ���Ҥ��Ȥ˷����֤��ƤФ�롥
 *
 * ���δؿ���Ȥä����ϲ����ǡ������˥��Ǥ���ۤ��ˡ��Хåե����
 * �ǡ�����ľ�ܽ񤭴����뤳�Ȥ�Ǥ��롥����ǧ���Ϥ��δؿ�������ä����Ȥ�
 * �ǡ������Ф��ƹԤ���Τǡ��㤨�л����ް������ʤɤ򤳤��ǹԤ�
 * ���Ȥ��ǽ�Ǥ��롥
 *
 * ʣ���Υץ饰����Ǥ��δؿ������ꤵ��Ƥ����硤�������ɤ߹��߽��
 * �¹Ԥ���롥
 * 
 * @param buf [i/o] �������ϥǡ������Ҥ����ä��Хåե�
 * @param len [in] @a buf ��Ĺ���ʥ���ץ����
 * 
 * </JA>
 * 
 */
void
adin_postprocess(SP16 *buf, int len)
{
  //printf("%d\n", len);
}
/* end of file */
