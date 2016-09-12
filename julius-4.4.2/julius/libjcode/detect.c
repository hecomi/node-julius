/*
 * detect.c - libjcode �����ɼ�ưȽ��롼����
 *                (C) Kuramitsu Kimio, Tokyo Univ. 1996-97 
 *
 * Ken Lunde �� �����ܸ��������� �� jconv.c �򻲹ͤ˺�� 
 * �ޤ�����
 * CGI �ץ����Ǥ����Ѥ�ͤ��ơ��Ҥ餬�ʡ��������ʤ��¤�
 * �ˤ��SJIS, EUC �ο��ꥢ�르�ꥺ������ä��Ƥ���ޤ���
 *
 * ��������
 *       - jcode.pl��SJIS/EUCȽ��˥����ɤ��Ѥ���
 *                Yasuyuki Furukawa (yasu@on.cs.keio.ac.jp)
 */

#include <stdio.h>
#include "jlibconfig.h"

static int _detect(unsigned char *str, int expected);
static int _detect_euc_or_sjis(unsigned char *str);
void printDetectCode(int detected);
int detectKanjiCode(char *str);


/* ------------------------------------------------------ ���������ɤμ��� --*/

static int _detect(unsigned char *str, int expected)
{
  register int c;

  while((c = (int)*str)!= '\0') {

    /* JIS �����ɤ�Ƚ�� */
    if(c == ESC) {
      if((c = (int)*(++str)) == '\0') return expected; 
      if (c == '$') {
	if((c = (int)*(++str)) == '\0') return expected; 
	/* ESC $ B --> ��JIS 
	   ESC $ @ --> ��JIS */
	if (c == 'B' || c == '@') return JIS;
      }
#ifdef NECKANJI
      if (c == 'K')
	return NEC;    /* ESC K --> NEC JIS */
#endif
      str++;
      continue;
    }
    
    /* SJIS �˰�դ˷��� */
    if ((c >= 129 && c <= 141) || (c >= 143 && c <= 159))
        return SJIS;
    /* SS2 */
    if (c == SS2) {
      if((c = (int)*(++str)) == '\0') return expected; 
      if ((c >= 64 && c <= 126) || (c >= 128 && c <= 160) || 
	  (c >= 224 && c <= 252)) return SJIS;
      if (c >= 161 && c <= 223) expected = EUCORSJIS;
      str++;
      continue;
    }

    if (c >= 161 && c <= 223) {
      if((c = (int)*(++str)) == '\0') return expected; 
      
      if (c >= 240 && c <= 254)
	return EUC;
      if (c >= 161 && c <= 223) {
	expected = EUCORSJIS;
	str++;
	continue;
      }
      if (c <= 159) return SJIS;
      if (c >= 240 && c <= 254) return EUC;
     
      if (c >= 224 && c <= 239) {
	expected = EUCORSJIS;
	while (c >= 64) {
	  if (c >= 129) {
	    if (c <= 141 || (c >= 143 && c <= 159))
	      return SJIS;
	    else if (c >= 253 && c <= 254) {
	      return EUC;
	    }
	  }
	  if((c = (int)*(++str)) == '\0') return EUCORSJIS; 
	}
	str++;
	continue;
      }
      
      if (c >= 224 && c <= 239) {
        if((c = (int)*(++str)) == '\0') return expected; 
        if ((c >= 64 && c <= 126) || (c >= 128 && c <= 160))
          return SJIS;
        if (c >= 253 && c <= 254) return EUC;
        if (c >= 161 && c <= 252)
          expected = EUCORSJIS;
      }
    }
    str++;
  }
  return expected;
}

/* ---------------------------------------------------SJIS �� EUC ��Ƚ�� -- */
/* ���ܸ����ħ(�Ҥ餬�ʡ���������)����Ƚ�ꤹ�뤿�ᡢ
   Ƚ��ߥ���ȯ��������⤢��ޤ��� */

static int _detect_euc_or_sjis(unsigned char *str) {
  int c1, c2;
  int euc_c = 0, sjis_c = 0;
  unsigned char *ptr;
  static int expected = EUCORSJIS;

#ifdef USE_CACHE_KANA_NARABI
  if (expected != EUCORSJIS)
      return expected;
#endif

  ptr = str, c2 = 0;
  while ((c1 = (int)*ptr++) != '\0') {
      if ((c2 >  0x80 && c2 < 0xa0) && (c2 >= 0xe0 && c2 < 0xfd) &&
	  (c1 >= 0x40 && c1 < 0x7f) && (c1 >= 0x80 && c1 < 0xfd))
	  sjis_c++, c1 = *ptr++;
      c2 = c1;
  }
  if (sjis_c == 0)
      expected = EUC;
  else {
      ptr = str, c2 = 0;
      while ((c1 = (int)*ptr++) != '\0') {
	  if ((c2 > 0xa0  && c2 < 0xff) &&
	      (c1 > 0xa0  && c1 < 0xff))
	    euc_c++, c1 = *ptr++;
	  c2 = c1;
      }
      if (sjis_c > euc_c)
	  expected = SJIS;
      else
	  expected = EUC;
  }
  return expected;
}

/* ---------------------------------------------------------- Public �ؿ� -- */


#ifdef DEBUG
void printDetectCode(int detected) {
  switch(detected) {
  case ASCII:
    fprintf(stderr, "ASCII/JIS-Roman characters(94 printable)\n");
    break;
  case JIS:
    fprintf(stderr, "JIS(iso-2022-jp)\n");
    break;
  case EUC:
    fprintf(stderr, "EUC(x-euc-jp)\n");
    break;
  case SJIS:
    fprintf(stderr, "SJIS(x-sjis)\n");
    break;
  case NEW:
    fprintf(stderr, "JIS X 0208-1990\n");
    break;
  case OLD:
    fprintf(stderr, "JIS X 0208-1978\n");
    break;
  case EUCORSJIS:
    fprintf(stderr, "EUC or SJIS\n");
    break;
  default:
    printf("Another Codes!!\n");
  }
}
#endif

int detectKanjiCode(char *str)
{
  static int detected = ASCII;

  if(!str) return (0);

  /* JIS, EUC, SJIS, EUCORSJIS ��Ƚ�� */
  detected = _detect((unsigned char *)str, ASCII);

  /* ��JIS, ��JIS, NEC JIS �ξ�硢JIS ���ѹ����� */
  if(detected == NEW || detected == OLD || detected == NEC)
    return JIS;

  /* SJIS �� EUC �ζ��̤򥫥ʤ��¤Ӥ�����ꤹ�� */
  if(detected == EUCORSJIS)
#ifdef KANA_NARABI
    detected = _detect_euc_or_sjis((unsigned char *)str);
#else
    detected = EUC;  /* �ǥե���� EUC */
#endif
#ifdef DEBUG
    printDetectCode(detected);
#endif

  return detected;
}
