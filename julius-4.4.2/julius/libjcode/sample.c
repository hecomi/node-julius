/*
  sample.c -- ����ץ�ץ����   by. �Ҹ� ��Ϻ, 1996

  ���Υץ����ϡ�����ץ�ץ����Ǥ���
  ɸ�����Ϥ����ɤ߹�����ƥ����Ȥ򥪥ץ����ˤ��碌�ơ�
  �������Ѵ����ޤ���
*/

#include <stdio.h>
#include <string.h>
#include "jlib.h"

extern void printDetectCode(int code);

void main(int ac, char *av[]) {
  char buffer[BUFSIZ];
  int mode=JIS;

  if(ac == 2) {
    if(!strcmp(av[1], "-e")) mode = EUC;
    if(!strcmp(av[1], "-s")) mode =SJIS;
  }

  while(fgets(buffer,BUFSIZ,stdin) != NULL) {
    switch(mode) {
    case EUC:
      printf("%s", toStringEUC(buffer));
      break;
    case SJIS:
      printf("%s", toStringSJIS(buffer));
      break;
    default:
      printf("%s", toStringJIS(buffer));
      break;
    }
  }
  exit(0);
}



