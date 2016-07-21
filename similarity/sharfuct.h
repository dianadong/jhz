
#ifndef _SHARING_FUNCTION_H_
#define _SHARING_FUNCTION_H_

#define	DCHASH_SIZE	5000000
#define	LEXC_SIZE	256

///////////////////////////////////////
typedef struct entitynode
{
	char * pentityk;   //实体名称   词体
	char * pidcode;    //实体编码   词性
	int startsit;      //实体起始位置  词频率
    struct entitynode * pnext;
}ENTNODE, * entlist;


//////////////////////////////////////////

///////////////////////////
///
////
////////////////////

int GetRealString(char *pbuf);
int removalspecstr(const char *psource, char * ptarget);
int removbrackstr(const char *psource, char * ptarget);
void prepareCryptTable();
unsigned long HashString(const char *lptermName, unsigned long dwHashType);

static void __kmp_test(unsigned char *W, unsigned int wlen, unsigned int *T);
static void __kmp_table(unsigned char *W, unsigned int wlen, unsigned int *T);
unsigned int kmp_search(unsigned char *S, unsigned int slen, unsigned char *W, unsigned int wlen);

int ins_entList(entlist *EL, const char * pentity, const char * pidcode, int startsit);
void writentlist(entlist elist, FILE * fp);
void printlist(entlist elist);
void freeEntlist(entlist *EL);
void CopyElist(entlist *BL, entlist xlist);
int __simimin(int a, int b, int c);
int __simicmp_distancA(char *strA, int pABegin, int pAEnd, char *strB, int pBBegin, int pBEnd, int **temp);
int str2similar(const char * pAcomkey, const char *pBcomkey);
///////////////////////////////
////
/////
//////////////////////////////
void sourceqiefen(char ** psource,char separator,char *result);
entlist * createuserlist(const char *pbuf, entlist *SL);

#endif
