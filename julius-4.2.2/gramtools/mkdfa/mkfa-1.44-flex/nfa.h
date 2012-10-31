/*
 * Copyright (c) 1991-2012 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2012 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */
void makeNFA( void );
void connectFA( FA *fa, int inp, FA *nextFA, CLASSFLAGS accpt, CLASSFLAGS start );
ARC *appendNextArc( ARC *top, int inp, FA *fa );
FA *makeNewFA( void );
FALIST *appendFAlist( FALIST *faList, FA *fa );
FALIST *cpyFAlist( FALIST *dst, FALIST *src );
FALIST *freeFAlist( FALIST *faList );
