/*
 * Copyright (c) 1991-2013 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2013 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */
#include "mkfa.h"
#include "nfa.h"

typedef struct _HIS{
    CLASS *class;                    /* $B%/%i%9$X$N%]%$%s%?(B */
    FA *fa;                          /* $B@8@.F0:n;~$N(BFA($B:F5"$KMQ$$$k(B) */
    struct _HIS *prev;               /* $B?F$N7PNr$X$N%]%$%s%?(B */
    ARC *nsList;                     /* $B%/%i%93+;O%N!<%I$+$i$NA+0\2DG=%"!<%/(B */
    FA *cloneFA;                     /* $B:F5";~$NLa$j@h$N(BFA */
} HIS;

typedef struct _TOKEN{               /* naming$B$,$A$H6l$7$$$,(B */
    CLASS *class;                    /* $B%/%i%9$X$N(Bpointer */
    FLAG abort;                      /* $BESCfC&=P2DG=%U%i%0(B */
} TOKEN;

FA *r_makeNFA( CLASS *class, FA *fa, FA *exitFA, FALIST *orgExtraFAs, HIS *his );
int getNextToken( TOKEN *token, BODYLIST **pBodyList, BODY **pBody );
void connectFAforNFA( FA *fa, int input, FA *nextFA, HIS *his );
FA *appendFA( FA *fa, int input, HIS *his );
ARC *appendArc( ARC *top, FA *dst, int inp, CLASSFLAGS accpt, CLASSFLAGS start );
void appendHisArc( HIS *his, FA *fa, int inp, FA *nextFA, CLASSFLAGS accpt, CLASSFLAGS start );
void chkClassInfo( CLASS *class );
CLASS *getClass( char *name );
FA *getRecursion( CLASS *class, HIS *his );
void chkLeftRecursion( CLASS *class, FA *fa, HIS *his );
char *strAncestors( HIS *me, HIS *ancestor );

extern CLASS *ClassList;       /* $B%/%i%9$N@~7A%j%9%H(B */
extern CLASS *ClassListTail;   /* $B%/%i%9$N@~7A%j%9%H$N:G8eHx$N%N!<%I(B */
extern CLASS *StartSymbol;     /* $B3+;O5-9f$N%/%i%9(B */
extern FA *FAlist;             /* FA$B%M%C%H%o!<%/$K$*$1$k3+;O(BFA$B$N%]%$%s%?(B */
static int FAprocessed = 0;    /* $B8=:_$N%9%F%C%W$K$*$$$F=hM}$5$l$?(BFA$B$N?t(B */
int FAtotal = 0;               /* FA$B$NAm?t(B */

extern int SW_Quiet;
extern int SW_SemiQuiet;
extern int NoNewLine;          /* $BJ#?t$NI=<(%b!<%I$G2~9TLdBj$r2r7h$9$k(B */

void makeNFA( void )
{
    if( StartSymbol == NULL ){
	errMes( "No definition of grammar" );
    }
    FAprocessed = 0;
    FAlist = makeNewFA();
    FAlist->psNum++; /* $B3+;O%N!<%I$N:o=|$rKI$0$?$a(B */
    if( !SW_Quiet ){
	fprintf( stderr, "\rNow making nondeterministic finite automaton" );
	NoNewLine = 1;
    }
    r_makeNFA( StartSymbol, FAlist, NULL, NULL, NULL );
    if( !SW_Quiet ){
	fprintf( stderr, "\rNow making nondeterministic finite automaton[%d/%d]\n", FAtotal, FAtotal );
	NoNewLine = 0;
    }
    chkClassInfo( ClassList );
}

FA *r_makeNFA( CLASS *class, FA *fa, FA *exitFA, FALIST *orgExtraFAs, HIS *his )
{
    FA *baseFA = fa;             /* $BEv3:%/%i%9$NF~8}%N!<%I(B */
    FA *loopFA = NULL;           /* $B:F5"%k!<%W$N%N!<%I(B */
    HIS curhis;                  /* $B2r@OLZ$N7PNr$K$*$1$k8=:_$N%N!<%I(B */
    TOKEN curToken;              /* $B8=:_CmL\$N%H!<%/%s(B */
    TOKEN nextToken;             /* $B@hFI$_%H!<%/%s(B */
    FLAG exitFlag = 0;           /* $BC&=P$G$-$k%H%]%m%8!<$+(B */
    BODYLIST *bodyList = class->bodyList;
    BODY *body = NULL;
    FALIST *extraFAs = NULL;
    CLASSFLAGS initStartFlag;

    if( !SW_SemiQuiet ){
	fprintf( stderr, "\rNow making nondeterministic finite automaton[%d/%d]", FAtotal, FAtotal );
	NoNewLine = 1;
    }

    class->usedFA= 1;
    if( class->no >= 0 ){
	fa->start |= (1 << class->no);
	fa->aStart |= (1 << class->no);
    }
    initStartFlag = fa->aStart;

    if( exitFA == NULL ) exitFA = makeNewFA();
    chkLeftRecursion( class, fa, his );
    curhis.class = class;
    curhis.fa = fa;
    curhis.prev = his;
    curhis.nsList = NULL;
    curhis.cloneFA = NULL;

    while( 1 ){ /* $BDj5ANs4V$N%k!<%W(B */
	getNextToken( &nextToken, &bodyList, &body );
	extraFAs = cpyFAlist( extraFAs, orgExtraFAs ); /*C++$B$J$iBeF~0lH/!D(B*/
	while( 1 ){ /* $BDj5ANsFb$N%k!<%W(B */
	    curToken = nextToken;
	    if( getNextToken( &nextToken, &bodyList, &body ) /*$B8eB3$,$"$k$+(B?*/ ){
		if( curToken.class->branch > 0 /* $B8eB3$,$"$jHs=*C<(B */ ){
		    if( (loopFA = getRecursion( nextToken.class, &curhis )) != NULL ){
			if( curToken.abort ){
			    /* $B$3$NDj5ANs$O=*N;$J$N$G?F$+$i0z$-7Q$$$@I{=P8}$bEO$9(B*/
			    /* !curToken.abort$B$J$i=*N;$G$J$$$N$G(BNULL$B$N$^$^(B */
			    extraFAs = appendFAlist( extraFAs, exitFA );
			    exitFlag = 1;
			}
			/* nextToken$B$,(Bloop$B$J$N$G$=$3$X$D$J$0(B */
			fa = r_makeNFA( curToken.class, fa, loopFA, extraFAs, &curhis );
			/* $B$?$@$7$=$N<!$K8eB3$,$"$k$J$i%(%i!<(B */
			if( getNextToken( &curToken, &bodyList, &body ) ){
			    errMes( "Symbols following recursion exist in class \"%s\"", class->name );
			}
			break;
		    } else {
			FALIST *anExtraFAs = NULL;
			if( curToken.abort ){
			    /* $B$3$NDj5ANs$O$3$N>l=j$GC&=P$9$k(B */
			    anExtraFAs = cpyFAlist( anExtraFAs, extraFAs );
			    anExtraFAs = appendFAlist( anExtraFAs, exitFA );
			    exitFlag = 1;
			}
			fa = r_makeNFA( curToken.class, fa, NULL, anExtraFAs, &curhis );
			freeFAlist( anExtraFAs );
			continue;
		    }
		} else { /* $B8eB3$,$"$j=*C<(B */
		    if( curToken.abort ){
			FA *extraFA;
			FALIST *pExtraFAs = extraFAs;
			connectFAforNFA( fa, curToken.class->no, exitFA, &curhis );
			while( pExtraFAs != NULL ){
			    extraFA = pExtraFAs->fa;
			    connectFAforNFA( fa, curToken.class->no, extraFA, &curhis );
			    pExtraFAs = pExtraFAs->next;
			}
			exitFlag = 1;
		    }
		    if( (loopFA = getRecursion( nextToken.class,&curhis )) != NULL ){
			connectFAforNFA( fa, curToken.class->no, loopFA, &curhis );
			if( getNextToken( &curToken, &bodyList, &body ) ){
			    errMes( "Symbols following recursion exist in class \"%s\"", class->name );
			}
			break;
		    } else {
			fa = appendFA( fa, curToken.class->no, &curhis );
			continue;
		    }
		}
	    } else { /* $B8eB3$,$J$$(B */
		exitFlag = 1;
		if( curToken.class->branch > 0 ){
		    exitFA = r_makeNFA( curToken.class, fa, exitFA, extraFAs, &curhis );
		} else {
		    FA *extraFA;
		    FALIST *pExtraFAs = extraFAs;
		    while( pExtraFAs != NULL ){
			extraFA = pExtraFAs->fa;
			connectFAforNFA( fa, curToken.class->no, extraFA, &curhis );
			pExtraFAs = pExtraFAs->next;
		    }
		    connectFAforNFA( fa, curToken.class->no, exitFA, &curhis );
		}
		break;
	    }
	} /* $BDj5ANsFb$N%k!<%W$N=*N;(B */

	if( class->no >= 0 ){
	    FALIST *extraFA = extraFAs;
	    while( extraFA != NULL ){
		extraFA->fa->accpt |= (1 << class->no);
		extraFA = extraFA->next;
	    }
	}
	if( bodyList == NULL ) break;
	fa = baseFA;
	fa->aStart = initStartFlag;
    } /* $BDj5ANs4V$N%k!<%W$N=*N;(B */

    if( !exitFlag ){
	errMes( "Infinite definition is formed %s", strAncestors( curhis.prev, NULL ) );
    }
    if( class->no >= 0 ){
	exitFA->accpt |= (1 << class->no);
	if( curhis.cloneFA != NULL ){
	    curhis.cloneFA->accpt |= (1 << class->no);
	}
    }
    extraFAs = freeFAlist( extraFAs ); /*C++$B$J$i(Bdestructor$B$,$d$k$N$K!D(B*/

    if( curhis.cloneFA == NULL ){
	ARC *curArc, *tmpArc;
	for( curArc = curhis.nsList; curArc != NULL; ){
	    curArc->fa->psNum--;
	    /* cloneFA$B$,;X$9(B($BM=Dj$@$C$?(B)FA$B$O$b$H$NA+0\$N%3%T!<$J$N$G(B
	       psNum$B$,(B0$B$NH=Dj(B($B$=$7$F(BFA$B$N>C5n(B)$B$O>JN,$G$-$k(B(0$B$K$O$J$i$J$$(B)*/
	    tmpArc = curArc->next;
	    free( curArc );
	    curArc = tmpArc;
	}
    }
    return( exitFA );
}

FALIST *appendFAlist( FALIST *faList, FA *fa )
{
    FALIST *atom;

    if( (atom = calloc( 1, sizeof(FALIST) )) == NULL ){
	errMes( "Can't alloc FA list buffer." );
    }

    atom->fa = fa;
    atom->next = faList;
    return( atom );
}

FALIST *cpyFAlist( FALIST *dst, FALIST *src )
{
    if( dst != NULL ) dst = freeFAlist( dst );
    while( src != NULL ){
	dst = appendFAlist( dst, src->fa );
	src = src->next;
    }
    return( dst );
}

FALIST *freeFAlist( FALIST *faList )
{
    while( faList != NULL ){
	FALIST *atom = faList;
	faList = faList->next;
	free( atom );
    }
    return( NULL );
}

int getNextToken( TOKEN *token, BODYLIST **pBodyList, BODY **pBody )
{
    BODYLIST *bodyList = *pBodyList;
    BODY *body = *pBody;

    if( body == NULL ){
	body = bodyList->body;
    } else {
	body = body->next;
	if( body == NULL ){
	    bodyList = bodyList->next;
	    *pBodyList = bodyList;
	    *pBody = body;
	    return( 0 );
	}
    }
    if( (token->class = getClass( body->name )) == NULL ){
	errMes( "undefined class \"%s\"", body->name );
    }
    token->abort = body->abort;
    *pBodyList = bodyList;
    *pBody = body;
    return( 1 );
}

FA *makeNewFA( void )
{
    FA *newFA;
    if( (newFA = calloc( 1, sizeof(FA) )) == NULL ){
	errMes( "Can't alloc Finite Automaton buffer" );
    }
    newFA->stat = -1; /* $B$^$@HV9f$,?6$i$l$F$$$J$$$N0UL#(B */
    FAtotal++;
    return( newFA );
}

FA *appendFA( FA *fa, int input, HIS *his )
{
    FA *newFA;

    newFA = makeNewFA();
    connectFAforNFA( fa, input, newFA, his );
    return( newFA );
}

void connectFAforNFA( FA *fa, int inp, FA *nextFA, HIS *his )
{
    CLASSFLAGS startOnArc = fa->aStart;

    fa->aStart = 0;
    connectFA( fa, inp, nextFA, 0, startOnArc );
    appendHisArc( his, fa, inp, nextFA, 0, startOnArc );
}

void connectFA( FA *fa, int inp, FA *nextFA, CLASSFLAGS accpt, CLASSFLAGS start )
{
    /* $BCm(B: nextFA$B$N(BpsNum$B$r(Bincrement$B$9$k(B */

    fa->nsList = appendArc( fa->nsList, nextFA, inp, accpt, start );
    nextFA->psNum++;
}

ARC *appendArc( ARC *top, FA *dst, int inp, CLASSFLAGS accpt, CLASSFLAGS start )
{
    /* $B%j%9%H$KF~NO$N<-=q=g$GE,@Z0LCV$KA^F~(B
       $B$^$?F1$8$b$N$,$"$k>l9g%"!<%/>e$N%U%i%0$r(Bor$B$9$k(B */
    ARC *newArc;
    ARC *curArc = NULL;
    ARC *nextArc;

    if( (newArc = calloc( 1, sizeof(ARC) )) == NULL ){
	errMes( "Can't alloc forward arc buffer of finite automaton." );
    }
    newArc->inp = inp;
    newArc->fa = dst;
    newArc->start = start;
    newArc->accpt = accpt;

    if( (nextArc = top) != NULL ){
	while( 1 ){
	    if( nextArc->inp > inp ) break;
	    if( nextArc->inp == inp && nextArc->fa == dst ){
		nextArc->start |= newArc->start;
		nextArc->accpt |= newArc->accpt;
		return( top );
	    }
	    curArc = nextArc;
	    if( (nextArc = nextArc->next) == NULL ) break;
	}
    }
    if( curArc == NULL ){
	newArc->next = top;
	return( newArc );
    } else {
	newArc->next = nextArc;
	curArc->next = newArc;
	return( top );
    }
}
    
void appendHisArc( HIS *his, FA *fa, int inp, FA *nextFA, CLASSFLAGS accpt, CLASSFLAGS start )
{
    /* $BCeL\%/%i%9$N3+;O(BFA$B$GA+0\2DG=$J$iMzNr%P%C%U%!$XEPO?(B
       $B$5$i$K?F$?$A$bA+0\2DG=$+D4$Y$k(B */
    while( his != NULL && his->fa == fa /* $B%/%i%9$N3+;O(BFA$B$G$J$$(B */ ){
	his->nsList = appendArc( his->nsList, nextFA, inp, accpt, start );
	if( his->cloneFA != NULL ) his->cloneFA->nsList = his->nsList;
	his = his->prev;
	nextFA->psNum++;
    }
}

void chkClassInfo( CLASS *class )
{
    CLASS *freeClass;
    int wrong = 0;

     while( 1 ){
	if( class == NULL ) break;
	if( class->branch > 0 && !class->usedFA && !class->tmp ){
	    warnMes( "Class \"%s\" isn't used", class->name );
	}
	if (! class->used) {
	  warnMes( "\"%s\" in voca not referred by grammar", class->name);
	  wrong = 1;
	}
	freeClass = class;
	class = class->next;
	free( freeClass );
    }
     if (wrong) {
       errMes( "Some vocabulary not referred in grammar, compilation terminated");
     }
}

FA *getRecursion( CLASS *class, HIS *his )
{
    while( his != NULL ){
	if( his->class == class ){
	    if( his->cloneFA == NULL ){
		his->cloneFA = makeNewFA();
		his->cloneFA->nsList = his->nsList;
	    }
	    return( his->cloneFA );
	}
	his = his->prev;
    }
    return( NULL );
}

void chkLeftRecursion( CLASS *class, FA *fa, HIS *his )
{
    HIS *hisPtr = his;

    while( hisPtr != NULL && hisPtr->fa == fa ){
	if( hisPtr->class == class ){
	    errMes( "Left recusion is formed %s", strAncestors( his, hisPtr ) );
	}
	hisPtr = hisPtr->prev;
    }
}

char *strAncestors( HIS *me, HIS *ancestor )
{
    static char ancestorsList[ 1024 ];
    if( me == NULL /* infinite err$B$NH/8+$NET9g>e(BNULL$B$,Mh$k>l9g$,$"$k(B */ ){
	sprintf( ancestorsList, "in class,\"%s\"", StartSymbol->name );
    } else if( me == ancestor ){
	sprintf( ancestorsList, "in class,\"%s\"", me->class->name );
    } else {
	static char className[ 1024 ];
	strcpy( ancestorsList, "between classes" );
	while( 1 ){
	    sprintf( className, ",\"%s\"", me->class->name );
	    strcat( ancestorsList, className );
	    if( me == ancestor ) break;
	    if( (me = me->prev) == NULL ) break;
	}
    }
    return( ancestorsList );
}
    
CLASS *getClass( char *name )
{
    CLASS *class = ClassList;
    if( class == NULL ) return( NULL );
    while( 1 ){
	if( strcmp( class->name, name ) == 0 ){
	  class->used = 1;
	    return( class );
	}
	class = class->next;
	if( class == NULL ) return( NULL );
    }
}
