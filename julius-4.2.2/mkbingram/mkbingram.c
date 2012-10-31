/**
 * @file   mkbingram.c
 * 
 * <JA>
 * @brief  ARPA標準形式のN-gramから Julius 用のバイナリN-gramに変換する．
 *
 * Julius で使える ARPA 標準形式の (前向き)2-gram と 後ろ向き
 * 3-gram を，単一のバイナリN-gramに変換する．
 *
 * バイナリN-gramの形式はまた古い形式(3.4.2以前)
 * のバイナリN-gramを 3.5 以降の新しい形式に変換することもできる．
 * </JA>
 * 
 * <EN>
 * @brief  
 * </EN>
 * 
 * @author Akinobu LEE
 * @date   Thu Mar 24 12:22:27 2005
 *
 * $Revision: 1.5 $
 * 
 */
/*
 * Copyright (c) 1991-2012 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2012 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

/* mkbingram --- make binary n-gram for JULIUS from ARPA standard format */

/* $Id: mkbingram.c,v 1.5 2012/07/27 08:44:57 sumomo Exp $ */

#include <sent/stddefs.h>
#include <sent/ngram2.h>
#include <sys/stat.h>
#include <time.h>

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
  boolean force_swap = FALSE;

  binfile = lrfile = rlfile = outfile = NULL;
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
