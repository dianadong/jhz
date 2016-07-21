
#ifndef _SEGNT_TERMME_H_
#define _SEGNT_TERMME_H_

#define	MAX_SEG_LEN		32
#define	MAX_WORD_LEN		64
#define	HASH_SIZE	4194304

//数据结构
typedef unsigned char		uchar;
typedef unsigned int		uint;
typedef unsigned short      ushort;
//方法函数
//============
unsigned int dhash(char *key, register int len, int i);
int getfq(uchar *p, int ilen);
int loaddic(char * singpath, char *  wordpath);
int loadmajordic(char * singpath, char *  wordpath, char * majopath);

int Initlex(char * singpath, char *  wordpath);
int Initlex3maj(char * singpath, char * wordpath, char * majopath);

void getsegmenter(char * const src, int *len, int ilen);
int repqiefen(char * srcbuf, int lensrc, char * resbuf, int lenres);
void freewdlist();

#endif
