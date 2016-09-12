/**
 * @file   generic_callback.c
 * 
 * <EN>
 * @brief  An example plugin using callback.
 * </EN>
 * 
 * <JA>
 * @brief  ������Хå���Ȥ��ץ饰����Υ���ץ�
 * </JA>
 * 
 * @author Akinobu Lee
 * @date   Wed Aug 13 23:50:27 2008
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
 * using plugin function:
 * 
 *   - engine_startup()
 * 
 */


// #include "plugin_defs.h"
#include <julius/juliuslib.h>

#define PLUGIN_TITLE "An example plugin using callback"

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
 * A sample callback function to output RECREADY message.
 * 
 * @param recog [in] engine instance
 * @param dummy [in] callback argument (dummy)
 * </EN>
 * <JA>
 * RECREADY ����Ϥ��륳����Хå��Ѵؿ��ʥ���ץ��
 *
 * @param recog [in] ���󥸥󥤥󥹥���
 * @param dummy [in] ������Хå������ʥ��ߡ���
 * </JA>
 * 
 */
static void
status_recready(Recog *recog, void *dummy)
{
  printf("<<<RECREADY>>>\n");
}

/** 
 * <EN>
 * @brief  plugin function that will be called after engine startup.
 *
 * When the function of this name is defined in a plugin, this will
 * be called just after Julius finished all startup sequence and before
 * input and recognition start.
 *
 * In this example, this function registers the local function
 * status_recready() as a CALLBACK_EVENT_SPEECH_READY callback.
 * This callback will be called on every time Julius is ready for
 * recognition for the next incoming input.
 * 
 * @param data [in] a data pointer, actually a pointer to an engine instance.
 * 
 * @return 0 on success, -1 on error.  On error, Julius will exit immediately.
 * </EN>
 * <JA>
 * @brief  ǧ�����󥸥�ư��λ���˸ƤӽФ����ץ饰����ؿ�
 *
 * ����̾���δؿ���������줿��硤���δؿ��ϡ�Julius �����Ƥν������
 * ��λ���Ƶ�ư�ץ����򽪤���ľ�塤�ºݤ˲������Ϥ򳫤���ǧ�����Ϥ�
 * �����˸ƤФ�ޤ���
 *
 * �����Ǥϡ����δؿ���Ȥäơ��嵭�δؿ� status_recready() ��
 * CALLBACK_EVENT_SPEECH_READY ������Хå��Ȥ�����Ͽ���Ƥ��ޤ���
 * ���Υ�����Хå��� Julius �����ϥ��ȥ꡼�फ��μ��β��������Ԥ�
 * ���֤ˤʤä��Ȥ��˸ƤФ�ޤ���
 * 
 * @param data [in] �ǡ����ؤΥݥ��󥿡����Τϥ��󥸥󥤥󥹥��󥹤ؤ�
 * �ݥ��󥿤��Ϥ���롥
 * 
 * @return ������ 0 �����顼�� -1 ���֤������顼�ξ�� Julius �ϰ۾ｪλ���롥
 * </JA>
 * 
 */
int
startup(void *data)
{
  Recog *recog = data;
  callback_add(recog, CALLBACK_EVENT_SPEECH_READY, status_recready, NULL);
  return 0;
}
