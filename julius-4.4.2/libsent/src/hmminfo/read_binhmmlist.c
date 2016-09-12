/**
 * @file   read_rdhmmlist.c
 * 
 * <JA>
 * @brief  HMMList�ե�������ɤ߹���ʥХ��ʥ��ǡ�
 *
 * HMMList �ե�����ϡ������β���ɽ���ʥȥ饤�ե���ɽ���ˤ���
 * �ºݤ��������Ƥ��� %HMM �ؤΥޥåԥ󥰤�Ԥʤ��ե�����Ǥ���
 * 
 * HMMList�ե�����Ǥϡ��о줷���벻�ǤˤĤ��ơ��б�����
 * HMM �����̾���򵭽Ҥ��ޤ�����Ԥˣ��ĤŤġ���1�����˲���̾��
 * ���ڡ����Ƕ��ڤä���2�������������Ƥ���ºݤ� %HMM ̾����ꤷ�ޤ���
 * ��1��������2����ब����Ʊ����硤���ʤ�����β���̾�Υ�ǥ뤬ľ��
 * %HMM �Ȥ����������Ƥ�����ϡ���2�����Ͼ�ά���뤳�Ȥ��Ǥ��ޤ���
 *
 * �ȥ饤�ե�����ѻ��ϡ�HMMList�ե�������о줷�������ƤΥȥ饤�ե����
 * �Ĥ��Ƶ��Ҥ���ɬ�פ�����������դ��Ʋ��������⤷Ϳ����줿ǧ������
 * ����о줷����ȥ饤�ե���HMMList�˵��Ҥ���Ƥ��ʤ���硤
 * ���顼�Ȥʤ�ޤ���
 * </JA>
 * 
 * <EN>
 * @brief  Read in HMMList file from binary format
 *
 * HMMList file specifies how the phones as described in word dictionary,
 * or their context-dependent form, should be mapped to actual defined %HMM.
 *
 * In HMMList file, the possible phone names and their corresponding %HMM
 * name should be specified one per line.  The phone name should be put on
 * the first column, and its corresponding %HMM name in the HTK %HMM definition
 * file should be defined on the second column.  If the two strings are
 * the same, which occurs when a %HMM of the phone name is directly defined,
 * the second column can be omitted.
 *
 * When using a triphone model, ALL the possible triphones that can appear
 * on the given word dictionary should be specified in the HMMList file.
 * If some possible triphone are not specified in the HMMList, Julius
 * produces error.
 * </EN>
 * 
 * @author Akinobu LEE
 * @date   Wed Feb 16 04:04:23 2005
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
#include <sent/htk_hmm.h>
#include <sent/ptree.h>

/** 
 * Callback to read hmmlist data from file.
 * 
 * @param data_p [out] pointer to index tree node to store the leaf content
 * @param data [in] user data
 * @param fp [in] file pointer to read
 * 
 * @return TRUE on success, FALSE on failure.
 */
static boolean
load_hmmlist_callback(void **data_p, void *data, FILE *fp)
{
  HTK_HMM_INFO *hmminfo = data;
  HMM_Logical *new;
  int len;
  char buf[MAX_HMMNAME_LEN];
  HTK_HMM_Data *d;

  new = (HMM_Logical *)mybmalloc2(sizeof(HMM_Logical), &(hmminfo->lroot));
  if (myfread(&(new->is_pseudo), sizeof(boolean), 1, fp) < 1) return FALSE;
  if (myfread(&len, sizeof(int), 1, fp) < 1) return FALSE;
  new->name = (char *)mybmalloc2(len, &(hmminfo->lroot));
  if (myfread(new->name, len, 1, fp) < 1) return FALSE;
  if (myfread(&len, sizeof(int), 1, fp) < 1) return FALSE;
  if (myfread(buf, len, 1, fp) < 1) return FALSE;
  if (new->is_pseudo) {
    jlog("Error: sorry, pseudo logical phone saving not implemented yet\n");
    return FALSE;
  } else {
    d = htk_hmmdata_lookup_physical(hmminfo, buf);
    if (d == NULL) {
      jlog("Error: load_hmmlist_callback: no name \"%s\" in hmmdefs!\n", buf);
      return FALSE;
    }
    new->body.defined = d;
  }
  new->next = hmminfo->lgstart;
  hmminfo->lgstart = new;

  *data_p = new;
  
  return TRUE;
}

typedef struct {
  HTK_HMM_Trans **tr;
  HTK_HMM_State **st;
} ListData;

/** 
 * Callback to read cdset data from file.
 * 
 * @param data_p [out] pointer to index tree node to store the leaf content
 * @param data [in] user data
 * @param fp [in] file pointer to read
 * 
 * @return TRUE on success, FALSE on failure.
 */
static boolean
load_cdset_callback(void **data_p, void *data, FILE *fp)
{
  HTK_HMM_INFO *hmminfo = data;
  ListData *ld = (ListData *)hmminfo->hook;
  CD_Set *new;
  int len;
  int id;
  int i, j;

  new = (CD_Set *)mybmalloc2(sizeof(CD_Set), &(hmminfo->cdset_root));
  if (myfread(&len, sizeof(int), 1, fp) < 1) return FALSE;
  new->name = (char *)mybmalloc2(len, &(hmminfo->cdset_root));
  if (myfread(new->name, len, 1, fp) < 1) return FALSE;
  if (myfread(&id, sizeof(int), 1, fp) < 1) return FALSE;
  new->tr = ld->tr[id];
  if (myfread(&(new->state_num), sizeof(unsigned short), 1, fp) < 1) return FALSE;
  new->stateset = (CD_State_Set *)mybmalloc2(sizeof(CD_State_Set) * new->state_num, &(hmminfo->cdset_root));
  for(i=0;i<new->state_num;i++) {
    if (myfread(&(new->stateset[i].num), sizeof(unsigned short), 1, fp) < 1) return FALSE;
    new->stateset[i].maxnum = new->stateset[i].num;
    new->stateset[i].s = (HTK_HMM_State **)mybmalloc2(sizeof(HTK_HMM_State *) * new->stateset[i].num, &(hmminfo->cdset_root));
    for(j=0;j<new->stateset[i].num;j++) {
      if (myfread(&id, sizeof(int), 1, fp) < 1) return FALSE;
      new->stateset[i].s[j] = ld->st[id];
    }
  }  

  *data_p = new;
  
  return TRUE;
}

/** 
 * qsort callback function to sort state pointers by their
 * address for indexing.
 * 
 * @param s1 [in] data 1
 * @param s2 [in] data 2
 * 
 * @return value required for qsort.
 */
static int
qsort_st_index(HTK_HMM_State **s1, HTK_HMM_State **s2)
{
  /* keep ID order */
  if ((*s1)->id > (*s2)->id) return 1;
  else if ((*s1)->id < (*s2)->id) return -1;
  else return 0;
}

/** 
 * Load HMMList and CDSet data from binary file.
 * 
 * @param fp [in] file pointer to read
 * @param hmminfo [in] HMM definition
 * 
 * @return TRUE on success, FALSE on failure.
 */
boolean
load_hmmlist_bin(FILE *fp, HTK_HMM_INFO *hmminfo)
{
  HMM_Logical *l;
  int n;
  HTK_HMM_Trans *t;
  HTK_HMM_State *s;
  ListData ld;

  /* build id->entity list */
  ld.tr = (HTK_HMM_Trans **)mymalloc(sizeof(HTK_HMM_Trans *) * hmminfo->totaltransnum);
  n = 0;
  for (t = hmminfo->trstart; t; t = t->next) {
    ld.tr[n++] = t;
  }
  ld.st = (HTK_HMM_State **)mymalloc(sizeof(HTK_HMM_State *) * hmminfo->totalstatenum);
  n = 0;
  for (s = hmminfo->ststart; s; s = s->next) {
    ld.st[n++] = s;
  }
  qsort(ld.st, hmminfo->totalstatenum, sizeof(HTK_HMM_State *), (int (*)(const void *, const void *))qsort_st_index);

  /* set it to hook */
  hmminfo->hook = &ld;

  /* load hmmlist */
  jlog("Stat: load_hmmlist_bin: reading hmmlist\n");
  if (aptree_read(fp, &(hmminfo->logical_root), &(hmminfo->lroot), hmminfo, load_hmmlist_callback) == FALSE) {
    jlog("Error: load_hmmlist_bin: failed to read hmmlist from binary file\n");
    return FALSE;
  }
  /* load cdset */
  jlog("Stat: load_hmmlist_bin: reading pseudo phone set\n");
  if (aptree_read(fp, &(hmminfo->cdset_info.cdtree), &(hmminfo->cdset_root), hmminfo, load_cdset_callback) == FALSE) {
    jlog("Warning: load_hmmlist_bin: cdset not in binary file, skip reading\n");
  } else {
    hmminfo->cdset_info.binary_malloc = TRUE;
  }

  /* remove data */
  free(ld.st);
  free(ld.tr);
  hmminfo->hook = NULL;

  /* form other data */
  n = 0;
  for(l=hmminfo->lgstart;l;l=l->next) n++;
  hmminfo->totallogicalnum = n;

  return TRUE;
}
