/*
 * Copyright (c) 1991-2013 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2013 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#include <stdlib.h>

#define VER_NO "ver.1.44-flex-p1"
#define SYMBOL_LEN 256
typedef short FLAG;
typedef unsigned int CLASSFLAGS;
#define CLASSFLAG_MAX sizeof(CLASSFLAGS)*8

typedef struct _BODY{
    char name[ SYMBOL_LEN ];         /* $B9=@.$9$k%/%i%9$NL>A0(B */
    FLAG abort;                      /* BODY$B$NESCf=*N;%U%i%0(B */
    struct _BODY *next;
} BODY;

typedef struct _BODYLIST{
    BODY *body;
    struct _BODYLIST *next;
} BODYLIST;

typedef struct _CLASS{
    short no;                        /* $B%/%i%9HV9f(B $BHs=*C<$H=*C<$GFHN)3dEv(B
					$BHs=*C<$O(B#31$B$^$G$NEPO?!"$=$NB>$O(B#-1 */
    char name[ SYMBOL_LEN ];         /* $B%/%i%9$NL>A0(B */
    struct _CLASS *next;             /* $B<!$N%/%i%9$X$N%]%$%s%?(B */
    BODYLIST *bodyList;              /* $B%/%i%9$N%\%G%#$N%j%9%H$X$N%]%$%s%?(B */
    int branch;                      /* $BDj5A?t(B */
    FLAG usedFA;                     /* $B%/%i%9$,;HMQ$5$l$?$+(B */
    FLAG used;
    FLAG tmp;                        /* $B:G>.2=$N$?$a0l;~E*$K$G$-$?%/%i%9$+(B */
} CLASS;

/*
   bodyList, branch $B$O=*C<$HHs=*C<$G0UL#$,A4$/0c$&(B
   non-terminal:
    bodyList: $B9=@.$9$k%/%i%9L>$N%j%9%H$N%j%9%H$X$N%]%$%s%?(B
    branch:   $BG[Ns$N?t(B($BDj5A$N?t$@$1B8:_$9$k(B)
   terminal:
    bodyList: $B$=$N=*C<5-9f$K3:Ev$9$k<B:]$NC18l(B
    branch:   $BC18l$N<oN`$K(B-1$B$r$+$1$?$b$N(B
   $B=*C<$HHs=*C<$O@5Ii$GH=CG$9$k(B
*/

typedef struct _ARC{
    int inp;                         /* $BF~NO(B */
    struct _FA *fa;                  /* $BA+0\@h$N(BFA */
    CLASSFLAGS start;                /* $B%/%i%93+;O%U%i%0(B */
    CLASSFLAGS accpt;                /* $B%/%i%9<uM}%U%i%0(B */
    struct _ARC *next;               /* $B%j%9%H$N<!9`L\(B */
} ARC;

typedef struct _UNIFYARC{
    int inp;                         /* $BF~NO(B */
    struct _FA *us;                  /* FA$B<!>uBV(B */
    CLASSFLAGS start;                /* $B%/%i%93+;O%U%i%0(B */
    CLASSFLAGS accpt;                /* $B%/%i%9<uM}%U%i%0(B */
    struct _UNIFYARC *next;          /* $B%j%9%H$N<!9`L\(B */
    FLAG reserved;                   /* $B<+8J%k!<%W$N;^$rM;9g$9$k;~$O!"JQ49(B
					$B=hM}$,40A4$K=*N;$7$F$+$i9T$J$&!#$=(B
					$B$NL$=hM}%U%i%0(B */
} UNIFYARC;

typedef struct _FALIST{
    struct _FA *fa;
    struct _FALIST *next;
} FALIST;

typedef struct _FA{
    /* common */
    int stat;                        /* $B>uBVHV9f(B(3$B$DAH:n@.;~$K?6$i$l$k(B) */
    ARC *nsList;                     /* $BF~NO$H<!>uBV%j%9%H(B */
    CLASSFLAGS start;                /* $B%/%i%93+;O%U%i%0(B($BA4$F$N%"!<%/$N(Bor) */
    CLASSFLAGS accpt;                /* $B%/%i%9<uM}%U%i%0(B($BA4$F$N%"!<%/$N(Bor) */
    CLASSFLAGS aStart;               /* $BCeL\Cf$N%"!<%/$N%/%i%93+;O%U%i%0(B */
    FLAG traversed;                  /* $BN)4s(B 1:NFA->DFA 2:3$B$DAH:n@.;~(B */

    /* for DFA */
    int psNum;                       /* ARC$B$G;X$5$l$F$$$k%"!<%/?t(B */
                   /* connectUnifyFA$B$G$O(Bincrement$B$5$l$J$$$3$H$KCm0U!#(B */
    UNIFYARC *usList;                /* NFA->DFA$B$GM;9g$5$l$?<!>uBV(B */
    FALIST *group;                   /* $BM;9g$7$?$H$-$N9=@.$9$k>uBV(B */
    FLAG volatiled;    /* $B%"!<%/JQ99Cf$N$?$a8IN)H=Dj$r<h$j$d$a$m$N0U(B */
} FA;

void errMes( char *fmt, ... );
void warnMes( char *fmt, ... );
void verboseMes( char *fmt, ... );
void setGramFile( char *fname );
void setVoca( void );
CLASS *getClass( char *name );
void setGram( void );

#define newLineAdjust()\
{\
    if( NoNewLine ){\
	putc( '\n', stderr );\
	NoNewLine = 0;\
    }\
}
