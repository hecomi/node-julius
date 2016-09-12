/**
 * @file   adin_oss.c
 * 
 * <EN>
 * @brief  A reference sample of A/D-in plugin
 *
 * This file describes the specifications of plugin functions to be
 * defined to make an A/D-in plugin.  An A/D-in plugin will extend a
 * new audio sream input into Julius by addin a new choice to the
 * "-input" option.
 *
 * The recording format should be 16 bit (signed short), and sampling
 * rate should be set to the given value at adin_standby().
 * 
 * </EN>
 * 
 * <JA>
 * @brief  �����ǥ������ϥץ饰����ΤҤʷ�
 *
 * ���Υե�����ϡ������ǥ������ϥץ饰������������ݤ�������٤���
 * �饰����ؿ��ˤĤ��Ʋ��⤷�Ƥ��롥�����ǥ������ϥץ饰����ϡ�
 * Julius �˿����ʲ������ϥǥХ������ɲä��롥"-input" �˿����������
 * ���ɲä��졤�¹Ի��� Julius ���Ф��Ƥ������ꤹ�뤳�Ȥǡ����Υץ�
 * �������ͳ�ǲ����������ǧ�����뤳�Ȥ��Ǥ��롥
 *
 * �����ǥ������ϥץ饰����Ǽ�����٤��ǡ����Υե����ޥåȤ� 16bit ��
 * ���뤳�ȡ�����ˡ�����ץ�󥰥졼�Ȥ� adin_standby() �ƤӽФ�����
 * Ϳ������졼�Ȥ˹�碌�뤳�ȡ�
 * 
 * </JA>
 *
 * Common functions that can be defined in any type of plugin:
 *   - get_plugin_info()
 *   - initialize()
 * 
 * A/D-in plugin functions:
 * 
 * Required:
 *   - adin_get_optname()
 *   - adin_get_configuration()
 *   - adin_standby()
 *   - adin_open()
 *   - adin_read()
 *   - adin_close()
 *
 * Optional:
 *   - adin_terminate()
 *   - adin_pause()
 *   - adin_resume()
 *   - adin_input_name()
 * 
 * 
 * @author Akinobu Lee
 * @date   Thu Aug  7 14:28:37 2008
 * 
 * $Revision: 1.3 $
 * 
 */

/***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "plugin_defs.h"

/**
 * <EN>
 * Description string of this plugin file.
 * </EN>
 * <JA>
 * ���Υץ饰����ե����������ʸ����
 * </JA>
 * 
 */
#define PLUGIN_TITLE "A/D-in plugin for Julius"

/**
 * <EN>
 * string to be specified at "-input" option at Julius to use this plugin
 * as input module.
 * </EN>
 * <JA>
 * ���Υץ饰�������Ѥ��Ʋ������Ϥ�Ԥ��ݤˡ�Julius�� "-input" ���ץ����
 * ��Ϳ����٤�ʸ����.
 * </JA>
 * 
 */
#define INPUT_OPT "myadin"

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
/* A/D-in plugin functions */

/** 
 * <EN>
 * @brief  Return option string to select at option. (required)
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
 * @brief  �������������ѤΥ��ץ����ʸ������֤���ɬ�ܡ�
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
adin_get_optname(char *buf, int buflen)
{
  strncpy(buf, INPUT_OPT, buflen);
}

/** 
 * <EN>
 * @brief  Return decoder parameter values related to this adin plugin (required)
 * 
 * This function should return configuration values about how to set up
 * decoder to use this adin plugin.  The return value is dependent on
 * the given opcode, as described below:
 *
 * opcode = 0:  return whether real-time processing of 1st pass
 * should be enabled by default.
 *
 * if returns 0 (disabled) , Julius will do buffered input, spooling
 * the incoming input till EOF or silence cut segmentation, extract
 * feature vector, then recognize the whole.  If returns 1 (enabled),
 * on-the-fly decoding will be performed, reading input and decoding
 * it concurrently.
 *
 * A real-time decoding uses some approximation on feature extraction
 * related to sentence-based normalization i.e. CMN or energy normalization.  
 * This value is typically 0 on off-line recognition, and 1 for on-line
 * recognition.
 *
 * This value is device-dependent default value, and can be overridden by
 * user option "-realtime" and "-norealtime".
 *
 * opcode = 1: return whether silence cut segmentation should be
 * enabled by default
 *
 * return 0 to disable, 1 to enable.
 * 
 * On file input, you can choose whether silence detection and
 * segmentation should be performed before recognition.  On live input
 * like microphone, where input stream is infinite, you would perfer
 * choose 1 to enable it.
 * 
 * This value is device-dependent default value, and can be overridden by
 * user option "-cutsilence" and "-nocutsilence".
 *
 * opcode = 2: return whether input threading is necessary or not.
 * 
 * On Unix, when set to 1, Julius forks a separate thread for A/D-in
 * input.  It can be useful when recognition is slow and some audio
 * inputs are dropped.  Note that this should be enabled only for
 * infinite input like microphone or line input, since EOF handling on
 * threaded mode is not supported yet.  Recommended value is 1 for
 * microphone input, 0 for file and network (tcp/ip) input.
 * Ignored on Win32.
 * 
 * @param opcode [in] requested operation code
 * 
 * @return values required for the opcode as described.
 * </EN>
 * 
 * <JA>
 * @brief  ���Ϥΰ����˴ؤ���ѥ�᡼��������֤���ɬ�ܡ�
 *
 * Julius ���������ϥץ饰�����ɤ������٤����ˤĤ��ơ�����ѥ�᡼����
 * �֤���Ϳ����줿�ʲ��� opcode ���Ȥˡ��ͤ��֤���
 *
 * opcode = 0: �ꥢ�륿����ǧ����Ԥ����ɤ����Υǥե������
 *
 * 1 ���֤��ȡ�Julius �����Ϥ��Ф�����ħ��Ф�ǧ��������ʿ�Ԥ��ƹԤ�
 * �ꥢ�륿����ǧ����Ԥ���0 �ξ�硤���ä������Ϥ�ü�ʤ��뤤�϶��ڤ��
 * �ޤǼ�����äƤ��顤��ħ��Ф�Ԥ������θ�ǧ���򳫻Ϥ��롥
 * �ꥢ�륿��������Ǥϡ�CMN �䥨�ͥ륮��ʿ�Ѥʤɡ�ȯ�����Τ��Ѥ���
 * ��ħ�̤����������������롥
 *
 * �̾�ޥ������Ϥʤɥꥢ�륿����ʷ�̤��ߤ������� 1��
 * �ե��������Ϥʤɥ��ե饤��ǧ���ξ��� 0 ���֤����Ȥ�¿����
 *
 * �ʤ����������ͤϡ��������Ϥ����ꤹ��ǥե�����ͤǤ��ꡤ
 * Julius�μ¹Ի����ץ���� "-realtime", "-norealtime" �Ǥ��ѹ��Ǥ��롥
 * ���ץ���󤬻��ꤵ�줿���Ϥ��λ��꤬ͥ�褵��롥
 * 
 * opcode = 1: ̵����ָ��Фˤ�����϶��ڤ�Υǥե������
 *
 * Julius �����ϲ������Ф��ƿ�������򺹤ˤ������Ƚ���Ԥ�����������
 * ���٥�ʲ�����ʬ�򥹥��åפ��������Ƕ��ڤä����ϤȤ��뤳�Ȥ��Ǥ�
 * �롥����̵���Ǥμ�ư���ڤ�Υǥե���Ȥ����� 1 ��ͭ���� 0 ��̵��
 * �ȤǤ��롥
 *
 * �̾�ޥ����ʤɤ�ľ�����ϤǤ� 1����ȯ�ä��Ȥβ����ե�����Ǥ� 0 ��
 * �֤����Ȥ�¿����
 *
 * �ʤ����������ͤϡ��������Ϥ����ꤹ��ǥե�����ͤǤ��ꡤ
 * Julius�μ¹Ի����ץ���� "-cutsilence", "-nocutsilence" �Ǥ��ѹ��Ǥ��롥
 * ���ץ���󤬻��ꤵ�줿���Ϥ��λ��꤬ͥ�褵��롥
 *
 * opcode = 2: �������Ϥ򥹥�åɲ����뤫�Υǥե������
 *
 * �������ϼ����������̥���åɤˤ��뤫�ɤ��������򤹤롥
 * ����ǧ����®�٤��٤��������ǡ����μ�ꤳ�ܤ���ȯ���������ͭ���Ǥ��롥
 * �����������ߤ�Julius�Ǥϡ�EOF �ˤ��ǧ����λ�������������ʤ��Τǡ�
 * �ޥ������Ϥʤɤ�����Ĺ��ͭ�¤Ǥʤ����ϤˤĤ��ƤΤߥ���åɲ���ͭ����
 * ���٤��Ǥ��롥
 *
 * �̾�ޥ��� UDP �ʤɤǤ� 1 �ˤ����ե������ TCP/IP �����åȤǤ�
 * 0 �ˤ��롥
 *
 * @param opcode [in] �׵�ư����� (���� 0 �Τ߼���)
 * 
 * @return opcode ���Ȥ��׵ᤵ�줿�ͤ��֤���
 * </JA>
 */
int
adin_get_configuration(int opcode)
{
  /* For your convenience, UNCOMMENT ONE OF THEM BELOW that match your needs */

  /* typical values for live microphone/line input */
  switch(opcode) {
  case 0:	
    return 1;
  case 1:
    return 1;
  case 2:
    return 1;
  }
  /* typical values for offline file input */
  /* 
   * switch(opcode) {
   * case 0:	   
   *   return 0;
   * case 1:
   *   return 0;
   * case 2:
   *   return 0;
   * }
   */
  /* typical setting for tcpip input */
  /* assuming speech to be segmented at sender */
  /* 
   * switch(opcode) {
   * case 0:	   
   *   return 1;
   * case 1:
   *   return 0;
   * case 2:
   *   return 0;
   * }
   */
  /* typical setting for tcpip input */
  /* assuming receiving continous speech stream and segmented
     should be done at Julius side */
  /* 
   * switch(opcode) {
   * case 0:	   
   *   return 1;
   * case 1:
   *   return 1;
   * case 2:
   *   return 0;
   * }
   */
}
 

/************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/soundcard.h>
static int audio_fd;
static int freq;

/**
 * <EN>
 * @brief  Initialize input device (required)
 *
 * This will be called only once at start up of Julius.  You can
 * initialize the device, check if the device exists or prepare a socket
 * for connection.
 *
 * If this function returns FALSE, Julius will exit.
 * 
 * JuliusLib: this function will be called at j_adin_init().
 *
 * @param sfreq [in] required sampling frequency
 * @param dummy [in] a dummy data
 * 
 * @return TRUE on success, FALSE on failure.
 * </EN>
 * <JA>
 * @brief  �ǥХ��������������ɬ�ܡ�
 *
 * ���δؿ��ϵ�ư���˰������ƤФ�롥�����ǤϥǥХ����Υ����å���
 * �����åȤ��ѰդȤ��ä����������ϤΤ���ν�����Ԥ��Τ˻Ȥ���
 *
 * FALSE ���֤�����硤Julius �Ͻ�λ���롥
 * 
 * JuliusLib: ���δؿ��� j_adin_init() �ǸƤФ�롥
 *
 * @param sfreq [in] ����ץ�󥰥졼��
 * @param dummy [in] ���ߡ��ǡ�����̤���ѡ�
 * 
 * @return ������ TRUE�����Ի� FALSE ���֤���
 * </JA>
 */
boolean
adin_standby(int sfreq, void *dummy)
{
  /* store the frequency */
  freq = sfreq;
  return TRUE;
}
 
/**
 * <EN>
 * @brief  Open an input stream (required)
 *
 * This function should open a new audio stream for input.
 * You may open a capture device, open an audio file, or wait for
 * connection with other network client at this function.
 *
 * If this function returns FALSE, Julius will exit recognition loop.
 * 
 * JuliusLib: this will be called at j_open_stream().
 *
 * @param pathname [in] file / device name to open or NULL for default
 * 
 * @return TRUE on success, FALSE on failure.
 * </EN>
 * <JA>
 * @brief  ���ϲ������ȥ꡼��򳫤���ɬ�ܡ�
 *
 * ���ϲ������ȥ꡼��򿷵��˳������̾�ǥХ�����ե�����Υ����ץ�
 * �ͥåȥ�����饤����Ȥ������³�ʤɤ򤳤��ǹԤ���
 *
 * FALSE ���֤����Ȥ���Julius ��ǧ���롼�פ�ȴ���롥
 * 
 * JuliusLib: ���δؿ��� j_open_stream() ��ǸƤФ�롥
 * 
 * @param pathname [in] �����ե����뤢�뤤�ϥǥХ���̾��NULL �ʤ�ǥե����
 * 
 * @return ������ TRUE�����Ի� FALSE ���֤���
 * </JA>
 */
boolean
adin_open(char *pathname)
{
  /* do open the device */
  int fmt;
  int stereo;
  int ret;
  int s;
  char buf[2];

  if ((audio_fd = open(pathname ? pathname : "/dev/dsp", O_RDONLY)) == -1) {
    printf("Error: cannot open %s\n", pathname ? pathname : "/dev/dsp");
    return FALSE;
  }
  fmt = AFMT_S16_LE;               /* 16bit signed (little endian) */
  if (ioctl(audio_fd, SNDCTL_DSP_SETFMT, &fmt) == -1) {
    printf("Error: failed set format to 16bit signed\n");
    return FALSE;
  }
  stereo = 0;			/* mono */
  ret = ioctl(audio_fd, SNDCTL_DSP_STEREO, &stereo);
  if (ret == -1 || stereo != 0) {
    stereo = 1;
    ret = ioctl(audio_fd, SNDCTL_DSP_CHANNELS, &stereo);
    if (ret == -1 || stereo != 1) {
      printf("Error: failed to set monoral channel\n");
      return FALSE;
    }
  }
  s = freq;
  if (ioctl(audio_fd, SNDCTL_DSP_SPEED, &s) == -1) {
    printf("Erorr: failed to set sample rate to %dHz\n", freq);
    return FALSE;
  }

  /* start recording... */
  read(audio_fd, buf, 2);

  return(TRUE);
}

/**
 * <EN>
 * @brief  Read samples from device (required)
 *
 * This function is for reading samples to be recognized from input stream.
 * This will be called repeatedly at each time the read samples are fully
 * processed.
 *
 * The sampling format should be 16bit, 1 channel.
 *
 * @a sampnum is the maximum number of samples that can be read into @a buf.
 * The actual number of read samples should be returned.
 *
 * Impotant notes about I/O blocking:
 *  - Do not wait until all the @a sampnum samples are read.
 *    Blocking inside this function will block the whole recognition process.
 *    If device allows, it is better to read only the available data
 *    in the stream and return immediately.
 *  - Avoid returning value of 0 when no data is available, wait for some
 *    data to come inside this function.  When you are using non-blocking
 *    operation, you may want to return 0 when no data is available.
 *    However, returning 0 will cause Julius to call again this function
 *    immediately, and cause busy loop to make CPU load to reach 100%.
 *
 * So the ideal operation will be first wait_for_some_data_to_come, and
 * if any data becomes available, read them at most @a sampnum samples
 * and return the number of read samples.
 *
 * Positive return value should be the number of read samples, or one
 * of ADIN_EOF, ADIN_SEGMENT or ADIN_ERROR.  Return value of ADIN_EOF
 * tells end of stream, which causes Julius to finish current
 * recognition and close stream.  ADIN_SEGMENT requests Julius to
 * segment the current input.  The current recognition will be stopped
 * at this point, recognition result will be output, and then Julius
 * continues to the next input.  The behavior of ADIN_SEGMENT is
 * similar to ADIN_EOF except that ADIN_SEGMENT does not close/open
 * stream, but just stop and restart the recognition.  At last, return
 * value should be ADIN_ERROR on error, in which Julius exits itself
 * immediately.
 * 
 * @param buf [out] output buffer to store samples obtained.
 * @param sampnum [in] maximum number of samples that can be stored in @a buf.
 * 
 * @return actural number of read samples, ADIN_EOF on end of stream,
 * ADIN_SEGMENT to request segmentation to Julius, or ADIN_ERROR on error.
 * </EN>
 * <JA>
 * @brief  �ǥХ������饵��ץ���ɤ߹����ɬ�ܡ�
 *
 * ���δؿ������ϥ��ȥ꡼�फ�鲻������ץ���ɤ߹��ࡥ
 *
 * �Хåե��˳�Ǽ�����֤������ǡ����η����� 16bit, 1 �����ͥ�Ǥ��뤳�ȡ�
 * 
 * @a sampnum �� @a buf �˳�Ǽ���뤳�ȤΤǤ������Υ���ץ���Ǥ��롥
 * �֤��ͤȤ��ơ��ºݤ��ɤ߹��ޤ줿����ץ�������뤤�ϰʲ�����������
 * ���顼�����ɤ��֤���
 * 
 * ���δؿ���ǧ����˲��٤�ƤФ졤�������ɤޤ줿�ǡ����� Julius �ˤ��
 * �� ǧ����������롥�ɤ߹����ʬ�ν�������λ����ȡ��������Ϥ��ɤ߹�
 * �ि��ˤ��δؿ������ٸƤФ�롥
 * 
 * ���δؿ���Ǥ� I/O blocking �ˤĤ��Ƥϰʲ�����դ�ɬ�פǤ��롧
 * 
 *  - Ĺ���֤Υ֥�å����򤱤뤳�ȡ�@a sampnum ���׵᥵��ץ���ǤϤ�
 *  ��@a buf �˳�Ǽ��ǽ�ʺ�����Ǥ���ˡ����δؿ���ǥ֥�å������ǧ
 *  ���������Τ��֥�å����롥�ɤ߹��ߤ�Ĺ���֥֥�å����ʤ��褦����ɴ
 *  ����ץ����٤����ɤ߹�����֤��������뤤�Ϻǽ�˥Хåե���ˤ����
 *  ��å��������ɤ߹��߲�ǽ�ʥǡ�������ץ���������������ʬ�����ɤ�
 *  ����褦�ˤ���Τ��褤��
 *    
 *  - non-blocking �⡼�ɤ��Ѥ����硤 0 ���֤��ʤ����ȡ�
 *    �Хåե��˥ǡ�����¸�ߤ��ʤ��Ȥ���0 ���֤��� Julius �ϥ���ץ�
 *    ̵���Τ���ޤ�¨�¤ˤ��δؿ���ƤӽФ������줬�ӥ����������Ȥ�
 *    ȯ��������CPU���ɤ������äƤ��ޤ����Хåե��˥ǡ�����̵���Ȥ���
 *    ¨�¤� 0 ���֤���������msec �Ǥ褤�ΤǤ��δؿ�����Ԥ�
 *    ���Ȥ�˾�ޤ�����
 *
 * �֤��ͤϡ��ºݤ��ɤ߹��������ץ���������ͤȤ����֤��������뤤��
 * ADIN_EOF, ADIN_SEGMENT, ADIN_ERROR �Τɤ줫���֤���ADIN_EOF �ϥ���
 * �꡼�ब��ü�ޤ�ã�������Ȥ�ɽ����������֤��ȡ�Julius �ϸ��ߤ�ǧ��
 * ������λ���������ȥ꡼����Ĥ��롥ADIN_ERROR �Ϥ��δؿ���ǿ����
 * ���顼�������������֤������줬�֤��줿��硤Julius �Ϥ��ξ�ǰ۾�
 * ��λ���롥
 *
 * ADIN_SEGMENT ���֤����Ȥǡ�Julius �˸��ߤ�ǧ���򸽻����Ƕ��ڤ뤳��
 * ���׵᤹�뤳�Ȥ��Ǥ��롥���ߤ�ǧ�������Ϥ��λ����Ǥ��ä�����ڤ�졤
 * �����ޤǤ�ǧ����̤����ꡦ���Ϥ��줿���ȡ�����ǧ���������Ϥޤꤳ��
 * �ؿ����ƤФ�롥ADIN_SEGMENT �� ADIN_EOF ��ư����Ƥ��뤬��
 * ADIN_EOF �� adin_close(), adin_open() ��Ƥ�ǥ��ȥ꡼���λ����
 * ��Τ��Ф��ơ�ADIN_SEGMENT �Ϥ�����ƤФ������Ϥ�³�Ԥ��롥���ε�
 * ǽ�ϡ����Ȥ��Хͥåȥ����ͳ�ǲ����ǡ�����������Ƥ���Ȥ��ˡ���
 * ��¦���鲻��ǧ����ON/OFF��VAD�򥳥�ȥ��뤷�������ʤɤ�
 * �Ȥ����Ȥ��Ǥ��롥
 * 
 * @param buf [out] ����줿����ץ���Ǽ����Хåե�
 * @param sampnum [in] @a buf ��˳�Ǽ�Ǥ�����祵��ץ��
 * 
 * @return �ºݤ��ɤ߹��ޤ줿����ץ�������뤤�� end of stream ���� ADIN_EOF,
 * Julius �˶��ڤ��׵��Ф��Ȥ��ˤ� ADIN_SEGMENT, ���顼����ADIN_ERROR ��
 * �֤���
 * </JA>
 */
int
adin_read(SP16 *buf, int sampnum)
{
  audio_buf_info info;
  int size, cnt;

  /* get sample num that can be read without blocking */
  if (ioctl(audio_fd, SNDCTL_DSP_GETISPACE, &info) == -1) {
    printf("Error: adin_oss: failed to get number of samples in the buffer\n");
    return(ADIN_ERROR);
  }
  /* get them as much as possible */
  size = sampnum * sizeof(SP16);
  if (size > info.bytes) size = info.bytes;
  size &= ~ 1;		/* Force 16bit alignment */
  cnt = read(audio_fd, buf, size);
  if ( cnt < 0 ) {
    printf("Error: adin_oss: failed to read samples\n");
    return (ADIN_ERROR);
  }
  cnt /= sizeof(short);
    
  return(cnt);
}

/**
 * <EN>
 * @brief  Close the current input stream (required)
 *
 * This function will be called when the input stream has reached
 * end of file (i.e. the last call of adin_read() returns ADIN_EOF)
 *       
 * You may close a capture device, close an audio file, or
 * disconnect network client.
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
 * @brief  ���ϥ��ȥ꡼����Ĥ����ɬ�ܡ�
 *
 * ���ߤΥ��ȥ꡼����Ĥ��롥���δؿ��ϡ����ϥ��ȥ꡼�ब��ü��EOF��
 * ��ã�����Ȥ��ʤ��ʤ�� adin_read() �� ADIN_EOF ���֤����Ȥ��ˤ�
 * �ƤФ�롥�ǥХ������Ĥ��롤�ե�������Ĥ��롤���뤤�ϥͥåȥ����³��
 * ���Ǥ���Τ˻Ȥ����Ȥ��Ǥ��롥
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
adin_close()
{
  close(audio_fd);
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
adin_terminate()
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
 * �����׵� (adin_terminate) �Ȥΰ㤤�ϡ�ǧ��������׵��������Ȥ���ư�
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
adin_pause()
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
adin_resume()
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
adin_input_name()
{
  printf("input name function was called\n");
  return("default");
}


/* end of file */
