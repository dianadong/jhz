#ifndef _HASH_WORD_STATISTICAL_VECTOR_H_
#define _HASH_WORD_STATISTICAL_VECTOR_H_

#include"EntityRecog.h"

typedef struct SIattribute_node
{
	char * pword;      //
	char * partk;      //
	int fqval;         //
	double score;  
    struct SIattribute_node * pnext;
}SIATTBNODE, * siattlist;

////////////////////////////////////////

///////////////////////////////
int Insattriblist(siattlist * SL, const char * psenckey, int ituval);
void freeAtrList(siattlist * SL);
void writlist2r(siattlist slist, FILE * fw);
int wfvhash2list(hashtable wsvhash, siattlist * SL);
char * wlist2wkstr(siattlist slist, char * pwktres);
/////////////

int intent_aggVectorW(hashtable ewhash, const char * SIaggpath, const char * krespath, const char * scekey);
int uqueryVecfeat(hashtable ewhash, const char * puqkey, char * presvfeat);
#endif
