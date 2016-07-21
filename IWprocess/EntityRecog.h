
#ifndef _HASH_ENTITY_NAME_NODE_H_
#define _HASH_ENTITY_NAME_NODE_H_
////////////////////////////////////////
//hash链表
typedef struct SIattribute_node
{
	char * scene;       //场景
	int intent;       //意图
	double score;       //描述程度
	struct SIattribute_node * pnext;
}SIATTBNODE, *siattlist;

typedef struct Hashmangnode
{
	long nHashA;
	long nHashB;
	int  bExists;

	char * pentityname;           //实体名称
	int  gnum;
	siattlist  slist;             //changjing

}HASHMANGNODE, *hashtable;

int Insattriblist0(siattlist * SL, const char * scene, const char * intid, double score);

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
int Ins4Hashmg(hashtable * HashMg, const char *pmkey, char * scene, int intid, double score);
int Insertsiattlist(siattlist * SL, char * scene, int  intid, double score);
void print(siattlist *SL, FILE * fw);
#endif
