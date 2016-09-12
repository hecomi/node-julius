/**
 * @file   calcmix.c
 * 
 * <EN>
 * @brief  A sample plugin for calculating Gaussians
 *
 * This sample uses Julius libraries.
 * </EN>
 * 
 * <JA>
 * @brief  ������ʬ�۷׻��ץ饰����Υ���ץ�
 *
 * ���Υ���ץ�� julius �Υ饤�֥�����Ѥ��ޤ���
 * </JA>
 * 
 * @author Akinobu Lee
 * @date   Mon Aug 11 15:29:45 2008
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
 * Gaussian mixture calculation plugin:
 * 
 * Required:
 *   - calcmix_get_optname()
 *   - calcmix()
 *   - calcmix_init()
 *   - calcmix_free()
 * 
 */

/***************************************************************************/

/* we refer to Julius libsent header */
#include <sent/stddefs.h>
#include <sent/hmm_calc.h>

//#include "plugin_defs.h"

#define PLUGIN_TITLE "Gaussian calculation plugin for Julius"
#define GPRUNE_OPT "mycalc"

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

/** 
 * <EN>
 * @brief  Return option string to select at option. (required)
 *
 * This function should return option string which should be specified
 * as an argument "-gprune" option, to be used on Julius.  The returning
 * string should not be the same with any existing value.
 *
 * This function will be called several times at option parsing at startup.
 *
 * @param buf [out] buffer to store the return string
 * @param buflen [in] maximum length of buf
 * </EN>
 * <JA>
 * @brief  �׻���ˡ�����ѥ��ץ����Τ����ʸ������֤���ɬ�ܡ�
 *
 * Julius �ǵ�ư���� "-gprune �������֤���" �Ȼ��ꤹ��Ȥ��Υץ饰����
 * �����Ѥ���롥���δؿ��Ǥϡ��嵭�� "-gprune" ��Ϳ����٤�ʸ������
 * Ǽ�����֤����֤�ʸ���ϡ������ƥ�� "-gprune" ���ץ����ˤ��Ǥˤ���
 * ��Τ䡤¾�Υץ饰���󤬻��Ѥ��Ƥ����Τ�Ʊ���Ǥʤ����ȡ��ʤ⤷Ʊ
 * �����ä���祷���ƥ�¦��ͥ�褵����
 *
 * ���δؿ��ϡ���ư���Υ��ץ������ϻ��˲��٤��ƤФ�롥
 *
 * @param buf [out] �ͤ��Ǽ�����֤��Хåե�
 * @param buflen [in] buf �κ���Ĺ
 * </JA>
 * 
 */
void
calcmix_get_optname(char *buf, int buflen)
{
  strncpy(buf, GPRUNE_OPT, buflen);
}

/**
 * <EN>
 * @brief  A basic implementaion of computing Gaussians
 *
 * This function should compute output probabilities for each
 * Gaussians.  after this function returns, Julius will do addlog to
 * get the final output log probability.
 *
 * The input vector to be computed is located at wrk->OP_vec[], at a
 * length of wrk->OP_veclen.  Gaussians are given by g[], at a number
 * of num.  last_id and lnum is for internal use for pruning, just ignore
 * them.
 *
 * The scores for each Gaussians computed in this function should be
 * stored in OP_calced_score[], with their corresponding Gaussian ids
 * to OP_calced_id.  The total number of calculated mixtures shuold
 * also stored in OP_calced_num.
 * 
 * @param wrk [i/o] HMM computation work area to store data
 * @param g [in] set of Gaussian densities to compute the output probability.
 * @param num [in] length of above
 * @param last_id [in] ID list of N-best mixture in previous input frame,
 * or NULL if not exist
 * @param lnum [in] length of last_id
 * </EN>
 * <JA>
 * @brief  ������ʬ�۷׻��ؿ�
 *
 * ���δؿ��Ǥϡ�Ϳ����줿ʣ���Υ�����ʬ�ۤ��Ф������ϥ٥��ȥ��
 * ���ϳ�Ψ����롥���δؿ����Ԥ��Τϡ�ʣ���Υ�����ʬ�ۤ��줾���
 * ���ϳ�Ψ�λ��Фȳ�Ǽ�ΤߤǤ��ꡤ����ʬ�ۤȤ��ƤνŤ߷׻��� addlog
 * �Ϥ��δؿ����֤ä����Ȥ� Julius ¦�ǹԤ��롥
 *
 * ���ϥ٥��ȥ�� wrk->OP_vec[] �˳�Ǽ����Ƥ��ꡤĹ���� wrk->OP_veclen
 * �Ǥ��롥������ʬ������� g[] ������Ȥ���ʣ���Ϥ��졤���ο��� num �Ǥ��롥
 * 
 * �ʤ���last_id �� lnum �Ϥ��Υ�����ʬ�۽��� g[] �ˤ�����ľ�������ϥ�
 * �졼��Ƿ׻����줿��Τ� id �����äƤ��롥Julius �����������ѤʤΤǡ�
 * �Ȥ�ʤ��Ƥ⺹���٤��ʤ���
 *
 * �ƥ�����ʬ�ۤ��Ф������ϥ٥��ȥ���п����ϳ�Ψ�ϡ����Υ�����ʬ�ۤ�
 * ID (0 ����Ϥޤ������ź����) �� wrk->OP_calced_id �ˡ��ͤ�
 * wrk->OP_calced_score �˳�Ǽ���뤳�ȡ��ޤ����ºݤ˷׻����줿
 * ������ʬ�ۤο��� wrk->OP_calced_num �˳�Ǽ���뤳�ȡ�
 * �ʤ���� Gaussian pruning �����ꤷ�������Ǥ����
 *
 * �ʲ��ϡ�pruning ����Ԥ�ʤ�ñ��ʽ��ϳ�Ψ�׻������������ΤǤ��롥
 * ������ʬ�ۤ��гѶ�ʬ�����ꤷ�Ƥ��롥�ʤ� Julius �Ǥ��ɤ߹��߻���
 * HTK �Ǥ����Ȥ���� gconst �ͤϤ��餫����׻�����롥���Τ��ᡤ�׻�����
 * ������ dens->gconst �Τ褦�����ѤǤ��롥
 * 
 * @param wrk [i/o] HMM�׻��ѥ�����ꥢ
 * @param g [in] ���ϳ�Ψ��׻����륬����ʬ�ۤ���
 * @param num [in] @a g �Υ�����ʬ�ۤο�
 * @param last_id [in] ľ�����ϥե졼��Ǿ�̤��ä�������ʬ�ۤ�ID�ꥹ�ȡ�
 * �ޤ�������� NULL
 * @param lnum [in] @a last_id ��Ĺ��
 * </JA>
 */
void
calcmix(HMMWork *wrk, HTK_HMM_Dens **g, int num, int *last_id, int lnum)
{
  int i;
  HTK_HMM_Dens *dens;
  LOGPROB *prob = wrk->OP_calced_score;
  int *id = wrk->OP_calced_id;
  VECT tmp, x;
  VECT *mean;
  VECT *var;
  VECT *vec;
  short veclen;

  for(i=0; i<num; i++) {
    /* store ID */
    *(id++) = i;
    /* get Gaussian to compute */
    dens = *(g++);
    if (dens == NULL) {
      /* no Gaussian, set LOG_ZERO as result */
      *(prob++) = LOG_ZERO;
      continue;
    }
    /* compute log outprob probability */
    mean = dens->mean;
    var = dens->var->vec;
    tmp = dens->gconst;
    vec = wrk->OP_vec;
    veclen = wrk->OP_veclen;
    for (; veclen > 0; veclen--) {
      x = *(vec++) - *(mean++);
      tmp += x * x * *(var++);
    }
    tmp *= -0.5;
    /* store it */
    *(prob++) = tmp;
  }
  wrk->OP_calced_num = num;
}

/**
 * <EN>
 * Free work area.
 * You should free all allocated at clacmix_init().
 * 
 * @param wrk [i/o] HMM computation work area
 * </EN>
 * <JA>
 * calcmix_init() �ǳ��ݤ��줿������ꥢ�������롥
 * 
 * @param wrk [i/o] HMM �׻��ѥ�����ꥢ
 * </JA>
 * 
 */
void
calcmix_free(HMMWork *wrk)
{
  free(wrk->OP_calced_score);
  free(wrk->OP_calced_id);
}
  
/**
 * <EN>
 * @brief  Initialize and setup work area for Gaussian computation.
 *
 * You should set value for OP_calced_maxnum, and allocate OP_calced_score
 * and OP_calced_id.  Remaining the content below is safe.
 *
 * This will be called once on instance initialization at startup.
 * 
 * @param wrk [i/o] HMM computation work area
 * 
 * @return TRUE on success, FALSE on failure.
 * </EN>
 * <JA>
 * @brief  �׻��ѤΥ�����ꥢ����ݤ��롥
 *
 * ������ʬ�۷׻��ѤΥ�����ꥢ����ݤ��롥�����ˤ��Ǥ˽񤤤Ƥ���ʬ�ϡ�
 * ���Τޤ� Julius �������Ǥ���Ѥ��Ƥ���Τǡ����ʤ����ȡ�
 *
 * ���δؿ��Ϻǽ�˲������ٷ׻����󥹥��󥹤����������Ȥ��˸ƤӽФ���롥
 * 
 * @param wrk [i/o] HMM �׻��ѥ�����ꥢ
 * 
 * @return ������ TRUE�����Ի� FALSE ���֤���
 * </JA>
 */
boolean
calcmix_init(HMMWork *wrk)
{
  /* maximum Gaussian set size = maximum mixture size * nstream */
  wrk->OP_calced_maxnum = wrk->OP_hmminfo->maxmixturenum * wrk->OP_nstream;
  wrk->OP_calced_score = (LOGPROB *)malloc(sizeof(LOGPROB) * wrk->OP_calced_maxnum);
  wrk->OP_calced_id = (int *)malloc(sizeof(int) * wrk->OP_calced_maxnum);
  /* force gprune_num to the max number */
  wrk->OP_gprune_num = wrk->OP_calced_maxnum;
  return TRUE;
}

/* end of file */
