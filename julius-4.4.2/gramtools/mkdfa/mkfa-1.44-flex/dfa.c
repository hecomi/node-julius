/*
 * Copyright (c) 1991-2013 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2013 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */
#include "mkfa.h"
#include "dfa.h"
#include "nfa.h"

void r_makeDFA( FA *fa );
ARC *unifyFA( FA *dstFA, ARC *prevarc, ARC *curarc, FA *prevFA );
void usArc2nsArc( FA *fa );
void connectUnifyFA( FA *fa, int inp, FA *nextFA, FLAG reserved,
		    CLASSFLAGS accpt, CLASSFLAGS start );
ARC *unconnectFA( FA *srcFA, ARC *arcPrev, ARC *arc );
void killFA( FA *fa );
void killIsolatedLoop( FA *vanishFA, FA *curFA );
int chkIsolatedLoop( FA *vanishFA, FA *curFA );
UNIFYARC *appendUnifyArc( UNIFYARC *top, int inp, FA *fa, int reserved );
FALIST *appendGroup( FALIST *groupTop, FA *fa );
FALIST *insertFAlist( FALIST *top, FALIST *preAtom, FALIST *nextAtom, FA *fa );
FA *chkGroup( FALIST *group, CLASSFLAGS accptFlag,
	     CLASSFLAGS startFlag, FLAG *newFlag );
int cmpFAlist( FALIST *group1, FALIST *group2 );
FALIST *volatileFA( FALIST *volatileList, FA *fa );
void unvolatileFA( FALIST *volatileList );
void verboseGroup( FALIST *group );

static FALIST *GroupList = NULL; /* $B>uBVM;9g$5$l$??7>uBV$N%j%9%H(B */

static int DFAtravTotal = 0;   /* DFA$B:n@.;~$KN)$A4s$C$?%N!<%I?t(B */
static int DFAtravSuccess = 0; /* $B$=$N$&$A:#$^$G$KN)$A4s$C$F$$$J$+$C$??t(B */
static int FAprocessed = 0;    /* $B8=:_$N%9%F%C%W$K$*$$$F=hM}$5$l$?(BFA$B$N?t(B */
extern int FAtotal;            /* FA$B$NAm?t(B */

extern FA *FAlist;             /* FA$B%M%C%H%o!<%/$K$*$1$k3+;O(BFA$B$N%]%$%s%?(B */
extern char FAfile[ 1024 ];    /* FA$B%U%!%$%kL>(B(DFAorNFA) */
extern int SW_Verbose;
extern int SW_Quiet;
extern int SW_SemiQuiet;
extern int SW_Compati;
extern int NoNewLine;          /* $BJ#?t$NI=<(%b!<%I$G2~9TLdBj$r2r7h$9$k(B */
extern char Clipboard[ 1024 ]; /* sprintf$BMQ$N0l;~=q$-9~$_%P%C%U%!(B */

void makeDFA( void )
{
    if( !SW_Quiet ){
	fprintf( stderr, "Now making deterministic finite automaton" );
	NoNewLine = 1;
    }
    r_makeDFA( FAlist );
    if( !SW_Quiet ){
	fprintf( stderr, "\rNow making deterministic finite automaton[%d/%d] \n", FAprocessed, FAtotal );
	if( FAtotal != FAprocessed ){
	    fprintf( stderr, "* %d released FA nodes are left on isolated loop\n", FAtotal - FAprocessed );
	}
	NoNewLine = 0;
    }
    /* $B2?$+%P%0$,$"$C$?$H$-62$$$,8IN)%k!<%W$N%A%'%C%/$,(B
       $BIT2DG=$J$N$G$7$g$&$,$J$$(B */
    FAtotal = FAprocessed;
    if( SW_Verbose ){
	verboseMes( "** traversing efficiency ( success/total )" );
	verboseMes( "r_makeDFA:     %d/%d(%d%%)",
		DFAtravSuccess, DFAtravTotal, 100*DFAtravSuccess/DFAtravTotal);
    }
    newLineAdjust();
    freeFAlist( GroupList );
}

void r_makeDFA( FA *fa )
{
    ARC *prevarc = NULL;
    ARC *curarc;
    int inp;
    int bundleNum;
    FLAG reserved = 0;
    int i;
    FLAG newFlag;
    FALIST *volatileList = NULL;
    CLASSFLAGS unifyAccptFlag;
    CLASSFLAGS unifyStartFlag;

    verboseMes( "[func]r_makeDFA(FA %08x)", (long)fa );
    DFAtravTotal++;
    if( fa->traversed == 1 ){
	verboseMes( "traversed..." );
	return;
    }
    fa->traversed = 1;
    DFAtravSuccess++;

    FAprocessed++;
    if( !SW_SemiQuiet ){
	fprintf( stderr, "\rNow making deterministic finite automaton[%d/%d] ", FAprocessed, FAtotal );
	NoNewLine = 1;
    }
    curarc = fa->nsList;
    while( curarc != NULL ){
	FA *unifyingDstFA = NULL;
	{
	    ARC *arc = curarc;
	    int inp = arc->inp;
	    FALIST *group = NULL;
	    CLASSFLAGS accptFlag = 0;
	    CLASSFLAGS startFlag = 0;

	    bundleNum = 0;
	    while( 1 ){
		if( arc == NULL || arc->inp != inp ) break;
		group = appendGroup( group, arc->fa );
		accptFlag |= arc->fa->accpt;
		startFlag |= arc->fa->start;
		arc = arc->next;
		bundleNum++;
	    }
	    if( bundleNum > 1 ){
		unifyingDstFA = chkGroup( group, accptFlag,
					 startFlag,&newFlag );
	    } else {
		/* $B$3$N2<(B4$B9T$O%V%m%C%/30$N(Bwhile$B$KBP$7$F$N$b$N(B */
		freeFAlist( group );
		prevarc = curarc;
		curarc = curarc->next;
		continue;
	    }
	}

	inp = curarc->inp;
	unifyAccptFlag = 0;
	unifyStartFlag = 0;
	for( i = 0; i < bundleNum; i++ ){
	    unifyAccptFlag |= curarc->accpt;
	    unifyStartFlag |= curarc->start;
	    if( !newFlag ){
/*		volatileList = volatileFA( volatileList, curarc->ns );*/
		curarc = unconnectFA( fa, prevarc, curarc );
	    } else {
		if( curarc->fa == fa /* self-loop */ ){
		    reserved = 1;
/*		    volatileList = volatileFA( volatileList, fa );*/
		    curarc = unconnectFA( fa, prevarc, curarc );
		} else {
		    curarc = unifyFA( unifyingDstFA, prevarc, curarc, fa );
		}
	    }
	}
	connectUnifyFA( fa, inp, unifyingDstFA, reserved,
		       unifyAccptFlag, unifyStartFlag );
	reserved = 0;
    }
    usArc2nsArc( fa );
/*    unvolatileFA( volatileList );*/

    curarc = fa->nsList;
    while( curarc != NULL ){
	r_makeDFA( curarc->fa );
	curarc = curarc->next;
    }
}

void connectUnifyFA( FA *fa, int inp, FA *nextFA, FLAG reserved,
		    CLASSFLAGS accpt, CLASSFLAGS start )
{
    /* unifyFA$B$X$N%"!<%/$N%j%9%H$KF~NO$N<-=q=g$GE,@Z0LCV$KA^F~(B
       $B$^$?F1$8$b$N$,$"$k>l9gEPO?$7$J$$(B */
    /* nextFA $B$N(BpsNum$B$r%$%s%/%j%a%s%H$7$J$$(B */
    UNIFYARC *newArc;
    UNIFYARC *curArc = NULL;
    UNIFYARC *nextArc;
    UNIFYARC *top = fa->usList;

    if( (newArc = calloc( 1, sizeof(UNIFYARC) )) == NULL ){
	errMes( "Can't alloc forward arc buffer of finite automaton." );
    }
    newArc->inp = inp;
    newArc->us = nextFA;
    newArc->reserved = reserved;
    newArc->accpt = accpt;
    newArc->start = start;

    if( (nextArc = top) != NULL ){
	while( 1 ){
	    if( nextArc->inp > inp ) break;
	    if( nextArc->inp == inp && nextArc->us == nextFA ) return;
	    curArc = nextArc;
	    if( (nextArc = nextArc->next) == NULL ) break;
	}
    }
    if( curArc == NULL ){
	newArc->next = top;
	fa->usList = newArc;
    } else {
	newArc->next = nextArc;
	curArc->next = newArc;
    }
}

void usArc2nsArc( FA *fa )
{
    UNIFYARC *uptr;
    UNIFYARC *disused_uptr;
    ARC *nptr;
    ARC *newarc;

    uptr = fa->usList;
    while( uptr != NULL ){
	if( (newarc = calloc( 1, sizeof(ARC) )) == NULL ){
	    errMes( "Can't alloc forward arc buffer of finite automaton." );
	}
	connectFA( fa, uptr->inp, uptr->us, uptr->accpt, uptr->start );
	uptr = uptr->next;
    }
    
    uptr = fa->usList;
    while( uptr != NULL ){
	if( uptr->reserved ){
	    uptr->us->accpt |= fa->accpt;
	    nptr = fa->nsList;
	    while( nptr != NULL ){
		connectFA( uptr->us, nptr->inp, nptr->fa, nptr->accpt, nptr->start );
		nptr = nptr->next;
	    }
	}
	disused_uptr = uptr;
	uptr = uptr->next;
	free( disused_uptr );
    }
}

FALIST *volatileFA( FALIST *volatileList, FA *fa )
{
    FALIST *atom;

    if( (atom = malloc( sizeof(FALIST) )) == NULL ){
	errMes( "Can't alloc FA list buffer." );
    }
    fa->volatiled = 1;

    atom->fa = fa;
    atom->next = volatileList;
    return( atom );
}

void unvolatileFA( FALIST *volatileList )
{
    FALIST *atom;
    FA *fa;

    while( volatileList != NULL ){
	atom = volatileList;
	fa = atom->fa;
	fa->volatiled = 0;
/*	if( chkIsolatedLoop( fa, fa ) ){
	    killIsolatedLoop( fa, fa );
	}*/
	volatileList = volatileList->next;
	free( atom );
    }
}

ARC *unifyFA( FA *dstFA, ARC *prevarc, ARC *curarc, FA *prevFA )
{
    FA *srcFA = curarc->fa;
    ARC *arc = srcFA->nsList;

    dstFA->accpt |= srcFA->accpt;
    while( arc != NULL ){
	connectFA( dstFA, arc->inp, arc->fa, arc->accpt, arc->start );
	arc = arc->next;
    }
    return( unconnectFA( prevFA, prevarc, curarc ) );
}

ARC *unconnectFA( FA *srcFA, ARC *arcPrev, ARC *arc )
/* $B@Z$C$?%"!<%/$N<!$N%"!<%/$rJV$9(B */
{
    /* $B;XDj$NA0%N!<%I$H$N@\B3$r@Z$j!">CLG$9$Y$-$J$i<!%N!<%I$9$Y$F$H$N@\B3$r(B
       $B@Z$C$F>CLG$5$;$k!#(B*/

    ARC *arcNext = arc->next;
    FA *vanishFA;

    if( arcPrev == NULL ){
	srcFA->nsList = arcNext;
    } else {
	arcPrev->next = arcNext;
    }
    vanishFA = arc->fa;
    free( arc );

    if( --vanishFA->psNum == 0 ){
	killFA( vanishFA );
    }/* else if( chkIsolatedLoop( vanishFA, vanishFA ) ){
	killIsolatedLoop( vanishFA, vanishFA );
    }*/
    return( arcNext );
}

void killFA( FA *fa )
{
    ARC *arc = fa->nsList;
    verboseMes( "a FA node is vanished" );
    while( arc != NULL ){
	arc = unconnectFA( fa, NULL, arc );
    }
    free( fa );
    FAtotal--;
}

int chkIsolatedLoop( FA *vanishFA, FA *curFA )
/* $B$b$7<+J,$,>CLG$9$k$H2>Dj$7$?$i<+J,$X$N%"!<%/$,L5$/$J$k$+$r%A%'%C%/(B
   $B$9$J$o$A%k!<%W$K$h$k@8$-;D$j$r6n=|$9$k(B */
{
    ARC *arc;
    int result;

    if( curFA->volatiled ) return( 0 );
    if( curFA->psNum > 1 ) return( 0 );
    arc = curFA->nsList;

    while( arc != NULL ){
	FA *nextFA = arc->fa;
	if( nextFA == vanishFA ) return( 1 );
	result = chkIsolatedLoop( vanishFA, nextFA );
	if( result ) return( 1 );
	arc = arc->next;
    }
    return( 0 );
}

void killIsolatedLoop( FA *vanishFA, FA *curFA )
/* $B$b$7<+J,$,>CLG$9$k$H2>Dj$7$?$i<+J,$X$N%"!<%/$,L5$/$J$k$+$r%A%'%C%/(B
   $B$9$J$o$A%k!<%W$K$h$k@8$-;D$j$r6n=|$9$k(B */
{
    ARC *arc;
    ARC *prevarc = NULL;

    if( curFA->volatiled ) return;
    if( curFA->psNum > 1 ) return;

    arc = curFA->nsList;
    while( arc != NULL ){
	FA *nextFA = arc->fa;
	if( nextFA != vanishFA ){
	    unconnectFA( curFA, prevarc, arc );
	}
	prevarc = arc;
	arc = arc->next;
    }
    free( curFA );
    FAtotal--;
}

FALIST *appendGroup( FALIST *groupTop, FA *fa )
{
    /* fa$B$,M;9g>uBV$G$J$$$J$i(BFA$B$N%]%$%s%?$r%=!<%H$7$F%0%k!<%W%j%9%H$X(B
       $BM;9g>uBV$J$i$=$N9=@.%j%9%H$H%0%k!<%W%j%9%H$r9g$o$;$F%=!<%H$9$k(B */

    FALIST *preAtom = NULL;
    FALIST *curAtom = groupTop;
    FALIST *srcCurAtom = NULL;
    long cmp;

    if( fa->group == NULL ){
	while( curAtom != NULL ){
	    cmp = (long)fa - (long)curAtom->fa;
	    if( cmp == 0 ) return( groupTop );
	    if( cmp < 0 ) break;
	    preAtom = curAtom;
	    curAtom = curAtom->next;
	}
	return( insertFAlist( groupTop, preAtom, curAtom, fa ) );
    } else {
	/* srcCurAtom$B$,%=!<%H$5$l$F$$$k$3$H$rMxMQ$9$l$P$b$C$H=hM}$,B.$/$J$k$,(B
	   $B$=$&$9$k$H$J$<$+>uBV?t$,B?>/A}$($F$7$^$&$N$GI,$:$7$bJ]>Z$5$l$F$$$J$$$+$b(B
	   "for"$B$NCm<a$r<h$C$F!"(BpreAtom = NULL; curAtom = groupTop;(2$B8D=j(B) $B$r;&$9(B */
	for( srcCurAtom = fa->group; srcCurAtom != NULL;
	    srcCurAtom = srcCurAtom->next ){
	    if( curAtom == NULL ){
		groupTop = insertFAlist( groupTop, preAtom, curAtom, srcCurAtom->fa );
		preAtom = NULL;
		curAtom = groupTop;
	    }
/*		for( ; srcCurAtom != NULL; srcCurAtom = srcCurAtom->next ){
		    groupTop = insertFAlist( groupTop, preAtom, NULL, srcCurAtom->fa );
		    if( preAtom == NULL ){
			preAtom = groupTop->next;
		    } else {
			preAtom = preAtom->next;
		    }
		}
		break;
	    }*/
	    cmp = (long)srcCurAtom->fa - (long)curAtom->fa;
	    if( cmp == 0 ) continue;
	    if( cmp < 0 ){
		groupTop = insertFAlist( groupTop, preAtom, curAtom, srcCurAtom->fa );
		preAtom = NULL;
		curAtom = groupTop;
	    } else {
		preAtom = curAtom;
		curAtom = curAtom->next;
	    }
	}
	return( groupTop );
    }
}

FALIST *insertFAlist( FALIST *top, FALIST *preAtom, FALIST *nextAtom, FA *fa )
{
    FALIST *atom;

    if( (atom = malloc( sizeof(FALIST) )) == NULL ){
	errMes( "Can't alloc group buffer for unifying FA" );
    }
    atom->fa = fa;
    if( preAtom == NULL ){
	atom->next = nextAtom;
	return( atom );
    } else {
	preAtom->next = atom;
	atom->next = nextAtom;
	return( top );
    }
}

FA *chkGroup( FALIST *group, CLASSFLAGS accptFlag ,
	     CLASSFLAGS startFlag, FLAG *newFlag )
{
    FALIST *curGroupList = GroupList;
    FALIST *preGroupList = NULL;
    int cmp;
    FA *fa;

    while( curGroupList != NULL ){
	cmp = cmpFAlist( curGroupList->fa->group, group );
	if( cmp == 0 ){
	    if( SW_Compati || (accptFlag == curGroupList->fa->accpt
	       || startFlag == curGroupList->fa->start) ){
		freeFAlist( group );
		*newFlag = 0;
		return( curGroupList->fa );
	    }
	}
	if( cmp < 0 ) break;
	preGroupList = curGroupList;
	curGroupList = curGroupList->next;
    }
    if( SW_Verbose ){ 
	verboseGroup( group );
    }
    fa = makeNewFA();
    GroupList = insertFAlist( GroupList, preGroupList, curGroupList, fa );
    fa->group = group;
    fa->accpt = accptFlag;
    fa->start = startFlag;
    *newFlag = 1;
    return( fa );
}

void verboseGroup( FALIST *group )
{
    verboseMes( "Created New Group" );
    while( group != NULL ){
	verboseMes( "  FAadr: %08x", (long)group->fa );
	group = group->next;
    }
}

int cmpFAlist( FALIST *group1, FALIST *group2 )
{
    long cmp;

    while( 1 ){
	if( group1 == NULL && group2 == NULL ) return( 0 );
	if( group1 == NULL ) return( -1 );
	if( group2 == NULL ) return( 1 );
	cmp = (long)group1->fa - (long)group2->fa;
	if( cmp != 0 ) return( cmp );
	group1 = group1->next;
	group2 = group2->next;
    }
}
