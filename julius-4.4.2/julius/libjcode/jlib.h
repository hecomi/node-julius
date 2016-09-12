/*
 *  jlib.h -- jlib.c �ѤΥإå��ե�����  by. �Ҹ� ��Ϻ
 *
 *  jlib.c �����Ѥ�����ϡ�include ���Ƥ���������
 *
 */

/* modified by ri */

#define ASCII         0
#define JIS           1
#define EUC           2
#define SJIS          3

extern int detectKanjiCode(char *str);
/* extern char *toStringJIS(char *str); */
extern char *toStringJIS(char *str, char *buf, int maxlen);
/* extern char *toStringEUC(char *str); */
extern char *toStringEUC(char *str, char *buf, int maxlen);
/* extern char *toStringSJIS(char *str); */
extern char *toStringSJIS(char *str, char *buf, int maxlen);
/* extern char *toStringAuto(char *str); */
extern char *toStringAuto(char *str, char *buf, int maxlen);

extern char *EUCtoSJIS(char *str, char *buf, int maxlen);
