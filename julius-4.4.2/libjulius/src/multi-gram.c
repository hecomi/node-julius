/**
 * @file   multi-gram.c
 * 
 * <JA>
 * @brief  ǧ����ʸˡ�δ���
 *
 * ���Υե�����ˤϡ�ǧ����ʸˡ���ɤ߹��ߤȴ�����Ԥ��ؿ����ޤޤ�Ƥ��ޤ�. 
 * �����δؿ��ϡ�ʸˡ�ե�������ɤ߹��ߡ�����ӳƼ�ǡ�����
 * ���åȥ��åפ�Ԥ��ޤ�. 
 *
 * ʣ��ʸˡ��Ʊ��ǧ�����б����Ƥ��ޤ�. ʣ����ʸˡ����٤��ɤ߹���ǡ�
 * �����ǧ����Ԥ��ޤ�. �ޤ����⥸�塼��⡼�ɤǤϡ����饤����Ȥ���
 * ǧ���¹����ʸˡ��ưŪ���ɲá���������ꡤ����ʬ��ʸˡ��̵������
 * ͭ����������Ǥ��ޤ�. �ޤ�Ϳ����줿�ġ���ʸˡ���Ȥ�ǧ����̤�
 * �Ф����Ȥ��Ǥ��ޤ�. 
 *
 * Ϳ����줿��ʣ���Ρ�ʸˡ�ϰ�ĤΥ����Х�ʸˡ�Ȥ��Ʒ�礵��,
 * ʸˡ���ɤ߹��ߤ����ʤɤξ����ѹ���Ԥä��Ȥ�����������ޤ�. 
 * ��礵�줿��ʸ��§ (DFA) �� global_dfa �ˡ����ü��� global_winfo ��
 * ���줾�������˳�Ǽ����ޤ�. ������Ŭ�ڤʥ����ߥ󥰤�
 * multigram_build() ���ƤӽФ��줿�Ȥ��ˡ�global.h �������ѿ� dfa
 * ����� winfo �˥��ԡ����졤ǧ�������ˤ����ƻ��Ѥ����褦�ˤʤ�ޤ�. 
 * </JA>
 * 
 * <EN>
 * @brief  Management of Recognition grammars
 *
 * This file contains functions to read and manage recognition grammar.
 * These function read in grammar and dictionary, and setup data for
 * recognition.
 *
 * Recognition with multiple grammars are supported.  Julian can read
 * several grammars specified at startup time, and perform recognition
 * with those grammars simultaneously.  In module mode, you can add /
 * delete / activate / deactivate each grammar while performing recognition,
 * and also can output optimum results for each grammar.
 *
 * Internally, the given grammars are composed to a single Global Grammar.
 * The global grammar will be updated whenever a new grammar has been read
 * or deleted.  The syntax rule (DFA) of the global grammar will be stored
 * at global_dfa, and the corresponding dictionary will be at global_winfo
 * locally, independent of the decoding timing.  After that, multigram_build()
 * will be called to make the prepared global grammar to be used in the
 * actual recognition process, by copying the grammar and the dictionary
 * to the global variable dfa and winfo.
 * 
 * @author Akinobu Lee
 * @date   Sat Jun 18 23:45:18 2005
 *
 * $Revision: 1.12 $
 * 
 */
/*
 * Copyright (c) 1991-2013 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2013 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */


#include <julius/julius.h>

/// For debug: define to enable grammar update messages to stdout
#define MDEBUG

/** 
 * <JA>
 * @brief  �����Х�ʸˡ�����ڹ�¤��������ۤ���. 
 *
 * Ϳ����줿ʸˡ��ǧ����Ԥ�����ˡ�ǧ���������󥹥��󥹤����߻���
 * �����Х�ʸˡ�����ڹ�¤�������ʺơ˹��ۤ��ޤ�. �ޤ��� 
 * ��ư���˥ӡ�����������Ū�˻ؼ�����Ƥ��ʤ�����ե륵�����ξ�硤
 * �ӡ������κ������Ԥ��ޤ�.
 * 
 * @param r [i/o] ǧ���������󥹥���
 * </JA>
 * <EN>
 * @brief  Build tree lexicon from global grammar.
 *
 * This function will re-construct the tree lexicon using the global grammar
 * in the recognition process instance.  If the beam width was not explicitly
 * specified on startup, the the beam width will be guessed
 * according to the size of the new lexicon.
 * 
 * @param r [i/o] recognition process instance
 * </EN>
 */
static boolean
multigram_rebuild_wchmm(RecogProcess *r)
{
  boolean ret;

  /* re-build wchmm */
  if (r->wchmm != NULL) {
    wchmm_free(r->wchmm);
  }
  r->wchmm = wchmm_new();
  r->wchmm->lmtype = r->lmtype;
  r->wchmm->lmvar  = r->lmvar;
  r->wchmm->ccd_flag = r->ccd_flag;
  r->wchmm->category_tree = TRUE;
  r->wchmm->hmmwrk = &(r->am->hmmwrk);
  /* assign models */
  r->wchmm->dfa = r->lm->dfa;
  r->wchmm->winfo = r->lm->winfo;
  r->wchmm->hmminfo = r->am->hmminfo;
  if (r->wchmm->category_tree) {
    if (r->config->pass1.old_tree_function_flag) {
      ret = build_wchmm(r->wchmm, r->lm->config);
    } else {
      ret = build_wchmm2(r->wchmm, r->lm->config);
    }
  } else {
    ret = build_wchmm2(r->wchmm, r->lm->config);
  }

  /* ��ư�� -check �ǥ����å��⡼�ɤ� */
  if (r->config->sw.wchmm_check_flag) {
    wchmm_check_interactive(r->wchmm);
  }
  
  if (ret == FALSE) {
    jlog("ERROR: multi-gram: failed to build (global) lexicon tree for recognition\n");
    return FALSE;
  }
  
  /* guess beam width from models, when not specified */
  r->trellis_beam_width = set_beam_width(r->wchmm, r->config->pass1.specified_trellis_beam_width);
  switch(r->config->pass1.specified_trellis_beam_width) {
  case 0:
    jlog("STAT: multi-gram: beam width set to %d (full) by lexicon change\n", r->trellis_beam_width);
    break;
  case -1:
    jlog("STAT: multi-gram: beam width set to %d (guess) by lexicon change\n", r->trellis_beam_width);
  }

  /* re-allocate factoring cache for the tree lexicon*/
  /* for n-gram only?? */
  //max_successor_cache_free(recog->wchmm);
  //max_successor_cache_init(recog->wchmm);

  /* finished! */

  return TRUE;
}

/** 
 * <EN>
 * @brief  Check for global grammar and (re-)build tree lexicon if needed.
 * 
 * If any modification of the global grammar has been occured, 
 * the tree lexicons and some other data for recognition will be re-constructed
 * from the updated global grammar.
 * </EN>
 * <JA>
 * @brief  �����Х�ʸˡ��Ĵ�١�ɬ�פ�������ڹ�¤�������ʺơ˹��ۤ���. 
 * 
 * �����Х뼭����ѹ�������С����ι������줿�����Х�
 * ���񤫤��ڹ�¤������ʤɤβ���ǧ���ѥǡ�����¤��ƹ��ۤ���. 
 * 
 * </JA>
 * 
 * @param r [in] recognition process instance
 * 
 * @return TRUE on success, FALSE on error.
 *
 * @callgraph
 * @callergraph
 * @ingroup grammar
 * 
 */
boolean
multigram_build(RecogProcess *r)
{
  if (r->lm->winfo != NULL) {
    /* re-build tree lexicon for recognition process */
    if (multigram_rebuild_wchmm(r) == FALSE) {
      jlog("ERROR: multi-gram: failed to re-build tree lexicon\n");
      return FALSE;
    }
#ifdef MDEBUG
    jlog("STAT: wchmm (re)build completed\n");
#endif
  }
  return(TRUE);
}

/** 
 * <JA>
 * @brief  �����Х�ʸˡ��������ʸˡ���ɲä���. 
 *
 * ��Ȥ�ʸˡ��¤�Τˤϡ������Х�ʸˡ�Τɤΰ��֤ˤ���ʸˡ���ɲ�
 * ���줿�������Υ��ƥ����ֹ�ȼ����ֹ���ϰϤ���Ͽ�����. 
 * 
 * @param gdfa [i/o] ������ʸˡ��DFA����
 * @param gwinfo [i/o] ������ʸˡ�μ������
 * @param m [i/o] ��礹��ʸˡ����. 
 * </JA>
 * <EN>
 * @brief  Append a grammar to the tail of global grammar.
 *
 * The location of the grammar in the global grammar (categories and words)
 * will be stored to the grammar structure for later access.
 * 
 * @param gdfa [i/o] DFA information of the global grammar
 * @param gwinfo [i/o] Dictionary information of the global grammar
 * @param m [i/o] New grammar information to be installed.
 * </EN>
 */
static boolean
multigram_append_to_global(DFA_INFO *gdfa, WORD_INFO *gwinfo, MULTIGRAM *m)
{
  /* the new grammar 'm' will be appended to the last of gdfa and gwinfo */
  m->state_begin = gdfa->state_num;	/* initial state ID */
  m->cate_begin = gdfa->term_num;	/* initial terminal ID */
  m->word_begin = gwinfo->num;	/* initial word ID */
  
  /* append category ID and node number of src DFA */
  /* Julius allow multiple initial states: connect each initial node
     is not necesarry. */
  dfa_append(gdfa, m->dfa, m->state_begin, m->cate_begin);
  /* append words of src vocabulary to global winfo */
  if (voca_append(gwinfo, m->winfo, m->cate_begin, m->word_begin) == FALSE) {
    return FALSE;
  }
  /* append category->word mapping table */
  terminfo_append(&(gdfa->term), &(m->dfa->term), m->cate_begin, m->word_begin);
  /* append catergory-pair information */
  /* pause has already been considered on m->dfa, so just append here */
  if (cpair_append(gdfa, m->dfa, m->cate_begin) == FALSE) {
    return FALSE;
  }
  /* re-set noise entry by merging */
  if (dfa_pause_word_append(gdfa, m->dfa, m->cate_begin) == FALSE) {
    return FALSE;
  }

  jlog("STAT: Gram #%d %s: installed\n", m->id, m->name);

  return TRUE;
}

/** 
 * <JA>
 * ������ʸˡ��ʸˡ�ꥹ�Ȥ��ɲä���.
 * ���ߥ��󥹥��󥹤��ݻ����Ƥ���ʸˡ�Υꥹ�Ȥ� lm->grammars ����¸�����. 
 * �ɲä���ʸˡ�ˤϡ�newbie �� inactive �Υե饰�����åȤ��졤�����
 * ʸˡ���������å����˹����оݤȤʤ�. 
 * 
 * @param dfa [in] �ɲ���Ͽ����ʸˡ��DFA����
 * @param winfo [in] �ɲ���Ͽ����ʸˡ�μ������
 * @param name [in] �ɲ���Ͽ����ʸˡ��̾��
 * @param lm [i/o] ����������󥹥���
 *
 * @return ʸˡID���֤�. 
 * </JA>
 * <EN>
 * Add a new grammar to the current list of grammars.
 * The list of grammars which the LM instance keeps currently is
 * at lm->grammars.
 * The new grammar is flagged at "newbie" and "inactive", to be treated
 * properly at the next grammar update check.
 * 
 * @param dfa [in] DFA information of the new grammar.
 * @param winfo [in] dictionary information of the new grammar.
 * @param name [in] name string of the new grammar.
 * @param lm [i/o] LM processing instance
 *
 * @return the new grammar ID for the given grammar.
 * </EN>
 *
 * @callgraph
 * @callergraph
 * @ingroup grammar
 */
int
multigram_add(DFA_INFO *dfa, WORD_INFO *winfo, char *name, PROCESS_LM *lm)
{
  MULTIGRAM *new;

  /* allocate new gram */
  new = (MULTIGRAM *)mymalloc(sizeof(MULTIGRAM));
  if (name != NULL) {
    strncpy(new->name, name, MAXGRAMNAMELEN);
  } else {
    strncpy(new->name, "(no name)", MAXGRAMNAMELEN);
  }

  new->id = lm->gram_maxid;
  new->dfa = dfa;
  new->winfo = winfo;
  /* will be setup and activated after multigram_update() is called once */
  new->hook = MULTIGRAM_DEFAULT | MULTIGRAM_ACTIVATE;
  new->newbie = TRUE;		/* need to setup */
  new->active = FALSE;		/* default: inactive */

  /* the new grammar is now added to gramlist */
  new->next = lm->grammars;
  lm->grammars = new;

  jlog("STAT: Gram #%d %s registered\n", new->id, new->name);
  lm->gram_maxid++;

  return new->id;
}

/** 
 * <JA>
 * ʸˡ��������. 
 *
 * ʸˡ�ꥹ����Τ���ʸˡ�ˤĤ��ơ�����ޡ������դ���. 
 * �ºݤκ���� multigram_exec_delete() �ǹԤ���. 
 * 
 * @param delid [in] �������ʸˡ��ʸˡID
 * @param lm [i/o] ����������󥹥���
 * 
 * @return �̾�� TRUE ���֤�. ���ꤵ�줿ID��ʸˡ��̵������ FALSE ���֤�. 
 * </JA>
 * <EN>
 * Mark a grammar in the grammar list to be deleted at the next grammar update.
 * 
 * @param delid [in] grammar id to be deleted
 * @param lm [i/o] LM processing instance
 * 
 * @return TRUE on normal exit, or FALSE if the specified grammar is not found
 * in the grammar list.
 * </EN>
 * @callgraph
 * @callergraph
 * @ingroup grammar
 */
boolean
multigram_delete(int delid, PROCESS_LM *lm)
{
  MULTIGRAM *m;
  for(m=lm->grammars;m;m=m->next) {
    if (m->id == delid) {
      m->hook |= MULTIGRAM_DELETE;
      jlog("STAT: Gram #%d %s: marked delete\n", m->id, m->name);
      break;
    }
  }
  if (! m) {
    jlog("STAT: Gram #%d: not found\n", delid);
    return FALSE;
  }
  return TRUE;
}

/** 
 * <JA>
 * ���٤Ƥ�ʸˡ�򼡲󹹿����˺������褦�ޡ�������.
 * 
 * @param lm [i/o] ����������󥹥���
 * </JA>
 * <EN>
 * Mark all grammars to be deleted at next grammar update.
 * 
 * @param lm [i/o] LM processing instance
 * </EN>
 * @callgraph
 * @callergraph
 * @ingroup grammar
 */
void
multigram_delete_all(PROCESS_LM *lm)
{
  MULTIGRAM *m;
  for(m=lm->grammars;m;m=m->next) {
    m->hook |= MULTIGRAM_DELETE;
  }
}

/** 
 * <JA>
 * ����ޡ����ΤĤ���ʸˡ��ꥹ�Ȥ���������. 
 * 
 * @param lm [i/o] ����������󥹥���
 * 
 * @return �����Х�ʸˡ�κƹ��ۤ�ɬ�פʤȤ��� TRUE ����ɬ�פʤȤ��� FALSE ���֤�. 
 * </JA>
 * <EN>
 * Purge grammars marked as delete.
 * 
 * @param lm [i/o] LM processing instance
 * 
 * @return TRUE if the global grammar must be re-constructed, or FALSE if not needed.
 * </EN>
 */
static boolean
multigram_exec_delete(PROCESS_LM *lm)
{
  MULTIGRAM *m, *mtmp, *mprev;
  boolean ret_flag = FALSE;

  /* exec delete */
  mprev = NULL;
  m = lm->grammars;
  while(m) {
    mtmp = m->next;
    if (m->hook & MULTIGRAM_DELETE) {
      /* if any grammar is deleted, we need to rebuild lexicons etc. */
      /* so tell it to the caller */
      if (! m->newbie) ret_flag = TRUE;
      if (m->dfa) dfa_info_free(m->dfa);
      word_info_free(m->winfo);
      jlog("STAT: Gram #%d %s: purged\n", m->id, m->name);
      free(m);
      if (mprev != NULL) {
	mprev->next = mtmp;
      } else {
	lm->grammars = mtmp;
      }
    } else {
      mprev = m;
    }
    m = mtmp;
  }

  return(ret_flag);
}

/** 
 * <JA>
 * ʸˡ��ͭ��������. �����Ǥϼ��󹹿�����
 * ȿ�Ǥ����褦�˥ޡ�����Ĥ���ΤߤǤ���. 
 * 
 * @param gid [in] ͭ����������ʸˡ�� ID
 * @param lm [i/o] ����������󥹥���
 * </JA>
 * <EN>
 * Activate a grammar in the grammar list.  The specified grammar
 * will only be marked as to be activated in the next grammar update timing.
 * 
 * @param gid [in] grammar ID to be activated
 * @param lm [i/o] LM processing instance
 *
 * @return 0 on success, -1 on error (when specified grammar not found),
 * of 1 if already active
 * </EN>
 * @callgraph
 * @callergraph
 * @ingroup grammar
 */
int
multigram_activate(int gid, PROCESS_LM *lm)	/* only mark */
{
  MULTIGRAM *m;
  int ret;

  for(m=lm->grammars;m;m=m->next) {
    if (m->id == gid) {
      if (m->hook & MULTIGRAM_DEACTIVATE) {
	ret = 0;
	m->hook &= ~(MULTIGRAM_DEACTIVATE);
	m->hook |= MULTIGRAM_ACTIVATE;
	jlog("STAT: Gram #%d %s: marked active, superceding deactivate\n", m->id, m->name);
      } else {
	if (m->hook & MULTIGRAM_ACTIVATE) {
	  jlog("STAT: Gram #%d %s: already marked active\n", m->id, m->name);
	  ret = 1;
	} else {
	  ret = 0;
	  m->hook |= MULTIGRAM_ACTIVATE;
	  jlog("STAT: Gram #%d %s: marked activate\n", m->id, m->name);
	}
      }
      break;
    }
  }
  if (! m) {
    jlog("WARNING: Gram #%d: not found, activation ignored\n", gid);
    ret = -1;
  }

  return(ret);
}

/** 
 * <JA>
 * ʸˡ��̵��������. ̵�������줿ʸˡ��
 * ǧ���ˤ����Ʋ���Ÿ������ʤ�. ����ˤ�äơ������Х뼭���
 * �ƹ��ۤ��뤳�Ȥʤ������Ū�˸ġ���ʸˡ��ON/OFF�Ǥ���. ̵��������
 * ʸˡ�� multigram_activate() �ǺƤ�ͭ�����Ǥ���. �ʤ������Ǥ�
 * �����ʸˡ���������ߥ󥰤�ȿ�Ǥ����褦�˥ޡ�����Ĥ���ΤߤǤ���. 
 * 
 * @param gid [in] ̵����������ʸˡ��ID
 * @param lm [i/o] ����������󥹥���
 * </JA>
 * <EN>
 * Deactivate a grammar in the grammar list.  The words of the de-activated
 * grammar will not be expanded in the recognition process.  This feature
 * enables rapid switching of grammars without re-building tree lexicon.
 * The de-activated grammar will again be activated by calling
 * multigram_activate().
 * 
 * @param gid [in] grammar ID to be de-activated
 * @param lm [i/o] LM processing instance
 * 
 * @return 0 on success, -1 on error (when specified grammar not found),
 * of 1 if already inactive
 * </EN>
 * @callgraph
 * @callergraph
 * @ingroup grammar
 */
int
multigram_deactivate(int gid, PROCESS_LM *lm)	/* only mark */
{
  MULTIGRAM *m;
  int ret;

  for(m=lm->grammars;m;m=m->next) {
    if (m->id == gid) {
      if (m->hook & MULTIGRAM_ACTIVATE) {
	ret = 0;
	m->hook &= ~(MULTIGRAM_ACTIVATE);
	m->hook |= MULTIGRAM_DEACTIVATE;
	jlog("STAT: Gram #%d %s: marked deactivate, superceding activate\n", m->id, m->name);
      } else {
	if (m->hook & MULTIGRAM_DEACTIVATE) {
	  jlog("STAT: Gram #%d %s: already marked deactivate\n", m->id, m->name);
	  ret = 1;
	} else {
	  ret = 0;
	  m->hook |= MULTIGRAM_DEACTIVATE;
	  jlog("STAT: Gram #%d %s: marked deactivate\n", m->id, m->name);
	}
      }
      break;
    }
  }
  if (! m) {
    jlog("WARNING: - Gram #%d: not found, deactivation ignored\n", gid);
    ret = -1;
  }

  return(ret);
}

/** 
 * <JA>
 * ʸˡ��ͭ������̵������¹Ԥ���. 
 * 
 * @param lm [i/o] ����������󥹥���
 * 
 * @return ̵������ͭ���ء����뤤��ͭ������̵���ؾ��֤��Ѳ�����ʸˡ����ĤǤ�
 * �����TRUE, ���֤������Ѳ����ʤ��ä����� FALSE ���֤�. 
 * </JA>
 * <EN>
 * Execute (de)activation of grammars.
 * 
 * @param lm [i/o] LM processing instance
 * 
 * @return TRUE if at least one grammar has been changed, or FALSE if no
 * grammar has changed its status.
 * </EN>
 */
static boolean
multigram_exec_activate(PROCESS_LM *lm)
{
  MULTIGRAM *m;
  boolean modified;
  
  modified = FALSE;
  for(m=lm->grammars;m;m=m->next) {
    if (m->hook & MULTIGRAM_ACTIVATE) {
      m->hook &= ~(MULTIGRAM_ACTIVATE);
      if (!m->active) {
	jlog("STAT: Gram #%d %s: turn on active\n", m->id, m->name);
      }
      m->active = TRUE;
      modified = TRUE;
    } else if (m->hook & MULTIGRAM_DEACTIVATE) {
      m->hook &= ~(MULTIGRAM_DEACTIVATE);
      if (m->active) {
	jlog("STAT: Gram #%d %s: turn off inactive\n", m->id, m->name);
      }
      m->active = FALSE;
      modified = TRUE;
    }
  }
  return(modified);
}
 
/** 
 * <JA>
 * @brief  �����Х�ʸˡ�ι���
 * 
 * ����ƽФ������ʸˡ�ꥹ�Ȥ��ѹ�������å�����. 
 * �ꥹ����˺���ޡ������Ĥ���줿ʸˡ��������ϡ�����ʸˡ��������
 * �����Х뼭���ƹ��ۤ���. �������ɲä��줿ʸˡ��������ϡ�
 * ����ʸˡ�򸽺ߤΥ����Х뼭����������ɲä���. 
 *
 * @param lm [i/o] ����������󥹥���
 * 
 * @return ��� TRUE ���֤�. 
 * </JA>
 * <EN>
 * @brief  Update  global grammar if needed.
 *
 * This function checks for any modification in the grammar list from
 * previous call, and update the global grammar if needed.
 *
 * If there are grammars marked to be deleted in the grammar list,
 * they will be actually deleted from memory.  Then the global grammar is
 * built from scratch using the rest grammars.
 * If there are new grammars, they are appended to the current global grammar.
 * 
 * @param lm [i/o] LM processing instance
 * 
 * @return TRUE when any of add/delete/active/inactive occurs, or FALSE if
 * nothing modified.
 * </EN>
 * @callgraph
 * @callergraph
 * @ingroup grammar
 */
boolean				/* return FALSE if no gram */
multigram_update(PROCESS_LM *lm)
{
  MULTIGRAM *m;
  boolean active_changed = FALSE;
  boolean rebuild_flag;

  if (lm->lmvar == LM_DFA_GRAMMAR) {
    /* setup additional grammar info of new ones */
    for(m=lm->grammars;m;m=m->next) {
      if (m->newbie) {
	jlog("STAT: Gram #%d %s: new grammar loaded, now mash it up for recognition\n", m->id, m->name);
	/* map dict item to dfa terminal symbols */
	if (make_dfa_voca_ref(m->dfa, m->winfo) == FALSE) {
	  jlog("ERROR: failed to map dict <-> DFA. This grammar will be deleted\n");
	  /* mark as to be deleted */
	  m->hook |= MULTIGRAM_DELETE;
	  continue;
	} 
	/* set dfa->sp_id and dfa->is_sp */
	dfa_find_pause_word(m->dfa, m->winfo, lm->am->hmminfo);
	/* build catergory-pair information */
	jlog("STAT: Gram #%d %s: extracting category-pair constraint for the 1st pass\n", m->id, m->name);
	if (extract_cpair(m->dfa) == FALSE) {
	  jlog("ERROR: failed to extract category pair. This grammar will be deleted\n");
	  /* mark as to be deleted */
	  m->hook |= MULTIGRAM_DELETE;
	}
      }
    }
  }

  rebuild_flag = FALSE;
  /* delete grammars marked as "delete" */
  if (multigram_exec_delete(lm)) { /* some built grammars deleted */
    rebuild_flag = TRUE;	/* needs rebuilding global grammar */
  }
  /* find modified grammar */
  for(m=lm->grammars;m;m=m->next) {
    if (m->hook & MULTIGRAM_MODIFIED) {
      rebuild_flag = TRUE;	/* needs rebuilding global grammar */
      m->hook &= ~(MULTIGRAM_MODIFIED);
    }
  }

  if (rebuild_flag) {
    /* rebuild global grammar from scratch (including new) */
    /* active status not changed here (inactive grammar will also included) */
    /* activate/deactivate hook will be handled later, so just keep it here */
#ifdef MDEBUG
    jlog("STAT: re-build whole global grammar...\n");
#endif
    /* free old if not yet */
    if (lm->dfa != NULL) {
      dfa_info_free(lm->dfa);
      lm->dfa = NULL;
    }
    if (lm->winfo != NULL) {
      word_info_free(lm->winfo);
      lm->winfo = NULL;
    }
    /* concatinate all existing grammars to global */
    for(m=lm->grammars;m;m=m->next) {
      if (lm->lmvar == LM_DFA_GRAMMAR && lm->dfa == NULL) {
	lm->dfa = dfa_info_new();
	dfa_state_init(lm->dfa);
      }
      if (lm->winfo == NULL) {
	lm->winfo = word_info_new();
	winfo_init(lm->winfo);
      }
      if (m->newbie) m->newbie = FALSE;
      if (lm->lmvar == LM_DFA_WORD) {
	/* just append dictionaty */
	m->word_begin = lm->winfo->num;
	if (voca_append(lm->winfo, m->winfo, m->id, m->word_begin) == FALSE) {
	  jlog("ERROR: multi-gram: failed to add dictionary #%d to recognition network\n", m->id);
	  /* mark as delete */
	  m->hook |= MULTIGRAM_DELETE;
	}
      } else {
	if (multigram_append_to_global(lm->dfa, lm->winfo, m) == FALSE) {
	  jlog("ERROR: multi-gram: failed to add grammar #%d to recognition network\n", m->id);
	  /* mark as delete */
	  m->hook |= MULTIGRAM_DELETE;
	}
      }
    }
    /* delete the error grammars if exist */
    if (multigram_exec_delete(lm)) {
      jlog("ERROR: errorous grammar deleted\n");
    }
    lm->global_modified = TRUE;
  } else {			/* global not need changed by the deletion */
    /* append only new grammars */
    for(m=lm->grammars;m;m=m->next) {
      if (m->newbie) {
	if (lm->lmvar == LM_DFA_GRAMMAR && lm->dfa == NULL) {
	  lm->dfa = dfa_info_new();
	  dfa_state_init(lm->dfa);
	}
	if (lm->winfo == NULL) {
	  lm->winfo = word_info_new();
	  winfo_init(lm->winfo);
	}
	if (m->newbie) m->newbie = FALSE;
	if (lm->lmvar == LM_DFA_WORD) {
	  /* just append dictionaty */
	  m->word_begin = lm->winfo->num;
	  if (voca_append(lm->winfo, m->winfo, m->id, m->word_begin) == FALSE) {
	    jlog("ERROR: multi-gram: failed to add dictionary #%d to recognition network\n", m->id);
	    /* mark as delete */
	    m->hook |= MULTIGRAM_DELETE;
	  }
	} else {
	  if (multigram_append_to_global(lm->dfa, lm->winfo, m) == FALSE) {
	    jlog("ERROR: multi-gram: failed to add grammar #%d to recognition network\n", m->id);
	    /* mark as delete */
	    m->hook |= MULTIGRAM_DELETE;
	  }
	}
	lm->global_modified = TRUE;
      }
    }
  }

  /* process activate/deactivate hook */
  active_changed = multigram_exec_activate(lm);

  if (lm->global_modified) {		/* if global lexicon has changed */
    /* now global grammar info has been updated */
    /* check if no grammar */
    if (lm->lmvar == LM_DFA_GRAMMAR) {
      if (lm->dfa == NULL || lm->winfo == NULL) {
	if (lm->dfa != NULL) {
	  dfa_info_free(lm->dfa);
	  lm->dfa = NULL;
	}
	if (lm->winfo != NULL) {
	  word_info_free(lm->winfo);
	  lm->winfo = NULL;
	}
      }
    }
#ifdef MDEBUG
    jlog("STAT: grammar update completed\n");
#endif
  }

  if (lm->global_modified || active_changed) {
    return (TRUE);
  }

  return FALSE;
}

/** 
 * <JA>
 * dfa�ե������dict�ե�������ɤ߹����ʸˡ�ꥹ�Ȥ��ɲä���. 
 * 
 * @param dfa_file [in] dfa �ե�����̾
 * @param dict_file [in] dict �ե�����̾
 * @param lm [i/o] ����������󥹥���
 * </JA>
 * <EN>
 * Add grammar to the grammar list specified by dfa file and dict file.
 * 
 * @param dfa_file [in] dfa file name
 * @param dict_file [in] dict file name
 * @param lm [i/o] LM processing instance
 * </EN>
 */
static boolean
multigram_read_file_and_add(char *dfa_file, char *dict_file, PROCESS_LM *lm)
{
  WORD_INFO *new_winfo;
  DFA_INFO *new_dfa;
  char buf[MAXGRAMNAMELEN], *p, *q;
  boolean ret;

  if (dfa_file != NULL) {
    jlog("STAT: reading [%s] and [%s]...\n", dfa_file, dict_file);
  } else {
    jlog("STAT: reading [%s]...\n", dict_file);
  }
  
  /* read dict*/
  new_winfo = word_info_new();

  if (lm->lmvar == LM_DFA_GRAMMAR) {
    ret = init_voca(new_winfo, dict_file, lm->am->hmminfo, 
#ifdef MONOTREE
		    TRUE,
#else 
		    FALSE,
#endif
		    lm->config->forcedict_flag);
    if ( ! ret ) {
      jlog("ERROR: failed to read dictionary \"%s\"\n", dict_file);
      word_info_free(new_winfo);
      return FALSE;
    }
  } else if (lm->lmvar == LM_DFA_WORD) {
    ret = init_wordlist(new_winfo, dict_file, lm->am->hmminfo, 
			lm->config->wordrecog_head_silence_model_name,
			lm->config->wordrecog_tail_silence_model_name,
			(lm->config->wordrecog_silence_context_name[0] == '\0') ? NULL : lm->config->wordrecog_silence_context_name,
			lm->config->forcedict_flag);
    if ( ! ret ) {
      jlog("ERROR: failed to read word list \"%s\"\n", dict_file);
      word_info_free(new_winfo);
      return FALSE;
    }
  }

  new_dfa = NULL;
  if (lm->lmvar == LM_DFA_GRAMMAR) {
    /* read dfa */
    new_dfa = dfa_info_new();
    if (init_dfa(new_dfa, dfa_file) == FALSE) {
      jlog("ERROR: multi-gram: error in reading DFA\n");
      word_info_free(new_winfo);
      dfa_info_free(new_dfa);
      return FALSE;
    }
  }

  jlog("STAT: done\n");

  /* extract name */
  p = &(dict_file[0]);
  q = p;
  while(*p != '\0') {
    if (*p == '/') q = p + 1;
    p++;
  }
  p = q;
  while(*p != '\0' && *p != '.') {
    buf[p-q] = *p;
    p++;
  }
  buf[p-q] = '\0';
  
  /* register the new grammar to multi-gram tree */
  multigram_add(new_dfa, new_winfo, buf, lm);

  return TRUE;

}


/** 
 * <JA>
 * ��ư���˻��ꤵ�줿���٤Ƥ�ʸˡ����ɤ���. 
 * 
 * @param lm [i/o] ����������󥹥���
 * 
 * </JA>
 * <EN>
 * Load all the grammars specified at startup.
 * 
 * @param lm [i/o] LM processing instance
 * 
 * </EN>
 * @callgraph
 * @callergraph
 */
boolean
multigram_load_all_gramlist(PROCESS_LM *lm)
{
  GRAMLIST *g;
  GRAMLIST *groot;
  boolean ok_p;

  switch(lm->config->lmvar) {
  case LM_DFA_GRAMMAR: groot = lm->config->gramlist_root; break;
  case LM_DFA_WORD:    groot = lm->config->wordlist_root; break;
  }

  ok_p = TRUE;
  for(g = groot; g; g = g->next) {
    if (multigram_read_file_and_add(g->dfafile, g->dictfile, lm) == FALSE) {
      ok_p = FALSE;
    }
  }
  return(ok_p);
}

/** 
 * <JA>
 * ���ߤ���ʸˡ�ο�������(active/inactive�Ȥ�). 
 * 
 * @param lm [i/o] ����������󥹥���
 * 
 * @return ʸˡ�ο����֤�. 
 * </JA>
 * <EN>
 * Get the number of current grammars (both active and inactive).
 * 
 * @param lm [i/o] LM processing instance
 * 
 * @return the number of grammars.
 * </EN>
 * @callgraph
 * @callergraph
 * @ingroup grammar
 */
int
multigram_get_all_num(PROCESS_LM *lm)
{
  MULTIGRAM *m;
  int cnt;
  
  cnt = 0;
  for(m=lm->grammars;m;m=m->next) cnt++;
  return(cnt);
}

/** 
 * <JA>
 * ñ�쥫�ƥ����°����ʸˡ������. 
 * 
 * @param category ñ�쥫�ƥ���ID
 * @param lm [i/o] ����������󥹥���
 * 
 * @return ñ�쥫�ƥ����°����ʸˡ��ID���֤�. 
 * </JA>
 * <EN>
 * Get which grammar the given category belongs to.
 * 
 * @param category word category ID
 * @param lm [i/o] LM processing instance
 * 
 * @return the id of the belonging grammar.
 * </EN>
 * @callgraph
 * @callergraph
 * @ingroup grammar
 */
int
multigram_get_gram_from_category(int category, PROCESS_LM *lm)
{
  MULTIGRAM *m;
  int tb, te;
  for(m = lm->grammars; m; m = m->next) {
    if (m->newbie) continue;
    tb = m->cate_begin;
    te = tb + m->dfa->term_num;
    if (tb <= category && category < te) { /* found */
      return(m->id);
    }
  }
  return(-1);
}

/** 
 * <JA>
 * ñ��ID����°����ʸˡ������. 
 * 
 * @param wid ñ��ID
 * @param lm [i/o] ����������󥹥���
 * 
 * @return ñ���°����ʸˡ��ID���֤�. 
 * </JA>
 * <EN>
 * Get which grammar the given word belongs to.
 * 
 * @param wid word ID
 * @param lm [i/o] LM processing instance
 * 
 * @return the id of the belonging grammar.
 * </EN>
 * @callgraph
 * @callergraph
 * @ingroup grammar
 */
int
multigram_get_gram_from_wid(WORD_ID wid, PROCESS_LM *lm)
{
  MULTIGRAM *m;
  int wb, we;

  for(m = lm->grammars; m; m = m->next) {
    if (m->newbie) continue;
    wb = m->word_begin;
    we = wb + m->winfo->num;
    if (wb <= wid && wid < we) { /* found */
      return(m->id);
    }
  }
  return(-1);
}


/** 
 * <JA>
 * �ݻ����Ƥ���ʸˡ�򤹤٤Ʋ������롣
 * 
 * @param root [in] root pointer of grammar list
 * </JA>
 * <EN>
 * Free all grammars.
 * 
 * @param root [in] root pointer of grammar list
 * </EN>
 * @callgraph
 * @callergraph
 */
void
multigram_free_all(MULTIGRAM *root)
{
  MULTIGRAM *m, *mtmp;

  m = root;
  while(m) {
    mtmp = m->next;
    if (m->dfa) dfa_info_free(m->dfa);
    word_info_free(m->winfo);
    free(m);
    m = mtmp;
  }
}

/** 
 * <EN>
 * Return a grammar ID of the given grammar name.
 * </EN>
 * <JA>
 * LM���ʸˡ��̾���Ǹ�����������ʸˡID���֤���
 * </JA>
 * 
 * @param lm [in] LM process instance
 * @param gramname [in] grammar name
 * 
 * @return grammar ID, or -1 if not found.
 *
 * @callgraph
 * @callergraph
 * @ingroup grammar
 * 
 */
int
multigram_get_id_by_name(PROCESS_LM *lm, char *gramname)
{
  MULTIGRAM *m;

  for(m=lm->grammars;m;m=m->next) {
    if (strmatch(m->name, gramname)) break;
  }
  if (!m) {
    jlog("ERROR: multigram: cannot find grammar \"%s\"\n", gramname);
    return -1;
  }
  return m->id;
}

/** 
 * <EN>
 * Find a grammar in LM by its name.
 * </EN>
 * <JA>
 * LM���ʸˡ��̾���Ǹ�������. 
 * </JA>
 * 
 * @param lm [in] LM process instance
 * @param gramname [in] grammar name
 * 
 * @return poitner to the grammar, or NULL if not found.
 *
 * @callgraph
 * @callergraph
 * @ingroup grammar
 * 
 */
MULTIGRAM *
multigram_get_grammar_by_name(PROCESS_LM *lm, char *gramname)
{
  MULTIGRAM *m;

  for(m=lm->grammars;m;m=m->next) {
    if (strmatch(m->name, gramname)) break;
  }
  if (!m) {
    jlog("ERROR: multigram: cannot find grammar \"%s\"\n", gramname);
    return NULL;
  }
  return m;
}

/** 
 * <EN>
 * Find a grammar in LM by its ID number.
 * </EN>
 * <JA>
 * LM���ʸˡ�� ID �ֹ�Ǹ�������. 
 * </JA>
 * 
 * @param lm [in] LM process instance
 * @param id [in] ID number
 * 
 * @return poitner to the grammar, or NULL if not found.
 *
 * @callgraph
 * @callergraph
 * @ingroup grammar
 * 
 */
MULTIGRAM *
multigram_get_grammar_by_id(PROCESS_LM *lm, unsigned short id)
{
  MULTIGRAM *m;

  for(m=lm->grammars;m;m=m->next) {
    if (m->id == id) break;
  }
  if (!m) {
    jlog("ERROR: multi-gram: cannot find grammar id \"%d\"\n", id);
    return NULL;
  }
  return m;
}

/** 
 * <EN>
 * @brief  Append words to a grammar.
 *
 * Category IDs of grammar in the adding words will be copied as is to
 * the target grammar, so they should be set beforehand correctly.
 * The whole tree lexicon will be rebuilt later.
 *
 * Currently adding words to N-gram LM is not supported yet.
 * 
 * </EN>
 * <JA>
 * @brief  ñ�콸���ʸˡ���ɲä��롥
 *
 * �ɲä���ñ���ʸˡ���ƥ���ID�ˤĤ��Ƥϡ����Ǥ˥������󤵤�Ƥ����Τ�
 * ���Τޤޥ��ԡ�����롥��äơ������Ϥ��δؿ���ƤӽФ����ˡ�
 * �ɲ��оݤ�ʸˡ��������������褦���������ꤵ��Ƥ���ɬ�פ����롥
 * �ڹ�¤���������Τ�����˺ƹ��ۤ���롥
 *
 * ñ��N-gram�����ǥ�ؤμ����ɲäϸ��ߥ��ݡ��Ȥ���Ƥ��ʤ���
 * 
 * </JA>
 * 
 * @param lm [i/o] LM process instance
 * @param m [i/o] grammar to which the winfo will be appended
 * @param winfo [in] words to be added to the grammar
 * 
 * @return TRUE on success, or FALSE on failure.
 *
 * @callgraph
 * @callergraph
 * @ingroup grammar
 * 
 */
boolean
multigram_add_words_to_grammar(PROCESS_LM *lm, MULTIGRAM *m, WORD_INFO *winfo)
{
  int offset;

  if (lm == NULL || m == NULL || winfo == NULL) return FALSE;

  offset = m->winfo->num;
  printf("adding %d words to grammar #%d (%d words)\n", winfo->num, m->id, m->winfo->num);
  /* append to the grammar */
  if (voca_append(m->winfo, winfo, m->id, offset) == FALSE) {
    jlog("ERROR: multi-gram: failed to add words to dict in grammar #%d \"%s\"\n", m->id, m->name);
    return FALSE;
  }
  /* update dictianary info */
  if (lm->lmvar == LM_DFA_GRAMMAR) {
    if (m->dfa->term_num != 0) free_terminfo(&(m->dfa->term));
    if (make_dfa_voca_ref(m->dfa, m->winfo) == FALSE) {
      jlog("ERROR: failed to map dict <-> DFA. This grammar will be deleted\n");
      return FALSE;
    } 
  }
  /* prepare for update */
  m->hook |= MULTIGRAM_MODIFIED;

  return TRUE;
}

/** 
 * <EN>
 * @brief  Append words to a grammar, given by its name.
 *
 * Call multigram_add_words_to_grammar() with target grammar
 * specified by its name.
 * </EN>
 * <JA>
 * @brief  ̾���ǻ��ꤵ�줿ʸˡ��ñ�콸����ɲä��롥
 *
 * multigram_add_words_to_grammar() ��ʸˡ̾�ǻ��ꤷ�Ƽ¹Ԥ��롥
 * 
 * </JA>
 * 
 * @param lm [i/o] LM process instance
 * @param gramname [in] name of the grammar to which the winfo will be appended
 * @param winfo [in] words to be added to the grammar
 * 
 * @return TRUE on success, or FALSE on failure.
 *
 * @callgraph
 * @callergraph
 * @ingroup grammar
 * 
 */
boolean
multigram_add_words_to_grammar_by_name(PROCESS_LM *lm, char *gramname, WORD_INFO *winfo)
{
  return(multigram_add_words_to_grammar(lm, multigram_get_grammar_by_name(lm, gramname), winfo));
}

/** 
 * <EN>
 * @brief  Append words to a grammar, given by its ID number.
 *
 * Call multigram_add_words_to_grammar() with target grammar
 * specified by its number.
 * </EN>
 * <JA>
 * @brief  �ֹ�ǻ��ꤵ�줿ʸˡ��ñ�콸����ɲä��롥
 *
 * multigram_add_words_to_grammar() ���ֹ�ǻ��ꤷ�Ƽ¹Ԥ��롥
 * 
 * </JA>
 * 
 * @param lm [i/o] LM process instance
 * @param id [in] ID number of the grammar to which the winfo will be appended
 * @param winfo [in] words to be added to the grammar
 * 
 * @return TRUE on success, or FALSE on failure.
 *
 * @callgraph
 * @callergraph
 * @ingroup grammar
 * 
 */
boolean
multigram_add_words_to_grammar_by_id(PROCESS_LM *lm, unsigned short id, WORD_INFO *winfo)
{
  return(multigram_add_words_to_grammar(lm, multigram_get_grammar_by_id(lm, id), winfo));
}

/* end of file */
