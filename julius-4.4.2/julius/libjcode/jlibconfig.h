/*
 * config.h
 */

#define ASCII         0
#define JIS           1
#define EUC           2
#define SJIS          3
#define NEW           4
#define OLD           5
#define NEC           6
#define EUCORSJIS     7
#define NUL           0
#define LF            10
#define FF            12
#define CR            13
#define ESC           27
#define SS2           142
#define TRUE          1
#define FALSE         0

/* Ⱦ�ѥ���(SJIS) ����Ū�����ѥ��ʤ��ѹ����� */
#undef NO_HANKAKU_SJIS

/* �Ҥ餬�ʡ��������ʤ��¤Ӥ��� EUC , SJIS ����ꤹ�� */
#define KANA_NARABI

/* EUC , SJIS ������Ϥ�ΰ��٤����Ԥʤ�ʤ� */
/* (�ե�������Ĥ����Ҥ餫�ʤ��ʤ�ͭ���Ǥ���) */
#undef USE_CACHE_KANA_NARABI

/* strdup�ؿ��������ƥ�ˤ����define */
#define HAVE_STRDUP     1

/* �ǥХå��Ѥδؿ���ͭ���ˤ��� */
#undef DEBUG

/* ������̾(��ʸ������ʸ���϶��̤���ʤ�) */
#define SJIS_LOCALE_NAME  "ja_JP.SJIS", "ja_JP.PCK"
#define JIS_LOCALE_NAME   "ja_JP.JIS", "ja_JP.jis7"
#define EUC_LOCALE_NAME   "ja_JP.ujis", "ja_JP.EUC",\
                          "ja_JP.eucJP","japanese", "ja"
