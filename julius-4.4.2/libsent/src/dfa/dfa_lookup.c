/**
 * @file   dfa_lookup.c
 * 
 * <JA>
 * @brief  ñ�켭���ñ����б����륫�ƥ����ֹ���֤�
 *
 * �ºݤˤ�ʸˡ����ѥ��� mkdfa.pl �ǳ�ñ��˥��ƥ����ֹ椬����
 * ������Ƥ��Ƥ���Τǡ������Ǥ��ͤΥ����å��Τ߹Ԥ��ޤ���
 * </JA>
 * 
 * <EN>
 * @brief  Return DFA terminal (category) id to a given word.
 *
 * Actually the category ids are assigned beforehand by the grammar
 * compiler mkdfa.pl.  This function only checks the value.
 * </EN>
 * 
 * @author Akinobu LEE
 * @date   Tue Feb 15 14:11:41 2005
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
#include <sent/dfa.h>
#include <ctype.h>

/** 
 * Return category id corresponding to the given terminal name.  Actually they
 * are mere strings of ID itself.
 * 
 * @param dinfo [in] DFA grammar information
 * @param terminalname [in] name string
 * 
 * @return the category id.
 */
WORD_ID
dfa_symbol_lookup(DFA_INFO *dinfo, char *terminalname)
{
  WORD_ID id;
  int c;
  char *p;

  /* check if terminal name is digit */
  for(p=terminalname;*p!='\0';p++) {
    c = *p;
    if (! isdigit(c)) {
      jlog("Error: dfa_lookup: terminal number is not digit in dict! [%s]\n", terminalname);
      return(WORD_INVALID);
    }
  }

  /* Currently, terminal ID is already assigned by mkdfa in wname,
     so this function only returns the ID */
  id = atoi(terminalname);
  if (id >= dinfo->term_num) return(WORD_INVALID); /* error */
  else return(id);
}
