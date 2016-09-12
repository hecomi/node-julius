/**
 * @file   fvin.c
 * 
 * <EN>
 * @brief  A skeleton code of feature input plugin
 * </EN>
 * 
 * <JA>
 * @brief  ��ħ�����ϥץ饰����ΤҤʷ�
 * </JA>
 * 
 * @author Akinobu Lee
 * @date   Mon Aug 11 17:05:17 2008
 * 
 * $Revision: 1.4 $
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
 * feature input plugin functions
 * 
 * Required:
 *   - fvin_get_optname()
 *   - fvin_get_configuration()
 *   - fvin_standby()
 *   - fvin_open()
 *   - fvin_read()
 *   - fvin_close()
 *
 * Optional:
 *   - fvin_terminate()
 *   - fvin_pause()
 *   - fvin_resume()
 *   - fvin_input_name()
 * 
 */

#include <stdio.h>
#include <string.h>
#include "plugin_defs.h"

#define PLUGIN_TITLE "Feature vector input plugin for Julius"
#define INPUT_OPT "myfvin"

/************************************************************************/

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

/************************************************************************/
/************************************************************************/
/* Feature-vector input plugin functions */

/** 
 * <EN>
 * @brief  Return option string to select this input at option. (required)
 *
 * This function should return option string which should be specified
 * as an argument "-input" option, to be used on Julius.  The returning
 * string should not be the same with any existing value.
 *
 * This function will be called several times at option parsing at startup.
 *
 * @param buf [out] buffer to store the return string
 * @param buflen [in] maximum length of buf
 * </EN>
 * <JA>
 * @brief  ���������ѤΥ��ץ����ʸ������֤���ɬ�ܡ�
 *
 * ���Υץ饰��������ϤȤ������򤹤�ݤˡ�"-input" ���ץ����ǻ��ꤹ
 * �٤�ʸ������Ǽ�����֤����֤�ʸ���ϡ������ƥ�ˤ��Ǥˤ����Τ䡤
 * ¾�Υץ饰���󤬻��Ѥ��Ƥ����Τ�Ʊ���Ǥʤ����ȡ�
 * �ʤ⤷Ʊ�����ä���祷���ƥ�¦��ͥ�褵����
 *
 * ���δؿ��ϡ���ư���Υ��ץ������ϻ��˲��٤��ƤФ�롥
 *
 * @param buf [out] �ͤ��Ǽ�����֤��Хåե�
 * @param buflen [in] buf �κ���Ĺ
 * </JA>
 * 
 */
void
fvin_get_optname(char *buf, int buflen)
{
  strncpy(buf, INPUT_OPT, buflen);
}

/** 
 * <EN>
 * @brief  Return configuration parameters for this input (required)
 * 
 * This function should return configuration parameters about the input.
 *
 * When opcode = 0, return the dimension (length) of input vector.
 * 
 * When opcode = 1, return the frame interval (time between frames) in
 * milliseconds.
 * 
 * When opcode = 2, parameter type code can be returned.  The code should
 * the same format as used in HTK parameter file header.  This is for
 * checking the input parameter type against acousitc model, and
 * you can disable the checking by returning "0xffff" to this opcode.
 * 
 * When opcode = 3, should return 0 if the input vector is feature
 * vector, and 1 if the input is outprob vector.
 * 
 * @param opcode [in] requested operation code
 * 
 * @return values required for the opcode as described.
 * </EN>
 * 
 * <JA>
 * @brief  ��ħ�̤Υѥ�᡼�����֤���ɬ�ܡ�
 *
 * �������ϥץ饰����Julius���Ϥ���ħ�̤˴ؤ���ѥ�᡼�����֤���
 * Ϳ����줿�ʲ��� opcode ���Ȥˡ��ͤ��֤���
 *
 * opcode = 0: �٥��ȥ�μ�����
 * opcode = 1: ���ե졼�ढ����λ�������ñ�̡��ߥ��á�
 * opcode = 2: �ѥ�᡼���η�
 *
 * opcode = 2 �Υѥ�᡼���η��ϡ�������ǥ����ħ�̤Ȥη�������
 * �����å��˻Ȥ��롥�ͤϡ�HTK ����ħ�̥ե�����Υإå�������
 * ���󥳡��ɤ��줿�ͤ��֤����������å���Ԥ�ʤ����ϡ�
 * 0xffff ���֤����ȡ�
 *
 * opcode =3 �ΤȤ���ħ�̥٥��ȥ����Ϥʤ� 0, ���ϳ�Ψ�٥��ȥ�ʤ�1���֤���
 *
 * @param opcode [in] �׵�ư����� (���� 0 �Τ߼���)
 * 
 * @return opcode ���Ȥ��׵ᤵ�줿�ͤ��֤���
 * </JA>
 */
int
fvin_get_configuration(int opcode)
{
  switch(opcode) {
  case 0:		   /* return number of elements in a vector */
    return(25);
  case 1:/* return msec per frame */
    return(10);
  case 2:/* return parameter type specification in HTK format */
    /* return 0xffff to disable checking */
    return(0xffff);
  case 3:/* return 0 if feature vector input, 1 if outprob vector input */
    return(0);
  }
}

/************************************************************************/

/**
 * <EN>
 * @brief  Initialize input device (required)
 *
 * This will be called only once at start up of Julius.  You can
 * check if the input file exists or prepare a socket for connection.
 *
 * If this function returns FALSE, Julius will exit.
 * 
 * JuliusLib: this function will be called at j_adin_init().
 *
 * @return TRUE on success, FALSE on failure.
 * </EN>
 * <JA>
 * @brief  �ǥХ��������������ɬ�ܡ�
 *
 * ���δؿ��ϵ�ư���˰������ƤФ�롥�����Ǥ����ϥե�����ν�����
 * �����åȤ��ѰդȤ��ä������ϤΤ���ν�����Ԥ��Τ˻Ȥ���
 *
 * FALSE ���֤�����硤Julius �Ͻ�λ���롥
 * 
 * JuliusLib: ���δؿ��� j_adin_init() �ǸƤФ�롥
 *
 * @return ������ TRUE�����Ի� FALSE ���֤���
 * </JA>
 */
boolean
fvin_standby()
{

  /* sever socket ready etc... */
  return TRUE;

}

/**
 * <EN>
 * @brief  Open an input (required)
 *
 * This function should open a new input.  You may open a feature
 * vector file, or wait for connection at this function.
 *
 * If this function returns FALSE, Julius will exit recognition loop.
 * 
 * JuliusLib: this will be called at j_open_stream().
 * 
 * @return TRUE on success, FALSE on failure.
 * </EN>
 * <JA>
 * @brief  ���Ϥ򳫤���ɬ�ܡ�
 *
 * ���Ϥ򿷵��˳������ե�����Υ����ץ󡤥ͥåȥ�����饤����Ȥ����
 * ��³�ʤɤϤ����ǹԤ���
 *
 * FALSE ���֤����Ȥ���Julius ��ǧ���롼�פ�ȴ���롥
 * 
 * JuliusLib: ���δؿ��� j_open_stream() ��ǸƤФ�롥
 * 
 * @return ������ TRUE�����Ի� FALSE ���֤���
 * </JA>
 */
boolean
fvin_open()
{
  /* listen and accept socket, or open a file */
  return TRUE;
}

/**
 * <EN>
 * @brief  Read a vector from input (required)
 *
 * This will be called repeatedly at each frame, and the read vector
 * will be processed immediately, and then this function is called again.
 *
 * Return value of ADIN_EOF tells end of stream to Julius, which
 * causes Julius to finish current recognition and close stream.
 * ADIN_SEGMENT requests Julius to segment the current input.  The
 * current recognition will be stopped at this point, recognition
 * result will be output, and then Julius continues to the next input.
 * The behavior of ADIN_SEGMENT is similar to ADIN_EOF except that
 * ADIN_SEGMENT does not close/open input, but just stop and restart
 * the recognition.  At last, return value should be ADIN_ERROR on
 * error, in which Julius exits itself immediately.
 * 
 * @param vecbuf [out] store a vector obtained in this function
 * @param veclen [in] vector length
 * 
 * @return 0 on success, ADIN_EOF on end of stream, ADIN_SEGMENT to
 * request segmentation to Julius, or ADIN_ERROR on error.
 * </EN>
 * <JA>
 * @brief ���Ϥ���٥��ȥ���ɤ߹����ɬ�ܡ�
 *
 * ���δؿ������Ϥ���٥��ȥ�򣱤Ĥ����ɤ߹��ࡥ���δؿ���
 * �ե졼�ऴ�Ȥ˸ƤФ졤�ɤ߹��ޤ줿�٥��ȥ�Ϥ��Τ��Ȥ�����ǧ���������졤
 * �ޤ����Υե졼��Υǡ������ɤि��ˤ��δؿ����ƤФ�롥
 *
 * ���Ϥ���ü�ޤ�ã�����Ȥ���ADIN_EOF ���֤������ΤȤ���Julius �ϸ���
 * ��ǧ��������λ���������Ϥ��Ĥ��롥
 *
 * ADIN_ERROR �Ϥ��δؿ���ǿ���ʥ��顼�������������֤������줬�֤�
 * �줿��硤Julius �Ϥ��ξ�ǰ۾ｪλ���롥
 *
 * ADIN_SEGMENT ���֤����Ȥǡ�Julius �˸��ߤ�ǧ���򸽻����Ƕ��ڤ뤳��
 * ���׵᤹�뤳�Ȥ��Ǥ��롥���ߤ�ǧ�������Ϥ��λ����Ǥ��ä�����ڤ�졤
 * �����ޤǤ�ǧ����̤����ꡦ���Ϥ��줿���ȡ�����ǧ���������Ϥޤꤳ��
 * �ؿ����ƤФ�롥ADIN_SEGMENT �� ADIN_EOF ��ư����Ƥ��뤬��
 * ADIN_EOF �� adin_close(), adin_open() ��Ƥ�����Ϥ�λ����
 * ��Τ��Ф��ơ�ADIN_SEGMENT �Ϥ�����ƤФ������Ϥ�³�Ԥ��롥
 * 
 * @param vecbuf [out] ����줿�٥��ȥ���Ǽ����Хåե�
 * @param veclen [in] �٥��ȥ�Ĺ
 * 
 * @return ������ 0 ���뤤�� end of stream ���� ADIN_EOF, Julius �˶�
 * �ڤ��׵��Ф��Ȥ��ˤ� ADIN_SEGMENT, ���顼����ADIN_ERROR ���֤���
 * </JA>
 */
int
fvin_read(float *vecbuf, int veclen)
{
  /* read one vector from the input */
  if (0/* error */) return ADIN_ERROR;
  if (0/* input should be segmented here */) return ADIN_SEGMENT;
  if (0/* EOF */) return ADIN_EOF;

  return(0);			/* success */
}

/**
 * <EN>
 * @brief  Close the current input (required)
 *
 * This function will be called when the input has reached end of file
 * (i.e. the last call of fvin_read() returns ADIN_EOF)
 *       
 * You may close a file or disconnect network client here.
 *
 * If this function returns TRUE, Julius will go again to adin_open()
 * to open another stream.  If returns FALSE, Julius will exit
 * the recognition loop.
 * 
 * JuliusLib: This will be called at the end of j_recognize_stream().
 * 
 * @return TRUE on success, FALSE on failure.
 * </EN>
 * <JA>
 * @brief  ���Ϥ��Ĥ����ɬ�ܡ�
 *
 * ���ߤ����Ϥ��Ĥ��롥���δؿ��ϡ����Ϥ���ü��EOF�ˤ�ã�����Ȥ��ʤ���
 * ��� fvin_read() �� ADIN_EOF ���֤����Ȥ��ˤ˸ƤФ�롥�̾�����Ǥ�
 * �ե�������Ĥ��롤�ͥåȥ����³�����Ǥ���ʤɤν�����Ԥ���
 *
 * ���ｪλ�Ȥ���TRUE���֤����Ȥ���Julius �� adin_open() ����ä�
 * ¾�Υ��ȥ꡼��򳫤����Ȥ��롥 FALSE ���֤����Ȥ��ϡ�Julius ��
 * ǧ���롼�פ�ȴ���롥
 * 
 * JuliusLib: ���δؿ��� j_recognize_stream() �κǸ�ǸƤФ�롥
 * 
 * @return ������ TRUE�����Ի� FALSE ���֤���
 * </JA>
 */
boolean
fvin_close()
{
  /* file close, connection close, etc.. */
  return TRUE;
}

/************************************************************************/

/**
 * <EN>
 * @brief  A hook for Termination request (optional)
 *
 * This function will be called when Julius receives a Termination
 * request to stop running.  This can be used to synchronize input
 * facility with Julius's running status.
 * 
 * Termination will occur when Julius is running on module mode and
 * received TERMINATE command from client, or j_request_terminate()
 * is called inside application.  On termination, Julius will stop
 * recognition immediately (discard current input if in process),
 * and wait until received RESUME command or call of j_request_resume().
 *
 * This hook function will be called just after a Termination request.
 * Please note that this will be called when Julius receives request,
 * not on actual termination.
 * 
 * @return TRUE on success, FALSE on failure.
 * </EN>
 * <JA>
 * @brief  �����׵��ѥեå���Ǥ�ա�
 *
 * ���δؿ����������ȡ�Julius �������׵�������ä��ݤˤ��δؿ���ƤӽФ���
 * �����Ȥäơ�Julius �����ǡ��Ƴ���Ʊ����������Ʊ��������������뤳�Ȥ�
 * �Ǥ��롥���㡧�������������Ф������������׵��Ф��ʤɡ�
 *
 * �����׵�ϡ�Julius �����ץꥱ�������䥯�饤����Ȥ��������
 * ǧ�����Ǥ��׵�Ǥ��롥����Ū�ˤϡ�Julius ���⥸�塼��⡼�ɤ�ư���
 * ������� TERMINATE ���ޥ�ɤ򥯥饤����Ȥ��������ä��Ȥ��䡤
 * JuliusLib���Ȥ߹�������ץꥱ������� j_request_terminate() ��
 * �Ƥ���Ȥ���ȯ�����롥
 *
 * �����׵��������ȡ�Julius �ϸ��ߤ�ǧ�����������Ǥ��롥
 * ǧ������Ǥ��ä���硤�������Ϥ��˴�����¨�����Ǥ��롥
 * �����κƳ��ϡ�RESUME ���ޥ�ɤ� j_request_resume() �θƤӽФ��ǹԤ��롥
 *
 * ���δؿ��������׵�� Julius ��������ä������ǸƤФ�롥
 * �ºݤ˽��������Ǥ�����ǸƤФ��ΤǤϤʤ����Ȥ���դ��줿����
 * 
 * @return ������ TRUE, ���顼�� FALSE ���֤���
 * </JA>
 * 
 */
boolean
fvin_terminate()
{
  printf("terminate request\n");
  return TRUE;
}

/**
 * <EN>
 * @brief  A hook for Pause request (optional)
 *
 * This function will be called when Julius receives a Pause request
 * to stop running.  This can be used to synchronize input facility
 * with Julius's running status.
 * 
 * Pause will occur when Julius is running on module mode and
 * received PAUSE command from client, or j_request_pause()
 * is called inside application.  On pausing, Julius will 
 * stop recognition and then wait until it receives RESUME command
 * or j_request_resume() is called.  When pausing occurs while recognition is
 * running, Julius will process it to the end before stops.
 *
 * This hook function will be called just after a Pause request.
 * Please note that this will be called when Julius receives request,
 * not on actual pause.
 *
 * @return TRUE on success, FALSE on failure.
 * </EN>
 * <JA>
 * @brief  ����׵��ѥեå���Ǥ�ա�
 *
 * ���δؿ����������ȡ�Julius ������׵�������ä��ݤˤ��δؿ���ƤӽФ���
 * �����Ȥäơ�Julius �����ǡ��Ƴ���Ʊ����������Ʊ��������������뤳�Ȥ�
 * �Ǥ��롥���㡧�������������Ф������������׵��Ф��ʤɡ�
 *
 * ����׵�ϡ�Julius �����ץꥱ�������䥯�饤����Ȥ�������롤
 * ǧ���ΰ����ߤ��׵�Ǥ��롥����Ū�ˤϡ�Julius ���⥸�塼��⡼�ɤ�ư���
 * ������� PAUSE ���ޥ�ɤ򥯥饤����Ȥ��������ä��Ȥ��䡤
 * JuliusLib���Ȥ߹�������ץꥱ������� j_request_pause() ��
 * �Ƥ���Ȥ���ȯ�����롥
 *
 * ����׵��������ȡ�Julius �ϸ��ߤ�ǧ�����������Ǥ��롥
 * ǧ������Ǥ��ä���硤����ǧ���������ޤ��ԤäƤ������Ǥ��롥
 * �����κƳ��ϡ�RESUME ���ޥ�ɤ� j_request_resume() �θƤӽФ��ǹԤ��롥
 * 
 * �����׵� (fvin_terminate) �Ȥΰ㤤�ϡ�ǧ��������׵��������Ȥ���ư�
 * �ۤʤ롥�����׵�Ǥ϶������Ǥ��뤬������׵�ǤϤ���ǧ���������ޤ�
 * �ԤäƤ�����ߤ��롥
 *
 * ���δؿ�������׵�� Julius ��������ä������ǸƤФ�롥
 * �ºݤ˽�������ߤ�����ǸƤФ��ΤǤϤʤ����Ȥ���դ��줿����
 * 
 * @return ������ TRUE, ���顼�� FALSE ���֤���
 * </JA>
 * 
 */
boolean
fvin_pause()
{
  printf("pause request\n");
  return TRUE;
}

/**
 * <EN>
 * @brief  A hook for Resume request (optional)
 *
 * This function will be called when Julius received a resume request
 * to recover from pause/termination status.
 * 
 * Resume will occur when Julius has been stopped by receiving RESUME
 * command from client on module mode, or j_request_resume() is called
 * inside application.
 *
 * This hook function will be called just after a resume request.
 * This can be used to make this A/D-in plugin cooperate with the
 * pause/resume status, for example to tell audio client to restart
 * audio streaming.
 *
 * This function is totally optional.
 * 
 * @return TRUE on success, FALSE on failure.
 * </EN>
 * <JA>
 * @brief  ǧ���Ƴ��׵��ѥեå���Ǥ�ա�
 *
 * ���δؿ����������ȡ�Julius ����߾��֤����ǧ���Ƴ��׵�κݤ�
 * ���δؿ���ƤӽФ���
 *
 * ǧ���Ƴ��׵�ϡ�Julius ���⥸�塼��⡼�ɤ�ư��� RESUME ���ޥ�ɤ�
 * ���饤����Ȥ��������ä��Ȥ��䡤JuliusLib���Ȥ߹�������ץꥱ�������
 * �� j_request_resume() ��Ƥ���Ȥ���ȯ�����롥���κƳ��׵᤬ȯ��
 * ����ȡ�Julius ����ߤ��Ƥ���ǧ����Ƴ����롥
 *
 * ��ա����δؿ��ϡ��ºݤ���ߤ����Ȥ��˸ƤФ��ΤǤϤʤ���Julius ��
 * �׵�������ä������ǡ����Τ��Ӥ˸ƤФ�롥ʣ����ƤФ�뤳�Ȥ䡤
 * ���Ǥ�ư����Ǥ�����ˤ���ˤ��Υ��ޥ�ɤ������ä��Ȥ��ˤ�Ƥ�
 * ��뤳�Ȥ����뤳�Ȥ���դ��줿����
 * 
 * @return ������ TRUE, ���顼�� FALSE ���֤���
 * </JA>
 * 
 */
boolean
fvin_resume()
{
  printf("resume request\n");
  return TRUE;
}

/**
 * <EN>
 * @brief  A function to return current device name for information (optional)
 *
 * This function is totally optional.
 * 
 * @return pointer to the device name string
 * </EN>
 * <JA>
 * @brief  ���ϥե����롦�ǥХ���̾���֤��ؿ���Ǥ�ա�
 *
 * @return ���ϥե����뤢�뤤�ϥǥХ���̾��ʸ����ؤΥݥ���
 * </JA>
 * 
 */
char *
fvin_input_name()
{
  printf("input name function was called\n");
  return("default");
}
/* end of file */
