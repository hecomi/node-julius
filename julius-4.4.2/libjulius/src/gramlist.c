/**
 * @file   gramlist.c
 * 
 * <EN>
 * @brief  Grammar file list management on startup.
 *
 * These functions are for managing list of grammar files to be loaded
 * at startup.  You can also specify (list of) grammars to be included
 * for recognition at startup by calling these functions.  If you want to
 * add, modify or remove grammars while recognition, you should prepare
 * grammar data and call functions in multi-gram.c directly.
 * @sa julius/module.c for the implementation details.
 * 
 * </EN>
 * 
 * <JA>
 * @brief  ��ư�����ɤ߹���ʸˡ�ե�����Υꥹ�ȴ���. 
 *
 * �����δؿ��ϥ��󥸥�ư�����ɤ߹��ޤ��ʸˡ�ե�����Υꥹ�Ȥ��������
 * �ؿ��Ǥ�. �����δؿ���ư���˸Ƥ֤��Ȥǡ�ǧ���Ѥ�ʸˡ�򥢥ץꥱ�������
 * �������Ū���ɲä��뤳�Ȥ��Ǥ��ޤ�. ���󥸥�ư���ưŪ��ʸˡ��
 * �ɲä������ѹ���Ԥ��������ϡ�ʸˡ�ǡ����������Ѱդ��ơ�multi-gram.c
 * ��δؿ���ľ�ܸƤӽФ�ɬ�פ�����ޤ�. ���ξ��� julius/module.c ��
 * �����λ��ͤˤʤ�Ǥ��礦. (@sa julius/module.c)
 * 
 * </JA>
 * 
 * @author Akinobu Lee
 * @date   Tue Oct 30 12:27:53 2007
 *
 * $Revision: 1.4 $
 * 
 */
/*
 * Copyright (c) 1991-2013 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2013 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#include <julius/julius.h>

/** 
 * <JA>
 * ��ư���ɤ߹��ߥꥹ�Ȥ�ʸˡ���ɲä���. 
 * 
 * @param dfafile [in] DFA�ե�����
 * @param dictfile [in] ñ�켭��
 * @param j [in] LM ����ѥ�᡼��
 * @param lmvar [in] LM �ܺٷ� id
 * </JA>
 * <EN>
 * Add a grammar to the grammar list to be read at startup.
 * 
 * @param dfafile [in] DFA file
 * @param dictfile [in] dictionary file
 * @param j [in] LM configuration variables
 * @param lmvar [in] LM type variant id
 * </EN>
 *
 * @callgraph
 * @callergraph
 * @ingroup grammar
 */
void
multigram_add_gramlist(char *dfafile, char *dictfile, JCONF_LM *j, int lmvar)
{
  GRAMLIST *new;

  new = (GRAMLIST *)mymalloc(sizeof(GRAMLIST));
  new->dfafile = new->dictfile = NULL;
  if (dfafile) new->dfafile = strcpy((char *)mymalloc(strlen(dfafile)+1), dfafile);
  if (dictfile) new->dictfile = strcpy((char *)mymalloc(strlen(dictfile)+1), dictfile);
  switch(lmvar) {
  case LM_DFA_GRAMMAR:
    new->next = j->gramlist_root;
    j->gramlist_root = new;
    break;
  case LM_DFA_WORD:
    new->next = j->wordlist_root;
    j->wordlist_root = new;
    break;
  }
}

/** 
 * <JA>
 * ��ư���ɤ߹��ߥꥹ�Ȥ�ä�. 
 * 
 * @param j [in] LM ����ѥ�᡼��
 * </JA>
 * <EN>
 * Remove the grammar list to be read at startup.
 * 
 * @param j [in] LM configuration variables
 * </EN>
 *
 * @callgraph
 * @callergraph
 * @ingroup grammar
 */
void
multigram_remove_gramlist(JCONF_LM *j)
{
  GRAMLIST *g;
  GRAMLIST *tmp;

  g = j->gramlist_root;
  while (g) {
    tmp = g->next;
    if (g->dfafile) free(g->dfafile);
    if (g->dictfile) free(g->dictfile);
    free(g);
    g = tmp;
  }
  j->gramlist_root = NULL;

  g = j->wordlist_root;
  while (g) {
    tmp = g->next;
    if (g->dfafile) free(g->dfafile);
    if (g->dictfile) free(g->dictfile);
    free(g);
    g = tmp;
  }
  j->wordlist_root = NULL;
}

/** 
 * <JA>
 * @brief  �ץ�ե��å�������ʣ����ʸˡ��ư���ɤ߹��ߥꥹ�Ȥ��ɲä���. 
 *
 * �ץ�ե��å����� "foo", ���뤤�� "foo,bar" �Τ褦�˥���޶��ڤ��
 * ʣ��Ϳ���뤳�Ȥ��Ǥ��ޤ�. ��ʸ����θ��� ".dfa", ".dict" ��Ĥ���
 * �ե�����򡤤��줾��ʸˡ�ե����롦����ե�����Ȥ��ƽ缡�ɤ߹��ߤޤ�. 
 * �ɤ߹��ޤ줿ʸˡ�Ͻ缡��ʸˡ�ꥹ�Ȥ��ɲä���ޤ�. 
 * 
 * @param prefix_list [in]  �ץ�ե��å����Υꥹ��
 * @param cwd [in] �����ȥǥ��쥯�ȥ��ʸ����
 * @param j [in] LM ����ѥ�᡼��
 * @param lmvar [in] LM �ܺٷ� id
 * </JA>
 * <EN>
 * @brief  Add multiple grammars given by their prefixs to the grammar list.
 *
 * This function read in several grammars, given a prefix string that
 * contains a list of file prefixes separated by comma: "foo" or "foo,bar".
 * For each prefix, string ".dfa" and ".dict" will be appended to read
 * dfa file and dict file.  The read grammars will be added to the grammar
 * list.
 * 
 * @param prefix_list [in] string that contains comma-separated list of grammar path prefixes
 * @param cwd [in] string of current working directory
 * @param j [in] LM configuration variables
 * @param lmvar [in] LM type variant id
 * </EN>
 * 
 * @callgraph
 * @callergraph
 * @ingroup grammar
 */
boolean
multigram_add_prefix_list(char *prefix_list, char *cwd, JCONF_LM *j, int lmvar)
{
  char buf[MAXGRAMNAMELEN], *p, *q;
  char buf2_d[MAXGRAMNAMELEN], *buf_d;
  char buf2_v[MAXGRAMNAMELEN], *buf_v;
  boolean ok_p, ok_p_total;

  if (prefix_list == NULL) return TRUE;
  
  p = &(prefix_list[0]);

  ok_p_total = TRUE;
  
  while(*p != '\0') {
    /* extract one prefix to buf[] */
    q = p;
    while(*p != '\0' && *p != ',') {
      buf[p-q] = *p;
      p++;
    }
    buf[p-q] = '\0';

    switch(lmvar) {
    case LM_DFA_GRAMMAR:
      /* register the new grammar to the grammar list to be read later */
      /* making file names from the prefix */
      ok_p = TRUE;
      strcpy(buf2_d, buf);
      strcat(buf2_d, ".dfa");
      buf_d = filepath(buf2_d, cwd);
      if (!checkpath(buf_d)) {
	jlog("ERROR: gramlist: cannot read dfa file \"%s\"\n", buf_d);
	ok_p = FALSE;
      }
      strcpy(buf2_v, buf);
      strcat(buf2_v, ".dict");
      buf_v = filepath(buf2_v, cwd);
      if (!checkpath(buf_v)) {
	jlog("ERROR: gramlist: cannot read dict file \"%s\"\n", buf_v);
	ok_p = FALSE;
      }
      if (ok_p == TRUE) {
	multigram_add_gramlist(buf_d, buf_v, j, lmvar);
      } else {
	ok_p_total = FALSE;
      }
      break;
    case LM_DFA_WORD:
      /* register the new word list to the list */
      /* treat the file name as a full file path (not prefix) */
      buf_v = filepath(buf, cwd);
      if (!checkpath(buf_v)) {
	jlog("ERROR: gramlist: cannot read wordlist file \"%s\"\n", buf_v);
	ok_p_total = FALSE;
      } else {
	multigram_add_gramlist(NULL, buf_v, j, lmvar);
      }
      break;
    } 

    /* move to next */
    if (*p == ',') p++;
  }

  return ok_p_total;
}

/** 
 * <JA>
 * @brief �ꥹ�ȥե�������ɤ߹���ʣ��ʸˡ��ư���ɤ߹��ߥꥹ�Ȥ��ɲä���. 
 *
 * �ե��������1�Ԥˣ��Ĥ��ĵ��Ҥ��줿ʸˡ�Υץ�ե��å�������,
 * �б�����ʸˡ�ե������缡�ɤ߹��ߤޤ�. 
 * 
 * �ƹԤ�ʸ����θ��� ".dfa", ".dict" ��Ĥ����ե������
 * ���줾��ʸˡ�ե����롦����ե�����Ȥ��ƽ缡�ɤ߹��ߤޤ�. 
 * �ɤ߹��ޤ줿ʸˡ�Ͻ缡��ʸˡ�ꥹ�Ȥ��ɲä���ޤ�. 
 * 
 * @param listfile [in] �ץ�ե��å����ꥹ�ȤΥե�����̾
 * @param j [in] LM ����ѥ�᡼��
 * @param lmvar [in] LM �ܺٷ� id
 * </JA>
 * <EN>
 * @brief  Add multiple grammars from prefix list file to the grammar list.
 *
 * This function read in multiple grammars at once, given a file that
 * contains a list of grammar prefixes, each per line.
 *
 * For each prefix, string ".dfa" and ".dict" will be appended to read the
 * corresponding dfa and dict file.  The read grammars will be added to the
 * grammar list.
 * 
 * @param listfile [in] path of the prefix list file
 * @param j [in] LM configuration variables
 * @param lmvar [in] LM type variant id
 * </EN>
 * 
 * @callgraph
 * @callergraph
 * @ingroup grammar
 */
boolean
multigram_add_prefix_filelist(char *listfile, JCONF_LM *j, int lmvar)
{
  FILE *fp;
  char buf[MAXGRAMNAMELEN], *p, *src_bgn, *src_end, *dst;
  char *cdir;
  char buf2_d[MAXGRAMNAMELEN], *buf_d;
  char buf2_v[MAXGRAMNAMELEN], *buf_v;
  boolean ok_p, ok_p_total;

  if (listfile == NULL) return FALSE;
  if ((fp = fopen(listfile, "r")) == NULL) {
    jlog("ERROR: gramlist: failed to open grammar list file %s\n", listfile);
    return FALSE;
  }

  /* convert relative paths as relative to this list file */
  cdir = strcpy((char *)mymalloc(strlen(listfile)+1), listfile);
  get_dirname(cdir);

  ok_p_total = TRUE;

  while(getl_fp(buf, MAXGRAMNAMELEN, fp) != NULL) {
    /* remove comment */
    p = &(buf[0]);
    while(*p != '\0') {
      if (*p == '#') {
	*p = '\0';
	break;
      }
      p++;
    }
    if (buf[0] == '\0') continue;
    
    /* trim head/tail blanks */
    p = (&buf[0]);
    while(*p == ' ' || *p == '\t' || *p == '\r') p++;
    if (*p == '\0') continue;
    src_bgn = p;
    p = (&buf[strlen(buf) - 1]);
    while((*p == ' ' || *p == '\t' || *p == '\r') && p > src_bgn) p--;
    src_end = p;
    dst = (&buf[0]);
    p = src_bgn;
    while(p <= src_end) *dst++ = *p++;
    *dst = '\0';
    if (buf[0] == '\0') continue;


    switch(lmvar) {
    case LM_DFA_GRAMMAR:
      /* register the new grammar to the grammar list to be read later */
      ok_p = TRUE;
      strcpy(buf2_d, buf);
      strcat(buf2_d, ".dfa");
      buf_d = filepath(buf2_d, cdir);
      if (!checkpath(buf_d)) {
	jlog("ERROR: gramlist: cannot read dfa file \"%s\"\n", buf_d);
	ok_p = FALSE;
      }
      strcpy(buf2_v, buf);
      strcat(buf2_v, ".dict");
      buf_v = filepath(buf2_v, cdir);
      if (!checkpath(buf_v)) {
	jlog("ERROR: gramlist: cannot read dict file \"%s\"\n", buf_v);
	ok_p = FALSE;
      }
      if (ok_p == TRUE) {
	multigram_add_gramlist(buf_d, buf_v, j, lmvar);
      } else {
	ok_p_total = FALSE;
      }
      break;
    case LM_DFA_WORD:
      /* register the new word list to the list */
      /* treat the file name as a full file path (not prefix) */
      buf_v = filepath(buf, cdir);
      if (!checkpath(buf_v)) {
	jlog("ERROR: gramlist: cannot read wordlist file \"%s\"\n", buf_v);
	ok_p_total = FALSE;
      } else {
	multigram_add_gramlist(NULL, buf_v, j, lmvar);
      }
      break;
    }

  }

  free(cdir);
  
  fclose(fp);

  return ok_p_total;
}

/* end of file */
