/*
 * Copyright (c) 1991-2013 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2013 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */


/* define this to test disabling expsum computation at softmax */
#undef NO_SUM_COMPUTATION

#ifdef _WIN32
#include <intrin.h>
#else
#ifndef __arm__
#include <cpuid.h>
#endif
#endif	/* _WIN32 */

#include <sent/stddefs.h>
#include <sent/htk_hmm.h>
#include <sent/htk_param.h>
#include <sent/hmm.h>
#include <sent/hmm_calc.h>

#if defined(HAS_SIMD_FMA) || defined(HAS_SIMD_AVX) || defined(HAS_SIMD_SSE) || defined(HAS_SIMD_NEON) || defined(HAS_SIMD_NEONV2)
#define SIMD_ENABLED
#endif

static int use_simd = USE_SIMD_NONE;

/************************************************************************/
/* determine which SIMD code to run */

#ifdef SIMD_ENABLED

static void cpu_id_check()
{
  int cpuinfo[4];
  boolean sse = FALSE, avx = FALSE, fma = FALSE;

  use_simd = USE_SIMD_NONE;

#ifdef __arm__
  /* on ARM NEON */

#if defined(HAS_SIMD_NEONV2)
  use_simd = USE_SIMD_NEONV2;
#elif defined(HAS_SIMD_NEON)
  use_simd = USE_SIMD_NEON;
#else
  use_simd = USE_SIMD_NONE;
#endif

#else  /* ~__arm__ */

#ifdef _WIN32
  __cpuid(cpuinfo, 0x00000001);
  if(cpuinfo[3] & (1 << 25)) {
    sse = TRUE;
  }
  if(cpuinfo[2] & (1 << 28)) {
    avx = TRUE;
  }
  if(cpuinfo[2] & (1 << 12)) {
    fma = TRUE;
  }
#else  /* ~_WIN32 */
  unsigned int eax, ebx, ecx, edx;

  if (__get_cpuid(1, &eax, &ebx, &ecx, &edx) == 0)
    return;
  if (edx & bit_SSE) {
    sse = TRUE;
  }
  if (ecx & bit_AVX) {
    avx = TRUE;
  }
  if (ecx & bit_FMA) {
    fma = TRUE;
  }
#endif	/* _WIN32 */

#ifdef HAS_SIMD_FMA
  if (fma == TRUE) {
    use_simd = USE_SIMD_FMA;
    return;
  }
#endif

#ifdef HAS_SIMD_AVX
  if (avx == TRUE) {
    use_simd = USE_SIMD_AVX;
    return;
  }
#endif

#ifdef HAS_SIMD_SSE
  if (sse == TRUE) {
    use_simd = USE_SIMD_SSE;
    return;
  }
#endif

#endif  /* ~__arm__ */

}

static void *mymalloc_simd_aligned(size_t size)
{
  void *ptr;

  switch(use_simd) {
  case USE_SIMD_FMA:
  case USE_SIMD_AVX:
    ptr = mymalloc_aligned(size, 32);
    break;
  case USE_SIMD_SSE:
  case USE_SIMD_NEON:
  case USE_SIMD_NEONV2:
    ptr = mymalloc_aligned(size, 16);
    break;
  default:
    ptr = mymalloc(size);
    break;
  }

  return ptr;
}

static void myfree_simd_aligned(void *ptr)
{
  switch(use_simd) {
  case USE_SIMD_FMA:
  case USE_SIMD_AVX:
  case USE_SIMD_SSE:
  case USE_SIMD_NEON:
  case USE_SIMD_NEONV2:
    if (ptr != NULL) myfree_aligned(ptr);
    break;
  default:
    if (ptr != NULL) free(ptr);
    break;
  }
}

#endif	/* SIMD_ENABLED */

void get_builtin_simd_string(char *buf)
{
  buf[0] = '\0';

#ifdef HAS_SIMD_NEON
  strcat(buf, " NEON");
#endif
#ifdef HAS_SIMD_NEONV2
  strcat(buf, " NEONv2");
#endif
#ifdef HAS_SIMD_SSE
  strcat(buf, " SSE");
#endif
#ifdef HAS_SIMD_AVX
  strcat(buf, " AVX");
#endif
#ifdef HAS_SIMD_FMA
  strcat(buf, " FMA");
#endif
}

int check_avail_simd()
{
#ifdef SIMD_ENABLED
  cpu_id_check();
#endif
  return use_simd;
}

static void output_use_simd()
{
#ifdef SIMD_ENABLED
#ifdef HAS_SIMD_NEON
  jlog("Stat: calc_dnn: ARM NEON instructions built-in\n");
#endif
#ifdef HAS_SIMD_NEONV2
  jlog("Stat: calc_dnn: ARM NEONv2 instructions built-in\n");
#endif
#ifdef HAS_SIMD_FMA
  jlog("Stat: calc_dnn: FMA instructions built-in\n");
#endif
#ifdef HAS_SIMD_AVX
  jlog("Stat: calc_dnn: AVX instructions built-in\n");
#endif
#ifdef HAS_SIMD_SSE
  jlog("Stat: calc_dnn: SSE instructions built-in\n");
#endif
#else  /* ~SIMD_ENABLED */
  jlog("Warning: NO built-in SIMD support, DNN computation may be too slow!\n");
  return;
#endif  /* ~SIMD_ENABLED */

#ifdef SIMD_ENABLED
  if (use_simd == USE_SIMD_SSE) {
    jlog("Stat: clac_dnn: use SSE SIMD instruction (128bit)\n");
  } else if (use_simd == USE_SIMD_AVX) {
    jlog("Stat: clac_dnn: use AVX SIMD instruction (256bit)\n");
  } else if (use_simd == USE_SIMD_FMA) {
    jlog("Stat: clac_dnn: use FMA SIMD instruction (256bit)\n");
  } else if (use_simd == USE_SIMD_NEON) {
    jlog("Stat: use ARM NEON instruction\n");
  } else if (use_simd == USE_SIMD_NEONV2) {
    jlog("Stat: use ARM NEONv2 instruction\n");
  } else {
    jlog("Warning: clac_dnn: no SIMD support, DNN computation may be too slow!\n");
  }
#endif	/* SIMD_ENABLED */
}

/************************************************************************/
/* .npy file load */

static boolean load_npy(float *array, char *filename, int x, int y)
{
  FILE *fp;
  unsigned char code;
  char magic[6];
  unsigned char major_version;
  unsigned char minor_version;
  unsigned short header_len;
  char *header;
  size_t len;
  boolean fortran_order;

  if ((fp = fopen_readfile(filename)) == NULL) {
    jlog("Error: load_npy: unable to open: %s\n", filename);
    return FALSE;
  }
  if ((len = myfread(&code, 1, 1, fp)) < 1) {
    jlog("Error: load_npy: failed to read header: %s\n", filename);
    fclose_readfile(fp);
    return FALSE;
  }
  if (code != 0x93) {
    jlog("Error: load_npy: wrong magic number, not an npy file: %s\n", filename);
    return FALSE;
  }
  if ((len = myfread(magic, 1, 5, fp)) < 5) {
    jlog("Error: load_npy: failed to read header: %s\n", filename);
    fclose_readfile(fp);
    return FALSE;
  }
  magic[5] = '\0';
  if (strmatch(magic, "NUMPY") == FALSE) {
    jlog("Error: load_npy: wrong magic header, not an npy file: %s\n", filename);
    return FALSE;
  }
  if ((len = myfread(&major_version, 1, 1, fp)) < 1) {
    jlog("Error: load_npy: failed to read header: %s\n", filename);
    fclose_readfile(fp);
    return FALSE;
  }
  /* we only assume Version 1.x format */
  /* not check subversion x */
  if (major_version != 1) {
    jlog("Error: load_npy: can read only Version 1.0 but this file is Version %d\n", major_version);
    fclose_readfile(fp);
    return FALSE;
  }
  if ((len = myfread(&minor_version, 1, 1, fp)) < 1) {
    jlog("Error: load_npy: failed to read header: %s\n", filename);
    fclose_readfile(fp);
    return FALSE;
  }

  /* currently not support all conversion */
  /* accept only littlen endian 4byte float, with fortran order */
  /* produce error if the file has other format */
  if ((len = myfread(&header_len, 2, 1, fp)) < 1) {
    jlog("Error: load_npy: failed to read header length: %s\n", filename);
    fclose_readfile(fp);
    return FALSE;
  }
#ifdef WORDS_BIGENDIAN
  swap_bytes(&header_len, 2, 1);
#endif  
  header = (char *)mymalloc(header_len + 1);
  if ((len = myfread(header, 1, header_len, fp)) < header_len) {
    jlog("Error: load_npy: failed to read header (%d bytes): %s\n", header_len, filename);
    free(header);
    fclose_readfile(fp);
    return FALSE;
  }
  header[header_len] = '\0';
  if (strstr(header, "'descr': '<f4'") == NULL) {
    jlog("Error: load_npy: not a little-endian float array: %s\n", filename);
    free(header);
    fclose_readfile(fp);
    return FALSE;
  }

  /* fortran order: data are stored per columns */
  /* C order: data are stored per row */
  if (strstr(header, "'fortran_order': True")) {
    fortran_order = TRUE;
  } else {
    fortran_order = FALSE;
  }

  char buf[100];
  sprintf(buf, "'shape': (%d, %d)", x, y);
  if (strstr(header, buf) == NULL) {
    sprintf(buf, "'shape': (%d, %d)", y, x);
    if (strstr(header, buf) == NULL) {
      jlog("Error: load_npy: not a (%d, %d) array? %s\n", x, y, filename);
      free(header);
      fclose_readfile(fp);
      return FALSE;
    }
  }
  free(header);

  /* just read them in the order */
  if ((len = myfread(array, 4, x * y, fp)) < x * y) {
    jlog("Error: load_npy: failed to read %d bytes: %s\n", x * y, filename);
    fclose_readfile(fp);
    return FALSE;
  }

  fclose_readfile(fp);
  return TRUE;
}


/************************************************************************/
/* standard logistic function value table: take range x[-6,6] */
/* table size: LOGISTIC_TABLE_FACTOR * 12 * 4 (bytes) */
#define LOGISTIC_TABLE_FACTOR 20000
#define LOGISTIC_TABLE_MAX (16 * LOGISTIC_TABLE_FACTOR)
#define LOGISTIC_MIN 0.000334
#define LOGISTIC_MAX 0.999666

static float logistic_table[LOGISTIC_TABLE_MAX+1]; /* logistic value table */

/* build logistic function value table */
static void logistic_table_build()
{
  int i;
  double d;
  double x;
  
  for (i = 0; i <= LOGISTIC_TABLE_MAX; i++) {
    x = (double)i / (double)LOGISTIC_TABLE_FACTOR - 8.0;
    d = 1.0 / (1.0 + exp(-x));
    logistic_table[i] = (float)d;
  }
}

/* return logistic function value, consulting table */
static float logistic_func(float x)
{
  if (x <= -8.0f) return LOGISTIC_MIN;
  if (x >=  8.0f) return LOGISTIC_MAX;
  return logistic_table[(int)((x + 8.0f) * LOGISTIC_TABLE_FACTOR + 0.5)];
}

/* initialize dnn layer */
static void dnn_layer_init(DNNLayer *l)
{
  l->w = NULL;
  l->b = NULL;
  l->in = 0;
  l->out = 0;
}

/* load dnn layer parameter from files */
static boolean dnn_layer_load(DNNLayer *l, int in, int out, char *wfile, char *bfile)
{
  l->in = in;
  l->out = out;
#ifdef SIMD_ENABLED
  if (use_simd == USE_SIMD_AVX && l->in % 8 != 0) {
    jlog("Error: dnn_layer_load: input vector length is not 8-element aligned (%d)\n", l->in);
    return FALSE;
  }
  if (use_simd == USE_SIMD_SSE && l->in % 4 != 0) {
    jlog("Error: dnn_layer_load: input vector length is not 4-element aligned (%d)\n", l->in);
    return FALSE;
  }
  l->w = (float *)mymalloc_simd_aligned(sizeof(float) * l->out * l->in);
  l->b = (float *)mymalloc_simd_aligned(sizeof(float) * l->out);
#else
  l->w = (float *)mymalloc(sizeof(float) * l->out * l->in);
  l->b = (float *)mymalloc(sizeof(float) * l->out);
#endif	/* SIMD_ENABLED */
  if (! load_npy(l->w, wfile, l->in, l->out)) return FALSE;
  jlog("Stat: dnn_layer_load: loaded %s\n", wfile);
  if (! load_npy(l->b, bfile, l->out, 1)) return FALSE;
  jlog("Stat: dnn_layer_load: loaded %s\n", bfile);

  return TRUE;
}

/* clear dnn layer */
static void dnn_layer_clear(DNNLayer *l)
{
#ifdef SIMD_ENABLED
  if (l->w != NULL) myfree_simd_aligned(l->w);
  if (l->b != NULL) myfree_simd_aligned(l->b);
#else
  if (l->w != NULL) free(l->w);
  if (l->b != NULL) free(l->b);
#endif	/* SIMD_ENABLED */
  dnn_layer_init(l);
}

/*********************************************************************/
DNNData *dnn_new()
{
  DNNData *d;

  d = (DNNData *)mymalloc(sizeof(DNNData));
  memset(d, 0, sizeof(DNNData));

  return d;
}

void dnn_clear(DNNData *dnn)
{
  int i;

  if (dnn->h) {
    for (i = 0; i < dnn->hnum; i++) {
      dnn_layer_clear(&(dnn->h[i]));
    }
    free(dnn->h);
  }
  if (dnn->state_prior) free(dnn->state_prior);
  for (i = 0; i < dnn->hnum; i++) {
    if (dnn->work[i]) {
#ifdef SIMD_ENABLED
      myfree_simd_aligned(dnn->work[i]);
#else
      free(dnn->work[i]);
#endif
    }
  }
  free(dnn->work);
#ifdef SIMD_ENABLED
  if (dnn->invec) myfree_simd_aligned(dnn->invec);
  if (dnn->accum) myfree_aligned(dnn->accum);
#endif

  memset(dnn, 0, sizeof(DNNData));
}

void dnn_free(DNNData *dnn)
{
  dnn_clear(dnn);
  free(dnn);
}

/************************************************************************/

static void
sub1(float *dst, float *src, float *w, float *b, int out, int in, float *fstore)
{
  float *s;
  int i, j;

  for (i = 0; i < out; i++) {
    float x = 0.0f;
    s = src;
    for (j = 0; j < in; j++) {
      x += *(w++) * *(s++);
    }
    *(dst++) = x + *(b++);
  }
}

/************************************************************************/

/* initialize dnn */
boolean dnn_setup(DNNData *dnn, int veclen, int contextlen, int inputnodes, int outputnodes, int hiddennodes, int hiddenlayernum, char **wfile, char **bfile, char *output_wfile, char *output_bfile, char *priorfile, float prior_factor, int batchsize)
{
  int i;

  /* check if CPU has SIMD instruction support */
#ifdef SIMD_ENABLED
  cpu_id_check();
#endif

  if (dnn == NULL) return FALSE;

  /* clear old data if exist */
  dnn_clear(dnn);

  /* build logistic table */
  logistic_table_build();

  /* set values */
  dnn->batch_size = batchsize;
  dnn->veclen = veclen;
  dnn->contextlen = contextlen;
  dnn->inputnodenum = inputnodes;
  dnn->hiddennodenum = hiddennodes;
  dnn->outputnodenum = outputnodes;
  dnn->prior_factor = prior_factor;

  /* check for input length */
  int inputlen = veclen * contextlen;
  if (inputnodes != inputlen) {
    jlog("Error: dnn_init: veclen(%d) * contextlen(%d) != inputnodes(%d)\n", veclen, contextlen, inputnodes);
    return FALSE;
  }

  jlog("Stat: dnn_init: input: vec %d * context %d = %d dim\n", veclen, contextlen, inputlen);
  jlog("Stat: dnn_init: input layer: %d dim\n", inputnodes);
  jlog("Stat: dnn_init: %d hidden layer(s): %d dim\n", hiddenlayernum, hiddennodes);
  jlog("Stat: dnn_init: output layer: %d dim\n", outputnodes);
  
  /* initialize layers */
  dnn->hnum = hiddenlayernum;
  dnn->h = (DNNLayer *)mymalloc(sizeof(DNNLayer) * dnn->hnum);
  for (i = 0; i < dnn->hnum; i++) {
    dnn_layer_init(&(dnn->h[i]));
  }
  dnn_layer_init(&(dnn->o));

  /* load layer parameters */
  if (dnn_layer_load(&(dnn->h[0]), inputnodes, hiddennodes, wfile[0], bfile[0]) == FALSE) return FALSE;
  for (i = 1; i < dnn->hnum; i++) {
    if (dnn_layer_load(&(dnn->h[i]), hiddennodes, hiddennodes, wfile[i], bfile[i]) == FALSE) return FALSE;
  }
  if (dnn_layer_load(&(dnn->o), hiddennodes, outputnodes, output_wfile, output_bfile) == FALSE) return FALSE;

  /* load state prior */
  {
    FILE *fp;
    int id;
    float val;

    dnn->state_prior_num = outputnodes;
    dnn->state_prior = (float *)mymalloc(sizeof(float) * dnn->state_prior_num);
    for (i = 0; i < dnn->state_prior_num; i++) {
      dnn->state_prior[i] = 0.0f;
    }
    if ((fp = fopen(priorfile, "r")) == NULL) {
      jlog("Error: cannot open %s\n", priorfile);
      return FALSE;
    }
    while (fscanf(fp, "%d %e", &id, &val) != EOF){
      if (id < 0 || id >= dnn->state_prior_num) {
	jlog("Error: wrong state id in prior file (%d)\n", id);
	fclose_readfile(fp);
	return FALSE;
      }
      dnn->state_prior[id] = val * prior_factor;
      // log10-nize prior
      dnn->state_prior[id] = log10(dnn->state_prior[id]);
    }
    fclose(fp);
    jlog("Stat: dnn_init: state prior loaded: %s\n", priorfile);
  }

  /* allocate work area */
  dnn->work = (float **)mymalloc(sizeof(float *) * dnn->hnum);
  for (i = 0; i < dnn->hnum; i++) {
#ifdef SIMD_ENABLED
    dnn->work[i] = (float *)mymalloc_simd_aligned(sizeof(float) * dnn->hiddennodenum);
#else
    dnn->work[i] = (float *)mymalloc(sizeof(float) * dnn->hiddennodenum);
#endif
  }
#ifdef SIMD_ENABLED
  dnn->invec = (float *)mymalloc_simd_aligned(sizeof(float) * inputnodes);
  dnn->accum = (float *)mymalloc_simd_aligned(32);
#endif

  /* choose sub function */
#ifdef SIMD_ENABLED
  switch(use_simd) {
  case USE_SIMD_FMA:
    dnn->subfunc = calc_dnn_fma;
    break;
  case USE_SIMD_AVX:
    dnn->subfunc = calc_dnn_avx;
    break;
  case USE_SIMD_SSE:
    dnn->subfunc = calc_dnn_sse;
    break;
  case USE_SIMD_NEON:
    dnn->subfunc = calc_dnn_neon;
    break;
  case USE_SIMD_NEONV2:
    dnn->subfunc = calc_dnn_neonv2;
    break;
  default:
    dnn->subfunc = sub1;
    break;
  }
#else
  dnn->subfunc = sub1;
#endif	/* SIMD_ENABLED */

  /* output CPU related info */
  output_use_simd();

  return TRUE;
}

void dnn_calc_outprob(HMMWork *wrk)
{
  int hidx, i, n;
  float *src, *dst;
  DNNLayer *h;
  DNNData *dnn = wrk->OP_dnn;

  /* frame = wrk->OP_time */
  /* param = wrk->OP_param */
  /* input vector = wrk->OP_param[wrk->OP_time][] */
  /* store state outprob to wrk->last_cache[]  */

  /* feed forward through hidden layers by standard logistic function */
  n = 0;
#ifdef SIMD_ENABLED
  memcpy(dnn->invec, &(wrk->OP_param->parvec[wrk->OP_time][0]), sizeof(float) * dnn->inputnodenum);
  src = dnn->invec;
#else
  src = &(wrk->OP_param->parvec[wrk->OP_time][0]);
#endif	/* SIMD_ENABLED */
  dst = dnn->work[n];
  for (hidx = 0; hidx < dnn->hnum; hidx++) {
    h = &(dnn->h[hidx]);
    (*dnn->subfunc)(dst, src, h->w, h->b, h->out, h->in, dnn->accum);
    for (i = 0; i < h->out; i++) {
      dst[i] = logistic_func(dst[i]);
    }
    src = dst;
    //if (++n > 1) n = 0;
    dst = dnn->work[++n];
  }
  /* output layer */
  (*dnn->subfunc)(wrk->last_cache, src, dnn->o.w, dnn->o.b, dnn->o.out, dnn->o.in, dnn->accum);

  /* do softmax */
  /* INV_LOG_TEN * (x - addlogarray(x)) - log10(state_prior)) */
#ifdef NO_SUM_COMPUTATION
  /* not compute sum */
  for (i = 0; i < wrk->statenum; i++) {
    wrk->last_cache[i] = INV_LOG_TEN * wrk->last_cache[i] - dnn->state_prior[i];
  }
#else
  /* compute sum */
  float logprob = addlog_array(wrk->last_cache, wrk->statenum);
  for (i = 0; i < wrk->statenum; i++) {
    wrk->last_cache[i] = INV_LOG_TEN * (wrk->last_cache[i] - logprob) - dnn->state_prior[i];
  }
}
#endif
