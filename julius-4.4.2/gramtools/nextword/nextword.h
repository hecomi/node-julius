/*
 * Copyright (c) 1991-2016 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2016 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#ifdef HAVE_LIBREADLINE
# if defined(HAVE_READLINE_READLINE_H)
#  include <readline/readline.h>
# elif defined(HAVE_READLINE_H)
#  include <readline.h>
# else
extern char *readline();
# endif
#endif	/* HAVE_LIBREADLINE */

#ifdef HAVE_LIBREADLINE_HISTORY
# if defined(HAVE_READLINE_HISTORY_H)
#  include <readline/history.h>
# elif defined(HAVE_HISTORY_H)
#  include <history.h>
# else
extern void add_history();
# endif
#endif	/* HAVE_LIBREADLINE_HISTORY */

typedef struct __wtoken__ {
  WORD_ID wid;
  struct __wtoken__ *next;
} WTOKEN;

typedef struct __state_chain__ {
  int state;
  struct __state_chain__ *next;
} STATECHAIN;

void put_wtoken(WTOKEN **wseq, int nseq);
boolean new_get_wtoken(char *buf, WTOKEN **wseq, int *nseq_ret);
void free_wtoken(WTOKEN **wseq, int nseq);
int next_terms(int stateid, int *termbuf, int *nextstatebuf);
int compaction_int(int *a, int num);
char *rl_gets (char *prompt);
char *dfaterm_generator(char *text, int state);
char *dfaword_generator(char *text, int state);
void init_term(char *filename, char **termname);
