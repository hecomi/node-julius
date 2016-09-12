/*
 * Copyright (c) 1991-2016 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2016 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#ifndef __SENT_DNN_H__
#define __SENT_DNN_H__

#include <sent/stddefs.h>
#include <sent/htk_hmm.h>
#include <sent/htk_param.h>
#include <sent/hmm_calc.h>

#define USE_SIMD_NONE   0
#define USE_SIMD_SSE    1
#define USE_SIMD_AVX    2
#define USE_SIMD_FMA    3
#define USE_SIMD_NEON   4
#define USE_SIMD_NEONV2 5

typedef void (*DNN_FUNC_VOID)();

typedef struct {
  float *w;			/* w [out * in]*/
  float *b;			/* b [out] */
  int in;
  int out;
} DNNLayer;

typedef struct {
  DNNLayer o;			/* output layer */
  DNNLayer *h;			/* hidden layer */
  int hnum;			/* number of hidden layers */

  float *state_prior;		/* state priors [id] */
  int state_prior_num;		/* num of above (= output layer length) */
  float prior_factor;		/* prior factor */

  int batch_size;		/* batch size */
  
  int veclen;		  /* input vector length (before expansion) */
  int contextlen;	  /* context length */
  
  int inputnodenum;		/* input layer node number */
  int hiddennodenum;		/* hidden layer node number */
  int outputnodenum;		/* output layer node number */

  float *invec;		    /* input vector holder (32byte aligned) */
  float **work;		    /* working buffer for ff computation */
  float *accum;		    /* working buffer for accumulation */

  DNN_FUNC_VOID subfunc;	/* sub function for DNN computation */
  
} DNNData;

#endif /* __SENT_DNN_H__ */
