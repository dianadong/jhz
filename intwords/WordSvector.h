#ifndef _HASH_WORD_STATISTICAL_VECTOR_H_
#define _HASH_WORD_STATISTICAL_VECTOR_H_
#define MAXCHARS 256
//#define MAXLINES 770      //停用表大小
#include"EntityRecog.h"
#include "sharfuct.h"



typedef struct SIattribute_node
{
	char * pword;      //词实体
	char * partk;      //词性
	int fqval;         //词频
	double score;
    struct SIattribute_node * pnext;
}SIATTBNODE, * siattlist;


////////////////////////////////////////

///////////////////////////////
int Insattriblist0(siattlist * SL, const char * psenckey, const char * partk, int fqval);
int Insattriblist(siattlist * SL, const char * psenckey, const char * partk, int fqval,double score);
void freeAtrList(siattlist * SL);
void writlist2r(siattlist slist, FILE * fw);
int wfvhash2list(hashtable wsvhash, siattlist * SL);
char * wlist2wkstr(siattlist slist, char * pwktres);
/////////////

//int intent_aggVectorW(hashtable ewhash, const char * SIaggpath, const char * krespath);
//int intent_aggVectorW(hashtable ewhash, const char * SIaggpath, FILE *fw);
int intent_aggVectorW(hashtable ewhash, const char * SIaggpath, char **stop, int nStop, FILE *fw);
int uqueryVecfeat(hashtable ewhash, const char * puqkey, char * presvfeat);
void print(siattlist *SL);
siattlist * createlist(const char *pbuf, siattlist *SL);
SIATTBNODE * LocateNode(const char *key,const siattlist intentL);
//void ComputeWeight(const SIATTBNODE *Lnode,const int fval,double *degreeA, double *degreeAB);
void ComputeWeight(const SIATTBNODE *Lnode, const char * pidcode, const int fval, double *degreeA, double *degreeAB);
double VectorSimil(const entlist userL, const siattlist intentL);
int readStopLib(char *f, char **stop);
void removeStopWord(char **src, char **stop,const int nStop);
siattlist SortInsert(siattlist head, siattlist node);
#endif
