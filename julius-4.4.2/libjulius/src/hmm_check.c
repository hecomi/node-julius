/**
 * @file   hmm_check.c
 * 
 * <JA>
 * @brief  �ȥ饤�ե���μ����Ǥ������������å�
 * </JA>
 * 
 * <EN>
 * @brief  Triphone checker on word dictionary
 * </EN>
 * 
 * @author Akinobu LEE
 * @date   Thu Mar 17 20:50:07 2005
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

#include <julius/julius.h>

#define PHSTEP 10		///< Malloc step for phoneme conversion

/** 
 * <JA>
 * @brief  �����󤫤�HMM��ؤ��Ѵ���Ԥʤ�����̤�ɽ������. 
 *
 * ���Υ롼����ϡ�Julius/Julian ��Ϳ����줿������ǥ��
 * HMMList �ե�����ˤ����ơ������󤫤�HMM��ؤ��Ѵ���ƥ��Ȥ���
 * ����δؿ��Ǥ���. 
 * 
 * ����Ƕ��ڤ�줿�������ʸ������Ф��ơ��ȥ饤�ե����ǥ���ѻ��ˤ�
 * ����ƥ����Ȥ���θ���졤�ǽ�Ū���б����� HMM ����Ѵ������. 
 * ���θ塤�Ѵ�������̤�
 *   - �����󤫤�Ƴ����������Ŭ�Ѥ��٤���ǥ�̾
 *   - �嵭�� HMMList �ˤ������ä��Ѵ��������� HMM ̾
 *   - �ºݤ˷׻����Ѥ�����ʪ��HMM̾�ޤ��� pseudo HMM ̾
 * �ν�˽��Ϥ���. 
 * 
 * �ʤ���ʸ������� "|" ��ޤ�뤳�Ȥǡ�������ñ����ڤ�Ȥ��ư�����
 * �ȥ饤�ե���ˤ�����ñ��֤�Ÿ�����θ���뤳�Ȥ��Ǥ���. 
 * 
 * @param str [i/o] ����Ƕ��ڤ�줿�������ʸ����
 * @param hmminfo [in] HMM�����¤��
 * @param len_ret [out] �֤��ͤ����� HMM �����ǿ�
 * 
 * @return �����˥������դ���줿�Ѵ��������HMM�Υݥ�����
 * </JA>
 * <EN>
 * @brief  Convert phoneme sequences to logical HMM sequences, and output the
 * result.
 *
 * This function is for testing procedure to convert words in dictionary
 * to corresponding HMM sequences in Julius/Julian, given an HMMList and
 * HTK HMM definition.
 *
 * Given a space-separated list of phoneme names in a string, each phonemes
 * will be converted to context-dependent style (if using triphone model),
 * and then converted to HMM sequence that will finally be used for
 * recognition.  Then, the following data will be output for all HMM:
 *   - Original phone HMM name,
 *   - Logical HMM name that is converted from above,
 *   - Physical or pseudo HMM name that will actually be used.
 *
 * Additionally, specifying '|' in the string gives a word boundary between
 * phonemes, and triphone conversion will consider the cross-word expansion.
 * 
 * @param str [i/o] string that contains space-saparated phoneme sequence.
 * @param hmminfo [in] HMM definition structure
 * @param len_ret [out] num of elements in the return value
 * 
 * @return the newly allocated pointer array to the converted logical HMMs.
 * </EN>
 */
static HMM_Logical **
new_str2phseq(char *str, HTK_HMM_INFO *hmminfo, int *len_ret)
{
  char **tokens;
  boolean *word_end;
  int phnum;
  boolean word_mode = FALSE;
  HMM_Logical **new;
  static char buf[MAX_HMMNAME_LEN];
  
  /* read in string and divide into token unit */
  {
    char *p;
    int tokenmax;
    tokenmax = PHSTEP;
    tokens = (char **)mymalloc(sizeof(char *) * tokenmax);
    word_end = (boolean *)mymalloc(sizeof(boolean) * tokenmax);
    phnum = 0;
    for(p = strtok(str, DELM); p; p = strtok(NULL, DELM)) {
      if (strmatch(p, "|")) {
	word_mode = TRUE;
	if (phnum > 0) word_end[phnum-1] = TRUE;
	continue;
      }
      if (phnum >= tokenmax) {
	tokenmax += PHSTEP;
	tokens = (char **)myrealloc(tokens, sizeof(char *) * tokenmax);
	word_end = (boolean *)myrealloc(word_end, sizeof(boolean) * tokenmax);
      }
      tokens[phnum] = strcpy((char *)mymalloc(strlen(p)+1), p);
      word_end[phnum] = FALSE;
      phnum++;
    }
    if (phnum == 0) {
      jlog("ERROR: hmm_check: no phone specified\n");
      printf("ERROR: hmm_check: no phone specified\n");
      new = NULL;
      goto spend;
    }
    word_end[phnum-1] = TRUE;
  }
  /* check if the phonemes exist in basephone list */
  {
    BASEPHONE *ph;
    int i;
    boolean ok_flag = TRUE;
    for (i=0;i<phnum;i++) {
      ph = aptree_search_data(tokens[i], hmminfo->basephone.root);
      if (ph == NULL || ! strmatch(ph->name, tokens[i])) {
	jlog("ERROR: hmm_check: %2d - unknown phone \"%s\"\n", i+1, tokens[i]);
	printf("ERROR: hmm_check: %2d - unknown phone \"%s\"\n", i+1, tokens[i]);
	ok_flag = FALSE;
	continue;
      }
    }
    if (! ok_flag) {
      jlog("ERROR: hmm_check: unknown phone(s)\n");
      printf("ERROR: hmm_check: unknown phone(s)\n");
      new = NULL;
      goto spend;
    }
  }
  /* token -> original logical name -> logical HMM -> physical/pseudo phone */
  /* cross-word conversion and fallback to bi/mono-phone is also considered */
  {
    int i;
    char *hmmstr;
    HMM_Logical *lg;
    boolean ok_flag = TRUE;

    new = (HMM_Logical **)mymalloc(sizeof(HMM_Logical *) * phnum);

    /* original logical name, applied logical HMM name (defined by HMMList),
       and the actual physical/pseudo HMM name (defined in hmmdefs) */
    printf("\n  id     original   logical    physical/pseudo\n");
    printf(" -------------------------------------------------\n");


    if (hmminfo->is_triphone) {
      cycle_triphone(NULL);
      cycle_triphone(tokens[0]);
      for (i = 0; i < phnum; i++) {
	if (i < phnum - 1) {
	  hmmstr = cycle_triphone(tokens[i+1]);
	} else {
	  hmmstr = cycle_triphone_flush();
	}
	lg = htk_hmmdata_lookup_logical(hmminfo, hmmstr);
	if (lg == NULL) {
	  if (word_mode) {
	    if (i > 0 && word_end[i-1]) {
	      if (word_end[i]) {
		center_name(hmmstr, buf);
	      } else {
		rightcenter_name(hmmstr, buf);
	      }
	    } else if (word_end[i]) {
	      leftcenter_name(hmmstr, buf);
	    }
	    lg = htk_hmmdata_lookup_logical(hmminfo, buf);
	    if (lg == NULL) {
	      jlog("ERROR: hmm_check: no defined/pseudo HMM for \"%s\"??\n", buf);
	      printf("ERROR: hmm_check: no defined/pseudo HMM for \"%s\"??\n", buf);
	      ok_flag = FALSE;
	      continue;
	    }
	    if (lg->is_pseudo) {
	      printf("  %2d: %11s -> (pseudo) -> {%s}\n", i+1, hmmstr, lg->body.pseudo->name);
	    } else {
	      printf("  %2d: %11s -> %8s -> [%s]\n", i+1, hmmstr, lg->name, lg->body.defined->name);
	    }
	  } else {
	    jlog("ERROR: hmm_check: UNKNOWN %2d: (%s)\n", i+1, hmmstr);
	    printf("ERROR: hmm_check: UNKNOWN %2d: (%s)\n", i+1, hmmstr);
	    ok_flag = FALSE;
	    continue;
	  }
	} else {
	  if (lg->is_pseudo) {
	    printf("  %2d: %11s -> (pseudo) -> {%s}\n", i+1, hmmstr, lg->body.pseudo->name);
	  } else {
	    printf("  %2d: %11s -> %8s -> [%s]\n", i+1, hmmstr, " ", lg->body.defined->name);
	  }
	}
	new[i] = lg;
      }
    } else {
      for (i = 0; i < phnum; i++) {
	lg = htk_hmmdata_lookup_logical(hmminfo, tokens[i]);
	if (lg == NULL) {
	  jlog("ERROR: hmm_check: %2d - unknown logical HMM \"%s\"\n", i+1, tokens[i]);
	  printf("ERROR: hmm_check: %2d - unknown logical HMM \"%s\"\n", i+1, tokens[i]);
	  ok_flag = FALSE;
	  continue;
	}
	new[i] = lg;
      }
    }
    if (ok_flag) {
      printf("succeeded\n");
    } else {
      jlog("ERROR: hmm_check: failed\n");
      printf("failed\n");
      free(new);
      new = NULL;
      goto spend;
    }
      
  }

 spend:
  {
    int i;
    for(i=0;i<phnum;i++) {
      free(tokens[i]);
    }
    free(tokens);
    free(word_end);
  }

  *len_ret = phnum;

  return new;
}

/** 
 * <JA>
 * ɸ�����Ϥ���1�Ԥ�����ɽ���Ȥ����ɤ߹��ߡ��ȥ饤�ե���ؤ��Ѵ������å���
 * �Ԥʤ�. 
 * 
 * @param hmminfo [in] HMM�����¤��
 * </JA>
 * <EN>
 * Read in line from stdin as phoneme sequence and try convertion to
 * triphone for checking.
 * 
 * @param hmminfo [in] HMM definition structure
 * </EN>
 */
static boolean
test_expand_triphone(HTK_HMM_INFO *hmminfo)
{
  char *buf;
  int newline;
  HMM_Logical **phseq;
  int phlen;
  boolean flag = FALSE;

  buf = (char *)mymalloc(4096);
  for(;;) {
    /* read in phoneme sequence from stdin */
    printf(">>> input phone sequence (word delimiter is `|', blank to return)\n");
    if (fgets(buf, 4096, stdin) == NULL) {
      flag = TRUE;
      break;
    }
    newline = strlen(buf)-1;    /* chop newline */
    if (buf[newline] == '\n') buf[newline] = '\0';
    if (buf[0] == '\0') break;
    /* convert string to phseq and output */
    phseq = new_str2phseq(buf, hmminfo, &phlen);
    free(phseq);
  }
  free(buf);
  return flag;
}

/** 
 * <JA>
 * ���ޥ�ɥ饤���ǥȥ饤�ե���Υ����å���Ԥʤ��⡼�� ("-check triphone"). 
 *
 * @param r [in] ǧ���������󥹥���
 * </JA>
 * <EN>
 * Mode to do interactive triphone conversion check ("-check triphone").
 * 
 * @param r [in] recognition process instance
 * </EN>
 *
 * @callgraph
 * @callergraph
 */
void
hmm_check(RecogProcess *r)
{
  boolean endflag;
  static char cmd[MAX_HMMNAME_LEN];
  int newline;

  printf("*************************************************\n");
  printf("********  TRIPHONE COHERENCE CHECK MODE  ********\n");
  printf("*************************************************\n");

  printf("hmmdefs=%s\n", r->am->config->hmmfilename);
  if (r->am->config->mapfilename != NULL) {
    printf("hmmlist=%s\n", r->am->config->mapfilename);
  }
  printf("dict=%s\n", r->lm->config->dictfilename);
  printf("headsil = "); put_voca(stdout, r->lm->winfo, r->lm->winfo->head_silwid);
  printf("tailsil = "); put_voca(stdout, r->lm->winfo, r->lm->winfo->tail_silwid);

  if (make_base_phone(r->am->hmminfo, r->lm->winfo) == FALSE) {
    jlog("ERROR: hmm_check: error in making base phone list\n");
    printf("ERROR: hmm_check: error in making base phone list\n");
    return;
  }

  print_phone_info(stdout, r->am->hmminfo);

  for(endflag = FALSE; endflag == FALSE;) {
    printf("===== command (\"H\" for help) > ");
    if (fgets(cmd, MAX_HMMNAME_LEN, stdin) == NULL) break;
    newline = strlen(cmd)-1;    /* chop newline */
    if (cmd[newline] == '\n') cmd[newline] = '\0';
    if (cmd[0] == '\0') continue; /* if blank line, read next */

    switch(cmd[0]) {
    case 'a':			/* all */
      /* check if logical HMMs cover all possible variants */
      test_interword_triphone(r->am->hmminfo, r->lm->winfo);
      break;
    case 'c':			/* conv */
      /* try to expand triphone for given phoneme sequence */
      endflag = test_expand_triphone(r->am->hmminfo);
      break;
    case 'i':			/* info */
      /* output data source */
      printf("hmmdefs=%s\n", r->am->config->hmmfilename);
      if (r->am->config->mapfilename != NULL) {
	printf("hmmlist=%s\n", r->am->config->mapfilename);
      }
      printf("dict=%s\n", r->lm->config->dictfilename);
      printf("headsil = "); put_voca(stdout, r->lm->winfo, r->lm->winfo->head_silwid);
      printf("tailsil = "); put_voca(stdout, r->lm->winfo, r->lm->winfo->tail_silwid);
      print_phone_info(stdout, r->am->hmminfo);
      break;
    case 'p':			/* phonelist */
      /* output basephone */
      print_all_basephone_name(&(r->am->hmminfo->basephone));
      break;
    case 'd':			/* phonelist in detail */
      /* output basephone */
      print_all_basephone_detail(&(r->am->hmminfo->basephone));
      break;
    case 'q':			/* quit */
      /* quit this check mode */
      endflag = TRUE;
      break;
    default:
      printf("COMMANDS:\n");
      printf(" info      --- output HMM information\n");
      printf(" conv      --- try HMM conversion for given phone sequence\n");
      printf(" phonelist --- print base phone list\n");
      printf(" all       --- check if all possible IW-triphone is covered\n");
      printf(" quit      --- quit\n");
      break;
    }
  }
  printf("*************************************************\n");
  printf("*****  END OF TRIPHONE COHERENCE CHECK MODE  ****\n");
  printf("*************************************************\n");
}
/* end of file */
