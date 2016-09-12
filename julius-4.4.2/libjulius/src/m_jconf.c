/**
 * @file   m_jconf.c
 * 
 * <JA>
 * @brief  ����ե�������ɤ߹���. 
 *
 * ���ץ�������򵭽Ҥ��� jconf ����ե�������ɤ߹��ߤޤ�. 
 * jconf ����ե�������Ǥϡ����֥륯�����ơ������ˤ��ʸ�����
 * ���ꡤ�Хå�����å���ˤ��ʸ���Υ��������פ��Ǥ��ޤ�. 
 * �ޤ����ƹԤˤ����� '#' �ʹߤϥ����åפ���ޤ�. 
 *
 * jconf ����ե�������Ǥϡ����Ƥ����Хѥ��ϡ����ץꥱ��������
 * �����ȥǥ��쥯�ȥ�ǤϤʤ������� jconf ��¸�ߤ���ǥ��쥯�ȥ꤫���
 * ���Хѥ��Ȥ��Ʋ�ᤵ��ޤ�. 
 *
 * �ޤ���$HOME, ${HOME}, $(HOME), �η��ǻ��ꤵ�줿��ʬ�ˤĤ���
 * �Ķ��ѿ���Ÿ���Ǥ��ޤ�. 
 * 
 * </JA>
 * 
 * <EN>
 * @brief  Read a configuration file.
 *
 * These functions are for reading jconf configuration file and set the
 * parameters into jconf structure.  String bracing by double quotation,
 * and escaping character with backslash are supproted.
 * Characters after '#' at each line will be ignored.
 *
 * Note that all relative paths in jconf file are treated as relative
 * to the jconf file, not the run-time current directory.
 *
 * You can expand environment variables in a format of $HOME, ${HOME} or
 * $(HOME) in jconf file.
 *
 * </EN>
 * 
 * @author Akinobu Lee
 * @date   Thu May 12 14:16:18 2005
 *
 * $Revision: 1.11 $
 * 
 */
/*
 * Copyright (c) 1991-2013 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2013 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#include <julius/julius.h>

#if defined(_WIN32) && !defined(__CYGWIN32__)
#include <mbstring.h>
#endif

#define ISTOKEN(A) (A == ' ' || A == '\t' || A == '\n') ///< Determine token characters
#define BUFLEN 512

/** 
 * <JA>
 * @brief  jconf �Ѥι��ɤ߹��ߥ롼����
 *
 * �Хå�����å���ˤ�륨�������׽���������� Mac/Win �β��ԥ����ɤ�
 * �б�����. ���Ԥϥ����åפ��졤���ԥ����ɤϾä����. 
 * 
 * @param buf [out] �ɤ߹����1��ʬ�Υƥ����Ȥ��Ǽ����Хåե�
 * @param size [in] @a buf ���礭���ʥХ��ȿ���
 * @param fp [in] �ե�����ݥ���
 * 
 * @return @a buf ���֤�. EOF �Ǥ���ʾ����Ϥ��ʤ���� NULL ���֤�. 
 * </JA>
 * <EN>
 * @brief  line reading function for jconf file.
 *
 * This function has capability of character escaping and newline codes
 * on Win/Mac.  Blank line will be skipped and newline characters will be
 * stripped.
 * 
 * @param buf [out] buffer to store the read text per line
 * @param size [in] size of @a buf in bytes
 * @param fp [in] file pointer
 * 
 * @return @a buf on success, or NULL when encountered EOF and no further input.
 * </EN>
 */
/* added by H.Banno for Windows & Mac */
static char *
fgets_jconf(char *buf, int size, FILE *fp)
{
  int c, prev_c;
  int pos;

  if (fp == NULL) return NULL;
    
  pos = 0;
  c = '\0';
  prev_c = '\0';
  while (1) {
    if (pos >= size) {
      pos--;
      break;
    }

    c = fgetc(fp);
    if (c == EOF) {
      buf[pos] = '\0';
      if (pos <= 0) {
	return NULL;
      } else {
	return buf;
      }
    } else if (c == '\n' || c == '\r') {
      if (c == '\r' && (c = fgetc(fp)) != '\n') { /* for Mac */
	ungetc(c, fp);
      }
      if (prev_c == '\\') {
	pos--;
      } else {
	break;
      }
    } else {
      buf[pos] = c;
      pos++;

#if defined(_WIN32) && !defined(__CYGWIN32__)
      if (c == '\\' && (_ismbblead(prev_c) && _ismbbtrail(c))) {
      c = '\0';
      }
#endif
    }
    prev_c = c;
  }
  buf[pos] = '\0';

  return buf;
}

/** 
 * <JA>
 * @brief  �ե�����Υѥ�̾����ǥ��쥯�ȥ�̾��ȴ���Ф�. 
 *
 * �Ǹ�� '/' �ϻĤ����. 
 * 
 * @param path [i/o] �ե�����Υѥ�̾�ʴؿ�����ѹ�������
 * </JA>
 * <EN>
 * @brief  Get directory name from a path name of a file.
 *
 * The trailing slash will be left, and the given buffer will be modified.
 * 
 * @param path [i/o] file path name, will be modified to directory name
 * </EN>
 */
void
get_dirname(char *path)
{
  char *p;
  /* /path/file -> /path/ */
  /* path/file  -> path/  */
  /* /file      -> / */
  /* file       ->  */
  /* ../file    -> ../ */
  p = path + strlen(path) - 1;
  while (*p != '/'
#if defined(_WIN32) && !defined(__CYGWIN32__)
	 && *p != '\\'
#endif
	 && p != path) p--;
  if (p == path && *p != '/') *p = '\0';
  else *(p+1) = '\0';
}

/** 
 * <JA>
 * @brief  �Ķ��ѿ���Ÿ��
 * 
 * �Ķ��ѿ���Ÿ������. $HOME �η���ʸ�����Ķ��ѿ��Ȥߤʤ��������ͤ�
 * �ִ�����. �ִ��������ä��ݤˤϡ�Ϳ����줿ʸ����Хåե���������
 * �����������餿�˳���դ���줿�Хåե����֤�. 
 *
 * �ѿ��λ���� $HOME, ${HOME}, $(HOME), �η��ǻ���Ǥ���. 
 * $ ��Ÿ���������ʤ����ϥХå�����å��� "\" �ǥ��������פǤ���. 
 * �ޤ����󥰥륯������ "'" �ǳ��줿�ϰϤ�Ÿ����Ԥ�ʤ�. 
 * 
 * @param str [in] �о�ʸ�����Ÿ��ȯ������������ free �����Τ���ա�
 * 
 * @return Ÿ�����٤��оݤ��ʤ��ä���硤str �����Τޤ��֤����. Ÿ�����Ԥ�줿��硤���餿�˳���դ���줿Ÿ�����ʸ�����ޤ�Хåե����֤����. 
 * </JA>
 * <EN>
 * @brief  Envronment valuable expansion for a string
 *
 * This function expands environment valuable in a string.  When an
 * expantion occurs, the given buffer will be released inside this
 * function and newly allocated buffer that holds the resulting string
 * will be returned.
 *
 * Environment valuables should be in a form of $HOME, ${HOME} or $(HOME).
 * '$' can be escaped by back slash, and strings enbraced by single quote
 * will be treated as is (no expansion).
 * 
 * @param str [in] target string
 * 
 * @return the str itself when no expansion performed, or newly
 * allocated buffer if expansion occurs.
 * </EN>
 */
static char *
expand_env(char *str)
{
  char *p, *q;
  char *bgn;
  char eb;
  char *target;
  char *envval;
  int target_malloclen;
  int len, n;
  boolean inbrace;
  char env[256];

  /* check if string contains '$' and return immediately if not */
  /* '$' = 36, '\'' = 39 */
  p = str;
  inbrace = FALSE;
  while (*p != '\0') {
    if (*p == 39) {
      if (inbrace == FALSE) {
	inbrace = TRUE;
      } else {
	inbrace = FALSE;
      }
      p++;
      continue;
    }
    if (! inbrace) {
      if (*p == '\\') {
	p++;
	if (*p == '\0') break;
      } else {
	if (*p == 36) break;
      }
    }
    p++;
  }
  if (*p == '\0') return str;

  /* prepare result buffer */
  target_malloclen = strlen(str) * 2;
  target = (char *)mymalloc(target_malloclen);

  p = str;
  q = target;

  /* parsing */
  inbrace = FALSE;
  while (*p != '\0') {

    /* look for next '$' */
    while (*p != '\0') {
      if (*p == 39) {
	if (inbrace == FALSE) {
	  inbrace = TRUE;
	} else {
	  inbrace = FALSE;
	}
	p++;
	continue;
      }
      if (! inbrace) {
	if (*p == '\\') {
	  p++;
	  if (*p == '\0') break;
	} else {
	  if (*p == 36) break;
	}
      }
      *q = *p;
      p++;
      q++;
      n = q - target;
      if (n >= target_malloclen) {
	target_malloclen *= 2;
	target = myrealloc(target, target_malloclen);
	q = target + n;
      }
    }
    if (*p == '\0') {		/* reached end of string */
      *q = '\0';
      break;
    }

    /* move to next */
    p++;

    /* check for brace */
    eb = 0;
    if (*p == '(') {
      eb = ')';
    } else if (*p == '{') {
      eb = '}';
    }

    /* proceed to find env end point and set the env string to env[] */
    if (eb != 0) {
      p++;
      bgn = p;
      while (*p != '\0' && *p != eb) p++;
      if (*p == '\0') {
	jlog("ERROR: failed to expand variable: no end brace: \"%s\"\n", str);
	free(target);
	return str;
      }
    } else {
      bgn = p;
      while (*p == '_'
	     || (*p >= '0' && *p <= '9')
	     || (*p >= 'a' && *p <= 'z')
	     || (*p >= 'A' && *p <= 'Z')) p++;
    }
    len = p - bgn;
    if (len >= 256 - 1) {
      jlog("ERROR: failed to expand variable: too long env name: \"%s\"\n", str);
      free(target);
      return str;
    }
    strncpy(env, bgn, len);
    env[len] = '\0';

    /* get value */
    if ((envval = getenv(env)) == NULL) {
      jlog("ERROR: failed to expand variable: no such variable \"%s\"\n", env);
      free(target);
      return str;
    }

    if (debug2_flag) {		/* for debug */
      jlog("DEBUG: expand $%s to %s\n", env, envval);
    }

    /* paste value to target */
    while(*envval != '\0') {
      *q = *envval;
      q++;
      envval++;
      n = q - target;
      if (n >= target_malloclen) {
	target_malloclen *= 2;
	target = myrealloc(target, target_malloclen);
	q = target + n;
      }
    }

    /* go on to next */
    if (eb != 0) p++;
  }

  free(str);
  return target;
}

/* read-in and parse jconf file and process those using m_options */
/** 
 * <JA>
 * @brief  ���ץ����ʸ�����ʬ�򤷤��ɲó�Ǽ����.
 *
 * @param buf [in] ʸ����
 * @param argv [i/o] ���ץ������ؤΥݥ���
 * @param argc [i/o] ���ץ������ο��ؤΥݥ���
 * @param maxnum [i/o] ���ץ������γ��պ����
 * </JA>
 * <EN>
 * @brief  Divide option string into option arguments and append to array.
 *
 * @param buf [in] option string
 * @param argv [i/o] pointer to option array
 * @param argc [i/o] pointer to the length of option array
 * @param maxnum [i/o] pointer to the allocated length of option array
 * </EN>
 */
static void
add_to_arglist(char *buf, char ***argv_ret, int *argc_ret, int *maxnum_ret)
{
  char *p = buf;
  char cpy[BUFLEN];
  char *dst, *dst_from;
  char **argv = *argv_ret;
  int argc = *argc_ret;
  int maxnum = *maxnum_ret;

  dst = cpy;
  while (1) {
    while (*p != '\0' && ISTOKEN(*p)) p++;
    if (*p == '\0') break;
      
    dst_from = dst;
      
    while (*p != '\0' && (!ISTOKEN(*p))) {
#if !defined(_WIN32)
      if (*p == '\\') {     /* escape by '\' */
	if (*(++p) == '\0') break;
	*(dst++) = *(p++);
      } else {
#endif
	if (*p == '"') { /* quote by "" */
	  p++;
	  while (*p != '\0' && *p != '"') *(dst++) = *(p++);
	  if (*p == '\0') break;
	  p++;
	} else if (*p == '\'') { /* quote by '' */
	  p++;
	  while (*p != '\0' && *p != '\'') *(dst++) = *(p++);
	  if (*p == '\0') break;
	  p++;
	} else if (*p == '#') { /* comment out by '#' */
	  *p = '\0';
	  break;
	} else {		/* other */
	  *(dst++) = *(p++);
	}
#if !defined(_WIN32)
      }
#endif
    }
    if (dst != dst_from) {
      *dst = '\0'; dst++;
      if ( argc >= maxnum) {
	maxnum += 20;
	argv = (char **)myrealloc(argv, sizeof(char *) * maxnum);
      }
      argv[argc++] = strcpy((char*)mymalloc(strlen(dst_from)+1), dst_from);
    }
  }
  *argv_ret = argv;
  *argc_ret = argc;
  *maxnum_ret = maxnum;
}

/** 
 * <JA>
 * ���ץ��������ޤ�ʸ�������Ϥ����ͤ򥻥åȤ���.
 * ���Хѥ�̾�ϥ����Ȥ�������ФȤ��ư�����.
 * 
 * @param str [in] ���ץ��������ޤ�ʸ����
 * @param jconf [out] �ͤ򥻥åȤ��� jconf ����ǡ���
 * </JA>
 * <EN>
 * Parse a string and set the specified option values.
 * Relative paths will be treated as relative to current directory.
 * 
 * @param str [in] string which contains options
 * @param jconf [out] global configuration data to be written.
 * </EN>
 *
 * @callgraph
 * @callergraph
 */
boolean
config_string_parse(char *str, Jconf *jconf) {
  /* relative paths in string are relative to current */
  return(config_string_parse_basedir(str, jconf, NULL));
}

/* config_string_parse with base dir */
boolean
config_string_parse_basedir(char *str, Jconf *jconf, char *dir)
{
  int c_argc;
  char **c_argv;
  int maxnum;
  int i;
  boolean ret;

  jlog("STAT: parsing option string: \"%s\"\n", str);
  
  /* set the content of jconf file into argument list c_argv[1..c_argc-1] */
  maxnum = 20;
  c_argv = (char **)mymalloc(sizeof(char *) * maxnum);
  c_argv[0] = strcpy((char *)mymalloc(7), "string");
  c_argc = 1;
  add_to_arglist(str, &c_argv, &c_argc, &maxnum);
  /* env expansion */
  for (i=1;i<c_argc;i++) {
    c_argv[i] = expand_env(c_argv[i]);
  }
  /* now that options are in c_argv[][], call opt_parse() to process them */
  ret = opt_parse(c_argc, c_argv, dir, jconf);

  /* free arguments */
  while (c_argc-- > 0) {
    free(c_argv[c_argc]);
  }
  free(c_argv);

  return(ret);
}

/** 
 * <JA>
 * jconf ����ե�������ɤ߹���ǲ��Ϥ����б����륪�ץ��������ꤹ��.
 * ���ץ����������Хѥ��ϡ����� jconf ����ե����뤫������ФȤʤ�.
 * 
 * @param conffile [in] jconf �ե�����Υѥ�̾
 * @param jconf [out] �ͤ򥻥åȤ��� jconf ����ǡ���
 * </JA>
 * <EN>
 * Read and parse a jconf file, and set the specified option values.
 * Relative paths in the file will be treated as relative to the file,
 * not the application current.
 * 
 * @param conffile [in] jconf file path name
 * @param jconf [out] global configuration data to be written.
 * </EN>
 *
 * @callgraph
 * @callergraph
 */
boolean
config_file_parse(char *conffile, Jconf *jconf)
{
  int c_argc;
  char **c_argv;
  FILE *fp;
  int maxnum;
  char buf[BUFLEN];
  char *cdir;
  int i;
  boolean ret;

  jlog("STAT: include config: %s\n", conffile);
  
  /* set the content of jconf file into argument list c_argv[1..c_argc-1] */
  /* c_argv[0] will be the original conffile name */
  /* inside jconf file, quoting by ", ' and escape by '\' is supported */
  if ((fp = fopen(conffile, "r")) == NULL) {
    jlog("ERROR: m_jconf: failed to open jconf file: %s\n", conffile);
    return FALSE;
  }
  maxnum = 20;
  c_argv = (char **)mymalloc(sizeof(char *) * maxnum);
  c_argv[0] = strcpy((char *)mymalloc(strlen(conffile)+1), conffile);
  c_argc = 1;
  while (fgets_jconf(buf, BUFLEN, fp) != NULL) {
    if (buf[0] == '\0') continue;
    add_to_arglist(buf, &c_argv, &c_argc, &maxnum);
  }
  if (fclose(fp) == -1) {
    jlog("ERROR: m_jconf: cannot close jconf file\n");
    return FALSE;
  }

  /* env expansion */
  for (i=1;i<c_argc;i++) {
    c_argv[i] = expand_env(c_argv[i]);
  }

  if (debug2_flag) {		/* for debug */
    jlog("DEBUG: args:");
    for (i=1;i<c_argc;i++) jlog(" %s",c_argv[i]);
    jlog("\n");
  }

  /* now that options are in c_argv[][], call opt_parse() to process them */
  /* relative paths in jconf file are relative to the jconf file (not current) */
  cdir = strcpy((char *)mymalloc(strlen(conffile)+1), conffile);
  get_dirname(cdir);
  ret = opt_parse(c_argc, c_argv, (cdir[0] == '\0') ? NULL : cdir, jconf);
  free(cdir);

  /* free arguments */
  while (c_argc-- > 0) {
    free(c_argv[c_argc]);
  }
  free(c_argv);

  return(ret);
}

/* parse DNN config file */
boolean
dnn_config_file_parse(char *filename, JCONF_AM *am, Jconf *jconf)
{
  FILE *fp;
  char buf[BUFLEN];
  char *p, *pp;
  char *v;
  int i, n;
  boolean error_flag;
  char *cdir;

  if (am->dnn.wfile != NULL) {
    jlog("ERROR: dnn_config_file_parse: duplicated loading: %s\n", filename);
    return FALSE;
  }

  if ((fp = fopen(filename, "r")) == NULL) {
    jlog("ERROR: dnn_config_file_parse: failed to open %s\n", filename);
    return FALSE;
  }

  cdir = strcpy((char *)mymalloc(strlen(filename)+1), filename);
  get_dirname(cdir);
  if (cdir[0] == '\0') {
    free(cdir);
    cdir = NULL;
  }

  while (fgets_jconf(buf, BUFLEN, fp) != NULL) {
    if ((p = strchr(buf, '#')) != NULL) {
      *p = '\0';
    }
    pp = &(buf[0]);
    while (*pp == ' ' || *pp == '\t') pp++;
    if (*pp == '\0') continue;
    p = strchr(pp, ' ');
    if (p == NULL) {
      jlog("ERROR: dnn_config_file_parse: wrong file format: %s\n", buf);
      if (cdir) free(cdir);
      fclose(fp);
      return FALSE;
    }
    v = p;
    while (*v == ' ') v++;
    *p = '\0';
    if (strmatch(pp, "feature_type")) {
      am->dnn.paramtype = param_str2code(v);
    } else if (strmatch(pp, "feature_options")) {
      am->dnn.optionstring = strdup(v);
    } else if (strmatch(pp, "feature_len")) am->dnn.veclen = atoi(v);
    else if (strmatch(pp, "context_len")) am->dnn.contextlen = atoi(v);
    else if (strmatch(pp, "input_nodes")) am->dnn.inputnodes = atoi(v);
    else if (strmatch(pp, "output_nodes")) am->dnn.outputnodes = atoi(v);
    else if (strmatch(pp, "hidden_nodes")) am->dnn.hiddennodes = atoi(v);
    else if (strmatch(pp, "hidden_layers")) {
      am->dnn.hiddenlayernum = atoi(v);
      am->dnn.wfile = (char **)mymalloc(sizeof(char *) * am->dnn.hiddenlayernum);
      am->dnn.bfile = (char **)mymalloc(sizeof(char *) * am->dnn.hiddenlayernum);
      for (i = 0; i < am->dnn.hiddenlayernum; i++) {
	am->dnn.wfile[i] = NULL;
	am->dnn.bfile[i] = NULL;
      }
    } else if (pp[0] == 'W') {
      n = atoi(&(pp[1]));
      if (n > am->dnn.hiddenlayernum) {
	jlog("ERROR: dnn_config_file_parse: W%d > # of hidden_layers (%d)\n", n, am->dnn.hiddenlayernum);
	if (cdir) free(cdir);
	fclose(fp);
	return FALSE;
      } else if (n <= 0) {
	jlog("ERROR: dnn_config_file_parse: layer id should begin with 1\n");
	if (cdir) free(cdir);
	fclose(fp);
	return FALSE;
      }
      am->dnn.wfile[n-1] = filepath(v, cdir);
    } else if (pp[0] == 'B') {
      n = atoi(&(pp[1]));
      if (n > am->dnn.hiddenlayernum) {
	jlog("ERROR: dnn_config_file_parse: B%d > # of hidden_layers (%d)\n", n, am->dnn.hiddenlayernum);
	if (cdir) free(cdir);
	fclose(fp);
	return FALSE;
      } else if (n <= 0) {
	jlog("ERROR: dnn_config_file_parse: layer id should begin with 1\n");
	if (cdir) free(cdir);
	fclose(fp);
	return FALSE;
      }
      am->dnn.bfile[n-1] = filepath(v, cdir);
    } else if (strmatch(pp, "output_W")) am->dnn.output_wfile = filepath(v, cdir);
    else if (strmatch(pp, "output_B")) am->dnn.output_bfile = filepath(v, cdir);
    else if (strmatch(pp, "state_prior")) am->dnn.priorfile = filepath(v, cdir);
    else if (strmatch(pp, "state_prior_factor")) am->dnn.prior_factor = atof(v);
    else if (strmatch(pp, "batch_size")) am->dnn.batchsize = atoi(v);
    else {
      jlog("ERROR: dnn_config_file_parse: unknown spec: %s %s\n", pp, v);
      if (cdir) free(cdir);
      fclose(fp);
      return FALSE;
    }
  }
  if (fclose(fp) == -1) {
    jlog("ERROR: dnn_config_file_parse: failed to close file\n");
    if (cdir) free(cdir);
    return FALSE;
  }

  /* check validity */
  error_flag = FALSE;
  for (i = 0; i < am->dnn.hiddenlayernum; i++) {
    if (am->dnn.wfile[i] == NULL) {
      jlog("ERROR: dnn_config_file_parse: no W file specified for hidden layer #%d\n", i + 1);
      error_flag = TRUE;
    }
    if (am->dnn.bfile[i] == NULL) {
      jlog("ERROR: dnn_config_file_parse: no B file specified for hidden layer #%d\n", i + 1);
      error_flag = TRUE;
    }
  }
  if (error_flag == TRUE) {
    if (cdir) free(cdir);
    return FALSE;
  }

  am->dnn.enabled = TRUE;

  /* load options */
  if (am->dnn.optionstring) {
    if (config_string_parse_basedir(jconf->amnow->dnn.optionstring, jconf, cdir) == FALSE) {
      if (cdir) free(cdir);
      return FALSE;
    }
  }

  if (cdir) free(cdir);

  return TRUE;
}



/* end of file */
