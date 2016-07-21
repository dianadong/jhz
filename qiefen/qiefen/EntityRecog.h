

#ifndef _HASH_ENTITY_NAME_NODE_H_
#define _HASH_ENTITY_NAME_NODE_H_
////////////////////////////////////////
///适应一个entity
typedef struct Hashmangnode
{
	long nHashA;
	long nHashB;
	int  bExists;

	char * pentityname;           //实体名称
	char * pidcode;               //编码  cixing
	int fqval;
	double score;                 //分值  
}HASHMANGNODE, *hashtable;

/////////////
void Init_mghash(hashtable * HashMg);
void free_mghash(hashtable * HashMg);
void writ_mghash(hashtable hashmg,const char * pnduppath);
long GetHashmg(hashtable hash_mg, const char *pmkey);

////////
int SetHashmg(hashtable * HashMg, const char *pmkey, const char * pidkey);
int EntityCode2Table(hashtable * HashMg, const char * sourcepath);
int Sceneinfo2hash(hashtable * HashMg, const char * Snamepath, const char * Sstarpath, const char * Ssentpath);
int SC_Ey2_Epail2hash(hashtable * HashMg, const char * Snamepath, const char * Sstarpath, const char * Ssentpath, const char * Epairpath);
int wordpart2Table(hashtable * HashMg, const char * sourcepath);
int Ins3Hashmg(hashtable * HashMg, const char *pmkey, const char * pidkey, int fqv);
#endif
