/**
 * @file   mkbingram.c
 * 
 * <JA>
 * @brief  ARPAɸ�������N-gram���� Julius �ѤΥХ��ʥ�N-gram���Ѵ����롥
 *
 * Julius �ǻȤ��� ARPA ɸ������� (������)2-gram �� ������
 * 3-gram ��ñ��ΥХ��ʥ�N-gram���Ѵ����롥
 *
 * �Х��ʥ�N-gram�η����Ϥޤ��Ť�����(3.4.2����)
 * �ΥХ��ʥ�N-gram�� 3.5 �ʹߤο������������Ѵ����뤳�Ȥ�Ǥ��롥
 * </JA>
 * 
 * <EN>
 * @brief  
 * </EN>
 * 
 * @author Akinobu LEE
 * @date   Thu Mar 24 12:22:27 2005
 *
 * $Revision: 1.7 $
 * 
 */
/*
 * Copyright (c) 1991-2016 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2016 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

/* mkbingram --- make binary n-gram for JULIUS from ARPA standard format */

#include <sent/stddefs.h>
#include <sent/ngram2.h>
#include <sys/stat.h>
#include <time.h>

#include "charconv.h"

static NGRAM_INFO *ngram;

void
usage(char *s)
{
  printf("mkbingram: convert ARPA format N-gram to binary format for Julius\n");
  printf("\nUsage: %s [options...] outfile\n", s);
  printf("\n    options:\n");
  printf("    -nlr file       forward  N-gram in ARPA format\n");
  printf("    -nrl file       backward N-gram in ARPA format\n");
  printf("    -d bingramfile  Julius binary N-gram file input\n");
#if defined(HAVE_WINNLS) || defined(HAVE_ICONV)
  printf("    -c from to      convert character code\n");
#endif
  printf("    -swap           swap \"%s\" and \"%s\"\n", BEGIN_WORD_DEFAULT, END_WORD_DEFAULT);
  printf("\n      When both \"-nlr\" and \"-nrl\" are specified, \n");
  printf("      Julius will use the BACKWARD N-gram as main LM\n");
  printf("      and use the forward 2-gram only at the 1st pass\n");
  printf("\nLibrary configuration: ");
  confout_version(stdout);
  confout_lm(stdout);
  printf("\n");
}

int
main(int argc, char *argv[])
{
  FILE *fp;
  char header[512];
  time_t now;
  char *binfile, *lrfile, *rlfile, *outfile;
  int i;
  char *from_code, *to_code, *buf;
  boolean charconv_enabled = FALSE;
  boolean force_swap = FALSE;
  WORD_ID w;

  binfile = lrfile = rlfile = outfile = NULL;
  from_code = to_code = NULL;
  if (argc <= 1) {
    usage(argv[0]);
    return -1;
  }

  for(i=1;i<argc;i++) {
    if (argv[i][0] == '-') {
      if (argv[i][1] == 'd') {
	if (++i >= argc) {
	  printf("Error: no argument for option \"%s\"\n", argv[i]);
	  usage(argv[0]);
	  return -1;
	}
	binfile = argv[i];
      } else if (argv[i][1] == 'n') {
	switch(argv[i][2]) {
	case 'l':
	  if (++i >= argc) {
	    printf("Error: no argument for option \"%s\"\n", argv[i]);
	    usage(argv[0]);
	    return -1;
	  }
	  lrfile = argv[i];
	  break;
	case 'r':
	  if (++i >= argc) {
	    printf("Error: no argument for option \"%s\"\n", argv[i]);
	    usage(argv[0]);
	    return -1;
	  }
	  rlfile = argv[i];
	  break;
	default:
	  printf("Error: no such option \"%s\"\n", argv[i]);
	  usage(argv[0]);
	  return -1;
	}
      } else if (argv[i][1] == 'c') {
#if defined(HAVE_WINNLS) || defined(HAVE_ICONV)
	if (++i >= argc) {
	  printf("Error: no argument for option \"%s\"\n", argv[i]);
	  usage(argv[0]);
	  return -1;
	}
	from_code = strcpy((char*)mymalloc(strlen(argv[i])+1), argv[i]);
	if (++i >= argc) {
	  printf("Error: no argument for option \"%s\"\n", argv[i]);
	  usage(argv[0]);
	  free(from_code);
	  return -1;
	}
	to_code = strcpy((char*)mymalloc(strlen(argv[i])+1),argv[i]);
	charconv_enabled = TRUE;
#else
	printf("Error: this binary was not built with character code conversion\n");
	usage(argv[0]);
	return -1;
#endif
      } else if (argv[i][1] == 's') {
	force_swap = TRUE;
      }
    } else {
      if (outfile == NULL) {
	outfile = argv[i];
      } else {
	printf("Error: more than one output file\n");
	usage(argv[0]);
	return -1;
      }
    }
  }

  if (!outfile) {
    printf("Error: no output file specified\n");
    usage(argv[0]);
    return -1;
  }

  if (binfile) {
    if (lrfile || rlfile) {
      printf("Error: both binary file and ARPA file are specified\n");
      usage(argv[0]);
      return -1;
    }
    printf("bingram: %s\n", binfile);
  } else {
    if (rlfile) {
      printf("backward n-gram: %s\n", rlfile);
      if (lrfile) {
	printf("additional forward 2-gram for 1st pass: %s\n", lrfile);
      }
    } else if (lrfile) {
      printf("forward n-gram: %s\n", lrfile);
    } else {
      printf("Error: no input N-gram file specified\n");
      usage(argv[0]);
      return -1;
    }
  }

  printf("\nSTART LOADING\n\n");

  /* make header string */
  now = time(NULL);
  if (binfile) {
    sprintf(header, "converted at %s\nfrom bingram = %s\n", ctime(&now), binfile);
  } else {
    if (rlfile && lrfile) {
      sprintf(header, "converted at %s\nfrom n-gram = %s, LR 2-gram = %s\n", ctime(&now),  rlfile, lrfile);
    } else if (rlfile) {
      sprintf(header, "converted at %s\nfrom n-gram = %s\n", ctime(&now),  rlfile);
    } else {
      sprintf(header, "converted at %s\nfrom n-gram = %s\n", ctime(&now),  lrfile);
    }
  }

  ngram = ngram_info_new();
  if (binfile) {
    /* read in bingram */
    if (init_ngram_bin(ngram, binfile) == FALSE) return -1;
  } else {
    /* read in ARPA n-gram */
    if (force_swap) {
      ngram->bos_eos_swap = TRUE;
    }
    if (rlfile) {
      if (init_ngram_arpa(ngram, rlfile, DIR_RL) == FALSE) return -1;
      if (lrfile) {
	if (init_ngram_arpa_additional(ngram, lrfile) == FALSE) return -1;
      }
    } else if (lrfile) {
      if (init_ngram_arpa(ngram, lrfile, DIR_LR) == FALSE) return -1;
    }
  }

  print_ngram_info(stdout, ngram);
  
#if defined(HAVE_WINNLS) || defined(HAVE_ICONV)
  if (charconv_enabled == TRUE) {
    int fail_count = 0;
    /* do character conversion */
    if (charconv_setup(from_code, to_code) == -1) {
      fprintf(stderr, "failed to setup character convertsion\n");
      return -1;
    }
    buf = (char *)mymalloc(4096);
    for (w = 0; w < ngram->max_word_num; w++) {
      if (charconv(ngram->wname[w], buf, 4096) == NULL) {
	fprintf(stderr, "failed to convert, leave original string #%d \"%s\"\n", w, ngram->wname[w]);
	fail_count++;
      } else {
	ngram->wname[w] = mybmalloc2(strlen(buf)+1, &(ngram->mroot));
	strcpy(ngram->wname[w], buf);
      }
    }
    free(buf);
    if (fail_count > 0) {
      fprintf(stderr, "total %d of %d words was not converted\n", fail_count, ngram->max_word_num);
    }
  }
#endif

  /* write in JULIUS binary format */
  if ((fp = fopen_writefile(outfile)) == NULL) {
    fprintf(stderr, "failed to open \"%s\"\n", outfile);
    return -1;
  }
  printf("\nWriting in v5 format to \"%s\"...\n", outfile);
  if (ngram_write_bin(fp, ngram, header) == FALSE){/* failed */
    fprintf(stderr, "failed to write \"%s\"\n",outfile);
    return -1;
  }
  fclose_writefile(fp);

  printf("completed\n");
  
  return 0;

}
